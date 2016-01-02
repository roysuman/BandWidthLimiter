-------------------------------------------------------------
#Bandwidth_Limiter:

BandWidth limiter is a tool to limit the bandwidth of a process in usser space(filtered by port).
 This tool works in linux environment.

------------------------------------------------------------

#Installation:

Pre-requisites
--------------

- gcc


Getting started
--------------
 - To build this project go to PWD:BandWidthLimiter.
 
 - sh make.sh

--------------------------------------------------------

# Execute:

I have used iperf ( a google tool) to test this tool.

1. You can use config file to provide port & limit info [ BandwidthLimiter/limiter.config ]
   Then to launch this tool use the folowing command. 
  [./blimiter -f -c "iperf -s -p 5000 -i 1"]

2. Command line option ( no need of config file )

[./blimiter -p 5000 -u 1024 -d 1024 -c "iperf -s -p 5000 -i 1" -g "/lib/x86_64-linux-gnu/libc.so.6"]

 replace iperf by your own programm. I have tested this tool under tomcat also.

 - you can provide only a single port to filter.

-----------------------------------------------------

#Example

- ServerSide: 

MY_PROJECT/BandWidthLimiter# ./blimiter -p 5000 -u 1024 -d 1024 -c "iperf -s -p 5000 -i 1" -g "/lib/x86_64-linux-gnu/libc.so.6"

IpAddress[ 127.0.0.1 ] 
Port[ 5000]
UpLimit [ 1024 ]
DownLimit [ 1024 ]
Latency [  ]
Command [ iperf -s -p 5000 -i 1 ] 

Argv2  [ iperf ] 
argv [ 0 ] = iperf
 argv [ 1 ] = -s
Port [ 5000 ] registerd 
------------------------------------------------------------
Server listening on TCP port 5000
TCP window size: 85.3 KByte (default)
------------------------------------------------------------
[  4] local 127.0.0.1 port 5000 connected with 127.0.0.1 port 55151
[ ID] Interval       Transfer     Bandwidth
[  4]  0.0- 1.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  1.0- 2.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  2.0- 3.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  3.0- 4.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  4.0- 5.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  5.0- 6.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  6.0- 7.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  7.0- 8.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  8.0- 9.0 sec  1000 KBytes  8.19 Mbits/sec
[  4]  9.0-10.0 sec  1000 KBytes  8.19 Mbits/sec
[  4] 10.0-11.0 sec  1000 KBytes  8.19 Mbits/sec
[  4] 11.0-12.0 sec  1000 KBytes  8.19 Mbits/sec
[  4] 12.0-13.0 sec  1000 KBytes  8.19 Mbits/sec
[  4] 13.0-14.0 sec  1000 KBytes  8.19 Mbits/sec
[  4] 14.0-15.0 sec  1000 KBytes  8.19 Mbits/sec


- ClientSide

MY_PROJECT/BandWidthLimiter# iperf -c 127.0.0.1 -p 5000
------------------------------------------------------------
Client connecting to 127.0.0.1, TCP port 5000
TCP window size: 2.50 MByte (default)
------------------------------------------------------------
[  3] local 127.0.0.1 port 55151 connected with 127.0.0.1 port 5000
[ ID] Interval       Transfer     Bandwidth
[  3]  0.0-10.0 sec  18.5 MBytes  15.5 Mbits/sec


---------------------------------------------------

BUG: mail me(email.suman.roy@x.y) where x = gmail
                                  y= com

