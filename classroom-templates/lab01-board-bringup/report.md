# Lab 01 Report

* Student name: Makar Zabolotnyi
* GitHub username: Makar-zabolotnyi
* Image filename: `am335x-debian-13.6-base-v6.12-armhf-2026-07-24-4gb.img.xz`
* SHA-256 (matches the value given in the lab: **yes**):
  `796777685b309328abcab3a122f426282c83fbde7f481ebe3e20829441f878a9`

## Running Linux on BBB

BBB: `uname -r`

```text
6.12.96-bone64
```

BBB: `cat /etc/os-release`

```text
PRETTY_NAME="Debian GNU/Linux 13 (trixie)"
NAME="Debian GNU/Linux"
VERSION_ID="13"
VERSION="13 (trixie)"
VERSION_CODENAME=trixie
DEBIAN_VERSION_FULL=13.6
ID=debian
HOME_URL="https://www.debian.org/"
SUPPORT_URL="https://www.debian.org/support"
BUG_REPORT_URL="https://bugs.debian.org/"
```

BBB: `lsblk`

```text
NAME        MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
mmcblk0     179:0    0 117.8G  0 disk 
├─mmcblk0p1 179:1    0    36M  0 part /boot/firmware
├─mmcblk0p2 179:2    0   512M  0 part [SWAP]
└─mmcblk0p3 179:3    0 117.2G  0 part /
zram0       253:0    0   241M  0 disk [SWAP]
```

The root filesystem `/` is mounted from `mmcblk0p3`, so the board booted from the microSD card.

BBB: `ip addr`

```text
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host noprefixroute 
       valid_lft forever preferred_lft forever
2: eth0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc mq state DOWN group default qlen 1000
    link/ether 78:04:73:41:0b:e1 brd ff:ff:ff:ff:ff:ff
    altname end0
    altname enx780473410be1
3: usb0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 78:04:73:41:0b:e4 brd ff:ff:ff:ff:ff:ff
    inet 192.168.7.2/30 brd 192.168.7.3 scope global usb0
       valid_lft forever preferred_lft forever
    inet6 fe80::7a04:73ff:fe41:be4/64 scope link proto kernel_ll 
       valid_lft forever preferred_lft forever
```

BBB IP address (USB networking): `192.168.7.2`

## First user-space program

HOST, in this lab directory: `file build/hello`

```text
build/hello: ELF 32-bit LSB pie executable, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, BuildID[sha1]=3b2c2a3a17a8f2278ea166c96e6ca582ad1a5198, for GNU/Linux 3.2.0, not stripped
```

BBB: `/home/debian/labs/lab01/hello`

```text
Hello from Lab 01 on BeagleBone Black!
```

## Kernel workspace

HOST: `ls -lh ~/bbb-workspace/kernel/bb-kernel/deploy`

```text
-rw-r--r-- 1 code_zero code_zero 195K Sep 20 16:05 6.12.96-bone64-dtbs.tar.zst
-rw-r--r-- 1 code_zero code_zero  27M Sep 20 16:05 6.12.96-bone64-modules.tar.zst
-rwxr-xr-x 1 code_zero code_zero 8.3M Sep 20 16:05 6.12.96-bone64.zImage
-rw-r--r-- 1 code_zero code_zero 206K Sep 20 16:05 config-6.12.96-bone64
```

Mandatory version check (full kernel releases):

```text
HOST: cat ~/bbb-workspace/kernel/bb-kernel/KERNEL/include/config/kernel.release
6.12.96-bone64

BBB: uname -r
6.12.96-bone64
```

The releases **match**.

Note: the current head of branch `am33x-v6.12` builds `6.12.109-bone72`, which does not
match the image. I therefore checked out the `bb-kernel` commit `5d11e9701`
(tag `6.12.96-bone64`) and rebuilt, so the HOST tree now matches the BBB kernel.

## Why Lab 02 needs a matching kernel tree

A kernel module is compiled against the headers and configuration of one specific
kernel. When the module is loaded, the running kernel compares the module's
`vermagic` (exact release and build options) with its own. A module built from a tree
with a different release, such as `6.12.109-bone72` instead of `6.12.96-bone64`, is
rejected or may break the system, so the HOST tree must match `uname -r` on the BBB exactly.