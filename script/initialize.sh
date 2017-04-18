#!/bin/sh

start()
{
    retlog=`lsmod | grep axi_m_epc`
    if [ x"$retlog" = x ];then
        `insmod ../driver/axi_m_epc.ko`
    fi

}

remove()
{
    retlog=`lsmod | grep axi_m_epc`
    if [ x"$retlog" = x ];then
        echo "ignore, no find axi_m_epc.ko module!"
    else
        `rmmod axi_m_epc.ko`
    fi
}

sleep 1
#start
#./XiXmlDevice set
#./Register write 0x40600100 1
#./Register read 0x40600100 

