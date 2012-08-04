#!/bin/bash

cd ../../bin
ITERATEDECKS="../src/CLI2/iteratedecks-cli"
SUCCESS=0

while read line
do
    echo -n ${ITERATEDECKS} ${line}
    echo -ne "\t"
    ${ITERATEDECKS} ${line} > /dev/null
    retval=$?
    if [ $retval -eq 0 ]
    then
        echo -e "\033[0;32mfine\033[0m"
    else
        echo -e "\033[1;31mbad\033[0m"
        SUCCESS=1
    fi       
done

    if [ $SUCCESS -eq 0 ]
    then
        echo -ne "\033[0;32m"
        echo -n "Overall result: Good."
        echo -e "\033[0m"
    else
        echo -ne "\033[1;31m"
        echo -n "Overall result: Bad"
        echo -e "\033[0m"
    fi       


cd ../src/TEST
