#!/bin/ash
#
# This script help to simulate multiple process write to one scull device, which
# has no lock for its globle data.
#

date

# create input_data_[1~5]
for j in `seq 1 5`
do
        touch ./input_data_$j
        for i in `seq 1 100`
        do
                if [ $i -eq 1 ]
                then
                        echo -n "$j$j$j$j$j$j$j$j$j$j" > ./input_data_$j
                fi
                echo -n "$j$j$j$j$j$j$j$j$j$j" >> ./input_data_$j
        done
done

for i in `seq 1 5`
do
        cat ./input_data_$j > /dev/scull0 &
done

date
