#include "MainWindow.hpp"



MainWindow::MainWindow(std::shared_ptr<VehicleManualControlUi> _vehicleManualControlUi)
{
    builder_master_layout = Gtk::Builder::create_from_file("ui/master_layout.glade");



    builder_master_layout->get_widget("window_LCC", window_LCC);
    builder_master_layout->get_widget("box_manual_control", box_manual_control);
    builder_master_layout->get_widget("box_map", box_map);
    builder_master_layout->get_widget("box_data_grid", box_data_grid);
    builder_master_layout->get_widget("paned1", pane1);
    builder_master_layout->get_widget("paned2", pane2);


    assert(window_LCC);
    assert(box_manual_control);
    assert(box_map);
    assert(box_data_grid);
    assert(pane1);
    assert(pane2);

    window_LCC->show();
    window_LCC->set_size_request(800, 600);

    pane1->set_position(400);
    pane2->set_position(400);


    Gtk::Label* label = Gtk::manage(new Gtk::Label());

    box_map->pack_start(*label,true,true);
    box_data_grid->pack_start(*label,true,true);

    label->set_text("asdasd");

    label->set_width_chars(10);
    label->set_xalign(1);
    label->show_all();


    Gtk::Widget* box1 = nullptr;
    box1 = _vehicleManualControlUi->get_parent();
    box_manual_control->pack_start(*box1,true,true);
    box1->show();
}

