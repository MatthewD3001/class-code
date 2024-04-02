#!/bin/bash

#test 1 archive present
if [ ! -f ./prog4.tgz ]; then
    echo "Project archive not found in current directory. Submission will not be graded."
    exit 1
fi

gtar zxf prog4.tgz

#test 2 make 
(make) >/dev/null
made=$?
if [ "$made" -ne "0" ]; then
	echo -e "Make returned an error. Submission will not be graded."
	exit 1
fi

#test 3 prose present
if [ ! -f ./prose ]; then
    echo "prose not generated by make. Full submission will not be graded, but you may receive partial credit due to the step*.c files."
    exit 1
fi


echo "Submission will be graded successfully. Please be aware this does not test your submission for functionality, only that the autograder will run successfully."
