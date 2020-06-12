#pragma once

/**
 * \brief Namespace for deploy / kill templates (bash commands)
 * Was removed from SetupViewUI because this function should in our opinion not be responsible for these actions
 */

#include <atomic>
#include <array>
#include <chrono>       //For time measurements (timeout for remote deployment)
#include <cstdio>       //For popen
#include <functional>
#include <fstream>
#include <iomanip>      // put_time
#include <iostream>
#include <memory>
#include <regex>        // to replace file contents
#include <stdexcept>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

//To spawn a process & get its PID
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class Deploy 
{
public:
    /**
     * \brief Constructor that sets variables for the deploy functions that do not change during execution
     * \param _cmd_domain_id domain ID set in the command line (when starting the LCC)
     * \param _cmd_dds_initial_peer dds initial peer set in the command line (when starting the LCC)
     * \param _stop_vehicle Callback function to make the vehicle stop immediately (given a vehicle ID)
     */
    Deploy(unsigned int _cmd_domain_id, std::string _cmd_dds_initial_peer, std::function<void(uint8_t)> _stop_vehicle);

    //Specific local deploy functions
    /**
     * \brief Start the middleware and the given Matlab/C++ script using tmux and a system call; pass parameters: domain id, initial peer, simulated time, vehicle id
     * \param use_simulated_time Whether simulated time or real time shall be used for the lab run
     * \param active_vehicle_ids All vehicle IDs that are in use in this run, no matter if real or simulated
     * \param script_path Path to the script, including the script name (and possible file ending)
     * \param script_params Additional script parameters
     */
    void deploy_local_hlc(bool use_simulated_time, std::vector<unsigned int> active_vehicle_ids, std::string script_path, std::string script_params);

    /**
     * \brief Deploy all vehicles that were set to be simulated locally, set simulated time (software is started using tmux)
     */
    void deploy_sim_vehicles(std::vector<unsigned int> simulated_vehicle_ids, bool use_simulated_time);
    /**
     * \brief Deploy a single simulated vehicle by passing its ID
     */
    void deploy_sim_vehicle(unsigned int id, bool use_simulated_time);

    //Local kill functions: Kill middleware, script and vehicles using their tmux ID 
    void kill_local_hlc();
    void kill_vehicles(std::vector<unsigned int> simulated_vehicle_ids, std::vector<unsigned int> active_vehicle_ids);
    void kill_vehicle(unsigned int id);

    //Deploy and kill the IPS (for position tracking of the real vehicles)
    void deploy_ips();
    void kill_ips();

    bool diagnosis_switch = true; 
    //Deploy and kill the rtirecordingservice
    void deploy_recording();
    void kill_recording();

    //Specific remote deploy functions
    /**
     * \brief Deploy the script + middleware specified in the UI with the given parameters on the HLC with the given ID. 
     * The script is responsible for one or multiple vehicle ids. The script and all data in the same folder gets copied to the HLC and is then called there.
     * \param hlc_id The ID of the HLC on which the software shall be deployed -> used to get the IP address
     * \param vehicle_ids One or multiple vehicle IDs, comma-separated, as string - to be passed to the middleware and the script
     * \param use_simulated_time Whether simulated time or real time shall be used for the lab run
     * \param script_path Path to the script, including the script name (and possible file ending)
     * \param script_params Additional script parameters
     * \param timeout_seconds Time to wait until the exection is aborted
     * \param is_online Function to check whether the HLC on which to deploy is still online (else: abort early)
     * \return True if the execution did not have to be aborted and no process-related error occured, false otherwise
     */
    bool deploy_remote_hlc(unsigned int hlc_id, std::string vehicle_ids, bool use_simulated_time, std::string script_path, std::string script_params, unsigned int timeout_seconds, std::function<bool()> is_online);
    /**
     * \brief Kill the script + middleware on the given HLC (again determine the IP from the HLC ID)
     * \param hlc_id ID of the HLC on which to kill the programs
     * \param timeout_seconds Timeout in seconds until the kill process should be terminated
     * \param is_online Function to check whether the HLC on which to deploy is still online (else: abort early)
     * \return True if the execution (of the bash script) did not have to be aborted and no process-related error occured, false otherwise
     */
    bool kill_remote_hlc(unsigned int hlc_id, unsigned int timeout_seconds, std::function<bool()> is_online);

    /**
     * \brief Used to create the folder ~/dev/name, in which logs of local tmux sessions started here are stored (for debugging purposes)
     * \param name name of the log folder
     */
    void create_log_folder(std::string name);

private:
    //Used for process forking
    enum PROCESS_STATE {DONE, RUNNING, ERROR};

    //These values/functions are set once at startup (as command line parameters), never change and are thus stored in this class
    unsigned int cmd_domain_id; 
    std::string cmd_dds_initial_peer;

    //Callback function to send a vehicle stop signal / control to the specified vehicle
    std::function<void(uint8_t)> stop_vehicle;

    //Helper functions
    /**
     * \brief Get only the path of the given script, and its name name
     * \param in string containing the path and name of the scrip in the form script_path/script_name
     * \param out_path string containing only script_path
     * \param out_name string containing only script_name
     */
    void get_path_name(std::string& in, std::string& out_path, std::string& out_name);

    /**
     * \brief Check if the given tmux session already exists - used to kill left-over sessions
     * \return True if the session exists, false otherwise
     */
    bool session_exists(std::string session_id);

    /**
     * \brief Kill a tmux session with the given session_id - only if it exists (uses session_exists)
     */
    void kill_session(std::string session_id);

    /**
     * \brief Convert boolean to string - used for command line parameters (for deployment)
     * \return String version of the boolean (true -> "true", false -> "false")
     */
    std::string bool_to_string(bool var);

    /**
     * \brief Function to execute a shell command and get its output
     * \return Output of the shell command
     */
    std::string execute_command(const char* cmd);

    /**
     * \brief Creates a command and manages it until it finished or a timeout occured or the HLC is no longer online; uses the three functions below
     * \param cmd Command string to be executed
     * \param timeout_seconds Timout until the process termination is forced
     * \param is_online Function to check whether the HLC on which to deploy is still online (else: abort early)
     * \return True if the execution (of the bash script) did not have to be aborted and no process-related error occured, false otherwise 
     */
    bool spawn_and_manage_process(const char* cmd, unsigned int timeout_seconds, std::function<bool()> is_online);

    /**
     * \brief Function to execute a shell command that returns the processes PID, so that the process can be controlled / monitored further
     * \return Output of the shell command
     */
    int execute_command_get_pid(const char* cmd);

    /**
     * \brief Function to find out which state a process spawned before is currently in
     * \param process_id The process id of the child process that was spawned before
     * \return The current state of the process
     */
    PROCESS_STATE get_child_process_state(int process_id);

    /**
     * \brief Kill a process - first allow it to terminate gracefully, else force-kill it
     * \param process_id The ID of the process
     */
    void kill_process(int process_id);

    // Session name for recording service
    std::string recording_session = "dds_record";
};