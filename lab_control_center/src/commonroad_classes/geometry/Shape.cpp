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

#include "commonroad_classes/geometry/Shape.hpp"

Shape::Shape(const xmlpp::Node* node)
{
    //Check if node is of type shape
    assert(node->get_name() == "shape");

    try
    {
        //2018 and 2020    
        //Optional parts (all unbounded -> lists)
        xml_translation::iterate_children(
            node,
            [&] (const xmlpp::Node* child)
            {
                circles.push_back(Circle(child));
            },
            "circle"
        );

        xml_translation::iterate_children(
            node,
            [&] (const xmlpp::Node* child)
            {
                polygons.push_back(Polygon(child));
            },
            "polygon"
        );

        xml_translation::iterate_children(
            node,
            [&] (const xmlpp::Node* child)
            {
                rectangles.push_back(Rectangle(child));
            },
            "rectangle"
        );
    }
    catch(const SpecificationError& e)
    {
        throw SpecificationError(std::string("Could not translate Shape:\n") + e.what());
    }
    catch(...)
    {
        //Propagate error, if any subclass of CommonRoadScenario fails, then the whole translation should fail
        throw;
    }
    

    //Test output
    std::cout << "Shape:" << std::endl;
    std::cout << "\tCircle size: " << circles.size() << std::endl;
    std::cout << "\tPolygon size: " << polygons.size() << std::endl;
    std::cout << "\tRectangle size: " << rectangles.size() << std::endl;
}

void Shape::transform_coordinate_system(double scale, double translate_x, double translate_y)
{
    for (auto& circle : circles)
    {
        circle.transform_coordinate_system(scale, translate_x, translate_y);
    }

    for (auto& polygon : polygons)
    {
        polygon.transform_coordinate_system(scale, translate_x, translate_y);
    }

    for (auto& rectangle : rectangles)
    {
        rectangle.transform_coordinate_system(scale, translate_x, translate_y);
    }
}

void Shape::draw(const DrawingContext& ctx, double scale, double global_orientation, double global_translate_x, double global_translate_y, double local_orientation)
{
    ctx->save();

    //Perform required translation + rotation
    ctx->translate(global_translate_x, global_translate_y);
    ctx->rotate(global_orientation);

    ctx->set_line_width(0.005);

    //TODO: Rotation of combined forms is more complex than just rotation of the parts
    for (auto circle : circles)
    {
        circle.draw(ctx, scale, 0, 0, 0, local_orientation);
    }

    for (auto polygon : polygons)
    {
        polygon.draw(ctx, scale, 0, 0, 0, local_orientation);
    }

    for (auto rectangle : rectangles)
    {
        rectangle.draw(ctx, scale, 0, 0, 0, local_orientation);
    }

    if (circles.size() + polygons.size() + rectangles.size() > 1)
    {
        std::cerr << "TODO: Better warning // Local rotation of position made of more than one form currently not supported" << std::endl;
        //TOOD: Implementation idea: Transform to center of the form, then rotate, then draw_relative_to(center_x, center_y) for the forms
    }

    ctx->restore();
}

std::pair<double, double> Shape::get_center()
{
    double x, y = 0.0;
    double center_count = 0.0;

    for (auto circle : circles)
    {
        auto center = circle.get_center();
        x += center.first;
        y += center.second;
        ++center_count;
    }

    for (auto polygon : polygons)
    {
        auto center = polygon.get_center();
        x += center.first;
        y += center.second;
        ++center_count;
    }

    for (auto rectangle : rectangles)
    {
        auto center = rectangle.get_center();
        x += center.first;
        y += center.second;
        ++center_count;
    }

    if (center_count > 0)
    {
        x /= center_count;
        y /= center_count;
    }

    return std::pair<double, double>(x, y);
}

void Shape::transform_context(const DrawingContext& ctx, double scale)
{
    //Just move the coordinate system's center to one of the shape's centroids
    //TODO: Find better point than just some random point within a part of the shape
    if (circles.size() > 0)
    {
        auto center = circles.at(0).get_center();
        ctx->translate(center.first * scale, center.second * scale);
    }
    else if (polygons.size() > 0)
    {
        auto center = polygons.at(0).get_center();
        //Center is computed from vertices, type is not std::optional and not const
        ctx->translate(center.first * scale, center.second * scale);
    }
    else if (rectangles.size() > 0)
    {
        auto center = rectangles.at(0).get_center();
        ctx->translate(center.first * scale, center.second * scale);
    }
}

//********************************************************************************************************************************************
//Getter
//********************************************************************************************************************************************

std::optional<double> Shape::get_orientation()
{
    //Get mean orientation from shapes, if they exist
    double orientation = 0.0;
    double orientation_count = 0.0;

    for (auto rectangle : rectangles)
    {
        orientation += rectangle.get_orientation().value_or(0.0);
        ++orientation_count;
    }

    if (orientation_count > 0)
    {
        orientation /= orientation_count;
    }

    return std::optional<double>(orientation);
}