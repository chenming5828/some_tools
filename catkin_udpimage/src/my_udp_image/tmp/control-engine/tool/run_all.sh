#!/bin/sh

#remove update.zip
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
		fi
	fi
	echo "" > $lunpath
	rm -f /media/internal/update.zip
fi

if [ ! -d /mnt/persist ];
then
	/bin/mkdir -p /mnt/persist
fi
if [ ! -f /mnt/persist/hostapd.conf ]
then
	/bin/cp /etc/hostapd.conf /mnt/persist/hostapd.conf
fi
#add by liumy1019 at 15/12/21 for bug:24527 start
if [ -f /var/log/syslog ];then
	echo "/var/log/syslog already exist!"
else
	touch /var/log/syslog
	dmesg >> /var/log/syslog
	chmod 777 /var/log/syslog
	sync
fi

sh /home/linaro/remove_invalid_thumb.sh

cd /home/linaro
mkdir -p test/emmc_sdcard
mkdir -p test/emmc_sdcard/pic
mkdir -p test/emmc_sdcard/video
mkdir -p testhttp/exter_sd
mkdir -p testhttp/exter_sd/pic
mkdir -p testhttp/exter_sd/video

cd /home/linaro/control-engine/tool/
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

echo "start control_engine"
#./control_engine > /tmp/control_engine.log 2>&1 &
./tsce_daemon

sleep 1

cd /home/linaro/camera/tool/
echo "start camera-service"
#./camera-service > /tmp/camera-service.log 2>&1 &
./cs_daemon > /tmp/camera-service.log 2>&1 &

sleep 1

cd /home/linaro/http/tool/
echo "start http test"
./mongoose > /tmp/mongoose.log 2>&1 &

cd -
#add by liumy1019 for logrotate

if [ -f /etc/logrotate.d/myrsyslog ];then
	cp /etc/logrotate.d/myrsyslog /etc/logrotate.d/rsyslog -Rp
	reload rsyslog
	rm /etc/logrotate.d/myrsyslog
fi

#add end by liumy1019
#add being for syslog
chmod 777 /home/linaro/do_logrotate
/home/linaro/do_logrotate > /var/log/do_logrotate
chmod 600 /etc/crontab
#add end for syslog

#add lk
/home/linaro/power_key/pk-app &

#start stereo-test for binocular algorithm
/usr/bin/stereo-test &

cd /home/linaro/afc_dream/
. ./run_afc.sh
