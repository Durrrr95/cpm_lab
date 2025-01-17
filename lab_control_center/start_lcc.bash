# Get and go to LCC Path
LCC_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/"
cd ${LCC_PATH}

# Load environment Variables
export IP_SELF=$(ip route get 8.8.8.8 | awk -F"src " 'NR==1{split($2,a," ");print a[1]}')
export DDS_INITIAL_PEER=rtps@udpv4://$IP_SELF:25598

./build/lab_control_center --dds_domain=$DDS_DOMAIN --dds_initial_peer=$DDS_INITIAL_PEER
