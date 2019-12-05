#!/bin/bash

# Argument 1: Vehicle ID
# Argument 2: Node ID (Identifier of the middleware)
vehicle_id=$1
simulated_time=$2
middleware_id="middleware"

#Load environment variables, like RTI location, library location, Matlab location...
. ./environment_variables.bash

# Start screen for middleware; detach and start middleware
cd ./middleware/build

./middleware --node_id=${middleware_id} --vehicle_ids=${vehicle_id} --dds_domain=${DDS_DOMAIN} --simulated_time=${simulated_time} --dds_initial_peer=${DDS_INITIAL_PEER}