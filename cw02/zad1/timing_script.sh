#!/bin/bash
RECORD_SIZES=(4 512 4096 8192)
RECORD_COUNTS=(1000 3000)
make record_handler > /dev/null
echo "Results in seconds."

for size in ${RECORD_SIZES[*]}
do
    for count in ${RECORD_COUNTS[*]}
    do
        temp_file=$(mktemp)
        copy=$(mktemp)
        echo "Record count: ${count} Record size: ${size}"
        yes | ./record_handler generate ${temp_file} ${count} ${size} > /dev/null

        echo "Sort:"
        cp ${temp_file} ${copy}
        echo "System:"
        time -p (yes | ./record_handler sort ${copy} ${count} ${size} sys > /dev/null)
        echo "Library:"
        cp ${temp_file} ${copy}
        time -p (yes | ./record_handler sort ${copy} ${count} ${size} lib > /dev/null) | tr '\r\n' ' '

        echo "Copy:"
        echo "System:"
        time -p (yes | ./record_handler copy ${temp_file} ${copy} ${count} ${size} sys > /dev/null) | tr '\r\n' ' '
        echo "Library:"
        time -p (yes | ./record_handler copy ${temp_file} ${copy} ${count} ${size} lib > /dev/null) | tr '\r\n' ' '

        rm ${temp_file}
        rm ${copy}
    done
done