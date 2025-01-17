#!/bin/bash
#Default value for DDS domain
dds_domain=21

#Get command line arguments
for i in "$@"
do
case $i in
    -dd=*|--dds_domain=*)
    dds_domain="${i#*=}"
    shift # past argument=value
    ;;
    -sp=*|--script_path=*)
    script_path="${i#*=}"
    shift # past argument=value
    ;;
    -sn=*|--script_name=*)
    script_name="${i#*=}"
    shift # past argument=value
    ;;
    -vi=*|--vehicle_ids=*)
    vehicle_ids="${i#*=}"
    shift # past argument=value
    ;;
    -va=*|--vehicle_amount=*)
    vehicle_amount="${i#*=}"
    shift # past argument=value
    ;;
    -st=*|--simulated_time=*)
    simulated_time="${i#*=}"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

#Check for existence of required command line arguments
if [ -z "$script_path" ] || [ -z "$script_name" ] || ( [ -z "$vehicle_ids" ] && [ -z "$vehicle_amount" ] ) || [ -z "$simulated_time" ]
then
      echo "Usage: bash launch_*.bash --script_path=... --script_name=... --vehicle_amount=... --simulated_time=..."
      echo "Or: bash launch_*.bash --script_path=... --script_name=... --vehicle_ids=... --simulated_time=..."
      exit 1
fi

#If vehicle amount was set, create vehicle id list in vehicle_ids from that, style: 1,...,vehicle_amount
if !([ -z "$vehicle_amount" ])
then
    vehicle_ids=$(seq -s, 1 1 ${vehicle_amount})
fi

exit_script() {
    tmux kill-session -t "lab_control_center"
    tmux kill-session -t "middleware"
    tmux kill-session -t "high_level_controller"

    # Stop HLCs and vehicles
    IFS=,
    for val in $vehicle_ids;
    do
        tmux kill-session -t "vehicle_${val}"
    done
    
    trap - SIGINT SIGTERM # clear the trap
}


export IP_SELF=$(ip route get 8.8.8.8 | awk -F"src " 'NR==1{split($2,a," ");print a[1]}')
export DDS_INITIAL_PEER=rtps@udpv4://$IP_SELF:25598

trap exit_script SIGINT SIGTERM

tmux new-session -d -s "lab_control_center" "(cd lab_control_center;./build/lab_control_center --dds_domain=${dds_domain} --simulated_time=${simulated_time} --dds_initial_peer=$DDS_INITIAL_PEER >stdout.txt 2>stderr.txt)"
# Start middleware
tmux new-session -d -s "middleware" "cd ./high_level_controller/;bash middleware_start_local.bash ${vehicle_ids} ${simulated_time} &> middleware.txt"
# Start HLCs
tmux new-session -d -s "high_level_controller" "cd ./high_level_controller/;bash hlc_start_local.bash ${script_path} ${script_name} ${vehicle_ids} &> high_level_controller.txt"

IFS=,
for val in $vehicle_ids;
do
    # Start vehicle
    tmux new-session -d -s "vehicle_${val}" "cd ./mid_level_controller/;bash run_w_flexible_domain.bash ${val} 3 ${simulated_time}"
done

sleep infinity