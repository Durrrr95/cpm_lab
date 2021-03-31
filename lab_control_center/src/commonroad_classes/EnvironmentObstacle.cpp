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

#include "commonroad_classes/EnvironmentObstacle.hpp"

/**
 * \file EnvironmentObstacle.cpp
 * \ingroup lcc_commonroad
 */

EnvironmentObstacle::EnvironmentObstacle(
    const xmlpp::Node* node
    )
{
    //Warn in case node does not have static obstacle role
    //Check if node is of type EnvironmentObstacle
    assert(node->get_name() == "environmentObstacle");
    
    commonroad_line = node->get_line();

    try
    {
        obstacle_type_text = xml_translation::get_child_child_text(node, "type", true).value(); //Must exist, error thrown anyway, so we can use .value() here
        if (obstacle_type_text.compare("unknown") == 0)
        {
            type = ObstacleTypeEnvironment::Unknown;
        }
        else if (obstacle_type_text.compare("building") == 0)
        {
            type = ObstacleTypeEnvironment::Building;
        }
        else if (obstacle_type_text.compare("pillar") == 0)
        {
            type = ObstacleTypeEnvironment::Pillar;
        }
        else if (obstacle_type_text.compare("median_strip") == 0)
        {
            type = ObstacleTypeEnvironment::MedianStrip;
        }
        else
        {
            std::stringstream error_msg_stream;
            error_msg_stream << "Node element not conformant to specs (environment obstacle, obstacleType), line: " << node->get_line();
            throw SpecificationError(error_msg_stream.str());
        }
        
        const auto shape_node = xml_translation::get_child_if_exists(node, "shape", true); //Must exist
        if (shape_node)
        {
            shape = std::optional<Shape>{std::in_place, shape_node};
        }
    }
    catch(const SpecificationError& e)
    {
        throw SpecificationError(std::string("Could not translate EnvironmentObstacle:\n") + e.what());
    }
    catch(...)
    {
        //Propagate error, if any subclass of CommonRoadScenario fails, then the whole translation should fail
        throw;
    }    
}

/******************************Interface functions***********************************/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void EnvironmentObstacle::transform_coordinate_system(double scale, double angle, double translate_x, double translate_y)
{
    if (scale > 0)
    {
        transform_scale *= scale;
    }
    
    if (shape.has_value())
    {
        shape->transform_coordinate_system(scale, angle, 0.0, 0.0); //Shape does not need to be modified as well, because we already transform state/occupancy and initial state position values
    }
}
#pragma GCC diagnostic pop

ObstacleSimulationData EnvironmentObstacle::get_obstacle_simulation_data()
{
    ObstacleSimulationData simulation_data;

    //Add initial point
    ObstacleSimulationSegment initial_point;

    if (shape.has_value())
    {
        initial_point.shape = shape->to_dds_msg();
    }

    simulation_data.trajectory.push_back(initial_point);

    //Translate type to DDS type
    switch(type)
    {
        case ObstacleTypeEnvironment::Building:
            simulation_data.obstacle_type = ObstacleType::Building;
            break;
        case ObstacleTypeEnvironment::Pillar:
            simulation_data.obstacle_type = ObstacleType::Pillar;
            break;
        case ObstacleTypeEnvironment::MedianStrip:
            simulation_data.obstacle_type = ObstacleType::MedianStrip;
            break;
        case ObstacleTypeEnvironment::Unknown:
            simulation_data.obstacle_type = ObstacleType::Unknown;
            break;
    }
    
    return simulation_data;
}

ObstacleTypeEnvironment EnvironmentObstacle::get_type()
{
    return type;
}

std::string EnvironmentObstacle::get_obstacle_type_text()
{
    return obstacle_type_text;
}

const std::optional<Shape>& EnvironmentObstacle::get_shape() const
{
    return shape;
}
