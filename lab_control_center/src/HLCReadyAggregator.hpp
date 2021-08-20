// MIT License
// 
// Copyright (c) 2020 Lehrstuhl Informatik 11 - RWTH Aachen University
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// This file is part of cpm_lab.
// 
// Author: i11 - Embedded Software, RWTH Aachen University

#pragma once

#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "HLCHello.hpp"

#include "cpm/Logging.hpp"
#include "cpm/ParticipantSingleton.hpp"
#include "cpm/get_topic.hpp"
#include "cpm/get_time_ns.hpp"
#include "cpm/AsyncReader.hpp"

/**
 * \brief This class collects all the HLCHello messages sent by the HLCs (NUCs) to present the GUI user the NUCs (/IDs) that are currently online
 * This class can also be used to just retrieve the currently available IDs for distribution of scripts on several NUCs
 * Furthermore, the messages also include information about the currently running script & middleware (if running or not), which can also be retreived
 * \ingroup lcc
 */
class HLCReadyAggregator
{
private:
    //Reader, mutex and list to get and store a list of currently online HLCs
    //A map is used because each ID has a time to live, which is updated whenever a new sample with this ID is received - this is supposed to handle NUC crashes (-> not shown to be online anymore)
    //! Reader to get the currently online HLCs
    cpm::AsyncReader<HLCHello> async_hlc_reader;
    //! Mutex for the maps storing which hlc is online and which is running script / middleware
    std::mutex hlc_list_mutex;
    //! Map to store when the last online message from an HLC was received
    std::map<uint8_t, uint64_t> hlc_map;
    //! Map to store if the script was running on the HLC in the last message
    std::map<uint8_t, bool> hlc_script_running;
    //! Map to store if the middleware was running on the HLC in the last message
    std::map<uint8_t, bool> hlc_middleware_running;

    //! The HLCs send a signal every second, so they are probably offline if no signal was received within 3 seconds
    const uint64_t time_to_live_ns = 3000000000;

public:
    HLCReadyAggregator();

    ~HLCReadyAggregator() {
        std::cout << "!!! --- HLCReadyAggregator destructor" << std::endl;
    }

    /**
     * \brief Gets currently online NUCs and checks if a NUC has crashed based on currently stored HLC entries and time to live
     * \return NUC IDs as strings
     */
    std::vector<std::string> get_hlc_ids_string();

    /**
     * \brief Gets currently online NUCs and checks if a NUC has crashed based on currently stored HLC entries and time to live
     * \return NUC IDs as uint8_t
     */
    std::vector<uint8_t> get_hlc_ids_uint8_t();

    /**
     * \brief Check if the script that was uploaded is currently running on the HLC
     * \param hlc_id HLC for which to check if the script is still running
     * \return True if the script is currently running, else false
     */
    bool script_running_on(uint8_t hlc_id);

    /**
     * \brief Check if the middleware that was uploaded is currently running on the HLC
     * \param hlc_id HLC for which to check if the middleware is still running
     * \return True if the middleware is currently running, else false
     */
    bool middleware_running_on(uint8_t hlc_id);
};