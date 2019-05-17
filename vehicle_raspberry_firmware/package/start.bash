#!/bin/bash


export IP_SELF="$(hostname -i)"

echo IP_SELF
echo $IP_SELF

export VEHICLE_ID="$(cat vehicle_id_map/$IP_SELF)"

echo VEHICLE_ID
echo $VEHICLE_ID

LD_LIBRARY_PATH=/tmp/package ./vehicle_rpi_firmware $VEHICLE_ID >stdout_$VEHICLE_ID.txt 2>stderr_$VEHICLE_ID.txt