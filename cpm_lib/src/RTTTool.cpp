#include "cpm/RTTTool.hpp"

cpm::RTTTool::RTTTool() : 
    rtt_topic(cpm::get_topic<RoundTripTime>(cpm::ParticipantSingleton::Instance(), "round_trip_time")),
    rtt_writer(dds::pub::Publisher(cpm::ParticipantSingleton::Instance()), rtt_topic)
{
    rtt_measurement_active.store(false);
    rtt_measure_requested.store(false);
    rtt_count.store(0);

    //Create reader that directly answers the messages and stores times of RTT answers
    rtt_reader = std::make_shared<cpm::AsyncReader<RoundTripTime>>(
        [this](dds::sub::LoanedSamples<RoundTripTime>& samples){
            //Ignore any message if the RTT tool was not activated by the user of the library
            if(!rtt_measurement_active.load())
                return;

            for(auto sample : samples)
            {
                if(sample.info().valid())
                {
                    auto id = sample.data().source_id();

                    //Only answer to the RTT request if the program ID is different from the own program ID
                    if (!(sample.data().is_answer()) && id != program_id)
                    {
                        RoundTripTime answer;
                        answer.count(sample.data().count());
                        answer.is_answer(true);
                        answer.source_id(program_id);
                        rtt_writer.write(answer);
                    }
                    else if (sample.data().is_answer() && rtt_measure_requested.load() && sample.data().count() == rtt_count.load())
                    {
                        //Store RTT answers if they were requested and match the current request count
                        std::lock_guard<std::mutex> lock(receive_times_mutex);
                        if (receive_times.find(id) == receive_times.end())
                        {
                            receive_times[id] = std::vector<uint64_t>();
                        }
                        receive_times[id].push_back(cpm::get_time_ns());
                    }
                }
            }
        },
        cpm::ParticipantSingleton::Instance(),
        rtt_topic,
        false,
        false
    );
}

cpm::RTTTool& cpm::RTTTool::Instance()
{
    static RTTTool instance;
    return instance;
}

void cpm::RTTTool::activate(std::string _program_id)
{
    program_id = _program_id;
    rtt_measurement_active.store(true);
}


std::map<std::string, std::pair<uint64_t, uint64_t>> cpm::RTTTool::measure_rtt()
{
    std::map<std::string, std::pair<uint64_t, uint64_t>> results;

    //Return (0, 0) if the measurement was not activated
    if(!rtt_measurement_active.load())
        return results;

    //First, increment the RTT count, so that old messages are discarded properly
    auto current_count = rtt_count.load();
    if (current_count < 255)
    {
        ++current_count;
    }
    else
    {
        current_count = 0;
    }
    rtt_count.store(current_count);
    

    //Then, clear the vector containing old answers (and hope that no old messages are still being sent)
    std::unique_lock<std::mutex> lock(receive_times_mutex);
    receive_times.clear();
    lock.unlock();

    RoundTripTime request;
    request.is_answer(false);
    request.source_id(program_id);
    request.count(current_count);

    rtt_measure_requested.store(true);
    auto start_time = cpm::get_time_ns();
    rtt_writer.write(request);

    //Wait for answers to appear in the RTT times vector
    unsigned int wait_count = 0; //Do not wait longer than 2 seconds
    lock.lock();
    while (receive_times.size() == 0)
    {
        lock.unlock();

        if (wait_count == 10)
        {
            break;
        }
        ++ wait_count;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        lock.lock();
    }

    //Check if the 2 second timeout was reached before any message was received
    if (wait_count < 10)
    {
        //Also wait another 200ms to get a worse RTT time
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        //Calculate values for each id
        for (auto& entry : receive_times)
        {
            //If an entry exists, it must have at least one member in its vector (because of the way the creation of entries is handled in the async reader)
            assert(entry.second.size() > 0);

            //Now that we got answers, calculate the 'fastest' RTT - due to the saving order, this is stored in the first entry of the vector
            auto fastest_answer = entry.second.at(0);
            
            auto amnt_answers = entry.second.size();
            auto slowest_answer = entry.second.at(amnt_answers - 1); //Could be the same as the fastest one

            //Now calculate 'best' and 'worst' RTT
            auto best_rtt = fastest_answer - start_time;
            auto worst_rtt = slowest_answer - start_time;

            //Consider who the best / worst RTT belongs to
            results[entry.first] = {best_rtt, worst_rtt};
        }
    }

    lock.unlock();
        
    return results;
}