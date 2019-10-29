#pragma once

#include <gtkmm/builder.h>
#include <gtkmm.h>
#include "VehicleManualControl.hpp"
#include "cpm/CommandLineReader.hpp"
#include "ui/file_chooser/FileChooserUI.hpp"
#include "ui/timer/TimerViewUI.hpp"
#include "ui/setup/VehicleToggle.hpp"

//For popen
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <array>
#include <vector>

class SetupViewUI
{
private:
    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::Widget* parent = nullptr;

    //Script and script parameters
    Gtk::Entry* script_path = nullptr;
    Gtk::Entry* script_params = nullptr;
    Gtk::Button* button_choose_script = nullptr;
    
    //Vehicle selection
    Gtk::Button* button_select_none = nullptr;
    Gtk::Button* button_select_all_simulated = nullptr;
    Gtk::Button* button_select_all_real = nullptr;

    //Set timer (simulated or real time)
    Gtk::Switch* switch_simulated_time = nullptr;
    
    //(De)Activate Middleware
    Gtk::Switch* switch_launch_middleware = nullptr;

    //Start / stop simulation
    Gtk::Button* button_deploy = nullptr;
    Gtk::Button* button_kill = nullptr;

    //Vehicles
    Gtk::FlowBox* vehicle_flowbox = nullptr;
    std::vector<std::shared_ptr<VehicleToggle>> vehicle_toggles;

    //Timer function - replace current timer in the whole system when user switches between simulated and real time
    std::shared_ptr<TimerViewUI> timer_ui;
    void switch_timer_set();

    //Overall deploy functions
    void deploy_applications();
    void kill_deployed_applications();

    //Specific deploy functions
    void deploy_hlc_scripts();
    void deploy_middleware();
    void deploy_sim_vehicles();
    void deploy_sim_vehicle(unsigned int id);

    void kill_hlc_scripts();
    void kill_middleware();
    void kill_vehicles();
    void kill_vehicle(unsigned int id);

    std::vector<unsigned int> get_active_vehicle_ids();
    std::vector<unsigned int> get_vehicle_ids_realtime();
    std::vector<unsigned int> get_vehicle_ids_simulated();

    //UI functions
    void set_sensitive(bool is_sensitive);

    //Function to execute a shell command
    std::string execute_command(const char* cmd);

    //Set command line parameters
    bool cmd_simulated_time;
    unsigned int cmd_domain_id;
    std::string cmd_dds_initial_peer;

    //File chooser to select script(s) + location
    void open_file_explorer();
    void file_explorer_callback(std::string file_string, bool has_file);
    std::shared_ptr<FileChooserUI> file_chooser_window;

    //Vehicle button toggles
    void select_all_vehicles_real();
    void select_all_vehicles_sim();
    void select_no_vehicles();

    //Helper functions
    void get_path_name(std::string& in, std::string& out_path, std::string& out_name);

public:
    SetupViewUI(std::shared_ptr<TimerViewUI> _timer_ui, unsigned int argc, char *argv[]);
    ~SetupViewUI();

    Gtk::Widget* get_parent();
};