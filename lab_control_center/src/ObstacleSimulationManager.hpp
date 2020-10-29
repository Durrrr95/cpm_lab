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

#include "commonroad_classes/CommonRoadScenario.hpp"
#include "commonroad_classes/ObstacleSimulationData.hpp"

#include "ObstacleSimulation.hpp"

#include "cpm/Timer.hpp"
#include "cpm/ParticipantSingleton.hpp"
#include "cpm/get_topic.hpp"
#include "CommonroadObstacleList.hpp"
#include "VehicleCommandTrajectory.hpp"

#include "ui/commonroad/ObstacleToggle.hpp" //For callback from vehicle toggle: Need enum defined here

#include <map>

/**
 * \brief This class simulates a traffic participant / obstacle logic based on the obstacle type(s) defined in a commonroad scenario
 * It sends trajectories/... defined in the scenario (which may define position, time, velocity...)
 * These are received by either a real vehicle or a special simulated participant, that also gets a starting position etc
 */
class ObstacleSimulationManager
{
private:
    std::shared_ptr<CommonRoadScenario> scenario; //Data object that can be used to access the obstacle's trajectories

    std::map<int, ObstacleSimulation> simulated_obstacles; //Store obstacles by ID
    //Simulation state - relevant to decide which data to send for this obstacle (DDS Obstacle / Trajectory / Nothing)
    std::map<int, ObstacleToggle::ToggleState> simulated_obstacle_states;
    std::mutex map_mutex;

    //Timing
    bool use_simulated_time;
    std::string node_id;
    uint64_t dt_nanos;
    uint64_t time_step_size; //Defined by Commonroad scenario, here translated from seconds to nanoseconds
    std::shared_ptr<cpm::Timer> simulation_timer;
    std::shared_ptr<cpm::SimpleTimer> standby_timer;

    //DDS
    dds::pub::DataWriter<CommonroadObstacleList> writer_commonroad_obstacle;
    dds::pub::DataWriter<VehicleCommandTrajectory> writer_vehicle_trajectory;

    /**
     * \brief Function that sets up the obstacle simulation based on the currently set scenario (callback for scenario)
     */
    void setup();

    /**
     * \brief Reset the simulation (callback for scenario)
     */
    void reset();

    //Stop cpm timers, if they are running
    void stop_timers();

    /**
     * \brief Create an obstacle simulation object given obstacle simulation data
     * \param id ID (set in commonroad scenario) of the obstacle
     * \param data An obstacle simulation data object containing all information relevant for simulating the translated object
     */
    void create_obstacle_simulation(int id, ObstacleSimulationData& data);

    //Send initial state of all simulation objects (when sim. is not running, to show initial position in MapView)
    void send_init_states();

    /**
     * \brief Compute next states of commonroad obstacles based on the current time and return them; only consider obstacles that are supposed to be simulated by the LCC
     * \param t_now Current time
     * \param start_time Time the simulation was started, to compute diff to t_now
     */
    std::vector<CommonroadObstacle> compute_all_next_states(uint64_t t_now, uint64_t start_time);

    //Either returns the content of the map or the default value (simulated); does not lock, so lock before calling!
    ObstacleToggle::ToggleState get_obstacle_simulation_state(int id);

public:
    /**
     * \brief Constructor to set up the simulation object
     * \param _scenario Data object to get the obstacle's data
     */
    ObstacleSimulationManager(std::shared_ptr<CommonRoadScenario> _scenario, bool use_simulated_time);

    //Destructor for threads & timer
    ~ObstacleSimulationManager();

    /**
     * \brief Scales the time used in this scenario by scale to a value in nanoseconds
     * \param scale The scale factor
     */
    void set_time_scale(double scale);

    /**
     * \brief Start the simulation (callback for UI)
     */
    void start();

    /**
     * \brief Stop the simulation (callback for UI)
     */
    void stop();

    /**
     * \brief Set the simulation state (off, visualized/simulated, trajectory) for an obstacle (default is simulated)
     * \param id ID of the obstacle in commonroad
     * \param state New state of the simulation
     */
    void set_obstacle_simulation_state(int id, ObstacleToggle::ToggleState state);
};