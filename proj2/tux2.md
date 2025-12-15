## Tux2 config



```bash
    ifconfig if_e1 up
    ifconfig if_e1 172.16.76.1/24

    sudo route add -net 172.16.75.0/24 gw 172.16.76.3 
    sudo route add -net 172.16.1.0/24 gw 172.16.76.254 


```

## Router Config

```bash
    /ip address add address=172.16.1.71/24 interface=ether1
    /ip address add address=172.16.76.254/24 interface=ether2

    /ip route add dst-address=172.16.75.0/24 gateway=172.16.76.3

```



## Switch config
```bash
    /interface bridge add name=bridge70
    /interface bridge add name=bridge71

    /interface bridge port remove [find interface =ether14]
    /interface bridge port remove [find interface =ether22]
    /interface bridge port remove [find interface =ether24]

    /interface bridge port add bridge=bridge71 interface=ether14
    /interface bridge port add bridge=bridge70 interface=ether22
    /interface bridge port add bridge=bridge70 interface=ether24

    /interface bridge port remove [find interface =ether23]
    /interface bridge port add bridge=bridge71 interface=ether23

    /interface bridge port remove [find interface =ether16]
    /interface bridge port add bridge=bridge71 interface=ether16

    /interface bridge port print brief


```
