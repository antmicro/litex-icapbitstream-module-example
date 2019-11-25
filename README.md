This repository contains an example of a kernel module that dynamically recongifures the FGPA chip using the ICAPBitstream LiteX driver.

# Testing ICAPBitstream FPGA Manager

0. Clone https://github.com/litex-hub/linux-on-litex-vexriscv repository.

1. Compile a Linux Kernel using Buildroot

```
cd <buildroot_dir>
make BR2_EXTERNAL=<linux-on-litex-vexriscv_dir>/buildroot/ litex_vexriscv_defconfig
make linux-menuconfig
```

Select:
```
[*] Enable loadable module support
    |-> [*] Module unloading
```
Exit and save configuration

```
make
```

2. Compile the ICAP test module

```
export ARCH=riscv
export CROSS_COMPILE=riscv64-unknown-linux-gnu-
export KERNEL=<buildroot_dir>/output/build/linux-5.0.13
make
```

3. Copy the test module to a buildroot rootfs overlay
```
mkdir <linux-on-litex-vexriscv_dir>/buildroot/board/litex_vexriscv/rootfs_overlay/root
cp icapbit_test.ko <linux-on-litex-vexriscv_dir>/buildroot/board/buildroot/board/litex_vexriscv/rootfs_overlay/root/
```

4. Compile the buildroot once again
```
cd <buildroot_dir>
make
```

5. Upload the **Image** and **rootfs.cpio** to a board

6. Load the test module and observe results (on the board):

    1. To test the ICAP without rebooting:

    ```
    cd /root
    rmmod icapbit_test                 # Only if the test module has been already loaded
    insmod icapbit_test.ko reboot=0
    ```

    Now you should see:
    ```
    [  459.575092] icapbit_test: loading out-of-tree module taints kernel.
    [  459.584557] ### ICAPBitstream TEST MODULE loaded ###!
    [  459.585454] FPGA chip will not reboot!
    ```

    2. To test the ICAP by rebooting from flash memory

    ```
    cd /root
    rmmod icapbit_test                 # Only if the test module has been already loaded
    insmod icapbit_test.ko reboot=1
    ```

    Now you should see:
    ```
    [  622.125369] ### ICAPBitstream TEST MODULE loaded ###!
    [  622.126276] FPGA chip will reboot from flash!
    ```

    Now FPGA chip will reboot from a flash memory!

    3. To test the ICAP by loading partial bitstream from a file.

    Put the partial bitstream to:
    `<linux-on-litex-vexriscv_dir>/buildroot/board/litex_vexriscv/rootfs_overlay/lib/firmware/bitstream.bit`

    ```
    cd /root
    rmmod icapbit_test                 # Only if the test module has been already loaded
    insmod icapbit_test.ko reboot=2
    ```
