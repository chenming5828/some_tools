#!/bin/bash

sdcardid=""
internalid=""
fileType=""
lunpath=""
blkname=""
sdmountpoint=""
intermountpoint=""

sdcardid=$( /bin/lsblk | awk '{if($1~/mmcblk1p1/) {print $2}}' )
internalid=$( /bin/lsblk | awk '{if($1~/mmcblk0p23/) {print $2}}' )
lunpath="/sys/class/android_usb/android0/f_mass_storage/"

start_mountInternal() {
	#/media/internal create
	if [ ! -d /media/internal ];
	then
		/bin/mkdir /media/internal
	else
		/bin/umount /media/internal
		if [ ! $? -eq 0 ]
		then
			/bin/umount /dev/mmcblk0p23
			if [ ! $? -eq 0 ]
			then
				/bin/rm -rf /media/internal
				if [ ! $? -eq 0 ]
				then
					echo "[ERROR] Internal storage busy, mount failed!"
					return 1
				fi
				/bin/mkdir /media/internal
			else
				/bin/rm -rf /media/internal/*
			fi
		else
			/bin/rm -rf /media/internal/*
		fi
	fi

	#set block23's filetype
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

		#mount internal storage to target path
		if /bin/mount  | grep /media/internal;
		then
			/bin/umount /media/internal || {
				echo "[ERROR] Could not unmount already mounted /media/internal!"
				return 1
			}
		fi

		if [ -d $lunpath/lun ];
		then
			echo "" > $lunpath/lun/file
		fi

		/bin/mount /dev/mmcblk0p23 /media/internal
		if [ ! $? -eq 0 ]
		then
			/bin/mount /dev/mmcblk0p23 /media/internal
			if [ ! $? -eq 0 ]
			then
				/sbin/mkfs.ext4 /dev/mmcblk0p23
				/bin/mount -t ext4 /dev/mmcblk0p23 /media/internal
				if [ ! $? -eq 0 ]
				then
					echo "[ERROR] Internal storage mount failed!"
					return 1
				fi
			fi
		fi

		#test current partition
		cd /media/internal
		/bin/touch .IndexerVolumeGuid
		if [ ! $? -eq 0 ]
		then
			echo "[ERROR] Internal storage was damaged and mount failed!"
			return 1
		else
			/bin/rm .IndexerVolumeGuid
			echo "[INFO] Internal storage mount succeed!"
			return 0
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

		echo "[INFO] Mounted failed， Internal storage don't exist!"
		return 1
	fi
}


start_mountSDcard() {
	#set SDcard's filetype
	if [ "$sdcardid" != "" ];
	then
		if [ ! -d /mnt/sdcard ];
		then
			/bin/mkdir /mnt/sdcard
		else
			/bin/umount /mnt/sdcard
			if [ ! $? -eq 0 ]
			then
				/bin/umount /dev/mmcblk1p1
				if [ ! $? -eq 0 ]
				then
					/bin/rm -rf /mnt/sdcard
					if [ ! $? -eq 0 ]
					then
						echo "[ERROR] SDcard busy, umount failed!"
						return 1
					fi
					/bin/mkdir /mnt/sdcard
				else
					/bin/rm -rf /mnt/sdcard/*
				fi
			else
				/bin/rm -rf /mnt/sdcard/*
			fi
		fi

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

		if [ -d $lunpath/lunext0 ];
		then
			echo "" > $lunpath/lunext0/file
		fi

		/bin/mount /dev/mmcblk1p1 /mnt/sdcard
		if [ ! $? -eq 0 ]
		then
			/bin/mount /dev/mmcblk1p1 /mnt/sdcard
			if [ ! $? -eq 0 ]
			then
				/sbin/mkfs.ext4 /dev/mmcblk1p1
				/bin/mount -t ext4 /dev/mmcblk1p1 /mnt/sdcard
				if [ ! $? -eq 0 ]
				then
					echo "[ERROR] SDcard was damaged and mount failed!"
					return 1
				fi
			fi
		fi

		sdmountpoint=$( /bin/lsblk | awk '{if($1~/mmcblk1p1/) {print $7}}' )
		#set sdcard link
		if [ "$sdmountpoint" = "" ]
		then
			/bin/mount /dev/mmcblk1p1 /mnt/sdcard
		fi

		#/mnt/sdcard link to /media/sdcard
		if [ -d /media/sdcard ];
		then
			/bin/rm -rf /media/sdcard
		fi
		/bin/ln -sf /mnt/sdcard /media/sdcard

		#test current partition
		cd /media/sdcard
		/bin/touch .IndexerVolumeGuid
		if [ ! $? -eq 0 ]
		then
			echo "[ERROR] SDcard was damaged and mount failed!"
			return 1
		else
			/bin/rm .IndexerVolumeGuid
			echo "[INFO] SDcard mount succeed!"
			return 0
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

		echo "[INFO] SDcard Mount failed, SDcard don't exist!"
		return 1
	fi
}

start_mount() {
	intermountpoint=$( /bin/lsblk | awk '{if($1~/mmcblk0p23/) {print $7}}' )
	if [ "$intermountpoint" != "/media/internal" ]
	then
		start_mountInternal
		retInternal=$?
	else
		retInternal=0;
		echo "[INFO] Internal storage mount succeed!"

		if [ -d $lunpath/lun ];
		then
			echo "" > $lunpath/lun/file
		fi
	fi

	sdmountpoint=$( /bin/lsblk | awk '{if($1~/mmcblk1p1/) {print $7}}' )
	#set sdcard link
	if [ "$sdmountpoint" = "/mnt/sdcard" ]
	then
		#/media/sdcard delete
		if [ -d /media/sdcard ];
		then
			/usr/bin/unlink /media/sdcard
		fi

		if [ -d /media/sdcard ];
		then
			/bin/rm -rf /media/sdcard
		fi
		/bin/ln -sf /mnt/sdcard /media/sdcard
		retSDcard=$?
		echo "[INFO] SDcard mount succeed!"

		if [ -d $lunpath/lunext0 ];
		then
			echo "" > $lunpath/lunext0/file
		fi
	else
		start_mountSDcard
		retSDcard=$?
	fi

	if [ $retInternal -eq 0 ] && [ $retSDcard -eq 0 ]
	then
		echo "[INFO] Internal storage and SDcard mount succeed!"
		return 0
	elif [ $retInternal -eq 1 ] && [ $retSDcard -eq 0 ]
	then
		echo "[INFO] Internal storage mount failed and SDcard mount succeed!"
		return 1
	elif [ $retInternal -eq 0 ] && [ $retSDcard -eq 1 ]
	then
		echo "[INFO] Internal storage mount succeed and SDcard mount failed!"
		return 2
	else
		echo "[ERROR] Internal storage mount failed and SDcard mount failed!"
		return 3
	fi
}

#start_mountInternal && start_mountSDcard
start_mount

