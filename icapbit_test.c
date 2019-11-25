/*
 * Copyright (C) 2019 Antmicro <www.antmicro.com>
 *
 * This is an example of a kernel module that dynamically recongifures
 * the FGPA chip using the ICAPBitstream LiteX driver.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/of.h>
#include <linux/fpga/fpga-mgr.h>
#include <linux/delay.h>

static int reboot;
module_param(reboot, int, 0644);
MODULE_PARM_DESC(reboot, "0 - load bitstream which does nothing\n"
			 "1 - load bitstream which reboots FPGA from flash\n"
			 "other - load bitstream from file /lib/firmware/bitstream.bit");

static char reboot_bit[] = {
	0xAA, 0x99, 0x55, 0x66,  // 0xAA995566
	0x20, 0x00, 0x00, 0x00,  // 0x20000000
	0x30, 0x02, 0x00, 0x01,  // 0x30020001
	0x00, 0x00, 0x00, 0x00,  // 0x00000000
	0x30, 0x00, 0x80, 0x01,  // 0x30008001
	0x00, 0x00, 0x00, 0x0F,  // 0x0000000F
	0x20, 0x00, 0x00, 0x00,  // 0x20000000
};

static char nothing_bit[] = {
	0xAA, 0x99, 0x55, 0x66,  // 0xAA995566
	0x20, 0x00, 0x00, 0x00,  // 0x20000000
	0x30, 0x00, 0x80, 0x01,  // 0x30008001
	0x00, 0x00, 0x00, 0x00,  // 0x00000000
	0x30, 0x00, 0x80, 0x01,  // 0x30008001
	0x00, 0x00, 0x00, 0x0D,  // 0x0000000D
	0x20, 0x00, 0x00, 0x00,  // 0x20000000
};

static int __init icapbit_init(void)
{
	struct fpga_image_info *info;
	struct fpga_manager *mgr;
	struct device_node *mgr_node = NULL;
	int ret = 0;

	printk(KERN_INFO "### ICAPBitstream TEST MODULE loaded ###!\n");

	mgr_node = of_find_node_by_name(NULL, "icap");
	if (mgr_node == NULL) {
		printk(KERN_ALERT "Cannot find the ICAP FPGA Manager node!\n");
	}

	mgr = of_fpga_mgr_get(mgr_node);

	info = fpga_image_info_alloc(&mgr->dev);
	info->flags = FPGA_MGR_PARTIAL_RECONFIG;

	if (reboot == 0) {
		printk(KERN_INFO "FPGA chip will not reboot!\n");
		info->buf = (char *) nothing_bit;
		info->count = sizeof(nothing_bit);
	} else if (reboot == 1) {
		printk(KERN_INFO "FPGA chip will reboot from flash!\n");
		info->buf = (char *) reboot_bit;
		info->count = sizeof(reboot_bit);
	} else {
		printk(KERN_INFO "Bitstream will be loaded from /lib/firmware/bitstream.bit\n");
		info->firmware_name = devm_kstrdup(&mgr->dev, "bitstream.bit", GFP_KERNEL);
	}

	mdelay(2000);

	fpga_mgr_lock(mgr);
	ret = fpga_mgr_load(mgr, info);
	if (ret < 0) {
		printk(KERN_ALERT "Error during bitstream loading: %d\n", ret);
	}
	fpga_mgr_unlock(mgr);

	return 0;
}

static void __exit icapbit_exit(void)
{
	printk(KERN_INFO "### ICAPBitstream TEST MODULE exits! ###\n");
}

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Antmicro <www.antmicro.com>");
MODULE_DESCRIPTION("Simple test module for LiteX ICAPBitstream driver");

module_init(icapbit_init);
module_exit(icapbit_exit);
