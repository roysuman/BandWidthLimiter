-------------------------------------------------------------
#BANDWIDTH_LIMITER:

BandWidth limiter is a tool to limit the bandwidth of a process in usser space(filtered by port).
 This tool works in linux environment.

------------------------------------------------------------

#INSTALLATION:

To install this software only a basic version of gcc is required. 

PWD:BandWidthLimiter

sh make.sh

--------------------------------------------------------

# Execute:

I have used iperf ( a google tool) to test this tool.

1. You can use config file to provide port & limit info [ BandwidthLimiter/limiter.config ]
   Then to launch this tool use the folowing command. 
  [./blimiter -f -c "iperf -s -p 5000 -i 1"]

2. Command line option ( no need of config file )

[./blimiter -p 5000 -u 1024 -d 1024 -c "iperf -s -p 5000 -i 1" -g "/lib/x86_64-linux-gnu/libc.so.6"]
you can provide only a single port to filter.

-----------------------------------------------------

BUG: mail me(email.suman.roy@x.y) where x = gmail
                                  y= com

