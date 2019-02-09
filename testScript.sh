#!/bin/bash

while [ 1 ]
do
	text=$(sudo cat /dev/nu11) 
	if [ "$text" = ""  ]
	then
		break
	else
		echo $text >> ./log.txt
	fi
done
