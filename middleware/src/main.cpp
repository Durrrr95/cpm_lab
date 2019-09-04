/**
 * \class main.cpp
 * \brief This class includes the main function and is mainly responsible for the initialization of the middleware
 */

#include <memory>
#include <sstream>
#include <string>
#include <functional>
#include <dds/core/QosProvider.hpp>

#include <dds/sub/ddssub.hpp>

#include "cpm/Timer.hpp"
#include "cpm/Parameter.hpp"
#include "cpm/Logging.hpp"
#include "cpm/CommandLineReader.hpp"
#include "cpm/init.hpp"

#include "../idl_compiled/VehicleStateList.hpp"

#include "Communication.hpp"

int main (int argc, char *argv[]) { 
    //Initialize the cpm logger, set domain id etc
    cpm::init(argc, argv);
    cpm::Logging::Instance().set_id("middleware_main"); 

    //Initial parameters that can partially be set in the command line
    //Timer parameters
    std::string node_id = cpm::cmd_parameter_string("node_id", "middleware", argc, argv);
    uint64_t period_nanoseconds = cpm::cmd_parameter_uint64_t("period_nanoseconds", 250000000, argc, argv);
    uint64_t offset_nanoseconds = cpm::cmd_parameter_uint64_t("offset_nanoseconds", 1, argc, argv);
    //uint64_t wait_timeout = cpm::cmd_parameter_uint64_t("wait_timeout", 6000000000, argc, argv); //6 seconds wait before HLC is asked for new data again
    bool simulated_time_allowed = true;
    bool simulated_time = cpm::cmd_parameter_bool("simulated_time", false, argc, argv);
    bool wait_for_start = cpm::cmd_parameter_bool("wait_for_start", true, argc, argv);

    //Communication parameters
    int hlcDomainNumber = cpm::cmd_parameter_int("domain_number", 1, argc, argv); 
    bool useParamServer = cpm::cmd_parameter_bool("use_param_server", false, argc, argv); //If set to true: Param server can be used to change the command type at runtime
    std::string logTopicName = cpm::cmd_parameter_string("log_topic", "logTopic", argc, argv);
    std::string hlcStateTopicName = cpm::cmd_parameter_string("hlc_state_topic", "stateTopic", argc, argv); 
    std::string vehicleStateTopicName = cpm::cmd_parameter_string("vehicle_state_topic", "vehicleState", argc, argv); 
    std::string hlcTrajectoryTopicName = cpm::cmd_parameter_string("hlc_trajectory_topic", "trajectoryTopic", argc, argv); 
    std::string vehicleTrajectoryTopicName = cpm::cmd_parameter_string("vehicle_trajectory_topic", "vehicleCommandTrajectory", argc, argv); 
    std::string hlcSpeedCurvatureTopicName = cpm::cmd_parameter_string("hlc_speed_curvature_topic", "speedCurvatureTopic", argc, argv); 
    std::string vehicleSpeedCurvatureTopicName = cpm::cmd_parameter_string("vehicle_speed_curvature_topic", "vehicleCommandSpeedCurvature", argc, argv); 
    std::string hlcDirectTopicName = cpm::cmd_parameter_string("hlc_direct_topic", "directTopic", argc, argv); 
    std::string vehicleDirectTopicName = cpm::cmd_parameter_string("vehicle_direct_topic", "vehicleCommandDirect", argc, argv); 
    int vehicleID = cpm::cmd_parameter_int("vehicle_id", 1, argc, argv); 
    std::vector<int> default_ids{ 1 };
    std::vector<int> vehicle_ids = cpm::cmd_parameter_ints("vehicle_ids", default_ids, argc, argv);

    std::cout << "Configuration: " << std::endl;
    std::stringstream ids;
    for (auto id : vehicle_ids) {
        ids << id << " ";
    }
    std::cout   << "\tHLC domain number: " << hlcDomainNumber << std::endl
                << "\tVehicle IDs (and HLC IDs): " << ids.str() << std::endl
                << "\tNode ID: " << node_id << std::endl
                << "\tPeriod: " << period_nanoseconds << std::endl
                << "\tOffset: " << offset_nanoseconds << std::endl
                << "\tParam server used: " << useParamServer << std::endl
                << "\tLog topic: " << logTopicName << std::endl
                << "\tHLC state topic: " << hlcStateTopicName << std::endl
                << "\tVehicle state topic: " << vehicleStateTopicName << std::endl
                << "\tHLC trajectory topic: " << hlcTrajectoryTopicName << std::endl
                << "\tVehicle trajectory topic: " << vehicleTrajectoryTopicName << std::endl
                << "\tHLC speed curvature topic: " << hlcSpeedCurvatureTopicName << std::endl
                << "\tVehicle speed curvature topic: " << vehicleSpeedCurvatureTopicName << std::endl
                << "\tHLC direct topic: " << hlcDirectTopicName << std::endl
                << "\tVehicle direct topic: " << vehicleDirectTopicName << std::endl;

    //Use Parameter Server to set initial parameters
    if (useParamServer) {
        period_nanoseconds = cpm::parameter_int("hlc/period_milliseconds"); //Cast this? (uint32_t vs uint64_t)
        offset_nanoseconds = cpm::parameter_int("hlc/offset_milliseconds");
        //wait_timeout = cpm::parameter_int("hlc/wait_timeout_milliseconds");
        vehicle_ids = cpm::parameter_ints("vehicle_ids");
    }

    //Get unsigned vehicle ids
    std::vector<uint8_t> unsigned_vehicle_ids;
    for (int vehicle_id : vehicle_ids) {
        if (vehicle_id >= 0 && vehicle_id <= 255) {
            unsigned_vehicle_ids.push_back(static_cast<uint8_t>(vehicle_id));
        }
        else {
            std::cerr << "Incompatible vehicle id" << std::endl;
            cpm::Logging::Instance().write("Incompatible vehicle ids sent - not within 0 and 255");
        }
    }
    if (unsigned_vehicle_ids.size() == 0) {
        std::cerr << "No vehicle ids set!" << std::endl; //TODO LOG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        exit(EXIT_FAILURE);
    }

    //Initialize the timer
    std::cout << "Initializing Timer..." << std::endl;
    std::shared_ptr<cpm::Timer> timer = cpm::Timer::create(node_id, period_nanoseconds, offset_nanoseconds, wait_for_start, simulated_time_allowed, simulated_time);

    //Initialize the communication (TODO later: depending on message type for commands, can change dynamically)
    std::cout << "Initializing Communication..." << std::endl;
    std::shared_ptr<Communication> communication = std::make_shared<Communication>(hlcDomainNumber, hlcStateTopicName, vehicleStateTopicName, hlcTrajectoryTopicName, vehicleTrajectoryTopicName, hlcSpeedCurvatureTopicName, vehicleSpeedCurvatureTopicName, hlcDirectTopicName, vehicleDirectTopicName, vehicleID, timer, unsigned_vehicle_ids);

    //Wait for HLC program to send ready signal
    communication->wait_for_hlc_ready_msg(unsigned_vehicle_ids);

    //Wait for start signal (done by the timer after start)
    //Start the communication with the HLC
    using namespace std::placeholders;
    timer->start_async([&](uint64_t t_now) {
        std::vector<VehicleState> states = communication->getLatestVehicleMessages(t_now);
        std::vector<VehicleObservation> observations = communication->getLatestVehicleObservationMessages(t_now);

        //Transform to VehicleStateList message
        rti::core::vector<VehicleState> rti_states(states);
        rti::core::vector<VehicleObservation> rti_observations(observations);
        VehicleStateList state_list;
        state_list.state_list(rti_states);
        state_list.vehicle_observation_list(rti_observations);
        state_list.t_now(t_now);

        //Send newest vehicle state list to the HLC
        communication->sendToHLC(state_list);

        //Log the received vehicle data size / sample size
        std::stringstream stream;
        stream << "Got latest messages, state array size: " << states.size();
        if (states.size() > 0) {
            stream << " - sample data: " << states.at(0).battery_voltage();
        }
        cpm::Logging::Instance().write(stream.str().c_str());

        //Get the last response time of the HLC
        // Real time -> Print an error message if a period has been missed
        // Simulated time -> Busy waiting until an answer for all connected HLCs (vehicle_ids) has been received
        std::map<uint8_t, uint64_t> lastHLCResponseTimes;
        lastHLCResponseTimes = communication->getLastHLCResponseTimes();

        if (simulated_time) {
            //Wait until any command has been received for all vehicle ids
            bool id_missing = true;
            while(id_missing) {
                id_missing = false;
                for (uint8_t id : unsigned_vehicle_ids) {
                    if (lastHLCResponseTimes.find(id) == lastHLCResponseTimes.end()) {
                        id_missing = true;
                        break;
                    }
                }

                usleep(100000);
                lastHLCResponseTimes = communication->getLastHLCResponseTimes();
            }

            //Check if the last message was received in the current timestep
            bool message_missing = true;
            while (message_missing) {
                message_missing = false;
                for (std::map<uint8_t, uint64_t>::iterator it = lastHLCResponseTimes.begin(); it != lastHLCResponseTimes.end(); ++it){
                    if (it->second != timer->get_time()) {
                        message_missing = true;
                        //break; -> Program does not work for some reason if break is used at this point
                    }
                }

                usleep(100000);
                lastHLCResponseTimes = communication->getLastHLCResponseTimes();
            }
        }
        else {
            //Log error for each HLC if a time step was missed
            for (std::map<uint8_t, uint64_t>::iterator it = lastHLCResponseTimes.begin(); it != lastHLCResponseTimes.end(); ++it) {
                uint64_t passed_time = (t_now - it->second);
                if (passed_time > period_nanoseconds) {
                    std::stringstream stream;
                    stream << "Timestep missed by HLC number " << (it->first) << ", last response: " << (it->second) 
                        << ", current time: " << t_now 
                        << ", periods missed: " << passed_time / period_nanoseconds;
                    cpm::Logging::Instance().write(stream.str().c_str());
                }
            }
            //Also log an error if no HLC data has yet been received
            for (uint8_t id : unsigned_vehicle_ids) {
                if (lastHLCResponseTimes.find(id) == lastHLCResponseTimes.end()) {
                    std::stringstream stream;
                    stream << "HLC number " << id << " has not yet sent any data";
                    cpm::Logging::Instance().write(stream.str().c_str());
                }
            }
        }
    });

    std::cout << "Program started" << std::endl;

    //Stop the program when enter is pressed
    std::cin.get();
    std::cout << "Exiting program" << std::endl;
    timer->stop();
}