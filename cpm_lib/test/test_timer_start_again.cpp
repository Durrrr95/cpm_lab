#include "catch.hpp"
#include "TimerFD.hpp"
#include "ErrorTimerStart.hpp"
#include <unistd.h>

#include <thread>

#include "cpm/ParticipantSingleton.hpp"
#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <dds/topic/ddstopic.hpp>
#include "ReadyStatus.hpp"
#include "SystemTrigger.hpp"

TEST_CASE( "TimerFD_start_again" ) {

    std::cout << "Starting TimerFD start-again test" << std::endl;

    const uint64_t period = 21000000;
    const uint64_t offset =  0; //This time check offset = 0

    int count = 0;
    uint64_t t_start_prev = 0;
    bool was_stopped = false;

    TimerFD timer("2", period, offset, true);

    //Starting time to check for:
    uint64_t starting_time = timer.get_time() + 2000000000;

    //Reader / Writer for ready status and system trigger
    dds::pub::DataWriter<SystemTrigger> writer(dds::pub::Publisher(cpm::ParticipantSingleton::Instance()),          
        dds::topic::find<dds::topic::Topic<SystemTrigger>>(cpm::ParticipantSingleton::Instance(), "system_trigger"), 
        (dds::pub::qos::DataWriterQos() << dds::core::policy::Reliability::Reliable()));
    dds::sub::DataReader<ReadyStatus> reader(dds::sub::Subscriber(cpm::ParticipantSingleton::Instance()), 
        dds::topic::find<dds::topic::Topic<ReadyStatus>>(cpm::ParticipantSingleton::Instance(), "ready"), 
        (dds::sub::qos::DataReaderQos() << dds::core::policy::Reliability::Reliable()));
    // Create a WaitSet
    dds::core::cond::WaitSet waitset;
    // Create a ReadCondition for a reader with a specific DataState
    dds::sub::cond::ReadCondition read_cond(
        reader, dds::sub::status::DataState::any());
    // Attach conditions
    waitset += read_cond;

    //Thread for start signal
    std::thread signal_thread = std::thread([&](){
        std::cout << "TimerFD: Receiving ready signal..." << std::endl;

        //Wait for ready signal
        ReadyStatus status;
        dds::core::cond::WaitSet::ConditionSeq active_conditions = waitset.wait();
        for (auto sample : reader.take()) {
            if (sample.info().valid()) {
                status.next_start_stamp(sample.data().next_start_stamp());
                status.source_id(sample.data().source_id());
                break;
            }
        }
        CHECK(status.source_id() == "2");
        CHECK(status.next_start_stamp().nanoseconds() == 0);

        std::cout << "TimerFD: Received ready signal: " << status.source_id() << " " << status.next_start_stamp() << std::endl;

        //Send start signal
        SystemTrigger trigger;
        trigger.next_start(TimeStamp(starting_time));
        writer.write(trigger);
    });

    //Check if start_async works as expected as well
    timer.start_async([&](uint64_t t_start){
        uint64_t now = timer.get_time();

        CHECK( now >= starting_time + 1 );
        CHECK( t_start <= now );
        CHECK( now <= t_start + 1000000 ); // actual start time is within 1 ms of declared start time
        CHECK( t_start % period == offset ); // start time corresponds to timer definition
    });

    //Check that the timer cannot be used while it is running (Use return codes here?)
    usleep(1000000);
    std::cout << "Starting the timer again" << std::endl;
    
    bool exception_called = false;
    try {
        timer.start([](uint64_t t_start) {});
    }
    catch (ErrorTimerStart &e) {
        exception_called = true;
    }
    CHECK(exception_called);

    exception_called = false;
    try {
        timer.start_async([](uint64_t t_start) {});
    }
    catch (ErrorTimerStart &e) {
        exception_called = true;
    }
    CHECK(exception_called);

    //Wait for above stuff to finish
    usleep(3000000);
    if (signal_thread.joinable()) {
        signal_thread.join();
    }
    timer.stop();
}
