#!/bin/bash
KERNELDIR=$1
KERNELNAME=$2

if grub-file --is-x86-multiboot $KERNELDIR/$KERNELNAME.kernel; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi
