#include "LogStorage.hpp"

using namespace std::placeholders;
LogStorage::LogStorage() :
    /*Set up communication*/
    log_reader(std::bind(&LogStorage::log_callback, this, _1), cpm::ParticipantSingleton::Instance(), cpm::get_topic<Log>("log"), true)
{    
    file.open(filename, std::ofstream::out | std::ofstream::trunc);
    file << "ID,Timestamp,Content" << std::endl;
}

LogStorage::~LogStorage()
{
    //Close the file in the destructor
    file.close();
}

void LogStorage::log_callback(dds::sub::LoanedSamples<Log>& samples) { 
    std::lock_guard<std::mutex> lock_1(log_storage_mutex);
    std::lock_guard<std::mutex> lock_2(log_buffer_mutex); 

    for (auto sample : samples) {
        if (sample.info().valid()) {
            log_storage.push_back(sample.data());
            log_buffer.push_back(sample.data());

            //Write logs immediately to csv file (taken from cpm library)
            //For the log file: csv, so escape '"'
            std::string str = sample.data().content();
            std::string log_string = std::string(str);
            std::string escaped_quote = std::string("\"\"");
            size_t pos = 0;
            while ((pos = log_string.find('"', pos)) != std::string::npos) {
                log_string.replace(pos, 1, escaped_quote);
                pos += escaped_quote.size();
            }
            //Also put the whole string in quotes
            log_string.insert(0, "\"");
            log_string += "\"";

            //Mutex for writing the message (file, writer) - is released when going out of scope
            std::lock_guard<std::mutex> lock(file_mutex);

            //Add the message to the log file
            file << sample.data().id() << "," << sample.data().stamp().nanoseconds() << "," << log_string << std::endl;
        }
    }

    //Clear storage and buffer when some max size was reached - keep last elements
    keep_last_elements(log_storage, 10000);
    keep_last_elements(log_buffer, 100);
}

std::vector<Log> LogStorage::get_new_logs() {
    std::lock_guard<std::mutex> lock(log_buffer_mutex);
    std::vector<Log> log_copy = log_buffer;
    log_buffer.clear();
    return log_copy;
}

std::vector<Log> LogStorage::get_all_logs() {
    std::lock_guard<std::mutex> lock(log_storage_mutex);
    std::vector<Log> log_copy = log_storage;
    return log_copy;
}

std::vector<Log> LogStorage::perform_abortable_search(std::string filter_value, FilterType filter_type, std::atomic_bool &continue_search) {
    //Copy log_storage and perform search on copy only
    std::unique_lock<std::mutex> lock(log_storage_mutex);
    std::vector<Log> log_storage_copy = std::vector<Log>(log_storage);
    lock.unlock();

    //Result vector
    std::vector<Log> search_result;

    try {
        std::regex search_regex(filter_value);

        for (auto iterator = log_storage_copy.begin(); iterator != log_storage_copy.end(); ++iterator) {
            if (!continue_search.load()) {
                //Return empty result list if the search was aborted
                search_result.clear();
                return search_result;
            }

            std::stringstream stream;
            std::string filter_by_text;
            switch (filter_type) {
                case ID:
                    filter_by_text = iterator->id();
                    break;
                case Content:
                    filter_by_text = iterator->content();
                    break;
                case Timestamp:
                    stream << iterator->stamp().nanoseconds();
                    filter_by_text = stream.str();
                    break;
                default:
                    stream << iterator->id() << iterator->content() << iterator->stamp().nanoseconds();
                    filter_by_text = stream.str();
                    break;
            }

            if (std::regex_search(filter_by_text, search_regex)) {
                search_result.emplace_back(*iterator);
            }

            // if (filter_by_text.find(filter_value) != string::npos) {
            //     search_result.emplace_back(*iterator);
            // }
        }
    }
    catch (std::regex_error& e) {
        std::cout << "Regex error (due to filter string): " << e.what() << std::endl;
        search_result.push_back(Log("", "Wrong regex expression!", TimeStamp(0)));
        search_result.push_back(Log("", "Regex search example: (vehicle)(.*)", TimeStamp(0)));
        search_result.push_back(Log("", "Reference: https://en.cppreference.com/w/cpp/regex/ecmascript", TimeStamp(0)));
    }

    if (search_result.size() == 0 && log_storage_copy.size() > 0) {
        search_result.push_back(Log("", "No matching string was found", TimeStamp(0)));
        search_result.push_back(Log("", "Regex search example: (vehicle)(.*)", TimeStamp(0)));
        search_result.push_back(Log("", "Reference: https://en.cppreference.com/w/cpp/regex/ecmascript", TimeStamp(0)));
    }

    keep_last_elements(search_result, 100);

    return search_result;
}

void LogStorage::keep_last_elements(std::vector<Log>& vector, size_t count) {
    //Does not use lock() because it is supposed to be called from a function where lock() has been called before
    if (vector.size() > count) {
        vector.erase(vector.begin(), vector.end() - count);
    }
}

void LogStorage::reset() 
{
    std::unique_lock<std::mutex> lock(log_storage_mutex);
    std::unique_lock<std::mutex> lock_2(log_buffer_mutex);
    log_storage.clear();
    log_buffer.clear();
}