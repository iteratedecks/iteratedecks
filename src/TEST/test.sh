#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

EXITONFAILURE=0

if [ $# -ge 1 ]
then
    cd $1
else
    cd ../../builddir/linux-on-linux
fi

ITERATEDECKS="./iteratedecks-cli"
if [ $# -ge 2 ]
then
    ITERATEDECKS=$2
fi

SUCCESS=0

while read line
do
    if [ "${#line}" -eq 0 ]; then
        continue
    elif [ "${line:0:1}" == "#" ]; then
        # I think the comments may actually be useful for understanding the tests
        echo -e "\033[1;30m${line}\033[0m"
        continue
    fi

    echo -n ${ITERATEDECKS} ${line}
    echo -ne "\t"
    ${ITERATEDECKS} ${line} > /dev/null 2>/dev/null
    retval=$?
    if [ $retval -eq 0 ]
    then
        echo -e "\033[0;32mfine\033[0m"
    else
        echo -e "\033[1;31mbad\033[0m"
        SUCCESS=1
        if [ $EXITONFAILURE -eq 1 ]; then exit; fi
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


#cd ${DIR}
