#!/bin/bash

internalid=""
sdcardid=""
fileType=""
lunpath=""
blkname=""
sdmountpoint=""
intermountpoint=""

internalid=$( /bin/lsblk | awk '{if($1~/mmcblk0p23/) {print $2}}' )
sdcardid=$( /bin/lsblk | awk '{if($1~/mmcblk1p1/) {print $2}}' )
lunpath="/sys/class/android_usb/android0/f_mass_storage/"

start_umountInternal() {
	#umount Internal storage
	if [ "$internalid" != "" ];
	then
		fileType=$( blkid | awk '{if($1=="/dev/mmcblk0p23:") {print $NF}}')
		if [ "$fileType" = "" ];
		then
			if [ -d $lunpath/lun ];
			then
				echo "" > $lunpath/lun/file
			fi
			/sbin/mkfs.ext4 /dev/mmcblk0p23
			echo "[INFO] Internal storage has been formatted to ext4!"
		else
			if [ "$fileType" = "TYPE=\"ext4\"" ];
			then
				echo "[INFO] Internal storage was ext4!"
			else
				echo "[INFO] Internal storage's format is $fileType!"
			fi
		fi

		#/media/internal delete
		if [ -d /media/internal ];
		then
			/bin/umount /dev/mmcblk0p23
			if [ ! $? -eq 0 ]
			then
				#/media/internal delete
				if [ -d /media/internal ];
				then
					/bin/rm -rf /media/internal
				fi
				echo "[ERROR] Internal storage busy, umount failed!"
				return 1
			else
				echo "[INFO] Internal storage has been umounted!"
			fi
		fi

		#set internal storage's block ID to lun
		if [ -d $lunpath/lun ];
		then
			echo "/dev/block/$internalid" > $lunpath/lun/file
		fi
	else
		if [ -d $lunpath/lun ];
		then
			echo "" > $lunpath/lun/file
		fi

		#/media/internal delete
		if [ -d /media/internal ];
		then
			/bin/rm -rf /media/internal
		fi
		echo "[INFO] Internal storage isn't found!"
		return 1
	fi

	#/media/internal delete
	if [ -d /media/internal ];
	then
		/bin/rm -rf /media/internal
	fi
	return 0
}

start_umountSDcard() {
	#umount SDcard
	if [ "$sdcardid" != "" ];
	then
		fileType=$( blkid | awk '{if($1=="/dev/mmcblk1p1:") {print $NF}}')
		if [ "$fileType" = "" ];
		then
			if [ -d $lunpath/lunext0 ];
			then
				echo "" > $lunpath/lunext0/file
			fi
			/sbin/mkfs.ext4 /dev/mmcblk1p1
			echo "[INFO] SDcard has been formatted to ext4!"
		else
			if [ "$fileType" = "TYPE=\"ext4\"" ];
			then
				echo "[INFO] SDcard was ext4!"
			else
				echo "[INFO] SDcard's format is $fileType!"
			fi
		fi

		if [ -d /media/sdcard ];
		then
			/bin/umount /dev/mmcblk1p1
			if [ ! $? -eq 0 ]
			then
				echo "[ERROR] SDcard umount failed!"
				#/media/sdcard delete
				if [ -d /media/sdcard ];
				then
					/bin/rm -rf /media/sdcard
				fi

				if [ -d /mnt/sdcard ];
				then
					/bin/rm -rf /mnt/sdcard
				fi
				return 1
			else
				echo "[INFO] SDcard has been umounted!"
			fi
		fi

		#set SDcard's block ID to lun
		if [ -d $lunpath/lunext0 ];
		then
			echo "/dev/block/$sdcardid" > $lunpath/lunext0/file
		fi
	else
		if [ -d $lunpath/lunext0 ];
		then
			echo "" > $lunpath/lunext0/file
		fi

		#/media/sdcard delete
		if [ -d /media/sdcard ];
		then
			/bin/rm -rf /media/sdcard
		fi

		if [ -d /mnt/sdcard ];
		then
			/bin/rm -rf /mnt/sdcard
		fi

		echo "[INFO] NO SDcard,Please insert SDcard and reboot!"
		return 1
	fi

	#/media/sdcard delete
	if [ -d /media/sdcard ];
	then
		/bin/rm -rf /media/sdcard
	fi

	if [ -d /mnt/sdcard ];
	then
		/bin/rm -rf /mnt/sdcard
	fi

	return 0
}

start_umount() {
	intermountpoint=$( /bin/lsblk | awk '{if($1~/mmcblk0p23/) {print $7}}' )
	if [ "$intermountpoint" = "/media/internal" ]
	then
		chmod -R 0777 /media/internal/.

		start_umountInternal
		retInternal=$?
	else
		#/media/internal delete
		if [ -d /media/internal ];
		then
			/bin/rm -rf /media/internal
		fi
		retInternal=0;
		echo "[INFO] Internal storage umount succeed!"

		#set internal storage's block ID to lun
		if [ -d $lunpath/lun ];
		then
			if [ "$internalid" != "" ]
			then
				echo "/dev/block/$internalid" > $lunpath/lun/file
			fi
		fi
	fi

	sdmountpoint=$( /bin/lsblk | awk '{if($1~/mmcblk1p1/) {print $7}}' )
	if [ "$sdmountpoint" = "/mnt/sdcard" ]
	then
		start_umountSDcard
		retSDcard=$?
	else
		#/media/sdcard delete
		if [ -d /media/sdcard ];
		then
			/usr/bin/unlink /media/sdcard
		fi

		if [ -d /media/sdcard ];
		then
			/bin/rm -rf /media/sdcard
		fi

		retSDcard=0;
		echo "[INFO] SDcard umount succeed!"

		#set SDcard's block ID to lun
		if [ -d $lunpath/lunext0 ];
		then
			if [ "$sdcardid" != "" ]
			then
				echo "/dev/block/$sdcardid" > $lunpath/lunext0/file
			fi
		fi
	fi

	if [ $retInternal -eq 0 ] && [ $retSDcard -eq 0 ]
	then
		echo "[INFO] Internal block and SDcard umount succeed!"
		return 0
	elif [ $retInternal -eq 1 ] && [ $retSDcard -eq 0 ]
	then
		echo "[INFO] Internal block umount failed and SDcard umount succeed!"
		return 1
	elif [ $retInternal -eq 0 ] && [ $retSDcard -eq 1 ]
	then
		echo "[INFO] Internal block umount succeed and SDcard umount failed!"
		return 2
	else
		echo "[ERROR] Internal block umount failed and SDcard umount failed!"
		return 3
	fi
}

#start_mountInternal && start_mountSDcard
start_umount
