#!bin/sh

if [ $# == 0 ]
then
    echo "Specify 1 or 2 to install the configuration for SAS-1 or SAS-2, respectively"
    return
else
    if [ $1 == 1 ]
    then
        sas_id=1
    elif [ $1 == 2 ]
    then
        sas_id=2
    elif [ $1 == 3 ]
    then
        sas_id=3
    else
        echo "Error: unknown SAS specified"
        return
    fi
fi

echo "-----------------------------"
echo "Installation script for SAS-${sas_id}"
echo "-----------------------------"

echo "Copying configuration files..."
sudo cp sysctl.conf /etc/sysctl.conf
sudo cp etc-resolv.conf /etc/resolv.conf
sudo cp etc-conf.d-network /etc/conf.d/network
sudo cp etc-conf.d-lm_sensors /etc/conf.d/lm_sensors
sudo cp ntp.conf /etc/ntp.conf

echo "Configuring NIC device names based on known MAC addresses..."
sudo cp 10-network-sas${sas_id}.rules /etc/udev/rules.d/10-network.rules

echo "Setting up services..."
sudo cp network-sas${sas_id}.service /etc/systemd/system/network.service
sudo cp sbc_info.service /etc/systemd/system/sbc_info.service
sudo cp sbc_shutdown.service /etc/systemd/system/sbc_shutdown.service
sudo cp relay_control.service /etc/systemd/system/relay_control.service
sudo cp sas.service /etc/systemd/system/sas.service

echo "Copying executable scripts..."
sudo cp sas /usr/local/bin/
sudo chmod a+x /usr/local/bin/sas

echo "Enabling services..."
sudo systemctl enable network
sudo systemctl enable lm_sensors
sudo systemctl enable sbc_info
sudo systemctl enable sbc_shutdown
sudo systemctl enable relay_control
sudo systemctl enable ntpd
#sudo systemctl enable sas

echo "    Note: you will also need to run 'make install' to install our compiled executables!"

echo "-----------------------------"
echo "Installation complete; reboot the computer!"
echo "-----------------------------"
