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

#include "CommonroadViewUI.hpp"

CommonroadViewUI::CommonroadViewUI
    (
    std::shared_ptr<CommonRoadScenario> _commonroad_scenario,
    std::function<void(int, ObstacleToggle::ToggleState state)> _set_obstacle_manager_obstacle_state
    ) 
    :
    commonroad_scenario(_commonroad_scenario),
    set_obstacle_manager_obstacle_state(_set_obstacle_manager_obstacle_state)
{
    builder = Gtk::Builder::create_from_file("ui/commonroad/commonroad.glade");

    builder->get_widget("parent", parent);
    builder->get_widget("commonroad_box", commonroad_box);
    builder->get_widget("commonroad_path", commonroad_path);
    builder->get_widget("entry_time_step_size", entry_time_step_size);
    builder->get_widget("entry_lane_width", entry_lane_width);
    builder->get_widget("entry_translate_x", entry_translate_x);
    builder->get_widget("entry_translate_y", entry_translate_y);
    builder->get_widget("button_choose_commonroad", button_choose_commonroad);
    builder->get_widget("button_load_commonroad", button_load_commonroad);
    builder->get_widget("button_apply_transformation", button_apply_transformation);
    builder->get_widget("static_obstacles_flowbox", static_obstacles_flowbox);
    builder->get_widget("dynamic_obstacles_flowbox", dynamic_obstacles_flowbox);
    builder->get_widget("problem_treeview", problem_treeview);
    builder->get_widget("problem_scrolled_window", problem_scrolled_window);
    builder->get_widget("button_load_profile", button_load_profile);
    builder->get_widget("button_save_profile", button_save_profile);
    builder->get_widget("button_reset_profile", button_reset_profile);

    assert(parent);
    assert(commonroad_box);
    assert(commonroad_path);
    assert(entry_time_step_size);
    assert(entry_lane_width);
    assert(entry_translate_x);
    assert(entry_translate_y);
    assert(button_choose_commonroad);
    assert(button_load_commonroad);
    assert(button_apply_transformation);
    assert(static_obstacles_flowbox);
    assert(dynamic_obstacles_flowbox);
    assert(problem_treeview);
    assert(problem_scrolled_window);
    assert(button_load_profile);
    assert(button_save_profile);
    assert(button_reset_profile);

    //Register button callbacks
    button_choose_commonroad->signal_clicked().connect(sigc::mem_fun(this, &CommonroadViewUI::open_file_explorer));
    button_load_commonroad->signal_clicked().connect(sigc::mem_fun(this, &CommonroadViewUI::load_button_callback));
    button_apply_transformation->signal_clicked().connect(sigc::mem_fun(this, &CommonroadViewUI::apply_transformation));
    button_load_profile->signal_clicked().connect(sigc::mem_fun(this, &CommonroadViewUI::load_transformation_from_profile));
    button_save_profile->signal_clicked().connect(sigc::mem_fun(this, &CommonroadViewUI::store_transform_profile));
    button_reset_profile->signal_clicked().connect(sigc::mem_fun(this, &CommonroadViewUI::reset_current_transform_profile));

    //Also, single transformation values can be applied on a single key press within the entry
    entry_time_step_size->signal_key_release_event().connect(sigc::mem_fun(this, &CommonroadViewUI::apply_entry_time));
    entry_lane_width->signal_key_release_event().connect(sigc::mem_fun(this, &CommonroadViewUI::apply_entry_scale));
    entry_translate_x->signal_key_release_event().connect(sigc::mem_fun(this, &CommonroadViewUI::apply_entry_translate_x));
    entry_translate_y->signal_key_release_event().connect(sigc::mem_fun(this, &CommonroadViewUI::apply_entry_translate_y));

    //Information on transformation on hover
    entry_time_step_size->set_tooltip_text("Set time step size for the simulation. <= 0 means no change desired. Applies w. Return.");
    entry_lane_width->set_tooltip_text("Set min. lane width. <= 0 means no change desired. Also applies w. Return.");
    entry_translate_x->set_tooltip_text("Set x translation. 0 means no change desired. Applied after scale change. Also applies w. Return.");
    entry_translate_y->set_tooltip_text("Set y translation. 0 means no change desired. Applied after scale change. Also applies w. Return.");
    button_apply_transformation->set_tooltip_text("Permanently apply set transformation to coordinate system. Future transformations are applied relative to new coordinate system.");

    //Set current time step size as initial text for entry
    std::stringstream current_time_step_size_stream;
    if (commonroad_scenario)
    {
        current_time_step_size_stream << commonroad_scenario->get_time_step_size();
    }
    entry_time_step_size->set_text(current_time_step_size_stream.str().c_str());

    //Setup for planning problem treeview
    //Create model for view
    problem_list_store = Gtk::ListStore::create(problem_record);

    //Use model_record, add it to the view
    problem_treeview->append_column("Problem ID", problem_record.problem_id);
    problem_treeview->append_column("Goal Speed", problem_record.problem_goal_speed);
    problem_treeview->append_column("Goal Time (sec.)", problem_record.problem_goal_time);
    problem_treeview->set_model(problem_list_store);

    problem_treeview->get_column(0)->set_resizable(true);
    problem_treeview->get_column(0)->set_expand(true);

    //Create UI thread and register dispatcher callback
    ui_dispatcher.connect(sigc::mem_fun(*this, &CommonroadViewUI::dispatcher_callback));
    run_thread.store(true);
    ui_thread = std::thread(&CommonroadViewUI::update_ui, this);

    //Set tooltip
    problem_treeview->set_has_tooltip(true);
    problem_treeview->signal_query_tooltip().connect(sigc::mem_fun(*this, &CommonroadViewUI::tooltip_callback));

    //Try to load planning problems from current translation, if they exist
    reload_problems.store(true);
    //Also load the obstacle list
    load_obstacle_list.store(true);

    //Set initial text of script path (from previous program execution, if that existed)
    commonroad_path->set_text(FileChooserUI::get_last_execution_path(config_file_location));

    //Load yaml profile
    load_transform_profile();
    profile_applied.store(false);
}

using namespace std::placeholders;
void CommonroadViewUI::dispatcher_callback() {
    if (reload_problems.exchange(false))
    {
        //Reset time step size
        std::stringstream current_time_step_size_stream;
        current_time_step_size_stream << commonroad_scenario->get_time_step_size();
        entry_time_step_size->set_text(current_time_step_size_stream.str().c_str());

        //Get current number of elements
        size_t count = 0;
        for (auto iter = problem_list_store->children().begin(); iter != problem_list_store->children().end(); ++iter) {
            ++count;
        }

        //Delete them all
        for (size_t i = 0; i < count; ++i) { 
            auto iter = problem_list_store->children().begin();
            problem_list_store->erase(iter);
        }

        //Load current planning problems
        for (auto planning_problem_id : commonroad_scenario->get_planning_problem_ids())
        {
            //We still check for the existence of the problem, as the file may have been reloaded in between
            auto planning_problem = commonroad_scenario->get_planning_problem(planning_problem_id);
            if (!planning_problem.has_value())
            {
                break;
            }

            std::stringstream id_stream; 
            id_stream << planning_problem_id;
            Glib::ustring id_ustring(id_stream.str());

            for (auto planning_problem_element : planning_problem->get_planning_problems())
            {
                for (auto goal_state : planning_problem_element.goal_states)
                {
                    //Get goal speed(s)
                    std::stringstream goal_stream; 

                    auto goal_velocity = goal_state.get_velocity();
                    if (goal_velocity.has_value())
                    {
                        for (auto it = goal_velocity.value().cbegin(); it != goal_velocity.value().cend(); ++it)
                        {
                            goal_stream << " [" << it->first << ", " << it->second << "] ";
                        }
                    }
                    else
                    {
                        goal_stream << "Not specified";
                    }

                    Glib::ustring goal_speed_ustring(goal_stream.str());

                    //Get goal time(s)
                    goal_stream.str( std::string() );
                    goal_stream.clear();
                    auto time_step_size = commonroad_scenario->get_time_step_size();

                    auto goal_time = goal_state.get_time();
                    if (goal_time.has_value())
                    {
                        auto exact_value = goal_time->get_exact_value();
                        if (exact_value.has_value())
                        {
                            goal_stream << exact_value.value() * time_step_size;
                        }

                        auto interval = goal_time->get_interval();
                        if (interval.has_value())
                        {
                            for (auto it = interval->cbegin(); it != interval->cend(); ++it)
                            {
                                goal_stream << " [" << it->first * time_step_size << ", " << it->second * time_step_size << "] ";
                            }
                        }
                    }
                    else
                    {
                        goal_stream << "Not specified";
                    }

                    Glib::ustring goal_time_ustring(goal_stream.str());

                    Gtk::TreeModel::Row row;
                    row = *(problem_list_store->append());
                    
                    row[problem_record.problem_id] = id_ustring;
                    row[problem_record.problem_goal_speed] = goal_speed_ustring;
                    row[problem_record.problem_goal_time] = goal_time_ustring;
                }
            }
        }
    }
    if (load_obstacle_list.exchange(false))
    {
        //Remove old vehicle toggles
        for (auto& vehicle_toggle : static_vehicle_toggles)
        {
            static_obstacles_flowbox->remove(*(vehicle_toggle->get_parent()->get_parent()));
        }
        for (auto& vehicle_toggle : dynamic_vehicle_toggles)
        {
            dynamic_obstacles_flowbox->remove(*(vehicle_toggle->get_parent()->get_parent()));
        }
        static_vehicle_toggles.clear();
        dynamic_vehicle_toggles.clear();

        
        //Create vehicle toggles for static and dynamic IDs
        //Set vehicle toggles to "simulation" by default
        //Set listener for vehicle toggle state changes
        for (auto id : commonroad_scenario->get_static_obstacle_ids())
        {
            static_vehicle_toggles.emplace_back(std::make_shared<ObstacleToggle>(id));
        }
        for (auto& vehicle_toggle : static_vehicle_toggles)
        {
            static_obstacles_flowbox->add(*(vehicle_toggle->get_parent()));
            vehicle_toggle->set_selection_callback(std::bind(&CommonroadViewUI::vehicle_selection_changed, this, _1, _2));
            vehicle_toggle->set_state(ObstacleToggle::ToggleState::Simulated);
        }

        for (auto id : commonroad_scenario->get_dynamic_obstacle_ids())
        {
            dynamic_vehicle_toggles.emplace_back(std::make_shared<ObstacleToggle>(id));
        }
        for (auto& vehicle_toggle : dynamic_vehicle_toggles)
        {
            dynamic_obstacles_flowbox->add(*(vehicle_toggle->get_parent()));
            vehicle_toggle->set_selection_callback(std::bind(&CommonroadViewUI::vehicle_selection_changed, this, _1, _2));
            vehicle_toggle->set_state(ObstacleToggle::ToggleState::Simulated);
        }
    }
}

void CommonroadViewUI::apply_current_vehicle_selection()
{
    for (auto& vehicle_toggle : static_vehicle_toggles)
    {
        vehicle_selection_changed(vehicle_toggle->get_id(), vehicle_toggle->get_state());
    } 

    for (auto& vehicle_toggle : dynamic_vehicle_toggles)
    {
        vehicle_selection_changed(vehicle_toggle->get_id(), vehicle_toggle->get_state());
    }
}

void CommonroadViewUI::vehicle_selection_changed(unsigned int id, ObstacleToggle::ToggleState state)
{
    if(set_obstacle_manager_obstacle_state)
    {
        set_obstacle_manager_obstacle_state(static_cast<int>(id), state);
    }
}


void CommonroadViewUI::update_ui() {
    while (run_thread.load()) {
        ui_dispatcher.emit();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

bool CommonroadViewUI::tooltip_callback(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
    int cell_x, cell_y = 0;
    Gtk::TreeModel::Path path;
    Gtk::TreeViewColumn* column;
    bool path_exists;

    //Get the current path and column at the selected point
    if (keyboard_tooltip) {
        problem_treeview->get_cursor(path, column);
        path_exists = column != nullptr;
    }
    else {
        int window_x, window_y;
        problem_treeview->convert_widget_to_bin_window_coords(x, y, window_x, window_y);
        path_exists = problem_treeview->get_path_at_pos(window_x, window_y, path, column, cell_x, cell_y);
    }

    if (path_exists) {
        //Get selected row
        Gtk::TreeModel::iterator iter = problem_list_store->get_iter(path);
        Gtk::TreeModel::Row row = *iter;

        //Get tooltip text depending on current column
        Glib::ustring content_ustring;
        if (column->get_title() == "Problem ID") {
            content_ustring = Glib::ustring(row[problem_record.problem_id]);
        } 
        else if (column->get_title() == "Goal Speed") {
            content_ustring = Glib::ustring(row[problem_record.problem_goal_speed]);
        }
        else if (column->get_title() == "Goal Time (sec.)") {
            content_ustring = Glib::ustring(row[problem_record.problem_goal_time]);
        }

        //Get text at iter
        tooltip->set_text(content_ustring);
        return true;
    }
    else {
        return false;
    }
}

using namespace std::placeholders;
void CommonroadViewUI::open_file_explorer()
{
    //Filter to show only XML files
    FileChooserUI::Filter xml_filter;
    xml_filter.name = "XML";
    xml_filter.pattern_filter_types = std::vector<std::string> {"*.xml"};

    //Only create the window if we can get the main window
    if (get_main_window)
    {
        file_chooser_window = std::make_shared<FileChooserUI>(
            get_main_window(), 
            std::bind(&CommonroadViewUI::file_explorer_callback, this, _1, _2), 
            std::vector<FileChooserUI::Filter> { xml_filter },
            config_file_location
        );
    }
    else
    {
        cpm::Logging::Instance().write(
            1,
            "%s",
            "ERROR: Main window reference is missing, cannot create file chooser dialog"
        );
    }
    
}

void CommonroadViewUI::file_explorer_callback(std::string file_string, bool has_file)
{
    if (has_file)
    {
        commonroad_path->set_text(file_string.c_str());

        //Load chosen file - this function is also used for a button callback and thus does not take the file path as a parameter
        load_chosen_file();

        //Reload/reset shown planning problems
        reload_problems.store(true);
        load_obstacle_list.store(true);
    }
}

double CommonroadViewUI::string_to_double(std::string value, double default_value)
{
    //Disregard empty fields / use default then
    if (value.size() > 0)
    {
        try
        {
            return std::stod(value);
        }
        catch(...)
        {
            std::cerr << "Could not translate string to double (transformation string entry : scale or translate_x/_y)" << std::endl;
            return default_value;
        }
    }

    return default_value;
}

void CommonroadViewUI::apply_transformation()
{
    //Get desired lane width and translation
    double lane_width = string_to_double(std::string(entry_lane_width->get_text().c_str()), 0.0);
    double translate_x = string_to_double(std::string(entry_translate_x->get_text().c_str()), 0.0);
    double translate_y = string_to_double(std::string(entry_translate_y->get_text().c_str()), 0.0);

    if (commonroad_scenario)
    {
        commonroad_scenario->transform_coordinate_system(lane_width, translate_x, translate_y);
    }

    //Re-enter vehicle selection for obstacle simulation manager
    apply_current_vehicle_selection();

    //Remember change in profile yaml
    add_change_to_transform_profile(0.0, lane_width, translate_x, translate_y);

    entry_lane_width->set_text("");
    entry_translate_x->set_text("");
    entry_translate_y->set_text("");
}

bool CommonroadViewUI::apply_entry_time(GdkEventKey* event)
{
    if (event->type == GDK_KEY_RELEASE && event->keyval == GDK_KEY_Return)
    {
        //Get desired lane width and translation
        double new_step_size = string_to_double(std::string(entry_time_step_size->get_text().c_str()), 0.0);

        if (commonroad_scenario)
        {
            commonroad_scenario->set_time_step_size(new_step_size);
        }

        //Re-enter vehicle selection for obstacle simulation manager
        apply_current_vehicle_selection();

        //Remember change in profile yaml
        add_change_to_transform_profile(new_step_size, 0.0, 0.0, 0.0);
        return true;
    }
    return false;
}

bool CommonroadViewUI::apply_entry_scale(GdkEventKey* event)
{
    if (event->type == GDK_KEY_RELEASE && event->keyval == GDK_KEY_Return)
    {
        //Get desired lane width and translation
        double lane_width = string_to_double(std::string(entry_lane_width->get_text().c_str()), 0.0);

        if (commonroad_scenario)
        {
            commonroad_scenario->transform_coordinate_system(lane_width, 0.0, 0.0);
        }

        //Re-enter vehicle selection for obstacle simulation manager
        apply_current_vehicle_selection();

        //Remember change in profile yaml
        add_change_to_transform_profile(0.0, lane_width, 0.0, 0.0);

        entry_lane_width->set_text("");

        return true;
    }
    return false;
}

bool CommonroadViewUI::apply_entry_translate_x(GdkEventKey* event)
{
    if (event->type == GDK_KEY_RELEASE && event->keyval == GDK_KEY_Return)
    {
        //Get desired lane width and translation
        double translate_x = string_to_double(std::string(entry_translate_x->get_text().c_str()), 0.0);

        if (commonroad_scenario)
        {
            commonroad_scenario->transform_coordinate_system(0.0, translate_x, 0.0);
        }

        //Re-enter vehicle selection for obstacle simulation manager
        apply_current_vehicle_selection();
        
        //Remember change in profile yaml
        add_change_to_transform_profile(0.0, 0.0, translate_x, 0.0);

        entry_translate_x->set_text("");

        return true;
    }
    return false;
}

bool CommonroadViewUI::apply_entry_translate_y(GdkEventKey* event)
{
    if (event->type == GDK_KEY_RELEASE && event->keyval == GDK_KEY_Return)
    {
        //Get desired lane width and translation
        double translate_y = string_to_double(std::string(entry_translate_y->get_text().c_str()), 0.0);

        if (commonroad_scenario)
        {
            commonroad_scenario->transform_coordinate_system(0.0, 0.0, translate_y);
        }

        //Re-enter vehicle selection for obstacle simulation manager
        apply_current_vehicle_selection();

        //Remember change in profile yaml
        add_change_to_transform_profile(0.0, 0.0, 0.0, translate_y);

        entry_translate_y->set_text("");

        return true;
    }
    return false;
}

void CommonroadViewUI::load_button_callback()
{
    //Load chosen file
    load_chosen_file();

    //Reload/reset shown planning problems
    reload_problems.store(true);
    load_obstacle_list.store(true);
}

void CommonroadViewUI::load_chosen_file()
{
    std::string filepath = std::string(commonroad_path->get_text().c_str());

    try
    {
        std::lock_guard<std::mutex> lock(yaml_profile_mutex);

        commonroad_scenario->load_file(filepath);
        profile_applied.store(false);

        //Re-enter vehicle selection for obstacle simulation manager
        apply_current_vehicle_selection();

        //Reset yaml profile to last saved state / get rid of changes that were not supposed to be stored
        yaml_transform_profile = YAML::Clone(old_yaml_transform_profile);
    }
    catch(const std::exception& e)
    {
        std::stringstream error_msg_stream;
        error_msg_stream << "The chosen scenario file could not be loaded / is not spec-conform. Error message is:\n";
        error_msg_stream << e.what();
        if (get_main_window)
        {
            Gtk::MessageDialog load_failed_dialog = Gtk::MessageDialog(
                get_main_window(),
                error_msg_stream.str(),
                false,
                Gtk::MessageType::MESSAGE_INFO,
                Gtk::ButtonsType::BUTTONS_OK,
                true
            );
            load_failed_dialog.run();
        }
        else
        {
            std::cerr << "Could not load error dialog (UI) - main window callback not set for CommonroadViewUI!" << std::endl;
        }
    }
}

void CommonroadViewUI::set_main_window_callback(std::function<Gtk::Window&()> _get_main_window)
{
    get_main_window = _get_main_window;
}

void CommonroadViewUI::set_sensitive(bool is_sensitive)
{
    parent->set_sensitive(is_sensitive);
    commonroad_box->set_sensitive(is_sensitive);
    commonroad_path->set_sensitive(is_sensitive);
    entry_lane_width->set_sensitive(is_sensitive);
    entry_translate_x->set_sensitive(is_sensitive);
    entry_translate_y->set_sensitive(is_sensitive);
    button_choose_commonroad->set_sensitive(is_sensitive);
    button_load_commonroad->set_sensitive(is_sensitive);
    button_apply_transformation->set_sensitive(is_sensitive);
    problem_treeview->set_sensitive(is_sensitive);
    problem_scrolled_window->set_sensitive(is_sensitive);
}

Gtk::Widget* CommonroadViewUI::get_parent()
{
    return parent;
}

//-------------------------------------------------------- YAML / Profile for transformation ------------------------------
void CommonroadViewUI::load_transform_profile()
{
    //Make sure that the file exists, else create a file first
    std::experimental::filesystem::path filepath = transformation_file_location;
    bool file_exists = std::experimental::filesystem::exists(filepath);

    if (!file_exists)
    {
        //Create file
        std::ofstream yaml_file(transformation_file_location);
        yaml_file.close();
    }

    //Now load the profile
    std::lock_guard<std::mutex> lock(yaml_profile_mutex);
    yaml_transform_profile = YAML::LoadFile(transformation_file_location);
    old_yaml_transform_profile = YAML::Clone(yaml_transform_profile);
}

void CommonroadViewUI::load_transformation_from_profile()
{
    std::lock_guard<std::mutex> lock(yaml_profile_mutex);

    if (current_file_name != "" && old_yaml_transform_profile[current_file_name])
    {
        //Load profile
        auto profile = old_yaml_transform_profile[current_file_name];

        //Make sure that values exist
        if (! (profile[time_scale_key] && profile[scale_key] && profile[translate_x_key] && profile[translate_y_key]))
        {
            cpm::Logging::Instance().write(1, "Commonroad profile not properly defined for %s in %s", current_file_name.c_str(), transformation_file_location.c_str());
        }
    
        //Load values from profile
        auto time_scale = profile[time_scale_key].as<double>();
        auto scale = profile[scale_key].as<double>();
        auto translate_x = profile[translate_x_key].as<double>();
        auto translate_y = profile[translate_y_key].as<double>();

        if (commonroad_scenario)
        {
            //TODO: Load must reset
            std::cout << "Start tr" << std::endl;
            //WARNING Complications possible if in between these steps the scenario is reloaded
            commonroad_scenario->transform_coordinate_system(scale, translate_x, translate_y);
            std::cout << "Stop tr, start t" << std::endl;
            commonroad_scenario->set_time_step_size(time_scale);
            std::cout << "Stop t" << std::endl;
        }
    }

    profile_applied.store(true);
}

void CommonroadViewUI::store_transform_profile()
{
    std::lock_guard<std::mutex> lock(yaml_profile_mutex);

    //Store changed profile
    std::ofstream yaml_file(transformation_file_location, std::ofstream::out | std::ofstream::trunc);
    yaml_file << yaml_transform_profile;
    yaml_file.close();

    //Now also keep the change for loading from the newly set profile (copy)
    old_yaml_transform_profile = YAML::Clone(yaml_transform_profile);
}

void CommonroadViewUI::add_change_to_transform_profile(double time_scale, double scale, double translate_x, double translate_y)
{
    std::lock_guard<std::mutex> lock(yaml_profile_mutex);

    if (current_file_name != "")
    {
        //Load profile
        auto profile = yaml_transform_profile[current_file_name];

        double translate_x_old = 0.0;
        double translate_y_old = 0.0;

        //Only regard old values if they have been loaded before
        if (profile && profile_applied.load())
        {
            //Make sure that values exist
            if (! (profile[time_scale_key] && profile[scale_key] && profile[translate_x_key] && profile[translate_y_key]))
            {
                cpm::Logging::Instance().write(1, "Commonroad profile not properly defined for %s in %s", current_file_name.c_str(), transformation_file_location.c_str());
            }

            //Load old values from profile
            translate_x_old = profile[translate_x_key].as<double>();
            translate_y_old = profile[translate_y_key].as<double>();
        }

        //Store values
        if ( time_scale > 0 ) profile[time_scale_key] = time_scale;
        if ( scale > 0 ) profile[scale_key] = scale;
        profile[translate_x_key] = translate_x_old + translate_x;
        profile[translate_y_key] = translate_y_old + translate_y;

        //Now we are in a situation where, if the profile was applied before or not, we now use the same profile as the one stored in the profile node
        //As changes are additive from here on, we need to reuse old values
        profile_applied.store(true);
    }
}

void CommonroadViewUI::reset_current_transform_profile()
{
    std::lock_guard<std::mutex> lock(yaml_profile_mutex);

    if (current_file_name != "")
    {
        //Load profile
        auto profile = yaml_transform_profile[current_file_name];

        if (profile)
        {
            yaml_transform_profile.remove(current_file_name);
        }

        old_yaml_transform_profile = YAML::Clone(yaml_transform_profile);
    }
}
