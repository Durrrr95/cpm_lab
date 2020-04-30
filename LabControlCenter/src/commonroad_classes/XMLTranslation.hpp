#pragma once

#include <libxml++-2.6/libxml++/libxml++.h>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <sstream>
#include "commonroad_classes/SpecificationError.hpp"

/**
 * This is a utility namespace
 * It includes functions that are used for XML Translation throughout the commonroad_classes folder
 */

namespace xml_translation
{
    //**********************************************************************
    //Elements
    //**********************************************************************

    /**
     * \brief Takes a node as input, assuming it is of type TextNode (which is tested within the function, thus xmlpp::Node, not xmlpp::TextNode was chosen)
     * Then, it gets its content in form of a string
     * \param node An XML node, assumed to be of type TextNode (which must not be checked by the user)
     * \return A string containing the content of the TextNode, or 'empty' if there is none
     */
    std::string get_node_text(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type TextNode (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::TextNode was chosen)
     * Then, it gets its content in form of an int, if that is possible, or else 
     * \param node An XML node, assumed to be of type TextNode (which must not be checked by the user)
     * \return An int containing the content of the TextNode, or -1 in case of an error
     */
    int get_node_int(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type TextNode (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::TextNode was chosen)
     * Then, it gets its content in form of an int, if that is possible, or else 
     * \param node An XML node, assumed to be of type TextNode (which must not be checked by the user)
     * \return An unsigned long long value containing the content of the TextNode, or 0 in case of an error
     */
    unsigned long long get_node_uint(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type TextNode (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::TextNode was chosen)
     * Then, it gets its content in form of an double, if that is possible, or else 
     * \param node An XML node, assumed to be of type TextNode (which must not be checked by the user)
     * \return A double value containing the content of the TextNode, or -1.0 in case of an error
     */
    double get_node_double(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * \param node An XML node 
     * \param child_name Expected name of the child node
     * \param throw_error throw_error if the child does not exist (if true, else stay silent) - optional 
     * \return Pointer to the child node (if it does not exist, returns nullptr
     */
    const xmlpp::Node* get_child_if_exists(const xmlpp::Node* node, std::string child_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, assuming that it is one of the common commonroad element nodes like '<country>ZAM</country>', where the first child node is a text node containint 'ZAM', 
     * it checks whether this node exists, and, if so, gets its content in form of a string
     * 
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \return A string containing the content, or 'empty' if there is none
     */
    std::string get_first_child_text(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, assuming that it is one of the common commonroad element nodes like '<country>ZAM</country>', where the first child node is a text node containint 'ZAM', 
     * it checks whether this node exists, and, if so, gets its content in form of an int
     * 
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \return An int containing the content, or -1 in case of an error
     */
    int get_first_child_int(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, assuming that it is one of the common commonroad element nodes like '<country>ZAM</country>', where the first child node is a text node containint 'ZAM', 
     * it checks whether this node exists, and, if so, gets its content in form of an unsigned long long
     * 
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \return A uint containing the content, or 0 in case of an error
     */
    unsigned long long get_first_child_uint(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, assuming that it is one of the common commonroad element nodes like '<country>ZAM</country>', where the first child node is a text node containint 'ZAM', 
     * it checks whether this node exists, and, if so, gets its content in form of a double
     * 
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \return A double containing the content, or -1.0 in case of an error
     */
    double get_first_child_double(const xmlpp::Node* node);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * \param element_node An XML node
     * \param child_name Expected name of the child node
     * \param throw_error throw_error if the child does not exist (if true, else stay silent) - optional 
     * \return Value of the first child of the first child with name child_name of node, or 'empty' in case of an error
     */
    std::string get_child_child_text(const xmlpp::Node* node, std::string child_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * \param element_node An XML node
     * \param child_name Expected name of the child node
     * \param throw_error throw_error if the child does not exist (if true, else stay silent) - optional 
     * \return Value of the first child of the first child with name child_name of node, or -1 in case of an error
     */
    int get_child_child_int(const xmlpp::Node* node, std::string child_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * \param element_node An XML node
     * \param child_name Expected name of the child node
     * \param throw_error throw_error if the child does not exist (if true, else stay silent) - optional 
     * \return Value of the first child of the first child with name child_name of node, or 0 in case of an error
     */
    unsigned long long get_child_child_uint(const xmlpp::Node* node, std::string child_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * \param element_node An XML node
     * \param child_name Expected name of the child node
     * \param throw_error throw_error if the child does not exist (if true, else stay silent) - optional 
     * \return Value of the first child of the first child with name child_name of node, or -1.0 in case of an error
     */
    double get_child_child_double(const xmlpp::Node* node, std::string child_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * Goes another step deeper to extract the value in the "exact" node for exact decimal values
     * \param element_node An XML node
     * \param child_name Expected name of the child node
     * \param throw_error throw_error if the child does not exist (if true, else stay silent) - optional 
     * \return Value of the first child of the first child with name child_name of node of type exact, or -1.0 in case of an error
     */
    double get_child_child_double_exact(const xmlpp::Node* node, std::string child_name, bool throw_error = false);

    //**********************************************************************
    //Attributes
    //**********************************************************************

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, it gets its content in form of a string
     * Shows /throws an error if desired, always warns if node is not of type Element
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \param attribute_name Name of the attribute of which to get the value
     * \param throw_error Optional parameter, throw_error / throw error if the attribute does not exists only if desired (it might not be required by specs)
     * \return A string containing the content of the Element, or 'empty' if there is none
     */
    std::string get_attribute_text(const xmlpp::Node* node, std::string attribute_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, it gets its content in form of an int, if that is possible, or else 
     * Shows /throws an error if desired, always warns if node is not of type Element
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \param attribute_name Name of the attribute of which to get the value
     * \param throw_error Optional parameter, throw_error / throw error if the attribute does not exists only if desired (it might not be required by specs)
     * \return An int containing the content of the Element, or -1 in case of an error
     */
    int get_attribute_int(const xmlpp::Node* node, std::string attribute_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, it gets its content in form of an unsigned long long, if that is possible, or else 
     * Shows /throws an error if desired, always warns if node is not of type Element
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \param attribute_name Name of the attribute of which to get the value
     * \param throw_error Optional parameter, throw_error / throw error if the attribute does not exists only if desired (it might not be required by specs)
     * \return An unsigned long long value containing the content of the Element, or 0 in case of an error
     */
    unsigned long long get_attribute_uint(const xmlpp::Node* node, std::string attribute_name, bool throw_error = false);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Then, it gets its content in form of a double, if that is possible, or else 
     * Shows /throws an error if desired, always warns if node is not of type Element
     * \param node An XML node, assumed to be of type Element (which must not be checked by the user)
     * \param attribute_name Name of the attribute of which to get the value
     * \param throw_error Optional parameter, throw_error / throw error if the attribute does not exists only if desired (it might not be required by specs)
     * \return An double value containing the content of the Element, or -1.0 in case of an error
     */
    double get_attribute_double(const xmlpp::Node* node, std::string attribute_name, bool throw_error = false);

    //**********************************************************************
    //Helper functions
    //**********************************************************************

    /**
     * \brief Gets a string (if desired)
     * Transforms the string to an int, (if desired) if no transformation is possible
     * \param text A string that represents an int value
     * \return The int value of the string, else -1
     */
    int string_to_int(std::string text);

    /**
     * \brief Gets a string (if desired)
     * Transforms the string to an unsigned long long, (if desired) if no transformation is possible
     * \param text A string that represents an unsigned long long value
     * \return The unsigned long long value of the string, else 0
     */
    unsigned long long string_to_uint(std::string text);

    /**
     * \brief Gets a string (if desired)
     * Transforms the string to an double, (if desired) if no transformation is possible
     * \param text A string that represents an double value
     * \return The double value of the string, else -1.0
     */
    double string_to_double(std::string text);

    //**********************************************************************
    //Iteration functions
    //**********************************************************************

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * Then, it iterates through all its childs and passes these to the node_function, which then can use these childs as desired
     * \param node An XML node 
     * \param node_function Function that takes a node 
     * \param child_name Expected name of the child node - optional, return all if not set
     */
    void iterate_children(const xmlpp::Node* node, std::function<void (xmlpp::Node* node)> node_function, std::string child_name);

    /**
     * \brief Takes a node as input, assuming it is of type Element (which is tested within the function, so that the user does not have to do it, thus xmlpp::Node, not xmlpp::Element was chosen)
     * Always warns if it is not an Element node 
     * Then, the attribute of all elements with the given attribute are given one by one to the function
     * Example: <a ref="1"/> <a ref="2"/> ... -> function gets "1", "2", ...
     * \param node An XML node 
     * \param node_function Function that takes an attribute
     * \param node_name Expected name of the node
     * \param attribute_name Expected name of the attribute
     */
    void iterate_elements_with_attribute(const xmlpp::Node* node, std::function<void (std::string)> attribute_function, std::string node_name, std::string attribute_name);
}