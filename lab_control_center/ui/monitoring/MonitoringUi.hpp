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

#include <gtkmm/builder.h>
#include <gtkmm.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <math.h>

#include "TimeSeries.hpp"
#include "defaults.hpp"
#include "cpm/Logging.hpp"
#include "cpm/get_time_ns.hpp"
#include "ui/setup/Deploy.hpp"

#include "TrajectoryInterpolation.hpp"

#include "ui/setup/CrashChecker.hpp"

using VehicleData = map<uint8_t, map<string, shared_ptr<TimeSeries> > >;
using VehicleTrajectories = map<uint8_t, VehicleCommandTrajectory >;

/**
 * \class MonitoringUi
 * \brief UI class for the monitoring part on the bottom of the LCC's UI, below the map view. Shows detailed vehicle information (i.e. battery etc.), RTT info, 
 * currently connected NUCs, reboot options for the NUCs
 * \ingroup lcc_ui
 */
class MonitoringUi
{
public:
    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Box* parent;
    Gtk::Viewport* viewport_monitoring;
    Gtk::Grid* grid_vehicle_monitor;
    Gtk::Box* box_buttons;
    Gtk::Button* button_reset_view;
    Gtk::Label* label_hlc_description_short;
    Gtk::Label* label_hlc_description_long;
    Gtk::Entry* entry_hlc_reboot;
    Gtk::Button* button_hlc_reboot;
    Gtk::Label* label_rtt_hlc_short;
    Gtk::Label* label_rtt_hlc_long;
    Gtk::Label* label_rtt_vehicle_short;
    Gtk::Label* label_rtt_vehicle_long;
    Gtk::Label* label_experiment_time;
    std::shared_ptr<Deploy> deploy_functions;
    std::shared_ptr<CrashChecker> crash_checker;
    std::function<VehicleData()> get_vehicle_data;
    std::function<std::vector<uint8_t>()> get_hlc_data;
    std::function<std::pair<bool, std::map<uint32_t, uint8_t>>()> get_vehicle_to_hlc_mapping;
    std::function<void()> reset_data;
    std::function<VehicleTrajectories()> get_vehicle_trajectory;
    std::function<bool(std::string, uint64_t&, uint64_t&, uint64_t&, double&)> get_rtt_values;
    std::function<void()> kill_deployed_applications; 

    // Before: TimerFD, but this class is stopped by stop signals which might be emitted multiple times by the LCC depending on user interaction
    // Thus: Own timer implementation instead
    void ui_update_loop();
    Glib::Dispatcher update_dispatcher; //to communicate between thread and GUI
    std::thread ui_thread;
    std::atomic_bool run_thread;

    //To measure how long the simulation has been running
    std::atomic_uint64_t sim_start_time;

    //full rows
    const vector<string> rows = { "battery_voltage", "battery_level", "last_msg_state", "clock_delta", "pose_x", "pose_y", "pose_yaw", "ips_x", "ips_y", "ips_yaw", "odometer_distance", "imu_acceleration_forward", "imu_acceleration_left", "speed", "motor_current" };
    //We do not want to show all vehicle information to the user - empty string become empty rows (better formatting)
    const vector<string> rows_restricted = {"battery_level", "ips_dt", "last_msg_state", "clock_delta", "reference_deviation", "speed", "nuc_connected"};

    // Indicates the starting time of the last error occurance for each vehicle
    vector<vector<uint64_t> > error_timestamps{vector<vector<uint64_t> > (rows_restricted.size(), vector<uint64_t>(30,0))};

    // Indicates if an error already triggered a kill 
    vector<vector<bool> > error_triggered{vector<vector<bool> > (rows_restricted.size(), vector<bool>(30,false))};
    
    // Init is also called when the object is constructed. It initializes the ui thread, callbacks etc to update the ui regularly when a new vehicle connects
    void init_ui_thread();

    // This resets the ui thread, to get rid of old vehicle boxes (if the user desires to get rid of them)
    void reset_ui_thread();

    // Called when resetting the ui or when the object gets deleted - kills the currently running thread
    void stop_ui_thread();

    // Stop vehicles and HLCs 
    void stop_experiment(std::vector<unsigned int>, std::vector<uint8_t>);

public:
    explicit MonitoringUi(
        std::shared_ptr<Deploy> deploy_functions_callback,  
        std::function<VehicleData()> get_vehicle_data_callback, 
        std::function<std::vector<uint8_t>()> get_hlc_data_callback,
        std::function<VehicleTrajectories()> get_vehicle_trajectory_command_callback, 
        std::function<void()> reset_data_callback,
        std::function<bool(std::string, uint64_t&, uint64_t&, uint64_t&, double&)> get_rtt_values,
        std::function<void()> kill_deployed_applications_callback 
    );
    ~MonitoringUi();
    Gtk::Box* get_parent();
    void reset_vehicle_view();
    void register_vehicle_to_hlc_mapping(std::function<std::pair<bool, std::map<uint32_t, uint8_t>>()> get_vehicle_to_hlc_mapping);

    /**
     * \brief Checker needs to be set up in SetupView, and SetupView requires access to monitoring, so we have to do this after construction
     */
    void register_crash_checker(std::shared_ptr<CrashChecker> _crash_checker);

    /**
     * \brief Function to call when the simulation starts, to reset data structures, start timers etc
     */
    void notify_sim_start();

    /**
     * \brief Function to call when the simulation stops, to reset data structures, start timers etc
     */
    void notify_sim_stop();
};