#!/usr/bin/env python3

# Helper script to start QEMU with Saturn and VMs
# Copyright (C) 2023 Alexander Smirnov <alex.bluesman.smirnov@gmail.com>

import argparse
import os
import subprocess

def start_qemu(topfolder, os, extraparams):
    # QEMU command
    cmdline = (['qemu-system-aarch64'])
    cmdline.extend(['-machine', 'virt,gic_version=3'])
    cmdline.extend(['-machine', 'virtualization=true'])
    cmdline.extend(['-machine', 'type=virt'])
    cmdline.extend(['-cpu', 'cortex-a57'])                  # Use ARMv8 64-bit
    cmdline.extend(['-smp', '4'])                           # Use SMP with 4 cores
    cmdline.extend(['-m', '1024M'])                         # Use 1GB of RAM
    cmdline.extend(['-nographic'])                          # Only console output

    # Saturn kernel
    cmdline.extend(['-device', 'loader,file=' + topfolder + '/source/saturn,addr=0x7fc00000'])
    cmdline.extend(['-device', 'loader,addr=0x7fc00000,cpu-num=0'])

    if os == 'asteroid':
        # Asteroid kernel
        cmdline.extend(['-device', 'loader,file=' + topfolder + '/tools/asteroid/asteroid,addr=0x41000000'])
    elif os == 'linux':
        cmdline.extend(['-device', 'loader,file=' + topfolder + '/images/Image,addr=0x7e000000'])
        cmdline.extend(['-device', 'loader,file=' + topfolder + '/images/qemuarm64.dtb,addr=0x7f500000'])
        cmdline.extend(['-device', 'loader,file=' + topfolder + '/images/core-image-minimal-qemuarm64.cpio.gz,addr=0x7f510000'])
        # /dev/vda with root filesystem
        #cmdline.extend(['-drive', 'id=disk0,file=images/core-image-minimal-qemuarm64.ext4,if=none,format=raw'])
        #cmdline.extend(['-device', 'virtio-scsi,drive=disk0'])

    cmdline.extend([extraparams])

    print(cmdline)
    p1 = subprocess.call('exec ' + ' '.join(cmdline), shell=True)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-b', '--build', help='set the path to build directory', default=os.getcwd())
    parser.add_argument('-e', '--extra', help='pass extra parameters to QEMU', default='')
    parser.add_argument('-g', '--guest', choices=['asteroid', 'linux'], help='set guest OS type', default='asteroid')
    args = parser.parse_args()

    start_qemu(args.build, args.guest, args.extra)
