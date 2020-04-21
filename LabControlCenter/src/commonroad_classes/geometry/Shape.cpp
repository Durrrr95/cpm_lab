#include "commonroad_classes/geometry/Shape.hpp"

Shape::Shape(const xmlpp::Node* node)
{
    //TODO: Assert node name to be shape

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

    //Test output
    std::cout << "Shape:" << std::endl;
    std::cout << "\tCircle size: " << circles.size() << std::endl;
    std::cout << "\tPolygon size: " << polygons.size() << std::endl;
    std::cout << "\tRectangle size: " << rectangles.size() << std::endl;
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

    ctx->restore();
}

void Shape::transform_context(const DrawingContext& ctx, double scale)
{
    //Just move the coordinate system's center to one of the shape's centroids
    //TODO: Find better point than just some random point within a part of the shape
    if (circles.size() > 0)
    {
        auto center = circles.at(0).get_center();
        if(center.has_value())
        {
            ctx->translate(center->get_x() * scale, center->get_y() * scale);
        }
    }
    else if (polygons.size() > 0)
    {
        auto center = polygons.at(0).get_center();
        //Center is computed from vertices, type is not std::optional and not const
        ctx->translate(center.get_x() * scale, center.get_y() * scale);
    }
    else if (rectangles.size() > 0)
    {
        auto center = rectangles.at(0).get_center();
        if(center.has_value())
        {
            ctx->translate(center->get_x() * scale, center->get_y() * scale);
        }
    }
}