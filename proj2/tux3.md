
# Setup of TUX_Y3




```bash
    export PATH=$PATH:'/sbin/'
    systemctl restart networking

    ifconfig if_e1 up
    ifconfig if_e1 172.16.75.1/24

    sudo route add -net 172.16.76.0/24 gw 172.16.75.2 
    sudo route add -net 172.16.1.0/24 gw 172.16.75.2 



```