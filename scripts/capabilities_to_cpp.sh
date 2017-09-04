#!/bin/bash

# Please use autoformat and change the newlines according to https://github.com/openrazer/openrazer/blob/master/pylib/openrazer/client/devices/__init__.py#L44

pyfile=$(curl -s https://raw.githubusercontent.com/openrazer/openrazer/master/pylib/openrazer/client/devices/__init__.py)
incapabilities=false
while read -r line; do
    if [ $incapabilities = false ]; then
        # Check if in startline
        if [[ $line == *"self._capabilities = {"* ]]; then
            incapabilities=true
        fi
    else
        # Exit when method is finished
        if [[ $line == *"}"* ]]; then
            exit 0
        fi
# for debugging
#        echo $line
#        continue
        # Do stuff
        if [[ $line == *"self._has_feature"* ]]; then
            variable=$(echo $line | cut -d "'" -f 2)
            interface=$(echo $line | cut -d "'" -f 4)
            method=$(echo $line | cut -d "'" -f 6)
            if [ -z "$method" ]; then
                echo 'capabilities.insert("'$variable'", hasCapabilityInternal("'$interface'"));'
            else
                echo 'capabilities.insert("'$variable'", hasCapabilityInternal("'$interface'", "'$method'"));'
            fi
        elif [[ $line == *"#"* ]]; then
            echo $line | sed 's/#/\/\//' | sed -e 's/^[[:space:]]*//'
        elif [[ $line == *": True"* ]]; then
            variable=$(echo $line | cut -d "'" -f 2)
            echo 'capabilities.insert("'$variable'", true);'
        elif [[ $line == "" ]]; then
            echo
        else # unknown and special cases
            if [[ $line == *"lighting_led_matrix"* ]]; then # a different format is used here
                echo 'capabilities.insert("lighting_led_matrix", hasMatrix());'
            else
                # About the xargs: lol http://stackoverflow.com/a/12973694/3527128
                echo "// FIXME: "$line | xargs
            fi
        fi
    fi
done <<< "$pyfile"

