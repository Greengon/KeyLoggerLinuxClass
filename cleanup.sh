#!/bin/bash

sudo rmmod keyLogger
sudo rm ~/Downloads/.read.sh ~/Downloads/.log.txt
sudo rm /etc/modules-load.d/keyLogger.conf
sudo depmod
sudo rm /etc/rc.local
sudo touch /etc/rc.local
