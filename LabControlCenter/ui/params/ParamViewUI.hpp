#pragma once

#include "defaults.hpp"
#include "ParamModelRecord.hpp"
#include "ParamsCreateView.hpp"

#include <gtkmm/builder.h>
#include <gtkmm.h>
#include <gtkmm/liststore.h>
#include <cassert>
#include <string>
#include <atomic>
#include <memory>

class ParamViewUI {
private:
    Glib::RefPtr<Gtk::Builder> params_builder;

    Gtk::Widget* parent;

    //Top box: search bar and filter
    Gtk::FlowBox* parameters_flow_top;
    Gtk::SearchEntry* parameters_search;
    Gtk::Box* parameters_box_filter;
    Gtk::Label* parameters_filter_description;
    Gtk::ComboBoxText* parameters_filter;

    //Middle box: Parameter list
    Gtk::ScrolledWindow* parameters_list_scroll_window;
    Gtk::TreeView* parameters_list_tree;

    //Bottom box: Buttons like edit, delete, new
    Gtk::FlowBox* parameters_box_buttons;
    Gtk::FlowBoxChild* parameters_box_delete;
    Gtk::Button* parameters_button_delete;
    Gtk::FlowBoxChild* parameters_box_edit;
    Gtk::Button* parameters_button_edit;
    Gtk::FlowBoxChild* parameters_box_create;
    Gtk::Button* parameters_button_create;

    //TreeView Layout, Parameters storage
    ParamModelRecord model_record;
    Glib::RefPtr<Gtk::ListStore> parameter_list_storage;

    //Edit / create window
    std::shared_ptr<ParamsCreateView> create_window;
    std::atomic<bool> parameter_view_unchangeable; //If true, another window is opened that might modify parameters, thus other modification is not allowed
    bool create_window_open = false; //To check whether a new parameter was added or if the selected parameter was modified

    //Manipulate rows
    bool get_selected_row(std::string &name, std::string &type, std::string &value, std::string &info);
    //"Callback" function: Delete the row selected by the user if the delete button was clicked
    void delete_selected_row();
    //Open edit / create window
    //TODO: Properly destroy window, rewrite ParamsCreateView (callback übergeben? sonst ParamsCreateView selbst alles managen lassen (create() Fkt.))
    void open_param_create_window();
public:
    ParamViewUI();
    Gtk::Widget* get_parent();
    
    //Callback: Allow to only create another create window when the former window was closed
    //Handles callback for close and for create operations
    void window_on_close_callback(std::string name = "", std::string type = "", std::string value = "", std::string info = "");
};