#!/bin/sh

sas_id=2

echo "-----------------------------"
echo "Installation script for SAS-${sas_id}"
echo "-----------------------------"

echo "Configuring NIC device names based on known MAC addresses..."
sudo cp 10-network-sas${sas_id}.rules /etc/udev/rules.d/10-network.rules

echo "Copying configuration files..."
sudo cp sysctl.conf /etc/sysctl.conf
sudo cp etc-conf.d-network /etc/conf.d/network

echo "Setting up network-configuration service..."
sudo cp network-sas${sas_id}.service /etc/systemd/system/network.service
sudo systemctl enable network

echo "-----------------------------"
echo "Installation complete; reboot the computer!"
echo "-----------------------------"
