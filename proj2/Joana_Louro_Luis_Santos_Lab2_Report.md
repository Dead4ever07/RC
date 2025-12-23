<div align="center">

# **Lab 2 – Computer Networks**

**Computer Networks – L.EIC, FEUP (2025/2026)**

**Authors:** Joana Louro (class 4), Luís Santos (class 4)  
**Date:** *23/12/2025*

</div>

# Summary
(do the summary latter)

# Introduction
(do the introduction latter)

# Part 1 - Development of a download application
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


– Report of a successful download, including print-screen of Wireshark logs showing the FTP packets -> Important ter a priint disto!!!!!!!!

# Part 2 - Configuration and Study of a Network
## Experience 1 - Configure an IP Network

### What are the ARP packets and what are they used for?
ARP (Address Resolution Protocol) packets are used to map a known IP address to an unknown MAC address inside a LAN.
In this experience, when we did a ping to tuxY4, after deleting the arp tables entries in the computer we were using tuxY3. 
Since we deleted the cache, when tuxY3 tries to see an entry for the 172.16.Y0.254 ip and does not found it, it sends and ARP request.
Therefore, tuxY3 sent an ARP Request (Who has 172.16.Y0.254? Tell 172.16.Y0.1). 
Then the tuxY4 sent the Arp reply (172.16.Y0.254 is at 8c:86:dd:84:c0:2e).

### What are the MAC and IP addresses of ARP packets and why?
#### ARP Request

| Field | MAC Address | IP Address |
|-------|-------------|------------|
| **Origin** |  ec:75:0c:c2:51:c1 (tuxY3)   | 172.16.Y0.1 (tuxY3) |
| **Destination** | ff:ff:ff:ff:ff:ff (broadcast) | 172.16.Y0.254 (tuxY4) |

#### ARP Reply

| Field | MAC Address | IP Address |
|------|------------|------------|
| **Origin** | 8c:86:dd:84:c0:2e (tuxY4) | 172.16.Y0.254 (tuxY4) |
| **Destination** | ec:75:0c:c2:51:c1 (tuxY3) | 172.16.Y0.1 (tux Y3) |

![ARP](images/arp_mac_adresses.png) 

### What packets does the ping command generate?
It can generate ARP request if the destination is not in the ARP tables. It also generates a ICMP (Internet Control Message Protocol), this is used to test the connectivity and mesure latency. 
### What are the MAC and IP addresses of the ping packets?
In the case of the ARP packets this is answered before.
For the ICMP packets:
#### ICMP Echo request
| Field | MAC Address | IP Address |
|-------|-------------|------------|
| **Origin** |  ec:75:0c:c2:51:c1 (tuxY3)   | 172.16.Y0.1 (tuxY3) |
| **Destination** | 8c:86:dd:84:c0:2e (tuxY4) | 172.16.Y0.254 (tuxY4) |

#### ICMP Echo reply

| Field | MAC Address | IP Address |
|------|------------|------------|
| **Origin** | 8c:86:dd:84:c0:2e (tuxY4) | 172.16.Y0.254 (tuxY4) |
| **Destination** | ec:75:0c:c2:51:c1(tuxY3) | 172.16.Y0.1 (tux Y3) |

### How to determine if a receiving Ethernet frame is ARP, IP, ICMP?
The value in the Ethernet II type changes. The type is in the header after the origin mac adress and the destination mac adress. As you can see in this image.

![Frame type](images/arp_icmp_type.png) 

When the frame type is:
- **0x0806** we know it contains an **ARP packet**
- **0x0800** we know it contains an **IP packet**

#### IP Packet Protocol Identification:

Once we've identified that an Ethernet frame contains an IP packet, we need to determine what protocol is encapsulated inside the IP packet. 

We do this by examining the Protocol field in the IP header.
- When the IP Protocol field is 1, the IP packet contains an ICMP message.
- When the IP Protocol field is 6, the IP packet contains a TCP segment.
- When the IP Protocol field is 17, the IP packet contains a UDP datagram.

![Protocol Field](images/protocol_field.png)

### How to determine the length of a receiving frame?
In Ethernet II, there is no Length field in the MAC header. The frame length is determined by the MAC using signals from the Physical layer.
The phisical layer detects the Start Frame Delimiter (SFD), which indicates the beginning of the frame, and the MAC starts counting bytes from that point.
The end of the frame is detected when the phisical layer signals end of carrier / end of reception. At that moment, the MAC stops counting bytes.

### What is the loopback interface and why is it important?
A loopback interface is a virtual software network connection that enables the computer to send data to itself. Any packet sent to the loopback interface never leaves the system; it is immediately received and processed internally by the network stack as if it had arrived from an external network.

This interface is always active and unaffected by the state of the physical network interfaces. It has a consistent IP address that remains available even if physical links go down. 

By allowing the system to act simultaneously as both sender and receiver, the loopback interface provides a reliable mechanism for self-communication.

## Experience 2 - Implement two bridges in a switch

### How to configure bridgeY0?
To configure bridgeY0 we need to create the bridgeY0 interface on a Mikrotik switch. Next, we remove the selected ports from the original bridge to prevent configuration conflicts.
Finaly, we add these Ethernet ports to the newly created bridgeY0, assigning them as bridge ports.

### How many broadcast domains are there? 
A broadcast domain is a logical network segment where all devices receive a copy of every broadcast frame sent. There are two broadcast domains. Each configured bridge (bridgeY0 and bridgeY1) defines a separate broadcast domain. Devices connected to the same bridge belong to the same broadcast domain, while broadcasts are not forwarded between different bridges.
### How can you conclude it from the logs?
A broadcast frame uses the destination MAC address ff:ff:ff:ff:ff:ff, which is flooded by the switch to all ports belonging to the same broadcast domain only.
#### When a broadcast ping was sent from tuxY3:
- The broadcast ICMP Echo Request was captured on tuxY4, which is connected to the same bridge (bridgeY0).
- No broadcast packet was captured on tuxY2, which belongs to bridgeY1.

This shows that the broadcast was confined to the bridgeY0 broadcast domain.

PICC do tux4 ao fazer broadcast no 3 + pic do y2!! (mostrar a primeira parte) !!!

#### When a broadcast ping was sent from tuxY2:
- No packets were captured on tuxY3 or tuxY4, which are connected to bridgeY0.

This confirms that the broadcast was confined to the bridgeY1 broadcast domain.

PICC do tux4 ao fazer broadcast no 3 + pic do y2!! (mostrar a segunda parte) !!!

Therefore, the captured logs demonstrate that broadcasts do not cross bridges and confirm the existence of two distinct broadcast domains.

## Experience 3 - Configure a Router in Linux

### What routes are there in the tuxes? 
Each tux will have different routing table entries:
#### tuxY3:
| Number | Destination | Gateway | Genmask | Iface |
| --     |-------------|---------|---------|-------|
|     1  | 0.0.0.0 | 10.227.20.254 | 0.0.0.0 | if_mng |
|     2  | 10.227.20.0 | 0.0.0.0 | 255.255.255.0 | if_mng |
|     3  | 172.16.Y0.0 | 0.0.0.0 | 255.255.255.0 | if_e1 |
|     4  | 172.16.Y1.0 | 172.16.Y0.254 | 255.255.255.0 | if_e1 |

#### tuxY4:
| Number | Destination | Gateway | Genmask | Iface |
| --     |-------------|---------|---------|-------|
|     1  | 0.0.0.0 | 10.227.20.254 | 0.0.0.0 | if_mng |
|     2  | 10.227.20.0 | 0.0.0.0 | 255.255.255.0 | if_mng |
|     5  | 172.16.Y0.0 | 0.0.0.0 | 255.255.255.0 | if_e1 |
|     6  | 172.16.Y1.0 | 0.0.0.0 | 255.255.255.0 | if_e2 |

#### tuxY2:
| Number | Destination | Gateway | Genmask | Iface |
| --     |-------------|---------|---------|-------|
|     1  | 0.0.0.0 | 10.227.20.254 | 0.0.0.0 | if_mng | 
|     2  | 10.227.20.0 | 0.0.0.0 | 255.255.255.0 | if_mng | 
|     7  | 172.16.Y0.0 | 172.16.Y1.253 | 255.255.255.0 | if_e1 | 
|     8  | 172.16.Y1.0 | 0.0.0.0 | 255.255.255.0 | if_e1 |

### What are their meaning?

**1.** The route is used to send all trafic that does not match 

The route Nº 1 is used to send all trafic that does not fit the other routes into the router.


### What information does an entry of the forwarding table contain?
Each forwarding table entry contains:
#### Destination network
The Destination network is the IP address or network prefix that this route applies to. It can be a single host (a specific IP address) or a network/subnet (e.g. 192.168.1.0/24). A destination of 0.0.0.0 represents the default route, which matches any destination not covered by more specific routes.

When forwarding packets, the router compares the destination IP of each outgoing packet against this field. The routing algorithm uses **longest prefix matching**, meaning the route with the most specific match is chosen.
#### Gateway (gw)
The Gateway is the IP address of the next router that should receive packets for this destination. 

- When the gateway is **0.0.0.0**, the route is **directly connected**. No intermediate router is needed because the destination is on the local network segment. The system can reach it directly.

- When the gateway is an IP address (e.g. 172.16.80.254) the route is indirect.Packets must first be sent to this gateway router, which then forwards them toward the final destination. The gateway must be reachable via a directly connected network. This means the gateway's IP address must be on the same subnet as one of the router's interfaces.
#### Genmask:
The Genmask defines which bits of the IP address represent the network portion versus the host portion. The mask is applied (bitwise AND) to both the destination IP and the route's destination. If they match after masking, the route applies.
It can be written as:
- Dotted decimal: 255.255.255.0
- CIDR prefix length: /24
**Example:**
- Genmask 255.255.255.0 (/24) = 24 network bits, 8 host bits = 256 addresses
- Genmask 0.0.0.0 (/0) = 0 network bits = matches all addresses (default route)

#### Flags
Flags are single-letter indicators showing the route's properties and status.

Common flags include:
- **U (Up)**: Route is active and usable
- **G (Gateway)**: Route uses a gateway
- **H (Host)**: Route is to a specific host (/32), not a network
- **D (Dynamic)**: Route was created dynamically by routing protocols or ICMP redirects

Multiple flags can be combined. For example, **UG** means the route is both Up and uses a Gateway.

#### Metric
Metric is a numeric value representing the cost or distance of a route. Lower values are preferred. When multiple routes exist to the same destination, the metric determines which one to use.

#### Ref
Ref is the number of references to this route. It counts how many times this route entry is currently being referenced by the kernel. Primarily used internally by the Linux kernel for memory management. In modern Linux systems, this field is almost always 0 and is largely obsolete.

#### Use
Use is the number of times this route has been looked up by the routing algorithm.
It increments each time a packet needs routing and matches this route entry.
Shows how frequently a particular route is being selected. Counter only increases, never decreases. It helps identify which routes are actively carrying traffic. Useful for network monitoring and optimization.

#### Iface:
The Iface is the physical or logical network interface card (NIC) through which packets should be sent. A system with multiple NICs needs to know which port to use.
Each interface typically has its own IP address and connects to a different network
The interface must be active for the route to work.

##### Physical interfaces: 
- Correspond to actual network hardware
- Each typically connects to a different network segment

**Examples**: if_e1, if_e2

##### Virtual interfaces: 
- Software-defined interfaces

**Example**: lo (loopback)

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
### What packets are exchanged by DNS and what information is transported?

## Experience 6 - TCP connections

### How many TCP connections are opened by your FTP application?
The FTP protocol uses two separate TCP connections the Control Connection and Data Connection.
This separation provides:
- Independence: Commands can be sent while data is transferring.
- Flexibility: Different transfer modes and parameters.
- Reliability: Control channel remains open even if data transfer fails.

#### Control Connection (Command Channel)
- Port: Server port 21
- Purpose: Send FTP commands and receive server responses
- Lifetime: Established first, remains open during entire FTP session, closed at the end
- Examples: USER, PASS, TYPE, PASV, RETR, QUIT commands

#### Data Connection (Data Channel)

- Port: Negotiated dynamically, in passive mode, server tells client which port to connect to
- Purpose: Transfer the actual file binary data
- Lifetime: Opened after entering passive mode, closed after file transfer completes
- Usage: File downloads, directory listings

### In what connection is transported the FTP control information?
The ftp control information is transported in the **Control connection**. This connection is established from the client to the server using TCP on port 21, and remains open for the duration of the FTP session. The control connection carries commands and responses, such as authentication credentials (username and password), directory navigation, and file operation commands.

### What are the phases of a TCP connection?
#### Connection Establishment (3-Way Handshake)
It establish connection and synchronize sequence number.
Steps:
1. Client → Server: SYN. Client sends TCP segment with SYN flag set and the Initial Sequence Number (ISN)
2. Server → Client: SYN-ACK. Server responds with SYN and ACK flags set. Server's ISN. Acknowledges client's ISN 
3. Client → Server: ACK. Client acknowledges server's ISN. Connection is now ESTABLISHED
#### Data Transfer Phase
Reliable data transmission with flow control and error recovery.
Characteristics:
- Bidirectional data flow
- Sequence numbers track every byte
- Acknowledgment: Receiver confirms received bytes
- Segmentation: Large data divided into TCP segments 

#### Connection Termination (4-Way Handshake)
Gracefully close connection, ensuring all data is received.
Steps:
1. Client → Server: FIN. Client finished sending data (FIN flag set).
2. Server → Client: ACK. Server acknowledges client's FIN.
3. Server → Client: FIN. Server finished sending data (FIN flag set).
4. Client → Server: ACK. Client acknowledges server's FIN.
Connection fully closed.

### How does the ARQ TCP mechanism work? What are the relevant TCP fields? What relevant information can be observed in the logs?
### How does the TCP congestion control mechanism work? What are the relevant fields. How did the throughput of the data connection evolve along the time? Is it according to the TCP congestion control mechanism?
### Is the throughput of a TCP data connections disturbed by the appearance of a second TCP connection? How?

# Conclusion


!PROF!
- inicio os comandos!!



