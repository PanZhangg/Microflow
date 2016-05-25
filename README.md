# Microflow
The light-weighted, lightning fast OpenFlow SDN controller capable of multi-threads and clustering.

#Build & Run
cd src  

sudo make  

sudo ./microflow  


#Current status  
  

###Architecture Design                               DONE
Event drive  
One receive msg queue(able to adopt to multiple queue easily)  
Multiple workers  
Shared information among workers  

###Socket/epoll communication                        DONE    
Enable non-blocking & Async socket interface  

###Multithread implementation                        DONE
Separated thread for socket\worker\timer\others  
Allows full-utilization of CPU time when busy and zero CPU usage when free  
Upgrade to lock-free in the future    

###Memory pool implementation                        DONE
Memory pool for received msg  
Eliminate time-consumption malloc/free operations    

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
Packet_in (Arp) handler  
Register/unregister customized handler according to different msg type   

###Topology manager implementation Phase I
Maintain network links  
Maintain switch links  
Pre-alloced memory cells, eliminate malloc/free  
#TODO List  

###OpenFlow msg handler implementation Phase II  
LLDP handler  
Statistic msgs handler  
Flow-mod msgs  

###Topology manager implementation Phase II
Path calculation  
  
###Http server/Web GUI  
Http server  
Web GUI based on html & css(bootstrap)  

###cluster
A cluster service based on network games' mechanism  
  

###Documents  
An introduction doc & manual  
Architecture design doc  

#Contact & Bug Report
Pan Zhang  
dazhangpan@gmail.com  
