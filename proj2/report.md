- Network architecture, experiment objectives, main configuration commands, relevant logs
- Analysis of the logs captured that are relevant for the learning objectives

# Part 2 - Network configuration and analysis
## Experience 1
### What are the ARP packets and what are they used for?
ARP (Address Resolution Protocol) packets are used to map a known IP address to an unknown MAC address inside a LAN.
In this experience, when we did a ping to tuxY4, after deleting the arp tables entries in the computer we were using (tuxY3). 
Since we deleted the ARP, when tuxY3 tries to see an entry for the ip and does not found it, it sends and ARP request.
The tuxY3 sent an ARP Request (Who has 172.16.Y0.254? Tell 172.16.Y0.1). 
Then the tuxY4 sent the Arp reply (172.16.Y0.254 is at 8c:86:dd:84:c0:2e).


### What are the MAC and IP addresses of ARP packets and why?


### What packets does the ping command generate?
It can generate ARP request if the destination is not in the ARP tables. It also generates a ICMP (Internet Control Message Protocol), this is used to test the connectivity and mesure latency. 
### What are the MAC and IP addresses of the ping packets?
??
colocar o que está no wireshark, src and destination

### How to determine if a receiving Ethernet frame is ARP, IP, ICMP?
?? mostrar print o cabeçalho mac tem os pacotes ...

### How to determine the length of a receiving frame?
The lenght of a frame is already

### What is the loopback interface and why is it important?


at the end put the wireshark picture!