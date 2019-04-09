#pragma once

#include <gtkmm.h>
#include "defaults.hpp"
#include "TimeSeries.hpp"
#include "cpm/Timer.hpp"

using VehicleData = map<uint8_t, map<string, shared_ptr<TimeSeries> > >;

class MapViewUi
{
    shared_ptr<Gtk::Window> window;
    Gtk::DrawingArea* drawingArea;
    std::function<VehicleData()> get_vehicle_data;
    Glib::Dispatcher update_dispatcher;
    shared_ptr<cpm::Timer> update_loop;
    Cairo::RefPtr<Cairo::ImageSurface> image_car;
    double zoom = 300.0;
    double pan_x = 0;
    double pan_y = 0;

public:
    explicit MapViewUi(std::function<VehicleData()> get_vehicle_data_callback);
    Gtk::Window& get_window();    
};