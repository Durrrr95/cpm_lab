#include "defaults.hpp"
#include "stdio.h"
#include <unistd.h>
#include <dds/pub/ddspub.hpp>
#include <dds/sub/ddssub.hpp>
#include "TimeSeriesAggregator.hpp"
#include "VisualizationCommandsAggregator.hpp"
#include "VehicleManualControl.hpp"
#include "ui/monitoring/MonitoringUi.hpp"
#include "ui/manual_control/VehicleManualControlUi.hpp"
#include "ui/map_view/MapViewUi.hpp"
#include "ui/right_tabs/TabsViewUI.hpp"
#include "ui/params/ParamViewUI.hpp"
#include "ui/timer/TimerViewUI.hpp"
#include "ui/logger/LoggerViewUI.hpp"
#include "ui/setup/SetupViewUI.hpp"
#include "LogStorage.hpp"
#include "ParameterServer.hpp"
#include "ParameterStorage.hpp"
#include "TrajectoryCommand.hpp"
#include "ui/MainWindow.hpp"
#include "cpm/Logging.hpp"
#include "cpm/CommandLineReader.hpp"
#include "TimerTrigger.hpp"
#include "cpm/init.hpp"

#include <gtkmm/builder.h>
#include <gtkmm.h>
#include <functional>
#include <sstream>

//For exit handlers
#include <signal.h>
#include <stdlib.h>
#include <cstdlib>

using namespace std::placeholders;

void deploy_ips(int cmd_domain_id, std::string cmd_dds_initial_peer) {
    //Generate command
    std::stringstream command_ips;
    command_ips 
        << "tmux new-session -d "
        << "-s \"ips_pipeline\" "
        << "\"cd ~/dev/software/ips2/;./build/ips_pipeline "
        << " --dds_domain=" << cmd_domain_id;
    if (cmd_dds_initial_peer.size() > 0) {
        command_ips 
            << " --dds_initial_peer=" << cmd_dds_initial_peer;
    }
    command_ips 
        << " >stdout_ips.txt 2>stderr_ips.txt\"";

    //Generate command
    std::stringstream command_basler;
    command_basler 
        << "tmux new-session -d "
        << "-s \"ips_basler\" "
        << "\"cd ~/dev/software/ips2/;./build/BaslerLedDetection "
        << " --dds_domain=" << cmd_domain_id;
    if (cmd_dds_initial_peer.size() > 0) {
        command_basler 
            << " --dds_initial_peer=" << cmd_dds_initial_peer;
    }
    command_basler 
        << " >stdout_basler.txt 2>stderr_basler.txt\"";

    //Execute command
    system(command_ips.str().c_str());
    system(command_basler.str().c_str());
}

void kill_ips() {
    //Generate command
    std::stringstream command_ips;
    command_ips 
        << "tmux kill-session -t \"ips_pipeline\"";

    //Generate command
    std::stringstream command_basler;
    command_basler
        << "tmux kill-session -t \"ips_basler\"";

    //Execute command
    system(command_ips.str().c_str());
    system(command_basler.str().c_str());
}

void deploy_cloud_discovery() {
    std::string command = "tmux new-session -d -s \"rticlouddiscoveryservice\" \"rticlouddiscoveryservice -transport 25598\"";
    system(command.c_str());
}

void kill_cloud_discovery() {
    std::string command = "tmux kill-session -t \"rticlouddiscoveryservice\"";
    system(command.c_str());
}

void interrupt_handler(int s) {
    kill_ips();
    kill_cloud_discovery();
    exit(1);
}

void exit_handler() {
    kill_ips();
    kill_cloud_discovery();
}

int main(int argc, char *argv[])
{
    //Must be done first, s.t. no class using the logger produces an error
    cpm::init(argc, argv);
    cpm::Logging::Instance().set_id("LabControlCenter");

    //Create regular and irregular (interrupt) exit handlers for IPS and Cloud Discovery Service
    struct sigaction interruptHandler;
    interruptHandler.sa_handler = interrupt_handler;
    sigemptyset(&interruptHandler.sa_mask);
    interruptHandler.sa_flags = 0;
    sigaction(SIGINT, &interruptHandler, NULL);

    std::atexit(exit_handler);

    //Start IPS and Cloud Discovery Service which are always required to communicate with real vehicles
    deploy_ips(cpm::cmd_parameter_int("dds_domain", 0, argc, argv), cpm::cmd_parameter_string("dds_initial_peer", "", argc, argv));
    deploy_cloud_discovery();

    //Read command line parameters (current params: auto_start and config_file)
    //TODO auto_start: User does not need to trigger the process manually / does not need to press 'start' when all participants are ready

    std::string config_file = cpm::cmd_parameter_string("config_file", "parameters.yaml", argc, argv);

    auto storage = make_shared<ParameterStorage>(config_file, 32);
    ParameterServer server(storage);
    storage->register_on_param_changed_callback(std::bind(&ParameterServer::resend_param_callback, &server, _1));

    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create();
    Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_path("ui/style.css");
    Gtk::StyleContext::create()->add_provider_for_screen (Gdk::Display::get_default()->get_default_screen(),cssProvider,500);

    bool use_simulated_time = cpm::cmd_parameter_bool("simulated_time", false, argc, argv);
    auto timerTrigger = make_shared<TimerTrigger>(use_simulated_time);
    auto timerViewUi = make_shared<TimerViewUI>(timerTrigger);
    auto logStorage = make_shared<LogStorage>();
    auto loggerViewUi = make_shared<LoggerViewUI>(logStorage);
    auto vehicleManualControl = make_shared<VehicleManualControl>();
    auto trajectoryCommand = make_shared<TrajectoryCommand>();
    auto timeSeriesAggregator = make_shared<TimeSeriesAggregator>();
    auto visualizationCommandsAggregator = make_shared<VisualizationCommandsAggregator>();
    auto mapViewUi = make_shared<MapViewUi>(
        trajectoryCommand, 
        [=](){return timeSeriesAggregator->get_vehicle_data();},
        [=](){return timeSeriesAggregator->get_vehicle_trajectory_commands();},
        [=](){return visualizationCommandsAggregator->get_all_visualization_messages();}
    );
    auto monitoringUi = make_shared<MonitoringUi>([=](){return timeSeriesAggregator->get_vehicle_data();});
    auto vehicleManualControlUi = make_shared<VehicleManualControlUi>(vehicleManualControl);
    auto paramViewUi = make_shared<ParamViewUI>(storage, 5);
    auto setupViewUi = make_shared<SetupViewUI>(timerViewUi, argc, argv);
    auto tabsViewUi = make_shared<TabsViewUI>(setupViewUi, vehicleManualControlUi, paramViewUi, timerViewUi, loggerViewUi);
    auto mainWindow = make_shared<MainWindow>(tabsViewUi, monitoringUi, mapViewUi);


    vehicleManualControl->set_callback([&](){vehicleManualControlUi->update();});

    return app->run(mainWindow->get_window());
}