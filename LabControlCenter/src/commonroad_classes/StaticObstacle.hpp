#pragma once

#include <libxml++-2.6/libxml++/libxml++.h>

#include <optional>
//Optional is used for 3 reasons:
//1. Some values are optional according to the specification
//2. Some values might be missing if the file is not spec-conform, which is easy to handle when we do not require that they exist (though we still check for their existance)
//3. It is easier to set up an object piece by piece in the constructor, but that is not possible if the member object we want to set up does not have a default constructor (we would have to use the initializer list then)

#include "commonroad_classes/geometry/Shape.hpp"

#include "commonroad_classes/states/Occupancy.hpp"
#include "commonroad_classes/states/State.hpp"

#include "commonroad_classes/InterfaceDraw.hpp"
#include "commonroad_classes/InterfaceTransform.hpp"
#include "commonroad_classes/XMLTranslation.hpp"

/**
 * \enum class ObstacleTypeStatic
 * \brief Specifies static obstacle types, as in commonroad, NotInSpec for types that should not exist
 * 2018 and 2020 differ because in 2020, we have static and dynamic obstacles, whereas in 2018, we only have obstacles of type static or dynamic
 * Nonetheless, we should only use the static types here, also according to 2018 specs - thus, if in a 2018 translation a type of dynamic obstacles is used
 * although the role is static, we set the type to WrongDynamicType
 * We do not need to store "role", as we already have two different classes for that
 */
enum class ObstacleTypeStatic {Unknown, ParkedVehicle, ConstructionZone, RoadBoundary, NotInSpec, WrongDynamicType};

/**
 * \class StaticObstacle
 * \brief This class, like all other classes in this folder, are heavily inspired by the current (2020) common road XML specification (https://gitlab.lrz.de/tum-cps/commonroad-scenarios/blob/master/documentation/XML_commonRoad_2020a.pdf)
 * It is used to store / represent a StaticObstacle specified in an XML file
 */
class StaticObstacle : public InterfaceTransform, public InterfaceDraw
{
private:
    ObstacleTypeStatic type;
    std::optional<Shape> shape;
    std::optional<State> initial_state;
    //Other 2018 obstacle-values should only be set for dynamic obstacles - nonetheless, we check for their existence in the constructor (and show warnings, if necessary)

public:
    /**
     * \brief The constructor gets an XML node and parses it once, translating it to the C++ data structure
     * An error is thrown in case the node is invalid / does not match the expected CommonRoad specs (TODO: Custom error type for this case)
     * \param node A (static) obstacle node
     */
    StaticObstacle(const xmlpp::Node* node);
    
    //TODO: Getter

    /**
     * \brief This function is used to fit the imported XML scenario to a given min. lane width
     * The lane with min width gets assigned min. width by scaling the whole scenario up until it fits
     * This scale value is used for the whole coordinate system
     * \param scale The factor by which to transform all number values related to position
     */
    void transform_coordinate_system(double scale) override {}

    /**
     * \brief This function is used to draw the data structure that imports this interface
     * If you want to set a color for drawing, perform this action on the context before using the draw function
     * \param ctx A DrawingContext, used to draw on
     * \param scale - optional: The factor by which to transform all number values related to position - this is not permanent, only for drawing (else, use InterfaceTransform's functions)
     */
    void draw(const DrawingContext& ctx, double scale = 1.0) override;
};