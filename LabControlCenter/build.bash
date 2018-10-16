#!/bin/bash

clear
rm -rf build
mkdir build
rm -rf rtidds
mkdir rtidds

rtiddsgen -language C++11 -d ./rtidds/ ../dds_idl/TimeStamp.idl
rtiddsgen -language C++11 -d ./rtidds/ ../dds_idl/VehicleCommand.idl
rtiddsgen -language C++11 -d ./rtidds/ ../dds_idl/VehicleState.idl
rtiddsgen -language C++11 -d ./rtidds/ ../dds_idl/Pose2D.idl

cd build
cmake .. 
make -j4
cd ..