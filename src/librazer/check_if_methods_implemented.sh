#!/bin/bash
#set -x
daemon_source=/mnt/hdd/razer-drivers/daemon

declare -a endpoints
IFS=$'\n'
endpoints=($(grep -rh "@endpoint" $daemon_source | cut -d"'" -f2,4 | sed "s/'/\", \"/"))

# TODO Somehow make entries in $endpoints unique

for method in "${endpoints[@]}"; do
  echo "Searching for: $method ..."
  method_grep=$(grep "$method" librazer.cpp)
  if [ ! -z $(echo $method_grep | grep "capabilities.insert") ]; then
    echo "Capabilities: ok"
  else
    echo "Capabilities: no"
  fi
  if [ ! -z $(echo $method_grep | grep -E "prepareDeviceQDBusMessage|prepareGeneralQDBusMessage") ]; then
    echo "Implementation: ok"
  else
    echo "Implementation: no"
  fi
done
