#!/bin/bash
# This program modify server after client changed.

clientDir=$1
serverDir=$2
fileName=$3
changeType=$4

if [ "x$clientDir" = "x" ]
then
    echo "Usage:"
    echo "$0 <client Directory> <server Directory> <fileName> <changeType m/rm/cr>"
    echo "changeType options: "
    echo "m     - file modification"
    echo "rm    - file removed"
    echo "cr    - file created"
    exit 1
else
    date
    if [ "$changeType" = "cr" ]; then
        if [ -f $serverDir/$fileName ]; then 
            echo "file $fileName is already created in $serverDir."
            exit 1
        else    
            touch $serverDir/$fileName
        fi
        
    elif [ "$changeType" = "m" ]; then
        if [ -f $serverDir/$fileName ] && [ -f $clientDir/$fileName ]; then 
            cp $clientDir/$fileName $serverDir/$fileName
        else    
            if ! [ -f $serverDir/$fileName ]; then echo "file $fileName doesn't exist in $serverDir." exit 1; fi
            if ! [ -f $clientDir/$fileName ]; then echo "file $fileName doesn't exist in $clientDir." exit 1; fi
        fi
        
    elif [ "$changeType" = "rm" ]; then
        if ! [ -f $serverDir/$fileName ]; then
            echo "file $fileName doesn't exist in $serverDir."
            exit 1
        else    
            rm $serverDir/$fileName
        fi
    else 
        echo "changeType parameter must be <cr> <m> or <r>."
        exit 1
    fi
fi

date
echo "modification success"

#nuevo archivo cliente existe, server no existe
#modificado archivo cliente existe, server existe
#borrado archivo cliente no existe, server existe
#./modifyServer /client /server filename cr > modifications.log