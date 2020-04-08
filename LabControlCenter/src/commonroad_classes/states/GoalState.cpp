#include "commonroad_classes/states/GoalState.hpp"

GoalState::GoalState(const xmlpp::Node* node)
{
    //2018 and 2020 specs are the same
    //TODO: Assert node "type"

    const auto position_node = xml_translation::get_child_if_exists(node, "position", false);
    if (position_node)
    {
        position = std::optional<Position>{std::in_place, position_node};
    }
    else
    {
        //TODO: Check if default position value is spec-conform if no value is specified here
        //Use default-value constructor (parameter is irrelevant)
        position = std::optional<Position>{std::in_place, 0};
    }

    const auto velocity_node = xml_translation::get_child_if_exists(node, "velocity", false);
    if (velocity_node)
    {
        velocity = std::optional<Interval>(std::in_place, velocity_node);
    }

    const auto orientation_node = xml_translation::get_child_if_exists(node, "orientation", false);
    if (orientation_node)
    {
        orientation = std::optional<Interval>(std::in_place, orientation_node);
    }

    //Time is defined using intervals
    const auto time_node = xml_translation::get_child_if_exists(node, "time", true);
    if (time_node)
    {
        time = std::optional<IntervalOrExact>(std::in_place, time_node);
    }
    else
    {
        //Time is the only actually required value
        std::cerr << "TODO: Better warning // No time node in GoalState - Line: " << node->get_line() << std::endl;
    }
    

    //Test output
    std::cout << "GoalState: " << std::endl;
    std::cout << "\tPosition exists: " << position.has_value() << std::endl;
    std::cout << "\tVelocity exists: " << velocity.has_value() << std::endl;
    std::cout << "\tOrientation exists: " << orientation.has_value() << std::endl;
    std::cout << "\tTime exists: " << time.has_value() << std::endl;
}