#!/bin/bash

paramSrc="/usr/share/data/adsp/params"
paramBak="/mnt/persist/params"

tofSrc="/usr/share/data/adsp/TOF.txt"
tofBak="/mnt/persist/TOF.txt"

cd /usr/share/data/adsp/

if [ ! -f "$paramSrc" ]; then	
	if [ ! -f "$paramBak" ]; then
		echo "Calibration params failed to synchronize. The params do not exist."
	else
		cp /mnt/persist/params /usr/share/data/adsp/
		md5sum params > params.md5
		echo "The calibration data is synchronized successfully. Bak to Src."
	fi
else
	if [ ! -f "$paramBak" ]; then
		md5sum params > params.md5
		cp /usr/share/data/adsp/params /mnt/persist/
		echo "The calibration data is synchronized successfully. Src to Bak."
	else
		
		md5sum -c --status params.md5
		status=$?
		if [ $status = 1 ]; then
			md5sum params > params.md5
			cp /usr/share/data/adsp/params /mnt/persist/
			echo "The calibration data is synchronized successfully. Src to Bak."
		else
			echo "No need to synchronize."	
		fi
	fi
fi




if [ ! -f "$tofSrc" ]; then	
	if [ ! -f "$tofBak" ]; then
		echo "Calibration tof failed to synchronize. The tof do not exist."
	else
		cp /mnt/persist/TOF.txt /usr/share/data/adsp/
		md5sum TOF.txt > tof.md5
		echo "The tof data is synchronized successfully. Bak to Src."
	fi
else
	if [ ! -f "$tofBak" ]; then
		md5sum TOF.txt > tof.md5
		cp /usr/share/data/adsp/TOF.txt /mnt/persist/
		echo "The tof data is synchronized successfully. Src to Bak."
	else
		
		md5sum -c --status tof.md5
		status=$?
		if [ $status = 1 ]; then
			md5sum TOF.txt > tof.md5
			cp /usr/share/data/adsp/TOF.txt /mnt/persist/
			echo "The tof data is synchronized successfully. Src to Bak."
		else
			echo "No need to synchronize."	
		fi
	fi
fi
