#pragma once

#include "cpm/Timer.hpp"
#include "cpm/ParticipantSingleton.hpp"
#include "cpm/Logging.hpp"
#include "ReadyStatus.hpp"
#include "SystemTrigger.hpp"

#include <thread>
#include <string>

#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>

#include "cpm/exceptions.hpp"
#include "cpm/get_time_ns.hpp"

/**
 * \class TimerFD.hpp
 * This class calls a callback function periodically 
 * based the system clock. The calls are synchronized in both frequency 
 * and phase to the clock.
 * This class should be used if a 'simple' timed callback is required. 
 * Whereas Timer.hpp relies on e.g. a parameter server to check if 
 * simulated time is used or not, directly using TimerFD allows to create
 * timed callbacks for methods that use the system clock independent on 
 * the run (real or simulated). This could be GUI tools, periodic tasks etc... 
 */

namespace cpm {

    class TimerFD : public cpm::Timer
    {
        uint64_t period_nanoseconds; 
        uint64_t offset_nanoseconds;
        dds::topic::Topic<ReadyStatus> ready_topic;
        dds::topic::Topic<SystemTrigger> trigger_topic;
        std::string node_id;
        dds::sub::DataReader<SystemTrigger> reader_system_trigger;

        dds::sub::cond::ReadCondition readCondition;
        dds::core::cond::WaitSet waitset;
        
        bool active = false;
        int timer_fd = -1;
        std::thread runner_thread;
        std::function<void(uint64_t t_now)> m_update_callback;


        void wait();
        uint64_t receiveStartTime(); //Bool: true if start signal was received, false if stop signal was received
        bool received_stop_signal ();
        
        const bool wait_for_start; //If false, do not use receiveStartTime()

        void createTimer ();

    public:
        /**
         * \brief Create a "real-time" timer that can be used for function callback
         * \param node_id ID of the timer in the network
         * \param period_nanoseconds The timer is called periodically with a period of period_nanoseconds
         * \param offset_nanoseconds Initial offset (from timestamp 0)
         * \param wait_for_start Set whether the timer is started only if a start signal is sent via DDS (true), or if it should should start immediately (false)
         */
        TimerFD(std::string _node_id, uint64_t period_nanoseconds, uint64_t offset_nanoseconds, bool wait_for_start);
        ~TimerFD();

        /**
         * Start the periodic callback of the callback function in the 
         * calling thread. The thread is blocked until stop() is 
         * called.
         * \param update_callback the callback function
         */
        void start       (std::function<void(uint64_t t_now)> update_callback) override;

        /**
         * Start the periodic callback of the callback function 
         * in a new thread. The calling thread is not blocked.
         * \param update_callback the callback function
         */
        void start_async (std::function<void(uint64_t t_now)> update_callback) override;

        /**
         * \brief Stops the periodic callback and kills the thread (if it was created using start_async).
         */
        void stop() override;

        /**
         * \brief Can be used to obtain the current system time in nanoseconds.
         * \return the current system time in nanoseconds
         */
        uint64_t get_time() override;
    };

}