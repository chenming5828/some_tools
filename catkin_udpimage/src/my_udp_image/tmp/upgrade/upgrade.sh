#!/bin/bash

lunpath="/sys/class/android_usb/android0/f_mass_storage/lun/file"
mountpoint=$( /bin/lsblk | awk '{if($1~/mmcblk0p23/) {print $7}}' )

if [ "$mountpoint" = "" ];
then
	if [ ! -d /media/internal ];
	then
		/bin/mkdir /media/internal
	fi

	/bin/mount /dev/mmcblk0p23 /media/internal
	if [ ! $? -eq 0 ]
	then
		/bin/mount /dev/mmcblk0p23 /media/internal
		if [ ! $? -eq 0 ]
		then
			echo "[ERROR] upgrade_package storage mount failed!"
			exit 1
		fi
	fi
	echo "" > $lunpath
fi

echo "check upgrade condition..."
echo "current build version is:"
cat /system/build.prop

if [ ! -f /media/internal/update.zip ];then
        if [ ! -f /home/linaro/upgrade/update.zip ];then
	        echo "can't find update.zip in /media/internal..."
	        echo  "will not trigger upgrade..."
            exit 1
        fi
        cp /home/linaro/upgrade/update.zip /media/internal/
        if [ ! $? -eq 0 ];
        then
           echo "can't find update.zip in /media/internal..."
           echo  "will not trigger upgrade..."
           exit 1
        fi
        sync
fi
echo "find update.zip in /media/internal..."

if [ ! -f /cache/recovery/command ];then
	if [ ! -f /home/linaro/upgrade/command ];then
		  echo "can't find command in /cache/recovery..."
		  echo  "will not trigger upgrade..."
		  exit 1
	fi

	cp /home/linaro/upgrade/command /cache/recovery
	if [ ! $? -eq 0 ];then
		echo "can't find command in /cache/recovery..."
		echo  "will not trigger upgrade..."
		exit 1
    fi
    sync
fi
echo "find command in /cache/recovery..."

echo "will trigger upgrade..."
echo "current build version is:"
cat /system/build.prop
echo "will reboot to recovery..."
reboot2fastboot recovery

exit 0

