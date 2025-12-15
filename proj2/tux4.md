


```bash
    ifconfig if_e1 up
    ifconfig if_e1 172.16.75.2/24

    ifconfig if_e2 up
    ifconfig if_e2 172.16.76.3/24

    sudo sysctl net.ipv4.ip_forward=1 # Enabling Ip forwarding
    sudo sysctl net.ipv4.icmp_echo_ignore_broadcasts=0 # Disabling ICMP echo ignore broadcast

    sudo route add -net 172.16.1.0/24 gw 172.16.76.254 

```
