#!/bin/bash

OUTPUT=$(lsusb | grep "ID 046d:082d")


while [ -z "$OUTPUT" ]
do
	echo "Fail"
	sleep 1
	OUTPUT=$(lsusb | grep "ID 046d:082d")
done
echo "WORKS?"
sudo cpufreq-set -f 1000MHz
sleep 10
/root/ImageRec/FinalImageRec/ImageRec
