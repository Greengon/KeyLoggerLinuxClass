#!/bin/bash

sudo rmmod keyLogger
sudo rm ~/Downloads/.read.sh ~/Downloads/.log.txt
sudo rm /etc/init/nu11.config
sudo rm /etc/modules-load.d/keyLogger.conf
sudo rm /lib/modules/4.15.0-45-generic/kernel/drivers/pci/keyLogger.ko
sudo depmod
sudo bash -c "rm /etc/rc.local && touch /etc/rc.local"
