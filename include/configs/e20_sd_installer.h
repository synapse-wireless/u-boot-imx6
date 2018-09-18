/*
 * Copyright (C) 2010-2011 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Boundary Devices Nitrogen6X
 * and Freescale i.MX6Q Sabre Lite boards.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"
#define CONFIG_MX6
#define CONFIG_E20_SD_INSTALLER		  1
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_MACH_TYPE	2020

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(10 * 1024 * 1024)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_MISC_INIT_R
#define CONFIG_MXC_GPIO

#define CONFIG_MV_UDC
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_CDC
#define CONFIG_NETCONSOLE

#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

// Temporarily send data over UART4 
#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE	       UART1_BASE


#define CONFIG_CMD_SF
#ifdef CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SST
#define CONFIG_MXC_SPI
#define CONFIG_SF_DEFAULT_BUS  0
#define CONFIG_SF_DEFAULT_CS   (0|(IMX_GPIO_NR(3, 19)<<8))
#define CONFIG_SF_DEFAULT_SPEED 25000000
#define CONFIG_SF_DEFAULT_MODE (SPI_MODE_0)
#endif


/* I2C Configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
//#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_I2C_SPEED		400000

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR      0
#define CONFIG_SYS_FSL_USDHC_NUM       2

#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION


#ifdef CONFIG_MX6Q
#define CONFIG_CMD_SATA
#endif

/*
 * SATA Configs
 */
/*
#ifdef CONFIG_CMD_SATA
#define CONFIG_DWC_AHSATA
#define CONFIG_SYS_SATA_MAX_DEVICE	1
#define CONFIG_DWC_AHSATA_PORT_ID	0
#define CONFIG_DWC_AHSATA_BASE_ADDR	SATA_ARB_BASE_ADDR
#define CONFIG_LBA48
#define CONFIG_LIBATA
#endif
*/


#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_FEC_MXC

#define CONFIG_MII
#define IMX_FEC_BASE			ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RMII
#define CONFIG_ETHPRIME			"FEC"
#define CONFIG_FEC_MXC_PHYADDR		1

//#define CONFIG_RESET_PHY_R

/* PHYLIB may not work... */
//#define CONFIG_PHYLIB
//#define CONFIG_PHY_DP83848
//#define PHY_DP83848
//#define CONFIG_DRIVER_TI_EMAC


/* USB Configs */
#define CONFIG_CMD_USB
#define CONFIG_CMD_FAT
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET	// For OTG port 
#define CONFIG_MXC_USB_PORTSC	(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS	0

//#define CONFIG_MD5
//#define CONFIG_CMD_MD5SUM

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			       115200

/* Command definition */
#include <config_cmd_default.h>

#undef CONFIG_CMD_IMLS

#define CONFIG_BOOTDELAY	       5

#define CONFIG_LOADADDR                0x12000000
#define CONFIG_SYS_TEXT_BASE           0x17800000

#ifdef CONFIG_CMD_MMC
#define CONFIG_DRIVE_MMC "mmc "
#else
#define CONFIG_DRIVE_MMC
#endif

#define CONFIG_DRIVE_TYPES CONFIG_DRIVE_MMC

#define CONFIG_EXTRA_ENV_SETTINGS \
	"uimage=/boot/uImage\0" \
	"console=ttymxc0\0" \
	"fdt_file=/boot/imx6s-e20.dtb\0" \
	"fdt_addr=0x18000000\0" \
	"macaddr=0x00,0x1c,0x2c,0xff,0xff,0xff\0" \
	"mmcdev=1\0" \
	"mmcpart=1\0" \
	"sdcarddev=0\0" \
	"sdcardpart=1\0" \
	"mmcroot=LABEL=INTERNAL\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} --no-log fec.macaddr=${macaddr} " \
		"root=${mmcroot} rootdelay=2 rw \0" \
	"loadaddr=0x12000000\0" \
	"loaduimage=ext4load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${uimage}\0" \
	"loadfdt=ext4load mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	"bootinstaller=usb start; setenv mmcdev 0; setenv uimage installer-uImage; setenv ftd_file imx6s-e20.dtb; " \
		"run loaduimage; run loadfdt; run mmcargs; bootm ${loadaddr} - ${fdt_addr}\0"

	//"bootmmc=usb start; " 
	//	"run loaduimage; run loadfdt; run mmcargs; bootm ${loadaddr} - ${fdt_addr} \0" 
        //"version=1.0.0" 

	//"mmcroot=/dev/mmcblk0p1 rootwait rw\0" 

#define CONFIG_BOOTCOMMAND \
        "fatload mmc 0:1 0x12000000 installer-uImage; fatload mmc 0:1 0x19000000 imx6s-e20.dtb; setenv bootargs console=ttymxc0,115200 --no-log; bootm 0x12000000 - 0x19000000;\0"

//"usb start; "
        //"setenv mmcdev 0; setenv uimage installer-uImage; setenv ftd_file imx6s-e20.dtb; "
        //"if fatload usb 0:1 0x12000000 u-boot.imx; then " 
//        "    mmc write 0x12000000 2 0x260; " 
 //       "else " 
  //      "    echo ERROR:  USB failure; " 
  //      "fi; "
        //"run loaduimage; run loadfdt; run mmcargs; bootm ${loadaddr} - ${fdt_addr}; "


//#define CONFIG_BOOTCOMMAND 
//	"run bootinstaller" 


/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT	       "U-Boot-E20> "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE	       1024

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	       16
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START       0x10000000
#define CONFIG_SYS_MEMTEST_END	       0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH     0x10800000

#define CONFIG_SYS_LOAD_ADDR	       CONFIG_LOADADDR

#define CONFIG_CMDLINE_EDITING

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS	       1
#define PHYS_SDRAM		       MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE	       PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR       IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE       IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_ENV_SIZE			(8 * 1024)

#define CONFIG_ENV_IS_IN_MMC

#define CONFIG_ENV_OFFSET		(6 * 64 * 1024)
#define CONFIG_SYS_MMC_ENV_DEV		0

#define CONFIG_ENV_OFFSET_REDUND        CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE
#define CONFIG_ENV_SIZE_REDUND          CONFIG_ENV_SIZE

#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_BOOTZ

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

/*#define CONFIG_CMD_BMP*/

#define CONFIG_CMD_TIME
#define CONFIG_SYS_ALT_MEMTEST

#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_CMD_FS_GENERIC

#endif	       /* __CONFIG_H */

