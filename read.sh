#!/bin/bash

while [ 1 ]
do
	text=$(sudo cat /dev/nu11) 
	if [ "$text" = ""  ]
	then
		sleep 30	
	else
		echo -n $text >> ./.log.txt
	fi
done
