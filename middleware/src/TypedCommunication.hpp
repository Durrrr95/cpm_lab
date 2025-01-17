#pragma once

#include <optional>
#include <string>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <cassert>
#include <type_traits>

#include "Header.hpp"
#include "VehicleState.hpp"
#include "VehicleCommandDirect.hpp"
#include "VehicleCommandTrajectory.hpp"
#include "VehicleCommandPathTracking.hpp"
#include "VehicleCommandSpeedCurvature.hpp"

#include "cpm/Logging.hpp"
#include "cpm/ParticipantSingleton.hpp"
#include "cpm/Reader.hpp"
#include "cpm/Timer.hpp"
#include "cpm/VehicleIDFilteredTopic.hpp"
#include "cpm/Writer.hpp"
#include "cpm/Participant.hpp"

using namespace std::placeholders;

/**
 * \class TypedCommunication
 * \brief This class can be used to create all readers and writers required for the middleware whose type can change, 
 * e.g. if instead of the trajectory the speed + curvature are used as commands
 * It also checks some messages for consistency and remembers when they were received (simulated or real time)
 * for further checks in main
 * WARNING: There is no error handling for incompatible types
 * \ingroup middleware
 */
template<class MessageType> class TypedCommunication {
    private:
        //! DDS async reader for HLC, to receive commands for a vehicle generated by the HLC
        cpm::AsyncReader<MessageType> hlcCommandReader;

        //! DDS writer to send commands received by the HLC to a vehicle
        cpm::Writer<MessageType> vehicleWriter;

        //! For access to get_time (in simulated time, only the timer instance knows the current simulated time)
        std::shared_ptr<cpm::Timer> timer;
        /** 
         * \brief Maps HLC ID -> Timestamp
         * 
         * Real time: Last receive time of HLC message (to check for violation of period) for each HLC ID.
         * 
         * Simulated time: Last response time should match the current time.
         */
        std::unordered_map<uint8_t, uint64_t> lastHLCResponseTimes;
        //! Mutex for access to lastHLCResponseTimes
        std::mutex map_mutex;

        //! To check messages received from the HLC regarding their consistency with the vehicle IDs set for the middleware
        std::vector<uint8_t> vehicle_ids;

        //! To check messages received from the HLC regarding their consistency with the timing managed by the middleware. In nanoseconds. 
        std::atomic<uint64_t> current_period_start{0};

        /**
         * \brief Handler for vehicle commands received by the HLC.
         * Passes the commands on to the vehicle.
         * Also performs some checks on the commands, e.g. if the vehicle ID is correct.
         * 
         * \param samples Received vehicle commands by the HLC
         */
        void handler(std::vector<MessageType>& samples)
        {
            // Process sample 
            for (auto& data : samples) {
                uint64_t receive_timestamp = timer->get_time();

                //First send the data to the vehicle
                sendToVehicle(data);

                //Then update the last response time of the HLC that sent the data
                std::lock_guard<std::mutex> lock(map_mutex);
                lastHLCResponseTimes[data.vehicle_id()] = receive_timestamp;

                //This might be problematic, but if we perform checks before sending the message then this 
                //might lead to a violation of timing boundaries

                //Then check if the sent data was plausible -> TODO? 
                // - Check if the valid after time is correct - TODO: Make sure that header() exists?
                //data.header().valid_after_stamp().nanoseconds()

                //1. Make sure that the set vehicle ID is valid (assertion of field done in constructor)
                auto set_id = data.vehicle_id();
                if(std::find(vehicle_ids.begin(), vehicle_ids.end(), set_id) == vehicle_ids.end())
                {
                    //ID should not have been sent, print warning
                    cpm::Logging::Instance().write(
                        1,
                        "Middleware received vehicle ID %i from HLC script - ID was not set to be used!",
                        static_cast<int>(set_id)
                    );
                }

                //2. Make sure that the creation timestamp is consistent with the current timing
                auto header_create_stamp = data.header().create_stamp().nanoseconds();
                auto current_time = cpm::get_time_ns();
                //  a) If the stamp is newer than the current time, then the timing function in the HLC script 
                //     must be wrong - after all, it runs on the same machine, so the same clock is being used
                if (header_create_stamp > current_time)
                {
                    cpm::Logging::Instance().write(
                        1,
                        "Middleware (ID %i) received creation stamp from HLC script that lies in the future - this must be a mistake",
                        static_cast<int>(set_id)
                    );
                }
                //  b) If the stamp is older than the beginning of the current period, than the HLC script took too long
                //     Missed periods are also checked in Communication, but only in terms of an absolute time diff
                if (header_create_stamp < current_period_start.load())
                {
                    cpm::Logging::Instance().write(
                        1,
                        "Middleware (ID %i): Received HLC message missed the current period",
                        static_cast<int>(set_id)
                    );
                }

                //Perform type specific checks (like amount of trajectory points for trajectory data)
                type_specific_msg_check(data);
            }
        }

        //Ignore warning that t_start is unused
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-parameter"

        /**
         * \brief Type specific / unspecific handler, 
         * the actual check depends on the message type and can be found in the .cpp file for this class
         * \param msg Message to check
         */
        void type_specific_msg_check(MessageType msg)
        {
            //Unspecific version, thus empty
            //Specializations can be found in the .cpp file
        }

        #pragma GCC diagnostic pop

        
    public:
        /**
         * \brief Constructor
         * \param hlcParticipant DDS participant instance for the communication with the HLC script
         * \param vehicleCommandTopicName Topic name for the selected message type
         * \param _timer To get the current time for real and simulated timing
         * \param _vehicle_ids List of IDs the Middleware and HLC are responsible for
         */
        TypedCommunication(
            cpm::Participant& hlcParticipant,
            std::string vehicleCommandTopicName,
            std::shared_ptr<cpm::Timer> _timer,
            std::vector<uint8_t> _vehicle_ids
        )
        :
        hlcCommandReader(std::bind(&TypedCommunication::handler, this, _1), hlcParticipant, vehicleCommandTopicName)
        ,vehicleWriter(vehicleCommandTopicName)
        ,timer(_timer)
        ,lastHLCResponseTimes()
        ,vehicle_ids(_vehicle_ids)
        {
            static_assert(std::is_same<decltype(std::declval<MessageType>().vehicle_id()), uint8_t>::value, "IDL type must have a vehicle_id.");
            static_assert(std::is_same<decltype(std::declval<MessageType>().header().create_stamp().nanoseconds()), unsigned long long>::value, "IDL type must use the Header IDL as header.");
        }

        /**
         * \brief Returns latest HLC response time (for the last received vehicle command) or an empty optional if no entry could be found
         * \param id ID of the HLC to get the response time of
         */
        std::optional<uint64_t> getLatestHLCResponseTime(uint8_t id) {
            std::lock_guard<std::mutex> lock(map_mutex);
            if (lastHLCResponseTimes.find(id) != lastHLCResponseTimes.end())
                return std::optional<uint64_t>(lastHLCResponseTimes[id]);
            
            return std::nullopt;
        }

        /**
         * \brief Deprecated. Only left for testing purposes, do not use for anything else.
         * To get the map (HLC ID -> Timestamp) of last HLC response times (for the last received vehicle commands).
         */
        const std::unordered_map<uint8_t, uint64_t> getLastHLCResponseTimes() {
            std::lock_guard<std::mutex> lock(map_mutex);
            return lastHLCResponseTimes;
        }

        /**
         * \brief Send a command to a vehicle
         * \param message The command to send
         */
        void sendToVehicle(MessageType message) {
            vehicleWriter.write(message);
        }

        /**
         * \brief Update the current period start time stored in typed communication for internal checks
         * \param t_now Current period time, obtained by the cpm timer
         */
        void update_period_t_now(uint64_t t_now)
        {
            current_period_start.store(t_now);
        }
};