#pragma once

#include "defaults.hpp"
#include <gtkmm/builder.h>
#include <gtkmm.h>
#include "ui/manual_control/VehicleManualControlUi.hpp"



class MainWindow
{
public:
    Glib::RefPtr<Gtk::Builder> builder_master_layout;

    Gtk::Window* window_LCC = nullptr;
    Gtk::Box* box_manual_control = nullptr;
    Gtk::Box* box_map = nullptr;
    Gtk::Box* box_data_grid = nullptr;
    Gtk::Paned* pane1 = nullptr;
    Gtk::Paned* pane2 = nullptr;

public:
    MainWindow(std::shared_ptr<VehicleManualControlUi> _vehicleManualControlUi);
};