#!/bin/sh
echo "check upgrade result in [sdcard_post]..."
buildUTC_cur=""
buildUTC_upgrade=""
#todo:compare version...
if [ -f /mnt/persist/build.prop ];then
		echo "Find /mnt/persist/build.prop..."
	else
		echo "Can't Find /mnt/persist/build.prop..."
		exit 1
	fi
buildUTC_cur=$( /bin/cat /system/build.prop | grep utc)
buildUTC_upgrade=$( /bin/cat /mnt/persist/build.prop | grep utc)
if [ "$buildUTC_cur" != "$buildUTC_upgrade" ];
then
	echo "upgrade version fail..."
	echo "buildUTC_cur is:$buildUTC_cur"
	echo "buildUTC_upgrade is:$buildUTC_upgrade"
	exit 1
else
	echo "upgrade version successful, build version: $buildUTC_cur"
	rm /mnt/persist/build.prop
fi
exit 0


