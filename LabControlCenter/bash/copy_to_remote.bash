#!/bin/bash
# IP and SCRIPT_PATH must be set, SCRIPT_ARGS and MIDDLEWARE_ARGS are not mandatory. If MIDDLEWARE_ARGS is set, the middleware is used, else it isn't (you must set a node id for the middleware).
# SCRIPT_PATH must contain the script name + type-ending as well
# This file is called by the LCC if remote deployment is selected
# DESCRIPTION: The given script is uploaded to the guest account on the specified NUC (IP), all other scripts within this folder (LabControlCenter/bash) are uploaded as well.
#   Then, remote_start.bash is executed on the remote system.
#Get command line arguments
for i in "$@"
do
case $i in
    --ip=*)
    IP="${i#*=}"
    shift # past argument=value
    ;;
    --script_path=*)
    SCRIPT_PATH="${i#*=}"
    shift # past argument=value
    ;;
    --script_arguments=*)
    SCRIPT_ARGS="${i#*=}"
    shift # past argument=value
    ;;
    --middleware_arguments=*)
    MIDDLEWARE_ARGS="${i#*=}"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

# Create scripts directory in remote /tmp folder
ssh guest@${IP} << 'EOF'
    cd /tmp
    rm -rf ./scripts
    mkdir scripts
EOF

# Create .tar that contains all relevant files and copy to host
#Omit /home/username and script name
PATH_TO_SCRIPT="${SCRIPT_PATH#*home/}"
PATH_TO_SCRIPT="${PATH_TO_SCRIPT#*/}"
PATH_TO_SCRIPT="${PATH_TO_SCRIPT%/*}" #Get string before last / (omit name of script)
cd ~
tar czvf - ./${PATH_TO_SCRIPT} | ssh guest@${IP} "cd ~;tar xzvf -"

# Copy further file modification orders to the NUC
scp ~/dev/software/LabControlCenter/bash/remote_start.bash guest@${IP}:/tmp/scripts
scp ~/dev/software/LabControlCenter/bash/environment_variables.bash guest@${IP}:/tmp/scripts
scp ~/dev/software/LabControlCenter/bash/tmux_middleware.bash guest@${IP}:/tmp/scripts
scp ~/dev/software/LabControlCenter/bash/tmux_script.bash guest@${IP}:/tmp/scripts

# Let the NUC handle the rest
sshpass ssh -t guest@${IP} 'bash /tmp/scripts/remote_start.bash' "--script_path=${SCRIPT_PATH} --script_arguments='${SCRIPT_ARGS}' --middleware_arguments='${MIDDLEWARE_ARGS}'"