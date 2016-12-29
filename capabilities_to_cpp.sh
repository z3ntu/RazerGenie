#!/bin/bash

# Please use autoformat and change the newlines according to https://github.com/terrycain/razer-drivers/blob/master/pylib/razer/client/devices/__init__.py#L43 

pyfile=$(curl -s https://raw.githubusercontent.com/terrycain/razer-drivers/master/pylib/razer/client/devices/__init__.py)
incapabilities=false
IFS=$'\n'
for line in $pyfile; do
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
                echo 'capabilites.insert("'$variable'", hasCapability("'$interface'"));'
            else
                echo 'capabilites.insert("'$variable'", hasCapability("'$interface'", "'$method'"));'
            fi
        elif [[ $line == *"#"* ]]; then
            echo $line | sed 's/#/\/\//' | sed -e 's/^[[:space:]]*//'
        elif [[ $line == *": True"* ]]; then
            variable=$(echo $line | cut -d "'" -f 2)
            echo 'capabilites.insert("'$variable'", true);'
        else
            # About the xargs: lol http://stackoverflow.com/a/12973694/3527128
            echo "// FIXME: "$line | xargs
        fi
    fi
done

