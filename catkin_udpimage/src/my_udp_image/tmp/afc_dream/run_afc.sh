#!/bin/sh

PERSISTFILE=/mnt/persist/calib_innno_mgp01.txt
USERFILE=/usr/share/data/adsp/innno_fly_config.ini
FLAGS="="

if [ -f ${PERSISTFILE} ]; then
	for linecalib in $(grep -r "=" ${PERSISTFILE})
	do
	    CALIBFUNC=${linecalib%%=*}
		for lineuser in $(grep -r ${CALIBFUNC} ${USERFILE})
		do
			sed -i 's/'"$lineuser"'/'"$linecalib"'/g' ${USERFILE}
			sync
			break;
		done
	done
fi
guard()
{
	while true
	do 
		sn=`ps aux| grep $1 | grep -v grep |awk '{print $2}'`
		case $# in
		2)
		if [ "${sn}" = "" ]
		then
			cur_time=`date "+%Y-%m-%d_%H:%M:%S"`
			$1 &
			#$1 1>/data/${1}_${cur_time}.log 2>&1 &
		fi
		sleep $2 
		;;
		3)
		if [ "${sn}" = "" ]
		then
			cur_time=`date "+%Y-%m-%d_%H:%M:%S"`
			sleep 2
			$1 $2 &
			#$1 1>/data/${1}_${cur_time}.log 2>&1 &
		fi
		sleep $3 
		;;
		esac
	done
}

#sleep 2
#start afc
dir=/home/linaro/afc_dream/
cd /home/linaro/afc_dream/

./motor_cal
sleep 3

. ../control-engine/tool/setup.sh
./syncparam.sh
#./mainapp mainapp.config &
${dir}mainapp ${dir}mainapp.config &
guard ${dir}mainapp ${dir}mainapp.config 1 &

sleep 4
#./afc_dream &
${dir}afc_dream &
guard ${dir}afc_dream 1 &


