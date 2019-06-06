#pragma once

/**
 * \class ParameterStorage
 * \brief This class can be used in combination with yaml files and the parameter server. Do not confuse this class with the class in the cpm lib! That class is used on the client's side.
 */

#include "ParameterWithDescription.hpp"

#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <cassert>
#include <iostream>
#include <fstream>
#include <exception>

class ParameterStorage {
public:
    //Constructor
    ParameterStorage(std::string _filename, int precision);

    //Delete move and copy op
    ParameterStorage(ParameterStorage const&) = delete;
    ParameterStorage(ParameterStorage&&) = delete; 
    ParameterStorage& operator=(ParameterStorage const&) = delete;
    ParameterStorage& operator=(ParameterStorage &&) = delete;

    /**
     * \brief used to register a callback that is called whenever the value of a parameter changes
     * \param _on_param_changed_callback the callback function
     */
    void register_on_param_changed_callback(std::function<void(std::string)> _on_param_changed_callback);

    /**
     * \brief Load YAML file into memory, use mutex
     * \param _filename Change the current filename and use the given parameter for that, thus switch to the new file
     */
    void loadFile(std::string _filename);
    void loadFile();
    /**
     * \brief Store current configuration in YAML file
     * \param _filename Change the current filename and use the given parameter for that, thus switch to the new file
     */
    void storeFile(std::string _filename);
    void storeFile();
    /**
     * \brief Set the value of a parameter
     * \param name name of the parameter
     * \param value new value of the parameter
     * \param info description of the parameter's purpose
     */
    void set_parameter_bool(std::string name, bool value, std::string info = "");
    void set_parameter_int(std::string name, int32_t value, std::string info = "");
    void set_parameter_double(std::string name, double value, std::string info = "");
    void set_parameter_string(std::string name, std::string value, std::string info = "");
    void set_parameter_string(std::string name, const char* value, std::string info = "");
    void set_parameter_ints(std::string name, std::vector<int32_t> value, std::string info = "");
    void set_parameter_doubles(std::string name, std::vector<double> value, std::string info = "");
    void set_parameter(std::string name, ParameterWithDescription param);
    /**
     * \brief Get the value of a parameter
     * \param name name of the parameter
     * \param value value of the parameter
     * \param info info value of the parameter
     * \return true if the parameter exists
     */
    bool get_parameter_bool(std::string name, bool& value);
    bool get_parameter_int(std::string name, int32_t& value);
    bool get_parameter_double(std::string name, double& value);
    bool get_parameter_string(std::string name, std::string& value);
    bool get_parameter_ints(std::string name, std::vector<int32_t>& value);
    bool get_parameter_doubles(std::string name, std::vector<double>& value);
    bool get_parameter(std::string name, ParameterWithDescription& param);

    /**
     * \brief Delete a parameter
     * \param The parameter's name
     */
    void delete_parameter(std::string name);

    /**
     * \brief List all existing parameter names of that type
     */
    std::vector<std::string> list_bool();
    std::vector<std::string> list_int();
    std::vector<std::string> list_double();
    std::vector<std::string> list_string();
    std::vector<std::string> list_ints();
    std::vector<std::string> list_doubles();

    std::vector<ParameterWithDescription> get_all_parameters();

    int get_precision();
private:
    std::vector<std::string> list_names(ParameterType type);

    /**
     * Float / double precision for YAML
     */
    int PRECISION = 32;

    std::string filename;

    //Parameter storage
    std::map<std::string, ParameterWithDescription> param_storage;

    //Mutex for the map
    std::mutex param_storage_mutex;

    //Callback that gets called whenever a parameter changes
    std::function<void(std::string)> on_param_changed_callback;
};