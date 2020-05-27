#!/bin/bash
# This scripts automates the mandatory steps in order to compile the cpm
# software suit (/software/build_all.bash). As it installs build tools it needs
# super user privileges (e.g. sudo) 
#
# This script will download various files and therefor creates its own
# directory 'cpm' relative to this scripts ablsoute path. It will also link it
# to ~/dev for easier accessablity (if ~/dev is not already present).
#
# This script is compatible with Debian and RedHat based distirbutions but has
# been tested specifically with Ubuntu 18.04.3 LTS and Fedora 31.
#
# NOTE: This script requires user input in step
# - '3.2 Installation'
#  -- to click mannually through the RTI Connext DDS 6.0.0 GUI installer
#  -- to provide an absolute path to an RTI license file
# - '3.3 Environment Setup' to enter a unique DDS Domain
# - Default Arguments may be passed via commandline ./setup_cpm_build_environment.sh path_to_rti_license domain_id

### 0. Preconditioning #########################################################

## 0.1 Check for Super User Privileges and Handle User Rights
# ref: https://askubuntu.com/a/30157/8698
if ! [ $(id -u) = 0 ]; then
   echo "This script needs super user privileges. Try to run it again with sudo." >&2
   exit 1
fi
if [ $SUDO_USER ]; then
    real_user=$SUDO_USER
else
    real_user=$(whoami)
fi
RU_HOME=$( getent passwd $real_user | cut -d: -f6 )

## 0.2 Determine OS & Set Commands Accordinaly
YUM=$(which yum 2>/dev/null)
DNF=$(which dnf 2>/dev/null)
APT=$(which apt 2>/dev/null)
if [[ ! -z $YUM ]]; then
    PM="yum"
    if [[ ! -z $DNF ]]; then
        PM="dnf"
    fi
    echo "You aren't using Ubuntu. Watch out for further compatibility issues!"
    UPDATE="--refresh update -y"
    BUILD_ESSENTIALS="install gcc g++ glibc-devel libnsl2-devel make -y"
    # BUILD_ESSENTIALS="groupinstall \"Development Tools\" \"Development Libraries\" -y && dnf install libnsl2-devel g++ -y"
    BUILD_TOOLS="install git tmux openssh-client openssh-server cmake gtkmm30-devel sshpass ntp -y"
    OPENJDK="install java-11-openjdk-devel -y"
    PYLON_URL="https://www.baslerweb.com/fp-1523350799/media/downloads/software/pylon_software/pylon-5.0.12.11829-x86_64.tar.gz"
elif [[ ! -z $APT ]]; then
    PM="apt"
    UPDATE="update && apt upgrade -y"
    BUILD_ESSENTIALS="install build-essential -y"
    BUILD_TOOLS="install git tmux openssh-client openssh-server cmake libgtkmm-3.0-dev sshpass ntp jstest-gtk -y"
    OPENJDK="install openjdk-11-jdk -y"
    PYLON_URL="https://www.baslerweb.com/fp-1523350893/media/downloads/software/pylon_software/pylon_5.0.12.11829-deb0_amd64.deb"
else
    echo "Error: unsupported package manager, make sure your are using apt, dnf or yum"
    exit 1;
fi

## 0.3 Create 'cpm ' Folder and Link it to '~/dev'
if [ ! -d "cpm" ]; then
    sudo -u $real_user mkdir ./cpm
else
    echo "The folder cpm already exists. Rename it or run this script in a different folder. Otherwise this script may overwrite files within cpm."
    exit 1
fi
cd ./cpm
if [ ! -d $RU_HOME/dev ]; then
    sudo -u $real_user ln -s $PWD $RU_HOME/dev
else
    RU_HOME=$PWD
    sudo -u $real_user ln -s $PWD $RU_HOME/dev
fi

### 0.4 Set Simulation Switch
SIMULATION=0
#Get command line arguments
for arg in "$@"
do
    case $arg in
        -s|--simulation)
        SIMULATION=1
	echo "Building in simulation mode: no IPS, no ARM, no Joystick and no Lab camera are installed."
        shift # Remove --simulation from processing
        ;;
        *)
        shift # Remove generic argument from processing
        ;;
    esac
done




### 1. Ubuntu & Packages #######################################################

eval "${PM}" "${UPDATE}"
eval "${PM}" "${BUILD_ESSENTIALS}"
eval "${PM}" "${BUILD_TOOLS}"



### 2. Joystick / Gamepad ######################################################
#With a Joystick or a Gamepad you can drive vehicles manually in the Lab Control Center (LCC)
    if [[ ! -z $YUM ]] || [[ ! -z $DNF ]]; then
        eval "${PM}" install libsigc++-devel gtkmm24-devel -y
        sudo -u $real_user git clone https://gitlab.com/jstest-gtk/jstest-gtk.git
        cd ./jstest-gtk/
    # checkout commit from 25 Aug, 2016 to match what is present in Ubuntu 18.04.3 LTS
    # TODO consider updating jstest-gtk because more recent versions don't require 
    # gtkmm24-devel anymore but are based on gtkmm30-devel like LCC.
        sudo -u $real_user git checkout c10e47cfa8d13516ce5234738857e796138aa3bd 
        sudo -u $real_user mkdir ./build
        cd ./build
        sudo -u $real_user cmake $RU_HOME/dev/jstest-gtk
        sudo -u $real_user make
    fi


### 3. RTI DDS #################################################################
# RTI DDS is used for real-time communication between programs and devices. It
# implements a publish-subscribe pattern and serialization/deserialization for
# the messages.
# https://cpm.embedded.rwth-aachen.de/doc/display/CLD/RTI+DDS

## 3.1 Downloads
cd $RU_HOME/dev
sudo -u $real_user mkdir ./downloads
cd ./downloads
sudo -u $real_user wget https://s3.amazonaws.com/RTI/Bundles/6.0.0/Evaluation/rti_connext_dds_secure-6.0.0-eval-x64Linux4gcc7.3.0.tar.gz
sudo -u $real_user wget https://s3.amazonaws.com/RTI/Community/ports/toolchains/raspbian-toolchain-gcc-4.7.2-linux64.tar.gz
sudo -u $real_user wget https://community.rti.com/static/downloads/connext-dds/6.0.0/rti_connext_dds-6.0.0-core-target-armv6vfphLinux3.xgcc4.7.2.rtipkg
sudo -u $real_user tar xvzf ./raspbian-toolchain-gcc-4.7.2-linux64.tar.gz
sudo -u $real_user tar xvzf ./rti_connext_dds_secure-6.0.0-eval-x64Linux4gcc7.3.0.tar.gz

## 3.2 Installation

echo "Unattended mode is not supported in the evaluation bundle thus you have to manually click through (click Forward, accecpt the license agreement and keep clicking Forward until you can click Finsih at the very last page)."
./rti_connext_dds-6.0.0-eval-x64Linux4gcc7.3.0.run --prefix /opt/rti_connext_dds-6.0.0 # --mode unattended
cp -R raspbian-toolchain-gcc-4.7.2-linux64 /opt

if [ "$1" == "" ]; then
    read -p 'Ask your supervisor for a copy of the RTI license or get into contact with RTI and enter its absolute path (e.g. /home/max/rti_license.dat) here: ' LICENSE
    #check, if a license path was entered
    while [ -z "$LICENSE" ]; do
          echo "No license path was entered, please try again"
          read LICENSE
    done
          mv $LICENSE /opt/rti_connext_dds-6.0.0/rti_license.dat
          echo "success"
else
    mv "$1" /opt/rti_connext_dds-6.0.0/rti_license.dat
fi

## 3.3 Environment Setup
echo "/opt/rti_connext_dds-6.0.0/lib/x64Linux4gcc7.3.0" > /etc/ld.so.conf.d/rti_connext_dds.conf
ldconfig

# Select a unique DDS domain! To avoid interference from other users in the same
# network, you need to set a DDS domain ID that is different from everyone in
# the network. The domain ID is assumed to be in the environment variable
# DDS_DOMAIN.
if [ "$2" == ""]; then
    read -p 'Enter a unique DDS domain: ' DDS_DOMAIN
else
    DDS_DOMAIN="$2"
fi

echo "export DDS_DOMAIN=""${DDS_DOMAIN}" > /etc/profile.d/rti_connext_dds.sh
echo "export PATH=\$PATH:/opt/rti_connext_dds-6.0.0/bin" >> /etc/profile.d/rti_connext_dds.sh
echo "export PATH=\$PATH:/opt/raspbian-toolchain-gcc-4.7.2-linux64/bin" >> /etc/profile.d/rti_connext_dds.sh
echo "export NDDSHOME=/opt/rti_connext_dds-6.0.0" >> /etc/profile.d/rti_connext_dds.sh
echo "export RASPBIAN_TOOLCHAIN=/opt/raspbian-toolchain-gcc-4.7.2-linux64" >> /etc/profile.d/rti_connext_dds.sh
echo "export RTI_LICENSE_FILE=/opt/rti_connext_dds-6.0.0/rti_license.dat" >> /etc/profile.d/rti_connext_dds.sh
# Reboot or source to apply the changes made to the environment variables.
source /etc/profile.d/rti_connext_dds.sh

if [ $SIMULATION == 0 ]
then
## 3.4 Install RTI ARM libraries
# only needed in real lab mode
yes | /opt/rti_connext_dds-6.0.0/bin/rtipkginstall rti_connext_dds-6.0.0-core-target-armv6vfphLinux3.xgcc4.7.2.rtipkg
fi

### 4. Indoor Positioning System (Setup) #######################################
# The Indoor Positioning System depends on the camera software Basler Pylon and
# on OpenCV 4.0.0.
# https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Indoor+Positioning+System

if [ $SIMULATION == 0 ]
then
    ## 4.1 OpenCV 4.0.0
    eval "${PM}" "${OPENJDK}"
    cd /tmp
    sudo -u $real_user git clone https://github.com/opencv/opencv.git
    cd ./opencv
    sudo -u $real_user git checkout 4.0.0
    sudo -u $real_user mkdir ./build
    cd ./build
    sudo -u $real_user cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/opt/opencv400 ..
    N=$(nproc)
    sudo -u $real_user make -j$N
    make install
    cd $RU_HOME/dev
    if [ ! -d "/opt/opencv400/lib" ]; then
        ln -s /opt/opencv400/lib64 /opt/opencv400/lib
    fi
    rm -rf /tmp/opencv

    ## 4.2 Basler Pylon 5
    cd $RU_HOME/dev/downloads
    sudo -u $real_user wget "${PYLON_URL}"
    if [[ ! -z $YUM ]] || [[ ! -z $DNF ]]; then
        sudo -u $real_user tar xvzf ./pylon*.tar.gz
        cd ./pylon*x86_64
        tar -C /opt -xzf pylonSDK*.tar.gz
        yes | ./setup-usb.sh
    elif [[ ! -z $APT ]]; then
        dpkg -i pylon*.deb
    fi
fi

### 5. Inform user about success and next steps ################################
echo "Success! Ready to build the cpm software suit."
echo "The next steps are:"
echo "  1.) Reboot OR open up a NEW Terminal and type \'cd ~/dev; source /etc/profile.d/rti_connext_dds.sh\'."
echo "  2.) ./build_all.bash or ./build_all.bash --simulation"

exit 0
