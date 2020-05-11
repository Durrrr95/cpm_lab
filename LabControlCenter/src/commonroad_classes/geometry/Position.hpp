#pragma once

#include <libxml++-2.6/libxml++/libxml++.h>

#include <functional>
#include <memory>
#include <vector>

#include <optional>
//Optional is used for 3 reasons:
//1. Some values are optional according to the specification
//2. Some values might be missing if the file is not spec-conform, which is easy to handle when we do not require that they exist (though we still check for their existance)
//3. It is easier to set up an object piece by piece in the constructor, but that is not possible if the member object we want to set up does not have a default constructor (we would have to use the initializer list then)

#include "commonroad_classes/geometry/Circle.hpp"
#include "commonroad_classes/geometry/Polygon.hpp"
#include "commonroad_classes/geometry/Rectangle.hpp"
#include "commonroad_classes/geometry/Shape.hpp"

#include "commonroad_classes/InterfaceDraw.hpp"
#include "commonroad_classes/InterfaceGeometry.hpp"
#include "commonroad_classes/InterfaceTransform.hpp"
#include "commonroad_classes/XMLTranslation.hpp"

#include <cassert> //To make sure that the translation is performed on the right node types, which should haven been made sure by the programming (thus not an error, but an assertion is used)

/**
 * \class Position
 * \brief Auxiliary class from the XML specification: https://gitlab.lrz.de/tum-cps/commonroad-scenarios/-/blob/master/documentation/XML_commonRoad_XSD_2020a.xsd
 */
class Position : public InterfaceTransform, public InterfaceDraw, public InterfaceGeometry
{
private:
    //TODO: Solve below w. inheritance? Or keep it this way? -> Probably easier to keep it this way

    //According to the specification, a position might not be given exactly (as a point)
    bool is_exact;

    //Transformation scale of transform_coordinate_system is remembered to draw circles / arrows correctly scaled
    double transform_scale = 1.0;
    
    //Exact position (positionExact)
    std::optional<Point> point;

    //Inexact position (positionInterval)
    std::vector<Circle> circles;
    std::vector<int> lanelet_refs;
    std::vector<Polygon> polygons;
    std::vector<Rectangle> rectangles;

    //Function to draw lanelet_refs
    std::function<void (int, const DrawingContext&, double, double, double, double)> draw_lanelet_refs;

public:
    /**
     * \brief Constructor, set up a position object
     */
    Position(const xmlpp::Node* node);

    /**
     * \brief Second constructor, value is irrelevant - value given s.t. !!no default constructor exists!!
     * Call this if you need to use the specified default value from the specs because the value was not set explicitly
     */
    Position(int irrelevant_int);

    /**
     * \brief Setter for drawing lanelet references (Position can also be constructed without this)
     * \param _draw_lanelet_refs Function that, given an lanelet reference and the typical drawing arguments, draws a lanelet reference
     */
    void set_lanelet_ref_draw_function(std::function<void (int, const DrawingContext&, double, double, double, double)> _draw_lanelet_refs);

    /**
     * \brief This function is used to fit the imported XML scenario to a given min. lane width
     * The lane with min width gets assigned min. width by scaling the whole scenario up until it fits
     * This scale value is used for the whole coordinate system
     * \param scale The factor by which to transform all number values related to position
     */
    void transform_coordinate_system(double scale, double translate_x, double translate_y) override;

    /**
     * \brief This function is used to draw the data structure that imports this interface
     * If you want to set a color for drawing, perform this action on the context before using the draw function
     * To change local translation, just transform the coordinate system beforehand
     * As this does not always work with local orientation (where sometimes the translation in the object must be called before the rotation if performed, to rotate within the object's coordinate system),
     * local_orientation was added as a parameter
     * \param ctx A DrawingContext, used to draw on
     * \param scale - optional: The factor by which to transform all number values related to position - this is not permanent, only for drawing (else, use InterfaceTransform's functions)
     * \param global_orientation - optional: Rotation that needs to be applied before drawing - set as global transformation to the whole coordinate system
     * \param global_translate_x - optional: Translation in x-direction that needs to be applied before drawing - set as global transformation to the whole coordinate system
     * \param global_translate_y - optional: Translation in y-direction that needs to be applied before drawing - set as global transformation to the whole coordinate system
     * \param local_orientation - optional: Rotation that needs to be applied within the object's coordinate system
     */
    void draw(const DrawingContext& ctx, double scale = 1.0, double global_orientation = 0.0, double global_translate_x = 0.0, double global_translate_y = 0.0, double local_orientation = 0.0) override;

    /**
     * \brief Get center (positional value) of the shape
     * \return Center of the shape
     */
    std::pair<double, double> get_center() override;

    /**
     * \brief This function is used to transform (rotate, translate) a context, e.g. because position/orientation and shape information are given in different objects, but need to be combined for drawing
     * \param ctx A DrawingContext, used to draw on
     * \param scale - optional: The factor by which to transform all number values related to position - this is not permanent, only for drawing (else, use InterfaceTransform's functions)
     */
    void transform_context(const DrawingContext& ctx, double scale = 1.0);
    
    void to_dds_msg() {} 

    //TODO: Getter
};