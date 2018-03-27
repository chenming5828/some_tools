#!/bin/sh
#/home/linaro/wifi_reset/add_route.sh

WIFI_RELAY_STA_IP="192.168.1.99"

WIFI_DEST_IP_24="192.168.2.0"
USB_AND_DEST_IP_24="192.168.42.0"
USB_IOS_DEST_IP_24="172.20.10.0"

route add -net $WIFI_DEST_IP_24 netmask 255.255.255.0 gw $WIFI_RELAY_STA_IP
route add -net $USB_AND_DEST_IP_24 netmask 255.255.255.0 gw $WIFI_RELAY_STA_IP
route add -net $USB_IOS_DEST_IP_24 netmask 255.255.255.0 gw $WIFI_RELAY_STA_IP
