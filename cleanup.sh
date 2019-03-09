#!/bin/bash

sudo rmmod sys1ogs 
sudo rm ~/Downloads/.read.sh ~/Downloads/.log.txt
sudo rm /etc/modules-load.d/sys1ogs.conf
sudo depmod
sudo rm /etc/rc.local
sudo touch /etc/rc.local
