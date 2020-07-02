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

#include "ObstacleSimulationManager.hpp"

ObstacleSimulationManager::ObstacleSimulationManager(std::shared_ptr<CommonRoadScenario> _scenario, bool _use_simulated_time) 
:
scenario(_scenario),
use_simulated_time(_use_simulated_time)
{
    //Warning: Do not set up the simulation manager when multiple threads are already running, or you risk that during construction the scenario gets changed!
    //The scenario callbacks are always called with locked mutexes within the scenario, so there is no need to worry about them
    setup();

    //Register at scenario to get reset when a new scenario is loaded
    scenario->register_obstacle_sim(
        [=] ()
        {
            setup();
        },
        [=] ()
        {
            reset();
        }
    );
}

void ObstacleSimulationManager::setup()
{
    //Must be defined within the scenario according to specs - gives value (in seconds) of one time step unit
    double time_step_size = scenario->get_time_step_size();

    //Set up simulated obstacles
    for (auto obstacle_id : scenario->get_dynamic_obstacle_ids())
    {
        auto trajectory = scenario->get_dynamic_obstacle(obstacle_id).value().get_trajectory();
        //We need to modify the trajectory first: Lanelet refs need to be translated to a trajectory
        for (auto& point : trajectory)
        {
            if (point.lanelet_ref.has_value())
            {
                //Translate lanelet ref to positional value
                point.position = scenario->get_lanelet(point.lanelet_ref.value()).value().get_center();
            }
        }

        simulated_obstacles.push_back(
            ObstacleSimulation(trajectory, time_step_size, obstacle_id, use_simulated_time)
        );
    }

    for (auto& simulated_obstacle : simulated_obstacles)
    {
        simulated_obstacle.send_init_state();
    }

    //TODO: Part for real participant: Send trajectory
    //TODO: Put more information in trajectory: Need to know if they are based on exact or inexact values (IntervalOrExact) for visualization
}

//Suppress warning for unused parameter
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void ObstacleSimulationManager::set_time_scale(double scale)
{
    //TODO: If desired, adjust time scale
}
#pragma GCC diagnostic pop

void ObstacleSimulationManager::start()
{
    for (auto& entry : simulated_obstacles)
    {
        entry.start();
    }
}

void ObstacleSimulationManager::stop()
{
    for (auto& entry : simulated_obstacles)
    {
        entry.reset();
    }
}

void ObstacleSimulationManager::reset()
{
    for (auto& entry : simulated_obstacles)
    {
        entry.reset();
    }

    simulated_obstacles.clear();
}