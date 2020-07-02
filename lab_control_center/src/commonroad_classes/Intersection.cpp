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

#include "commonroad_classes/Intersection.hpp"

Intersection::Intersection(const xmlpp::Node* node)
{
    //Check if node is of type intersection
    assert(node->get_name() == "intersection");
    //2020 only

    try
    {
        xml_translation::iterate_children(
            node, 
            [&] (const xmlpp::Node* child) 
            {
                //Translate incoming node
                Incoming incoming;

                //Mandatory argument
                incoming.incoming_lanelet = get_child_attribute_ref(child, "incomingLanelet", true);

                //Non-mandatory arguments
                incoming.successors_right = get_child_attribute_ref(child, "successorsRight", false);
                incoming.successors_straight = get_child_attribute_ref(child, "successorsStraight", false);
                incoming.successors_left = get_child_attribute_ref(child, "successorsLeft", false);
                incoming.is_left_of = get_child_attribute_ref(child, "isLeftOf", false);

                incoming_map.insert({xml_translation::get_attribute_int(child, "id", true).value(), incoming}); //As mentioned in other classes: Value must exist, else error is thrown, so .value() can be used safely here
            }, 
            "incoming"
        );
    }
    catch(const SpecificationError& e)
    {
        throw SpecificationError(std::string("Could not translate Intersection:\n") + e.what());
    }
    catch(...)
    {
        //Propagate error, if any subclass of CommonRoadScenario fails, then the whole translation should fail
        throw;
    }
    

    if (incoming_map.size() == 0)
    {
        std::stringstream error_msg_stream;
        error_msg_stream << "Intersection should contain at least one incoming reference - line " << node->get_line();
        throw SpecificationError(error_msg_stream.str());
    }

    std::cout << "Lanelet: " << std::endl;
    std::cout << "\tIncoming references (only incomingLanelet shown): ";
    for (const auto entry : incoming_map)
    {
        std::cout << " | " << entry.second.incoming_lanelet.value_or(-1);
    }
    std::cout << std::endl;
}

std::optional<int> Intersection::get_child_attribute_ref(const xmlpp::Node* node, std::string child_name, bool warn)
{
    const auto child_node = xml_translation::get_child_if_exists(node, child_name, warn);
    if (child_node)
    {
        return std::optional<int>(xml_translation::get_attribute_int(child_node, "ref", true));
    }

    return std::optional<int>();
}