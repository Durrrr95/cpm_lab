#include "commonroad_classes/DynamicObstacle.hpp"

DynamicObstacle::DynamicObstacle(const xmlpp::Node* node)
{
    //Check if node is of type dynamicObstacle
    assert(node->get_name() == "dynamicObstacle" || node->get_name() == "obstacle");
    if (node->get_name() == "obstacle") //2018 specs
    {
        std::string role_text = xml_translation::get_child_child_text(node, "role", true).value(); //Must exist (2018 specs), else error is thrown
        assert(role_text.compare("dynamic") == 0);
    }

    std::string obstacle_type_text;

    try
    {
        obstacle_type_text = xml_translation::get_child_child_text(node, "type", true).value(); //Must exist, so an error is thrown anyway -> just use .value()
        if (obstacle_type_text.compare("unknown") == 0)
        {
            type = ObstacleTypeDynamic::Unknown;
        }
        else if (obstacle_type_text.compare("car") == 0)
        {
            type = ObstacleTypeDynamic::Car;
        }
        else if (obstacle_type_text.compare("truck") == 0)
        {
            type = ObstacleTypeDynamic::Truck;
        }
        else if (obstacle_type_text.compare("bus") == 0)
        {
            type = ObstacleTypeDynamic::Bus;
        }
        else if (obstacle_type_text.compare("motorcycle") == 0)
        {
            type = ObstacleTypeDynamic::Motorcycle;
        }
        else if (obstacle_type_text.compare("bicycle") == 0)
        {
            type = ObstacleTypeDynamic::Bicycle;
        }
        else if (obstacle_type_text.compare("pedestrian") == 0)
        {
            type = ObstacleTypeDynamic::Pedestrian;
        }
        else if (obstacle_type_text.compare("priorityVehicle") == 0)
        {
            type = ObstacleTypeDynamic::PriorityVehicle;
        }
        else if (obstacle_type_text.compare("train") == 0)
        {
            type = ObstacleTypeDynamic::Train;
        }
        else if (obstacle_type_text.compare("parkedVehicle") == 0 || 
            obstacle_type_text.compare("constructionZone") == 0 || 
            obstacle_type_text.compare("roadBoundary") == 0)
        {
            //Behavior for dynamic types, which should not be used here
            throw SpecificationError("Node element not conformant to specs - usage of dynamic type for static object (obstacleType)");
        }
        else
        {
            std::stringstream error_msg_stream;
            error_msg_stream << "Node element not conformant to specs (obstacleType) in " << node->get_line();
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

        //Only one of the two must exist
        const auto trajectory_node = xml_translation::get_child_if_exists(node, "trajectory", false);
        const auto occupancy_node = xml_translation::get_child_if_exists(node, "occupancySet", false);
        const auto signal_node = xml_translation::get_child_if_exists(node, "signalSeries", false);
        if (! (trajectory_node || occupancy_node || signal_node))
        {
            std::stringstream error_msg_stream;
            error_msg_stream << "Trajectory / occupancy / signal series not defined for dynamic object (one must be defined) - line " << trajectory_node->get_line();
            throw SpecificationError(error_msg_stream.str());
        }
        if ((trajectory_node && occupancy_node) || (trajectory_node && signal_node) || (occupancy_node && signal_node))
        {
            std::stringstream error_msg_stream;
            error_msg_stream << "Trajectory / occupancy / signal series both / all three defined for dynamic object (only one must be defined) - line " << trajectory_node->get_line();
            throw SpecificationError(error_msg_stream.str());
        }

        //Translate trajectory, occupancy, signal series
        if (trajectory_node)
        {
            xml_translation::iterate_children(
                trajectory_node, 
                [&] (const xmlpp::Node* child) 
                {
                    trajectory.push_back(State(child));
                }, 
                "state"
            );
        }
        if (occupancy_node)
        {
            xml_translation::iterate_children(
                occupancy_node, 
                [&] (const xmlpp::Node* child) 
                {
                    occupancy_set.push_back(Occupancy(child));
                }, 
                "occupancy"
            );
        }
        //2020 only
        if (signal_node)
        {
            xml_translation::iterate_children(
                signal_node, 
                [&] (const xmlpp::Node* child) 
                {
                    signal_series.push_back(SignalState(child));
                }, 
                "signalState"
            );
        }
    }
    catch(const SpecificationError& e)
    {
        throw SpecificationError(std::string("Could not translate DynamicObstacle:\n") + e.what());
    }
    catch(...)
    {
        //Propagate error, if any subclass of CommonRoadScenario fails, then the whole translation should fail
        throw;
    }
    

    //Test output
    std::cout << "Dynamic obstacle:" << std::endl;
    std::cout << "\tTrajectory size: " << trajectory.size() << std::endl;
    std::cout << "\tOccupancy size: " << occupancy_set.size() << std::endl;
    std::cout << "\tSignal series size: " << signal_series.size() << std::endl;
    std::cout << "\tObstacle type (text, before translation to enum): " << obstacle_type_text << std::endl;
    std::cout << "\tInitial state exists (it should): " << initial_state.has_value() << std::endl;
    std::cout << "\tShape exists (it should): " << shape.has_value() << std::endl;

    step = 0;
} 

/******************************Interface functions***********************************/

void DynamicObstacle::transform_coordinate_system(double scale, double translate_x, double translate_y)
{
    //TODO: Check if that's all
    
    if (shape.has_value())
    {
        shape->transform_coordinate_system(scale, 0.0, 0.0); //Shape does not need to be modified as well, because we already transform state/occupancy and initial state position values
    }

    if (initial_state.has_value())
    {
        initial_state->transform_coordinate_system(scale, translate_x, translate_y);
    }

    for (auto& state : trajectory)
    {
        state.transform_coordinate_system(scale, translate_x, translate_y);
    }

    for (auto& occupancy : occupancy_set)
    {
        occupancy.transform_coordinate_system(scale, translate_x, translate_y);
    }
}

void DynamicObstacle::draw(const DrawingContext& ctx, double scale, double global_orientation, double global_translate_x, double global_translate_y, double local_orientation)
{
    ctx->save();

    //Perform required translation + rotation
    ctx->translate(global_translate_x, global_translate_y);
    ctx->rotate(global_orientation);

    //TODO: Different color / sticker / ... based on type
    ctx->set_source_rgb(1.0,0.5,0.0);

    if (step == 0)
    {
        if (initial_state.has_value())
        {
            ctx->save();

            initial_state->transform_context(ctx, scale);

            if (shape.has_value())
            {
                shape->draw(ctx, scale, 0, 0, 0, local_orientation);
            }
            else
            {
                std::cerr << "TODO: Better warning // Cannot draw shape at position, no value set for shape" << std::endl;
            }

            ctx->restore();
        }
        else
        {
            std::cerr << "TODO: Better warning // Cannot draw Dynamicbstacle, initial state value is missing" << std::endl;
        }
    }
    else if (step <= trajectory.size() && trajectory.size() > 0)
    {
        ctx->save();

        trajectory.at(step - 1).transform_context(ctx, scale);

        if (shape.has_value())
        {
            shape->draw(ctx, scale, 0, 0, 0, local_orientation);
        }
        else
        {
            std::cerr << "TODO: Better warning // Cannot draw shape at position, no value set for shape" << std::endl;
        }

        ctx->restore();
    }
    else if (step <= occupancy_set.size() && occupancy_set.size() > 0)
    {
        //Draw occupancy shape
        ctx->save();
        occupancy_set.at(step - 1).draw(ctx, scale, 0, 0, 0, local_orientation);
        ctx->restore();

        //Draw obstacle shape at centroid of occupancy shape
        // ctx->save();
        // occupancy_set.at(step - 1).transform_context(ctx, scale);
        // if (shape.has_value())
        // {
        //     shape->draw(ctx, scale);
        // }
        // else
        // {
        //     std::cerr << "TODO: Better warning // Cannot draw shape at position, no value set for shape" << std::endl;
        // }
        // ctx->restore();
    }

    //Step - 1 is current trajectory index (0 for initial state)
    step = step + 1;
    if (step > trajectory.size() && trajectory.size() > 0) 
    {
        step = 0;
    }
    else if (step > occupancy_set.size() && occupancy_set.size() > 0)
    {
        step = 0;
    }

    ctx->restore();
}

void DynamicObstacle::set_lanelet_ref_draw_function(std::function<void (int, const DrawingContext&, double, double, double, double, double)> _draw_lanelet_refs)
{
    if(initial_state.has_value())
    {
        initial_state->set_lanelet_ref_draw_function(_draw_lanelet_refs);
    }

    for (auto& state : trajectory)
    {
        state.set_lanelet_ref_draw_function(_draw_lanelet_refs);
    }
}