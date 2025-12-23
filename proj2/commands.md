# Shell code to be executed in order to setup the lab environment

```bash
    systemctl restart networking
    /system reset-configuration #login:admin password: 
```
 
## Exp1


### Connect the following cables:
 - Connect E1 of tuxY3 to the ether22
 - Connect E1 of tuxY4 to the ether24

### Configure the if_e1 in tux83

```bash
    ifconfig if_e1 up
    ifconfig if_e1 172.16.80.1/24
```

### Configure the if_e1 in tux84

```bash
    ifconfig if_e1 up
    ifconfig if_e1 172.16.80.254/24
```

### Verify if the setup is well done

#### In tux83
```bash
    ping 172.16.80.254
```

#### In tux84
```bash
    ping 172.16.80.1
```

If they both succeed the configuration is well done
## Exp2

### Connect the following cables:
 - Connect E1 of tuxY2 to the ether14

```bash
    ifconfig if_e1 up
    ifconfig if_e1 172.16.81.1/24
```

### Creation of bridges:
```bash
/interface bridge add name=bridgeY0
/interface bridge add name=bridgeY1
/interface bridge print
```

### Removing the ports from the original bridges:
```bash
/interface bridge port remove [find interface =ether14]
/interface bridge port remove [find interface =ether22]
/interface bridge port remove [find interface =ether24]
/interface bridge port print brief
```
### Add the ports to their bridges:
```bash
/interface bridge port add bridge=bridgeY1 interface=ether14
/interface bridge port add bridge=bridgeY0 interface=ether22
/interface bridge port add bridge=bridgeY0 interface=ether24
/interface bridge port print brief
```





## Exp 3 

### Transform TuxY4 into a router

#### Connect the following cables:
 - Connect E2 of tuxY4 to the ether23

#### Configure interface if_e2 on TuxY4 and add it to bridgeY1 on E2  

```bash
    ifconfig if_e2 up
    ifconfig if_e2 172.16.81.253/24
```

#### Add the ports to their bridges:
```bash
/interface bridge port add bridge=bridgeY1 interface=ether23
/interface bridge port print brief
```

#### Enable IP forwarding and disable ICMP echo-ignore-broadcast
```bash
sudo sysctl net.ipv4.ip_forward=1 # Enabling Ip forwarding
sudo sysctl net.ipv4.icmp_echo_ignore_broadcasts=0 # Disabling ICMP echo ignore broadcast
```

### Observer MAC addresses and IP addresses in tuxY4.if_e1 and tuxY4.if_e2


### Reconfigure tuxY3 and tuxY2 so that each of them can reach the other
#### TUX2
```bash
sudo route add -net 172.16.Y0.0/24 gw 172.16.Y1.253 
```
#### TUX3
```bash
sudo route add -net 172.16.Y1.0/24 gw 172.16.Y0.254 
```
### Observe the routes available at the 3 tuxes
```bash
route -n
```





## Exp4
### Connect ether1 of RC to the lab network on PY.12


### Remove the ether16 and add it to bridgeY1:
```bash
/interface bridge port remove [find interface =ether16]
/interface bridge port add bridge=bridgeY1 interface=ether16
/interface bridge port print brief
```

### Configure the IP addresses of RC through the router serial console
```bash
/ip address add address=172.16.1.Y1/24 interface=ether1
/ip address add address=172.16.Y1.254/24 interface=ether2
/ip address print
```

### Configure the IP addresses of RC through the router serial console:
```bash
/ip address add address=172.16.1.Y9/24 interface=ether1
/ip address print
```

### Verify routes and add if necessary:
#### TUX2 – routes for 172.16.Y0.0/24 and 172.16.1.0/24
```bash
route -n
sudo route add -net 172.16.1.0/24 gw 172.16.Y1.254 
```
#### TUX3 – routes for 172.16.Y1.0/24 and 172.16.1.0/24
```bash
route -n
sudo route add -net 172.16.1.0/24 gw 172.16.Y0.254 
```
#### TUX4 – routes for 172.16.1.0/24
```bash
route -n
sudo route add -net 172.16.1.0/24 gw 172.16.Y1.254 
```

#### RC – routes for 172.16.Y0.0/24
```bash
/ip route add dst-address=172.16.80.0/24 gateway=172.16.81.253
/ip route print
```

### TUX2
```bash
sysctl net.ipv4.conf.if_e1.accept_redirects=0
sysctl net.ipv4.conf.all.accept_redirects=0
```

### In tuxY2, change the routes to use RC as the gateway to subnet
```bash
route del -net 172.16.Y0.0/24 gw 172.16.Y1.253
sudo route add -net 172.16.Y0.0/24 gw 172.16.Y1.254 
```











# Utility commands
### Creating/Removing bridges from Mikrotik Switch

To open the switch terminal the Ethernet cable of the switch 
must be connected to one of the computers(tuxY2) and then the
GTKterm should be opened and when clicking enter the terminal
will open.


#### Creating a bridge

```bash
/interface bridge add name=bridgeY0
/interface bridge print
```

#### Removing a bridge
```bash
/interface bridge remove bridgeY0
/interface bridge print
```

#### Adding a port to a bridge

```bash
/interface bridge port add bridge=bridgeY0 interface=ether1
/interface bridge port print
```

#### Removing a port from a bridge

```bash
/interface bridge port remove [find interface =ether1]
/interface bridge port print
```

### Checking bridges and ports

```bash
/interface bridge port print brief
```

### Clean arp tables:

```bash
sudo arp -d ipaddress
```

### Configure the router ip
```bash
/ip address add address=172.16.1.Y9/24 interface=ether1
/ip address print
```





### Demontração
- Se fizermos scripts temos de ter em conta que o IP vai mudar!
- extremamente importante configurar direito o ip do rc!
- os ips podem mudar!!!
- traceroute de 4 para ftp ou 4 rc ftp ou 3 envolvido
- A rede tem que estar bem montada