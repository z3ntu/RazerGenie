#!/bin/bash
#set -x

# colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[1;34m'
NC='\033[0m' # No Color

daemon_source=/mnt/hdd/razer/openrazer/daemon

declare -a endpoints
IFS=$'\n'
endpoints=($(grep -rh "@endpoint" $daemon_source | cut -d"'" -f2,4 | sed "s/'/\", \"/"))

# TODO Somehow make entries in $endpoints unique

for method in "${endpoints[@]}"; do
  echo -e "Searching for: ${BLUE}$method${NC} ..."
  method_grep=$(grep "$method" libopenrazer.cpp)
  if [ ! -z $(echo $method_grep | grep "capabilities.insert") ]; then
    echo -e "Capabilities: ${GREEN}ok${NC}"
  else
    echo -e "Capabilities: ${RED}no${NC}"
  fi
  if [ ! -z $(echo $method_grep | grep -E "prepareDeviceQDBusMessage|prepareGeneralQDBusMessage") ]; then
    echo -e "Implementation: ${GREEN}ok${NC}"
  else
    echo -e "Implementation: ${RED}no${NC}"
  fi
done
