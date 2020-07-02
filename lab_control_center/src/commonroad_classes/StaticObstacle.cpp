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

#include "commonroad_classes/StaticObstacle.hpp"

StaticObstacle::StaticObstacle(const xmlpp::Node* node)
{
    //TODO: Warn in case node does not have static obstacle role
    //Check if node is of type staticObstacle
    assert(node->get_name() == "staticObstacle" || node->get_name() == "obstacle");
    if (node->get_name() == "obstacle") //2018 specs
    {
        std::string role_text = xml_translation::get_child_child_text(node, "role", true).value(); //Must exist (2018 specs), else error is thrown
        assert(role_text.compare("static") == 0);
    }
    
    try
    {
        obstacle_type_text = xml_translation::get_child_child_text(node, "type", true).value(); //Must exist, error thrown anyway, so we can use .value() here
        if (obstacle_type_text.compare("unknown") == 0)
        {
            type = ObstacleTypeStatic::Unknown;
        }
        else if (obstacle_type_text.compare("parkedVehicle") == 0)
        {
            type = ObstacleTypeStatic::ParkedVehicle;
        }
        else if (obstacle_type_text.compare("constructionZone") == 0)
        {
            type = ObstacleTypeStatic::ConstructionZone;
        }
        else if (obstacle_type_text.compare("roadBoundary") == 0)
        {
            type = ObstacleTypeStatic::RoadBoundary;
        }
        else if (obstacle_type_text.compare("car") == 0 || 
            obstacle_type_text.compare("truck") == 0 || 
            obstacle_type_text.compare("bus") == 0 || 
            obstacle_type_text.compare("motorcycle") == 0 || 
            obstacle_type_text.compare("bicycle") == 0 || 
            obstacle_type_text.compare("pedestrian") == 0 || 
            obstacle_type_text.compare("priorityVehicle") == 0 || 
            obstacle_type_text.compare("train") == 0)
        {
            //Behavior for dynamic types, which should not be used here
            std::stringstream error_msg_stream;
            error_msg_stream << "Node element not conformant to specs - usage of dynamic type for static object (obstacleType), line: " << node->get_line();
            throw SpecificationError(error_msg_stream.str());
        }
        else
        {
            std::stringstream error_msg_stream;
            error_msg_stream << "Node element not conformant to specs (static obstacle, obstacleType), line: " << node->get_line();
            throw SpecificationError(error_msg_stream.str());
        }
        
        const auto shape_node = xml_translation::get_child_if_exists(node, "shape", true); //Must exist
        if (shape_node)
        {
            shape = std::optional<Shape>{std::in_place, shape_node};
        }

        const auto state_node = xml_translation::get_child_if_exists(node, "initialState", true); //Must exist
        if (state_node)
        {
            initial_state = std::optional<State>{std::in_place, state_node};
        }

        const auto trajectory_node = xml_translation::get_child_if_exists(node, "trajectory", false); //Must not exist
        const auto occupancy_node = xml_translation::get_child_if_exists(node, "occupancySet", false); //Must not exist
        if (trajectory_node || occupancy_node)
        {
            std::stringstream error_msg_stream;
            error_msg_stream << "Trajectory / occupancy defined for static object (not allowed), line: " << node->get_line();
            throw SpecificationError(error_msg_stream.str());
        }
    }
    catch(const SpecificationError& e)
    {
        throw SpecificationError(std::string("Could not translate StaticObstacle:\n") + e.what());
    }
    catch(...)
    {
        //Propagate error, if any subclass of CommonRoadScenario fails, then the whole translation should fail
        throw;
    }
    
}

/******************************Interface functions***********************************/

void StaticObstacle::transform_coordinate_system(double scale, double translate_x, double translate_y)
{
    //TODO: Check if that's all

    if (scale > 0)
    {
        transform_scale *= scale;
    }
    
    if (shape.has_value())
    {
        shape->transform_coordinate_system(scale, 0.0, 0.0); //Shape does not need to be modified as well, because we already transform state/occupancy and initial state position values
    }

    if (initial_state.has_value())
    {
        initial_state->transform_coordinate_system(scale, translate_x, translate_y);
    }
}

void StaticObstacle::draw(const DrawingContext& ctx, double scale, double global_orientation, double global_translate_x, double global_translate_y, double local_orientation)
{
    ctx->save();

    //Perform required translation + rotation
    ctx->translate(global_translate_x, global_translate_y);
    ctx->rotate(global_orientation);

    //TODO: Different color / sticker / ... based on type
    switch(type)
    {
        case ObstacleTypeStatic::Unknown:
            ctx->set_source_rgb(0.1,0.1,0.1);
            break;
        case ObstacleTypeStatic::ParkedVehicle:
            ctx->set_source_rgb(0,0.1,0.5);
            break;
        case ObstacleTypeStatic::ConstructionZone:
            ctx->set_source_rgb(0.8,0,0.2);
            break;
        case ObstacleTypeStatic::RoadBoundary:
            ctx->set_source_rgb(0.5,0.5,0.5);
            break;
    }

    if (initial_state.has_value())
    {
        initial_state->transform_context(ctx, scale);

        if (shape.has_value())
        {
            shape->draw(ctx, scale, 0, 0, 0, local_orientation);

            //Draw text on shape position
            //Set text position
            auto shape_center = shape->get_center();
            ctx->translate(shape_center.first, shape_center.second);
            ctx->rotate(local_orientation);

            //Set font, flip bc of chosen coordinate system
            ctx->select_font_face("sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
            Cairo::Matrix font_matrix(0.4 * scale * transform_scale, 0.0, 0.0, -0.4 * scale * transform_scale, 0.0, 0.0);
            ctx->set_font_matrix(font_matrix);

            //Calculate text width to center the text on the obstacle's center
            Cairo::TextExtents text_extents;
            ctx->get_text_extents(obstacle_type_text, text_extents);
            ctx->translate(-text_extents.width / 2.0, - text_extents.height / 2.0);

            //Draw text
            ctx->text_path(obstacle_type_text);
            ctx->set_source_rgb(1.0, 1.0, 1.0);
            ctx->fill_preserve();
            ctx->set_source_rgb(0.0, 0.0, 0.0);
            ctx->set_line_width(0.002 * scale);
            ctx->stroke();
        }
        else
        {
            std::cerr << "TODO: Better warning // Cannot draw shape at position, no value set for shape" << std::endl;
        }
    }
    else
    {
        std::cerr << "TODO: Better warning // Cannot draw StaticObstacle, initial state value is missing" << std::endl;
    }

    ctx->restore();
}

void StaticObstacle::set_lanelet_ref_draw_function(std::function<void (int, const DrawingContext&, double, double, double, double)> _draw_lanelet_refs)
{
    if(initial_state.has_value())
    {
        initial_state->set_lanelet_ref_draw_function(_draw_lanelet_refs);
    }
}