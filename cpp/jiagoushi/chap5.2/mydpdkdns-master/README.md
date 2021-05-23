mydpdkdns
============

[![License](https://img.shields.io/badge/license-Apache%202-4EB1BA.svg)](https://www.apache.org/licenses/LICENSE-2.0.html)


dns server with high performance, based on dpdk.

Documentation
=============
Please refer to [http://dpdk.org/doc](http://dpdk.org/doc).

Install dpdk
=============
```
tar xf dpdk-17.02.1.tar.xz -C /root
cd /root
mv dpdk-17.02.1 dpdk
cd dpdk

make config T=x86_64-native-linuxapp-gcc
make
make install T=x86_64-native-linuxapp-gcc
```

Set environment
=============
edit `.profile` or `.bashrc`, add the following:
```
export RTE_SDK=/root/dpdk
export RTE_TARGET=x86_64-native-linuxapp-gcc
```

Usage
=============
```
# git clone https://github.com/alandtsang/mydpdkdns.git
# cd mydpdkdns/tools
```

### record eth info
```
# ./recordeth.sh eth0
```
It will record the eth0 information to ethinfo file.

If recordeth.sh fails to execute, you need to manually create `ethinfo`.

e.g.
```
DEV=eth0
IPADDR=192.168.44.129
NETMADK=255.255.255.0
MAC=08:00:27:cf:bb:76
GATEWAY=192.168.44.2
```

### insert modules and mount hugetlbfs
```
# ./preparedpdk.sh eth0
```
It will bind eth0 to DPDK.

### start program
```
# ./start.sh
/root/mydpdkdns
-- RTE_SDK path：/root/dpdk
-- RTE_TARGET path：x86_64-native-linuxapp-gcc
-- RTE_SDK include path：/root/dpdk/include
-- RTE_SDK include path：/root/dpdk/lib
-- Configuring done
-- Generating done
-- Build files have been written to: /home/mydpdkdns/build
Scanning dependencies of target dserver
[100%] Built target dserver
EAL: Detected 4 lcore(s)
EAL: No free hugepages reported in hugepages-1048576kB
EAL: Probing VFIO support...
EAL: PCI device 0000:02:01.0 on NUMA socket -1
EAL:   probe driver: 8086:100f net_e1000_em
EAL: PCI device 0000:02:06.0 on NUMA socket -1
EAL:   probe driver: 8086:100f net_e1000_em
EAL: PCI device 0000:02:07.0 on NUMA socket -1
EAL:   probe driver: 8086:100f net_e1000_em
before Initialise each port
APP: Initialising port 0 ...
KNI: pci: 02:06:00 	 8086:100f
after Initialise each port

Checking link status
done
Port 0 Link Up - speed 1000 Mbps - full-duplex
APP: Lcore 1 is writing to port 0
APP: Lcore 2 is working to port 0
APP: Lcore 3 is sending to port 0
APP: Lcore 0 is reading from port 0
```

### configure IP and MAC addresses

`upeth.sh` will read the nic information in the `ethinfo`, and then configure IP and MAC address for vEth0.
```
# ./upeth.sh
APP: Configure network interface of 0 up
```

**Now, you can test the dns server.**


### stop program
```
# ./stop.sh
```

### unbind dpdk
```
# ./unbind.sh
```

### traffic monitoring
```
# ./monitor.sh
port:0  rx:399910 p/s  31994168 bytes/s  tx:397452 p/s  36577360 bytes/s  dropped:0
port:0  rx:400474 p/s  32039324 bytes/s  tx:400478 p/s  36843976 bytes/s  dropped:0
port:0  rx:396535 p/s  31724162 bytes/s  tx:396478 p/s  36475976 bytes/s  dropped:0
port:0  rx:401855 p/s  32149828 bytes/s  tx:399069 p/s  36729068 bytes/s  dropped:0
```

Support edns
============
```
dig @192.168.44.129 www.baidu.com +subnet=1.2.3.4

; <<>> DiG 9.11.1-P3 <<>> @192.168.44.129 www.baidu.com +subnet=1.2.3.4
; (1 server found)
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 28688
;; flags: qr rd ra; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 1

;; OPT PSEUDOSECTION:
; EDNS: version: 0, flags:; udp: 512
; CLIENT-SUBNET: 1.2.3.4/32/0
;; QUESTION SECTION:
;www.baidu.com.	IN	A

;; ANSWER SECTION:
www.baidu.com. 0	IN	A	153.37.234.35

;; Query time: 0 msec
;; SERVER: 192.168.44.129#53(192.168.44.129)
;; WHEN: Tue Jul 18 17:02:16 CST 2017
;; MSG SIZE  rcvd: 82
```

Get Help
============
The fastest way to get response is to send email to my mail:
- <zengxianglong0@gmail.com>

License
============
Please refer to [LICENSE](https://github.com/alandtsang/mydpdkdns/blob/master/LICENSE) file.
