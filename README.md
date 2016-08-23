![](docs/Microflow.png)
# Microflow
The light-weighted, lightning fast OpenFlow SDN controller capable of multi-threads and clustering.

#Build & Run
cd src  

sudo make  

sudo ./microflow  

WebUI can be accessed at: localhost:8000  

#Current status  
  

###Architecture Design                               DONE  
Async event drive  
Multiple lock-free recv queues  
One producer & Multiple workers  
Shared memory(trying to make it lock-free) among workers  

###Socket/epoll communication                        DONE    
Enable non-blocking & Async socket interface  
Handled by dedicated thread & CPU core  

###Multi-thread implementation                        DONE
Separated thread for socket\worker\timer\others  
Allows full-utilization of CPU time when busy and zero CPU usage when free  
Lock-Free msg recv queue implementation  
CPU affinity allows to take the full advantage of multi-core CPUs  
 
###Memory pool implementation                        DONE
Memory pool for received msgs  
Eliminate time-consumption malloc/free operations  
Eliminate redundant memory copy actions in user space  

###Timer & Logger implementation                     DONE
Implement particular function in a cycled way  
Time precision is 1 ms  
Log particular msgs to log file with timestamp  
Not for real-time application  

###Device manager implementation Phase I&II             DONE
Maintain a hash-table for all the switches & hosts in the network  
Pre-alloced memory cells, eliminate malloc/free  

###OpenFlow msg handler implementation Phase I&II       DONE  
Hello & Echo & Feature_request handler  
Packet out sender  
Packet_in handler  
Multipart reply msg handler  
ARP msg handler  
LLDP msg handler  
Port statistic msg handler  
Register/unregister customized handler according to different msg type 

###Topology manager implementation Phase I DONE
Maintain network links  
Maintain switch links  
Pre-alloced memory cells, eliminate malloc/free  

###Logo Design  DONE
:D Thanks to Miss Hong :p  

###Http server/Web GUI  DONE
Http server  
Web GUI based on bootstrap jQuery and D3.js  

#TODO List  

###OpenFlow msg handler implementation Phase II  
Statistic msgs handler  
Flow-mod msgs  

###Topology manager implementation Phase II
Path calculation  
  
###Http server/Web GUI  Phase II
More WebUI features  
Devices(switches + hosts) status  
Topo status  
Warning/error messages  
Port/Link status  

###Lock-free linked-list & hashtable
Used for device manager and topo manager  
Eliminate performance bottleneck in multi-thread environment  

###Cluster
A cluster service based on network games' mechanism  
  
###Documents  
An introduction doc & manual  
Architecture design doc  

#Contact & Bug Report
Pan Zhang  
dazhangpan@gmail.com  
