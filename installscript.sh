#!/bin/bash
#
# Script to give new iotsa correct configuration.
# More documentation than anything else right now
#
# Enable next line if WiFI interface isn't en2
export IOTSA_WIFI=en1

# Default device name (from which )
tempHostname=xxxxxxxx
ssid=xxxxxxxx
ssidPassword=xxxxxxxx
hostName=xxxxxxxxx
credentials=owner:passwordXXX
#
# Step 1 - set correct ssid and password. Then reboot.
#
iotsaControl --ssid ${config-tempHostname} wifiConfig ssid="${ssid}" ssidPassword="${ssidPassword}"
echo "Please reboot"
sleep 10
#
# Step 2 - set correct hostname.
#
iotsaControl --target ${tempHostname} --credentials ${credentials} configWait config hostName=${hostname} reboot=1

#
# Step 3 - set issuer and issuer key
#
igor=https://boom.local:9333
issuer=${igor}/issuer
issuerKey=xxxxxxxxx
iotsaControl --target ${hostname} --credentials ${credentials} configWait xConfig capabilities trustedIssuer=${issuer} issuerKey=${issuerKey}
#
# Step 4 - (button only) set URLs using the device capability
#
# tbd
