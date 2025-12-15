<div align="center">

# **Lab 2 – Computer Networks**

**Computer Networks – L.EIC, FEUP (2025/2026)**

**Authors:** Joana Louro (class 4), Luís Santos (class 4)  
**Date:** *23/12/2026*

</div>

# Summary
(do the summary latter)

# Introduction
(do the introduction latter)

# Part 1 - Development of a download application


– Report of a successful download, including print-screen of Wireshark logs showing the FTP packets -> Important ter a priint disto!!!

# Part 2 - Configuration and Study of a Network
## Experience 1 - Configure an IP Network
### Architecture of the Download Application

The FTP download application follows a modular and layered architecture.
Each stage of the FTP protocol is implemented in an independent function,
with the `main` function orchestrating the overall execution flow.

The application is divided into the following logical components:

- URL Processing: responsible for parsing and validating the FTP URL.
- Network Utilities: handle DNS resolution and TCP socket creation.
- FTP Control Channel: manages FTP commands and server replies.
- FTP Data Channel: handles passive mode connections and file transfer.
- Application Controller: coordinates all stages in the correct protocol order.

This design improves readability, maintainability, and protocol correctness.

### What are the ARP packets and what are they used for?
ARP (Address Resolution Protocol) packets are used to map a known IP address to an unknown MAC address inside a LAN.
In this experience, when we did a ping to tuxY4, after deleting the arp tables entries in the computer we were using tuxY3. 
Since we deleted the cache, when tuxY3 tries to see an entry for the 172.16.Y0.254 ip and does not found it, it sends and ARP request.
Therefore, tuxY3 sent an ARP Request (Who has 172.16.Y0.254? Tell 172.16.Y0.1). 
Then the tuxY4 sent the Arp reply (172.16.Y0.254 is at 8c:86:dd:84:c0:2e).

### What are the MAC and IP addresses of ARP packets and why?
#### ARP Request

!!!FALTA PREENCHER O MACADRESS DO TUX 3!!!!
| Field | MAC Address | IP Address |
|-------|-------------|------------|
| **Origin** |  (tuxY3)   | 172.16.Y0.1 (tuxY3) |
| **Destination** | ff:ff:ff:ff:ff:ff | 172.16.Y0.254 (tuxY4) |

#### ARP Reply

| Field | MAC Address | IP Address |
|------|------------|------------|
| **Origin** | 8c:86:dd:84:c0:2e (tuxY4) | 172.16.Y0.254 (tuxY4) |
| **Destination** | (tuxY3) | 172.16.Y0.1 (tux Y3) |


### What packets does the ping command generate?
It can generate ARP request if the destination is not in the ARP tables. It also generates a ICMP (Internet Control Message Protocol), this is used to test the connectivity and mesure latency. 
### What are the MAC and IP addresses of the ping packets?
??
In the case of the ARP packets this is answered before.
!!!PROF posso dizer isto!!!
For the ICMP packets preencher!!!:

| Field | MAC Address | IP Address |
|-------|-------------|------------|
| **Origin** |  (tuxY3)   | 172.16.Y0.1 (tuxY3) |
| **Destination** | ff:ff:ff:ff:ff:ff | 172.16.Y0.254 (tuxY4) |

#### ARP Reply

| Field | MAC Address | IP Address |
|------|------------|------------|
| **Origin** | 8c:86:dd:84:c0:2e (tuxY4) | 172.16.Y0.254 (tuxY4) |
| **Destination** | (tuxY3) | 172.16.Y0.1 (tux Y3) |

colocar o que está no wireshark, src and destination

### How to determine if a receiving Ethernet frame is ARP, IP, ICMP?
?? mostrar print o cabeçalho mac tem os pacotes  parte do header que diz de que pacote pertecne!!!

### How to determine the length of a receiving frame?
The lenght of a frame is already
??

### What is the loopback interface and why is it important?


at the end put the wireshark picture!


## Experience 2 - Implement two bridges in a switch

### How to configure bridgeY0?
### How many broadcast domains are there? How can you conclude it from the logs?

## Experience 3 - Configure a Router in Linux

### What routes are there in the tuxes? What are their meaning?
### What information does an entry of the forwarding table contain?
### What ARP messages, and associated MAC addresses, are observed and why?
### What ICMP packets are observed and why?
### What are the IP and MAC addresses associated to ICMP packets and why?

## Experience 4 - Configure a Commercial Router and Implement NAT

### How to configure a static route in a commercial router?
### What are the paths followed by the packets, with and without ICMP redirect enabled, in the experiments carried out and why?
### How to configure NAT in a commercial router?
### What does NAT do?
### What happens when tuxY3 pings the FTP server with the NAT disabled? Why?

## Experience 5 - DNS

### How to configure the DNS service in a host?
### What packets are exchanged by DNS and what information is transported

## Experience 6 - TCP connections

### How many TCP connections are opened by your FTP application?
### In what connection is transported the FTP control information?
### What are the phases of a TCP connection?
### How does the ARQ TCP mechanism work? What are the relevant TCP fields? What relevant information can be observed in the logs?
### How does the TCP congestion control mechanism work? What are the relevant fields. How did the throughput of the data connection evolve along the time? Is it according to the TCP congestion control mechanism?
### Is the throughput of a TCP data connections disturbed by the appearance of a second TCP connection? How?

# Conclusion

# Anexes

!PROF!
- perguntar ao prof como ele quer os anexos e a estrutura de cada experiência e/ou os comandos de cada experiência e as fotos referentes!
- o prof quer indice? por causa do max...
- para por o código da aplication, o prof quer que eu literalmente copie e cole?
Adaptar os relatórios com os resultados recebidos!


