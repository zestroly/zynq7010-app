#!/bin/sh


stop_web()
{
    temp=`pidof appweb`
    if [ "x${temp}" == "x" ]
    then
	echo "no find appweb server"
    else
	echo "will kill appweb, and restart"
	killall appweb 
    fi
}


start_web()
{
    stop_web
    workdir=`pwd`
    cd /srv/bin
    ./appweb --log stdout:1 &
    cd ${workdir}
}

start_web
cat ../img/bitstream/vision_sensor.bit > /dev/xdevcfg

insmod ../driver/Sensor.ko
insmod ../driver/image.ko
#sh ./initialize.sh 



