#!/bin/bash

sha256value_original=""
sha256value_new=""
buildUTC_original=""
buildUTC_new=""
package_size=""
package_size_new=""
command_t="--update_package=/media/internal/update.zip"
command_f=""
echo "check SdCard condition..."
if [ -d /mnt/sdcard ];then
	echo "find SdCard Mounted in /mnt/sdcard..."	
	if [ -f /mnt/sdcard/upgrade_package.tar ];then
		echo "Find upgrade_package.tar in sdcard..."
	else
		echo "Can't Find update.tar in sdcard..."
		exit 1
	fi
else
	echo "can't find Sdcard Directory,plz check your sdcard..."
	exit 1
fi

if [ -d /tmp/upgrade_package ];then
	/bin/rm -rf /tmp/upgrade_package
fi
if [ -d /tmp/system ];then
	/bin/rm -rf /tmp/system
fi

/bin/tar -xvf /mnt/sdcard/upgrade_package.tar -C /tmp/
wait
/bin/unzip /tmp/upgrade_package/update.zip "system/build.prop" -d /tmp/

if cat /tmp/system/build.prop | grep eagle8074 ;then
	echo "update.zip is related with eagle8074..."
else
	echo "Bad update.zip...."
	exit 1
fi
package_size=$(/usr/bin/du -sh /tmp/upgrade_package/update.zip)
echo "package_size=$package_size"
package_size=$(echo ${package_size:0:5})
echo "package_size=$package_size"
#todo:check md5...
sha256value_original=$( /bin/cat /tmp/upgrade_package/sha256value)
sha256value_original=$(echo ${sha256value_original:0:64})
sha256value_new=$( /usr/bin/sha256sum /tmp/upgrade_package/update.zip)
sha256value_new=$(echo ${sha256value_new:0:64})
if [ "$sha256value_original" = "$sha256value_new" ];then
	echo "sha256value_original=$sha256value_original"
	echo "sha256value_new=$sha256value_new"
	echo "MD5 check pass..."
	
else
	echo "MD5 check fail, different sha256 value..."
	echo "sha256value_original=$sha256value_original"
	echo "sha256value_new=$sha256value_new"
	exit 1	
fi
#todo:compare version...
buildUTC_original=$( /bin/cat /system/build.prop | grep utc)
buildUTC_new=$( /bin/cat /tmp/system/build.prop | grep utc)		
if [ "$buildUTC_original" != "$buildUTC_new" ];
then
	echo "compare version pass..."
	echo "buildUTC_original is:$buildUTC_original"
	echo "buildUTC_new is:$buildUTC_new"
else
	echo "compare version pass, same value: $buildUTC_new"
	exit 1
fi
echo "update.zip is what we want ..."

cp -f /tmp/system/build.prop /mnt/persist/
cp -f /tmp/upgrade_package/update.zip /media/internal && echo "Copy update.zip to /media/internal...."
wait
/bin/sync
package_size_new=$(/usr/bin/du -sh /media/internal/update.zip)
echo "package_size_new=$package_size_new"
package_size_new=$(echo ${package_size_new:0:5})
echo "package_size_new=$package_size_new"
if [ "$package_size_new" = "$package_size" ];then
	echo "cp complete,ok..."
else
	echo "cp incomlete,fail..."
	exit 1
fi
cp -f /home/linaro/upgrade/command /cache/recovery && echo "Copy command to /cache/recovery...."
wait
/bin/sync
command_f=$( /bin/cat /cache/recovery/command)
if [ "$command_f" != "$command_t" ];then
	echo "command file is bak,fail..."
	echo "command_f=$command_f....."
	exit 1
else
	echo "good command file ,ok...."
	echo "command_f=$command_f....."
fi
#rm /tmp/system/build.prop
#rm -rf /tmp/upgrade_package
#todo:set LED blink.......
echo "Call upgrade.sh.....now....."
sh /home/linaro/upgrade/upgrade.sh





