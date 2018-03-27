#!/bin/bash
while true
do
echo test selction
echo 1--------------gpio  
echo 2--------------i2c
echo 3--------------uart
read
case "$REPLY" in 
'1')
echo "gpio test start......"
gpioloop=true
while $gpioloop
do
   echo "input gpio num:"
   read
   gpio=$REPLY
   echo "input is gpio:$gpio"
   echo $gpio > /sys/class/gpio/export
   echo "out" > /sys/class/gpio/gpio$gpio/direction
   valueloop=2
   value=1
   testresult=0
   while [ $valueloop -ne 0 ]
   do
      echo $value > /sys/class/gpio/gpio$gpio/value
      res=`cat /sys/class/gpio/gpio$gpio/value`
      #echo "res:$res value:$value"
      if [ $res = $value ]
      then
          valueloop=$(($valueloop - 1))
         # echo valueloop:$valueloop
          value=0
      else
          valueloop=0
          testresult=1
   	  echo "[gpio NG]"
      fi
   done
   if [ $testresult = 0 ]
   then
   echo "[gpio OK]"
   fi
   echo "continue gpio test:y? n?"
   read
   if [ $REPLY = "n" ]
   then
   break
   fi
done
;;
'2')
echo "i2c test start......."
i2cloop=true
while $i2cloop
do
echo "input i2c num"
read
i2c=$REPLY
echo "input is i2c:$i2c"
case "$i2c" in
*)
echo "i2c$i2c test start"
i2caddr=86
i2cdata=55
i2ccount=40
while [ $i2ccount -ne 0 ] 
do
i2ccount=$(($i2ccount - 1))
./i2ctest $i2c $i2caddr $i2cdata
done
echo "i2c test finish"
echo "i2c test continue: y? n?"
read
if [ $REPLY = "n" ] 
then
break
fi
;;
esac
done
;;
'3')
echo "uart test start......"
uartloop=true
uartopt1=u
while $uartloop
do
echo "input uart num"
read
uartnum=$REPLY
./uarttest ${uartopt1} $uartnum
echo "uart test continue: y? n?"
read
if [ $REPLY = "n" ]
then
break
fi
done
;;
esac
done
