# networkLog

This is a Linux kernel module for logging IPv4 addesses incoming to your device.

## Make It

Build it.

```
make
sudo insmod network.ko 
```

Read all IPv4 addresses and the number of connections.

```
sudo su
cat /proc/network
```

Remove and clean.

```
sudo rmmod network.ko
make clean
```