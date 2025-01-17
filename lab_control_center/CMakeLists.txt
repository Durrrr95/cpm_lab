cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_FLAGS_DEBUG  "-g")
set(CMAKE_BUILD_TYPE Debug)
add_definitions(-Wall -Wextra -Werror=return-type)
set (CMAKE_CXX_STANDARD 17)
link_libraries(dl nsl m pthread rt)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

message("Simulation Switch = ${SIMULATION}")

# Pass simulation to C code
if(SIMULATION)
    add_definitions(-DSIMULATION)
endif()

# RTI DDS
add_definitions(-DRTI_UNIX -DRTI_LINUX -DRTI_64BIT -DRTI_STATIC)
include_directories(SYSTEM $ENV{NDDSHOME}/include)
include_directories(SYSTEM $ENV{NDDSHOME}/include/ndds)
include_directories(SYSTEM $ENV{NDDSHOME}/include/ndds/hpp)
link_libraries(nddscpp2z nddscz nddscorez)
link_directories($ENV{NDDSHOME}/lib/x64Linux4gcc7.3.0)
include_directories(src)
include_directories(.)
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -m64 -Wl,--no-as-needed")

# GTK
find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)


# MATLAB
find_package(Matlab)
if(Matlab_FOUND)
    message("-- found MATLAB in ${Matlab_ROOT_DIR}")
    add_definitions(-DMATLAB)
endif()

# CPM lib
include_directories(SYSTEM ../cpm_lib/include/)
include_directories(SYSTEM ../cpm_lib/include/cpm/dds/)
link_directories(SYSTEM ../cpm_lib/build/)

# yaml-cpp
include_directories(../../yaml-cpp/include/)
link_directories(../../yaml-cpp/build/)

# commonroad integration
find_package(LibXML++ REQUIRED)
message("-- found libxml++2.6")
include_directories(${LibXML++_INCLUDE_DIRS})

# Trajectory interpolaton
include_directories(../mid_level_controller/src)

set(COMMONROAD
    src/commonroad_classes/CommonRoadScenario.hpp
    src/commonroad_classes/CommonRoadScenario.cpp
    src/commonroad_classes/CommonroadDrawConfiguration.hpp
    src/commonroad_classes/CommonRoadTransformation.hpp
    src/commonroad_classes/CommonRoadTransformation.cpp
    src/commonroad_classes/DynamicObstacle.hpp
    src/commonroad_classes/DynamicObstacle.cpp
    src/commonroad_classes/EnvironmentObstacle.hpp
    src/commonroad_classes/EnvironmentObstacle.cpp
    src/commonroad_classes/Intersection.hpp
    src/commonroad_classes/Intersection.cpp
    src/commonroad_classes/Lanelet.hpp
    src/commonroad_classes/Lanelet.cpp
    src/commonroad_classes/PlanningProblem.hpp
    src/commonroad_classes/PlanningProblem.cpp
    src/commonroad_classes/StaticObstacle.hpp
    src/commonroad_classes/StaticObstacle.cpp
    src/commonroad_classes/TrafficLight.hpp
    src/commonroad_classes/TrafficLight.cpp
    src/commonroad_classes/TrafficSign.hpp
    src/commonroad_classes/TrafficSign.cpp

    src/commonroad_classes/InterfaceTransform.hpp
    src/commonroad_classes/InterfaceTransformTime.hpp
    src/commonroad_classes/InterfaceDraw.hpp
    src/commonroad_classes/InterfaceGeometry.hpp

    src/commonroad_classes/XMLTranslation.hpp
    src/commonroad_classes/XMLTranslation.cpp

    src/commonroad_classes/datatypes/Interval.hpp
    src/commonroad_classes/datatypes/IntervalOrExact.hpp

    src/commonroad_classes/geometry/Circle.hpp
    src/commonroad_classes/geometry/Circle.cpp
    src/commonroad_classes/geometry/Point.hpp
    src/commonroad_classes/geometry/Point.cpp
    src/commonroad_classes/geometry/Polygon.hpp
    src/commonroad_classes/geometry/Polygon.cpp
    src/commonroad_classes/geometry/Position.hpp
    src/commonroad_classes/geometry/Position.cpp
    src/commonroad_classes/geometry/Rectangle.hpp
    src/commonroad_classes/geometry/Rectangle.cpp
    src/commonroad_classes/geometry/Shape.hpp
    src/commonroad_classes/geometry/Shape.cpp

    src/commonroad_classes/states/GoalState.hpp
    src/commonroad_classes/states/GoalState.cpp
    src/commonroad_classes/states/Occupancy.hpp
    src/commonroad_classes/states/Occupancy.cpp
    src/commonroad_classes/states/SignalState.hpp
    src/commonroad_classes/states/SignalState.cpp
    src/commonroad_classes/states/State.hpp
    src/commonroad_classes/states/State.cpp
    src/commonroad_classes/states/StateExact.hpp
    src/commonroad_classes/states/StateExact.cpp

    src/commonroad_classes/SpecificationError.hpp
    src/commonroad_classes/SpecificationError.cpp

    src/commonroad_classes/ObstacleSimulationData.hpp
)

# labcam
if(NOT SIMULATION)
    include_directories(labcam/include)
    add_subdirectory(labcam)
endif()

add_executable(lab_control_center
    src/ObstacleSimulation.hpp
    src/ObstacleSimulation.cpp
    src/ObstacleSimulationManager.hpp
    src/ObstacleSimulationManager.cpp
    src/ObstacleAggregator.cpp
    src/ObstacleAggregator.hpp
    src/LCCErrorLogger.hpp
    src/LCCErrorLogger.cpp
    src/LogLevelSetter.hpp
    src/LogLevelSetter.cpp
    src/LogStorage.hpp
    src/LogStorage.cpp
    src/TimerTrigger.hpp
    src/TimerTrigger.cpp
    src/TimeSeries.cpp
    src/TimeSeries.hpp
    src/TimeSeriesAggregator.cpp
    src/TimeSeriesAggregator.hpp
    src/HLCReadyAggregator.cpp
    src/HLCReadyAggregator.hpp
    src/VisualizationCommandsAggregator.cpp
    src/VisualizationCommandsAggregator.hpp
    src/GoToPlanner.cpp
    src/GoToPlanner.hpp
    src/VehicleManualControl.cpp
    src/VehicleManualControl.hpp

    ui/manual_control/VehicleManualControlUi.cpp
    ui/manual_control/VehicleManualControlUi.hpp

    src/VehicleAutomatedControl.cpp
    src/VehicleAutomatedControl.hpp

    ui/commonroad/CommonroadViewUI.hpp
    ui/commonroad/CommonroadViewUI.cpp
    ui/commonroad/ObstacleToggle.hpp
    ui/commonroad/ObstacleToggle.cpp
    ui/commonroad/ProblemModelRecord.hpp
    ui/monitoring/MonitoringUi.cpp
    ui/monitoring/MonitoringUi.hpp
    ui/map_view/MapViewUi.cpp
    ui/map_view/MapViewUi.hpp
    ui/file_chooser/FileChooserUI.hpp
    ui/file_chooser/FileChooserUI.cpp
    ui/file_chooser/FileSaverUI.hpp
    ui/file_chooser/FileSaverUI.cpp
    ui/file_chooser/FileDialogPaths.hpp
    ui/file_chooser/FileDialogPaths.cpp
    ui/params/ParamViewUI.hpp
    ui/params/ParamViewUI.hpp
    ui/params/ParamViewUI.cpp
    ui/params/ParamsCreateView.hpp
    ui/params/ParamsCreateView.cpp
    ui/params/ParamModelRecord.hpp
    ui/right_tabs/TabsViewUI.hpp
    ui/right_tabs/TabsViewUI.cpp
    ui/timer/TimerViewUI.hpp
    ui/timer/TimerViewUI.cpp
    ui/timer/TimerModelRecord.hpp
    ui/lcc_errors/LCCErrorViewUI.hpp
    ui/lcc_errors/LCCErrorViewUI.cpp
    ui/lcc_errors/LCCErrorModelRecord.hpp
    ui/logger/LoggerViewUI.hpp
    ui/logger/LoggerViewUI.cpp
    ui/logger/LoggerModelRecord.hpp
    ui/setup/SetupViewUI.hpp
    ui/setup/SetupViewUI.cpp
    ui/setup/CrashChecker.hpp
    ui/setup/CrashChecker.cpp
    ui/setup/Deploy.hpp
    ui/setup/Deploy.cpp
    ui/setup/Upload.hpp
    ui/setup/Upload.cpp
    ui/setup/UploadWindow.hpp
    ui/setup/UploadWindow.cpp
    ui/setup/VehicleToggle.hpp
    ui/setup/VehicleToggle.cpp
    ui/MainWindow.cpp
    ui/MainWindow.hpp

    src/defaults.cpp
    src/defaults.hpp
    src/Joystick.cpp
    src/Joystick.hpp
    src/ParameterServer.cpp
    src/ParameterServer.hpp
    src/ParameterStorage.cpp
    src/ParameterStorage.hpp
    src/ProgramExecutor.hpp
    src/ProgramExecutor.cpp
    src/RTTAggregator.hpp
    src/RTTAggregator.cpp
    src/TrajectoryCommand.cpp
    src/TrajectoryCommand.hpp
    src/main.cpp

    ${COMMONROAD}
)


if(NOT SIMULATION)
    target_link_libraries(lab_control_center cpm yaml-cpp stdc++fs labcamlib ${LibXML++_LIBRARIES} ${GTKMM_LIBRARIES})
else()
    target_link_libraries(lab_control_center cpm yaml-cpp stdc++fs ${LibXML++_LIBRARIES} ${GTKMM_LIBRARIES})
endif()

target_include_directories(lab_control_center PUBLIC ${GTKMM_INCLUDE_DIRS})
if(Matlab_FOUND)
    target_include_directories(
        lab_control_center PRIVATE
        ${Matlab_ROOT_DIR}/extern/include/
    )
target_link_libraries(
    lab_control_center
    ${Matlab_ROOT_DIR}/extern/bin/glnxa64/libMatlabDataArray.so
    ${Matlab_ROOT_DIR}/extern/bin/glnxa64/libMatlabEngine.so
)
endif()
target_compile_options(lab_control_center PUBLIC ${GTKMM_CFLAGS_OTHER})

add_executable(TimerTestSimulated
    test/TimerTestSimulated.cpp
)

target_link_libraries(TimerTestSimulated cpm)

add_executable(TimerTestRealtime
    test/TimerTestRealtime.cpp
)

target_link_libraries(TimerTestRealtime cpm)

add_executable(VisualizationTest
    test/VisualizationTest.cpp
)

target_link_libraries(VisualizationTest cpm)