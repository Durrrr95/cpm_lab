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

#include "commonroad_classes/TrafficSign.hpp"

/**
 * \file TrafficSign.cpp
 * \ingroup lcc_commonroad
 */

TrafficSign::TrafficSign(
    const xmlpp::Node* node,
    std::function<std::optional<std::pair<double, double>>(int)> _get_position_from_lanelet
) :
    get_position_from_lanelet(_get_position_from_lanelet)
{
    //Check if node is of type trafficSign
    assert(node->get_name() == "trafficSign");

    try
    {
        //Translate ID again, to be used in draw() 
        id = xml_translation::get_attribute_int(node, "id", true).value();

        //TrafficSigns are defined in a certain order:
        //- trafficSignElement consisting of what we called TrafficSignPost (must exist), in form of a list
        //- Position (optional)
        //- Virtual (optional), in form of a list
        //-> Start a new definition of our TrafficSignElement with each new TrafficSignPost (their / specs trafficSignElement). 
        size_t pos = -1; //To know which element to fill with new info
        std::vector<std::string> allowed_next_values = { "trafficSignElement" }; //Indicates which next values are allowed, to check for spec consistency

        xml_translation::iterate_children(
            node,
            [&] (xmlpp::Node* child)
            {
                auto child_name = std::string(child->get_name());

                //Check if the allowed order is kept
                if (std::find(allowed_next_values.begin(), allowed_next_values.end(), child_name) == allowed_next_values.end())
                {
                    std::stringstream error_stream;
                    error_stream << "TrafficSign in line " << child->get_line() << ", child " << child->get_name() << " does not fulfill specs, order of elements not kept / is empty."
                        << "If the order is not kept, the specification is no longer consistent and the TrafficSign cannot be translated properly!";
                    throw SpecificationError(error_stream.str());
                }

                //Go through possible types at this level, check that the order is kept
                if (child_name == "trafficSignElement")
                {
                    //Create next entry
                    ++pos;
                    traffic_sign_elements.push_back(TrafficSignElement());

                    //Add all trafficSignElement (commonroad) / TrafficSignPost (here) elements
                    traffic_sign_elements[pos].traffic_sign_posts = translate_traffic_sign_posts(child);

                    allowed_next_values = { "trafficSignElement", "position", "virtual" };
                }
                else if (child_name == "position")
                {
                    traffic_sign_elements[pos].position = std::optional<Position>{translate_position(child)};

                    allowed_next_values = { "trafficSignElement", "virtual" };
                }
                else if (child_name == "virtual")
                {
                    traffic_sign_elements[pos].is_virtual.push_back(translate_virtual(child));

                    allowed_next_values = { "trafficSignElement", "virtual" }; //Multiple virtual definitions are possible
                }
                else
                {
                    std::stringstream error_stream;
                    error_stream << "TrafficLight in line " << child->get_line() << " does not fulfill specs, unknown element: " << child_name;
                    throw SpecificationError(error_stream.str());
                }
            },
            "" //Iterate all children
        );

        //Make sure that the translation is not empty
        if(traffic_sign_elements.size() == 0)
        {
            std::stringstream error_msg_stream;
            error_msg_stream << "TrafficSign is empty, line: " << node->get_line();
            throw SpecificationError(error_msg_stream.str());
        }
    }
    catch(const SpecificationError& e)
    {
        throw SpecificationError(std::string("Could not translate TrafficSign:\n") + e.what());
    }
    catch(...)
    {
        //Propagate error, if any subclass of CommonRoadScenario fails, then the whole translation should fail
        throw;
    }
    

    //Test output
    // std::cout << "Traffic sign translated: " << std::endl;
    // std::cout << "\tNumber of elements: " << traffic_sign_elements.size() << std::endl;
    // std::cout << "\tNow iterating..." << std::endl;
    // for (const auto traffic_sign_element : traffic_sign_elements)
    // {
    //     std::cout << "\tTraffic sign element:" << std::endl;
    //     std::cout << "\t\t Is virtual - size: " << traffic_sign_element.is_virtual.size() << std::endl;
    //     std::cout << "\t\t Position has value: " << traffic_sign_element.position.has_value() << std::endl;
    //     std::cout << "\t\t Posts:" << std::endl;
    //     for (const auto traffic_sign_post : traffic_sign_element.traffic_sign_posts)
    //     {
    //         std::cout << "\t\t\t Post ID: " << traffic_sign_post.traffic_sign_id << std::endl;
    //         std::cout << "\t\t\t Additional values: ";
    //         for (const auto additional_value : traffic_sign_post.additional_values)
    //         {
    //             std::cout << " | " << additional_value;
    //         }
    //         std::cout << std::endl;
    //     }
    // }
}

std::vector<TrafficSignPost> TrafficSign::translate_traffic_sign_posts(const xmlpp::Node* element_node)
{
    //As for the whole TrafficSign definition, we need to iterate all children
    //For two entries with one being optional, checking for consistency does not make sense

    //A trafficSignElement entry is defined in a certain order:
    //- trafficSignID (must exist)
    //- additionalValue (optional), list
    //-> Start a new definition with every new trafficSignID definition. 
    size_t pos = -1; //To know which element to fill with new info
    std::vector<TrafficSignPost> traffic_sign_posts;

    bool is_first_element = true; //We need to make sure that the first element is trafficSignID (consistency check for first element)

    xml_translation::iterate_children(
        element_node,
        [&] (xmlpp::Node* child)
        {
            auto child_name = std::string(child->get_name());

            if (child_name == "trafficSignID")
            {
                //Create next entry
                ++pos;
                traffic_sign_posts.push_back(TrafficSignPost());

                traffic_sign_posts[pos].traffic_sign_id = xml_translation::get_first_child_text(child);
            }
            else if (child_name == "additionalValue")
            {
                if (is_first_element)
                {
                    //We cannot check for consistency after the first element, because after an additional value, more additional values
                    //or an ID might follow, and the same goes for ID
                    //BUT: According to the spec, the first element must be an ID. Otherwise, this implementation would also break
                    std::stringstream error_stream;
                    error_stream << "TrafficSign in line " << child->get_line() << " does not fulfill specs, order not kept: " << child_name;
                    throw SpecificationError(error_stream.str());
                }

                traffic_sign_posts[pos].additional_values.push_back(xml_translation::get_first_child_text(child));
            }
            else
            {
                std::stringstream error_stream;
                error_stream << "TrafficSign in line " << child->get_line() << " does not fulfill specs, unknown element: " << child_name;
                throw SpecificationError(error_stream.str());
            }

            is_first_element = false;
        },
        "" //Iterate all children
    );

    //We do not accept empty trafficSignElements (don't make sense)
    if (traffic_sign_posts.size() < 1)
    {
        std::stringstream error_msg_stream;
        error_msg_stream << "trafficSignElement is empty, line: " << element_node->get_line();
        throw SpecificationError(error_msg_stream.str());
    }

    return traffic_sign_posts;
}

Position TrafficSign::translate_position(const xmlpp::Node* position_node)
{
    //Get position value, which must not be specified
    auto pos = Position(position_node);

    //According to specs, the position must always be exact
    if (!pos.is_exact())
    {
        std::stringstream error_msg_stream;
        error_msg_stream << "Position in TrafficLight not conformant to specs (must be exact), line: " << position_node->get_line();
        throw SpecificationError(error_msg_stream.str());
    }

    return pos;
}

bool TrafficSign::translate_virtual(const xmlpp::Node* virtual_node)
{
    std::string active_string = xml_translation::get_first_child_text(virtual_node);
    if (active_string.compare("true") == 0)
    {
        return true;
    }
    else if (active_string.compare("false") == 0)
    {
        return false;
    } 
    else 
    {
        std::stringstream error_msg_stream;
        error_msg_stream << "Value of node element 'virtual' not conformant to specs, line: " << virtual_node->get_line();
        throw SpecificationError(error_msg_stream.str());
    }
}

void TrafficSign::transform_coordinate_system(double scale, double angle, double translate_x, double translate_y)
{    
    for (auto& element : traffic_sign_elements)
    {
        if (element.position)
        {
            element.position->transform_coordinate_system(scale, angle, translate_x, translate_y);
        }
    }
}

//Suppress warning for unused parameter (s)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void TrafficSign::draw(const DrawingContext& ctx, double scale, double global_orientation, double global_translate_x, double global_translate_y, double local_orientation) 
{    
    ctx->save();

    //Perform required translation + rotation
    //Local rotation does not really make sense here and is thus ignored (rotating a circle in its own coordinate system is pointless)
    ctx->translate(global_translate_x, global_translate_y);
    ctx->rotate(global_orientation);

    for (auto element : traffic_sign_elements)
    {
        double x = 0.0;
        double y = 0.0;
        bool position_exists = false;

        if (element.position.has_value())
        {
            //Position must be exact, this was made sure during translation
            x = element.position->get_point().value().get_x() * scale;
            y = element.position->get_point().value().get_y() * scale;
            position_exists = true;

            std::cout << "using position" << std::endl;
        }
        else if (get_position_from_lanelet)
        {
            auto position = get_position_from_lanelet(id);
            if (position.has_value())
            {
                x = position->first * scale;
                y = position->second * scale;
                position_exists = true;
            }
        }
        
        if (position_exists)
        {
            //Draw test-circle
            double radius = 0.05;
            ctx->set_source_rgb(0.6, 0.9, 0.2);
            ctx->set_line_width(0.005);
            ctx->move_to(x, y);
            ctx->begin_new_path();
            ctx->arc(x, y, radius * scale, 0.0, 2 * M_PI);
            ctx->close_path();
            ctx->fill_preserve();
            ctx->stroke();

            std::stringstream descr_stream;
            for(auto post : element.traffic_sign_posts)
            {
                descr_stream << post.traffic_sign_id << " ";
                if (post.additional_values.size() > 0)
                {
                    descr_stream << "( ";
                    for (auto add_val : post.additional_values)
                    {
                        descr_stream << add_val << " ";
                    }
                    descr_stream << ")";
                }
            }

            //Draw IDs
            ctx->translate(x, y);
            draw_text_centered(ctx, 0, 0, 0, 8, descr_stream.str());
        }
        else
        {
            LCCErrorLogger::Instance().log_error("Could not draw traffic sign: Could not obtain any valid position from defintion (also no lanelet reference exists)");
        }
    }

    ctx->restore();
}
#pragma GCC diagnostic pop

const std::vector<TrafficSignElement>& TrafficSign::get_traffic_sign_elements() const
{
    return traffic_sign_elements;
}