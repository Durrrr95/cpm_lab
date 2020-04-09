#include "commonroad_classes/geometry/Circle.hpp"

Circle::Circle(const xmlpp::Node* node)
{
    radius = xml_translation::get_child_child_double(node, "radius", true); //mandatory

    //Get point value, which must not be specified
    const auto point_node = xml_translation::get_child_if_exists(node, "center", false);
    if (point_node)
    {
        center = std::optional<Point>{std::in_place, point_node};
    }
    else
    {
        //Use default-value constructor (parameter is irrelevant)
        center = std::optional<Point>{std::in_place, 0};
    }

    //Test output
    std::cout << "Circle:" << std::endl;
    std::cout << "\tRadius: " << radius << std::endl;
    std::cout << "\tCenter set: " << center.has_value() << std::endl;
}