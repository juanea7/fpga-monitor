# FPGA Monitor

This repository contains a lightweight Monitor infrastructure to acquire synchronized power consumption and performance traces on FPGAs at run time.

## Repository Structure

- `ip-core/`: Monitor hardware IP, which can be imported into a block design in Vivado.
- `constraints/`: Board constraint for the Monitor IP.
- `linux/`: Linux driver to interact with the hardware of the Monitor on a Linux-based system. For detailed information, refer to its [Readme](linux/drivers/monitor/readme.md).
- `device-tree/`: Device tree overlay to register the Monitor on a Linux-based system.
- `lib/`: Software user-space library to configure and manage the Monitor.
- `setup_monitor/`: Set of files and script to load the Linux driver and device tree overlay of the Monitor on the target platform. For detailed information, refer to its [Readme](setup_monitor/readme.md).
- `visualization/`: Python tool to visualize the traces acquired with the Monitor. For detailed information, refer to its [Readme](visualization/readme.md).
- `artico3_integration/`: Set of files and scripts to integrate the Monitor infrastructure into the [ARTICo3 framework](https://github.com/des-cei/artico3.git). For detailed information, refer to its [Readme](artico3_integration/readme.md).
- `demos/dummy/`: Tutorial showing a Linux application using the Monitor IP. For detailed information, refer to its [Readme](demos/dummy/readme.md).

## Paper
The monitoring infrastructure is described in detail in the accompanying paper. You can read the paper [here](https://doi.org/10.1016/j.micpro.2024.105050).