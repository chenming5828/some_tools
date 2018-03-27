#!/bin/sh
## Copyright (c) 2015-2016 Thundersoft Technologies, Inc.  All Rights Reserved.
## Thundersoft Technologies Proprietary and Confidential.


##Set Wifi Original password
ifdown wlan0
rand=`awk -F: '{print $3$4$5$6}' /sys/class/net/wlan0/address`
sed -i -e "s/^ssid=.*/ssid=DAYBREAKER-$rand/" /etc/hostapd.conf
sed -i 's/wpa_passphrase=.*/wpa_passphrase=daybreaker/' /etc/hostapd.conf
sleep 1
ifup wlan0
return 0
