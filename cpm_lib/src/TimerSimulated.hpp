#pragma once

#include "cpm/Timer.hpp"
#include "cpm/ParticipantSingleton.hpp"
#include "ReadyStatus.hpp"
#include "SystemTrigger.hpp"

#include "ErrorTimerStart.hpp"

#include <thread>
#include <string>

#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>

class TimerSimulated : public cpm::Timer
{
    uint64_t period_nanoseconds; 
    uint64_t offset_nanoseconds;
    dds::topic::Topic<ReadyStatus> ready_topic;
    dds::topic::Topic<SystemTrigger> trigger_topic;
    dds::pub::DataWriter<ReadyStatus> writer;
    dds::sub::DataReader<SystemTrigger> reader;
    std::string node_id;
    uint64_t period_number;
    uint64_t current_time;

    bool active = false;

    std::thread runner_thread;
    std::function<void(uint64_t t_now)> m_update_callback;

    dds::core::cond::WaitSet waitset;

    void wait();

public:
    TimerSimulated(std::string _node_id, uint64_t period_nanoseconds, uint64_t offset_nanoseconds);
    ~TimerSimulated();

    void start       (std::function<void(uint64_t t_now)> update_callback) override;
    void start_async (std::function<void(uint64_t t_now)> update_callback) override;
    void stop() override;
    uint64_t get_time() override;
};