/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/sys_proto.h>
#include <malloc.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/sata.h>
#include <asm/imx-common/boot_mode.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <linux/fb.h>
#include <ipu_pixfmt.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mxc_hdmi.h>
#include <i2c.h>

#include "dp83848.h"



/////////////////////////////////////////////////////////////////
//
//  PAD CONTROL DEFINES
//
/////////////////////////////////////////////////////////////////

DECLARE_GLOBAL_DATA_PTR;
#define UART_PAD_CTRL  \
	(PAD_CTL_HYS 	  | PAD_CTL_PUS_100K_UP |	\
	PAD_CTL_PUE 	  | PAD_CTL_PKE         |	\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm   |	\
	PAD_CTL_SRE_SLOW  			)


#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define LED_PAD_CTRL	\
	(PAD_CTL_HYS	  | PAD_CTL_PUS_100K_UP |	\
	PAD_CTL_PUE	  | PAD_CTL_PKE		|	\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm   |	\
	PAD_CTL_SRE_SLOW			)

#define ENET_PAD_CTRL  \
	(PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |	\
	 PAD_CTL_DSE_40ohm   | PAD_CTL_HYS	 ) 	

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)

#define BUTTON_PAD_CTRL (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define I2C_PAD_CTRL	(PAD_CTL_HYS | PAD_CTL_PUS_100K_UP |	\
	PAD_CTL_PUE | PAD_CTL_PKE | PAD_CTL_SPEED_MED |         \
	PAD_CTL_DSE_40ohm | PAD_CTL_HYS | 			\
	PAD_CTL_ODE | PAD_CTL_SRE_FAST)

#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

#define WEAK_PULLDOWN	(PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

#define OUTPUT_40OHM (PAD_CTL_SPEED_MED|PAD_CTL_DSE_40ohm)

#define GPIO_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_PUS_100K_UP | \
	PAD_CTL_PUE | PAD_CTL_PKE | PAD_CTL_SPEED_MED | \
	PAD_CTL_DSE_40ohm | PAD_CTL_SRE_SLOW)

#define LED_PULLDOWN ()






int dram_init(void)
{
	printf("Entered dram_init...\n");

	gd->ram_size = ((ulong)CONFIG_DDR_MB * 1024 * 1024);

	printf("Exiting dram_init...\n");

	return 0;
}



/////////////////////////////////////////////////////////////////
//
//  PADS AND MUXES
//
/////////////////////////////////////////////////////////////////

iomux_v3_cfg_t const empty[] = {};


/**
* UART pads
*/
iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_CSI0_DAT11__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT10__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};

iomux_v3_cfg_t const uart4_pads[] = {
//CSI0_DATA 12,13, 16 and 17 according to iomux tool
	MX6_PAD_CSI0_DAT12__UART4_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT13__UART4_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT16__UART4_RTS_B | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT17__UART4_CTS_B | MUX_PAD_CTRL(UART_PAD_CTRL),
};


iomux_v3_cfg_t const uart5_pads[] = {
//CSI0_DATA 19,18,15,14 according to iomux tool
	MX6_PAD_CSI0_DAT19__UART5_CTS_B | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT18__UART5_RTS_B | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT15__UART5_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_CSI0_DAT14__UART5_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};


/**
* I2C pads
*/
#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)
// I2C - pad three - RTC
struct i2c_pads_info i2c3_pad_info = {
	.scl = {
		.i2c_mode = MX6_PAD_EIM_D17__I2C3_SCL | PC,
		.gpio_mode = MX6_PAD_EIM_D17__GPIO3_IO17 | PC,
		.gp = IMX_GPIO_NR(3, 17)
	},
	.sda = {
		.i2c_mode = MX6_PAD_EIM_D18__I2C3_SDA | PC,
		.gpio_mode = MX6_PAD_EIM_D18__GPIO3_IO18 | PC,
		.gp = IMX_GPIO_NR(3, 18)
	}
};


static iomux_v3_cfg_t const usdhc2_pads[] = {
        MX6_PAD_SD2_CMD__SD2_CMD    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
        MX6_PAD_SD2_CLK__SD2_CLK    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
        MX6_PAD_SD2_DAT0__SD2_DATA0    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
        MX6_PAD_SD2_DAT1__SD2_DATA1    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
        MX6_PAD_SD2_DAT2__SD2_DATA2    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
        MX6_PAD_SD2_DAT3__SD2_DATA3    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};



/*
  The nand on this board is eMMC, hooked up like a sdhc slot..  yay!
*/
static iomux_v3_cfg_t const usdhc3_pads[] = {
	MX6_PAD_SD3_CMD__SD3_CMD    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_CLK__SD3_CLK    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT0__SD3_DATA0    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT1__SD3_DATA1    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT2__SD3_DATA2    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT3__SD3_DATA3    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT4__SD3_DATA4    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT5__SD3_DATA5    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT6__SD3_DATA6    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT7__SD3_DATA7    | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_RST__SD3_RESET     | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

/*
* Ethernet pad - TODO
*/
iomux_v3_cfg_t const enet_pads3[] = {
	MX6_PAD_ENET_TXD0__ENET_TX_DATA0	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_TXD1__ENET_TX_DATA1	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_TX_EN__ENET_TX_EN 		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_RXD0__ENET_RX_DATA0 	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_RXD1__ENET_RX_DATA1 	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_RX_ER__ENET_RX_ER		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_REF_CLK__ENET_TX_CLK	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_MDIO__ENET_MDIO		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_MDC__ENET_MDC		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_CRS_DV__ENET_RX_EN		| MUX_PAD_CTRL(ENET_PAD_CTRL),
};


iomux_v3_cfg_t const enet_pads4[] = {
	MX6_PAD_ENET_TXD0__ENET_TX_DATA0	| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_TXD1__ENET_TX_DATA1	| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_TX_EN__ENET_TX_EN		| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_RXD0__ENET_RX_DATA0	| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_RXD1__ENET_RX_DATA1	| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_RX_ER__ENET_RX_ER		| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_MDIO__ENET_MDIO		| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_MDC__ENET_MDC		| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_ENET_CRS_DV__ENET_RX_EN		| MUX_PAD_CTRL(0x0e079),
	MX6_PAD_GPIO_16__ENET_REF_CLK      	| MUX_PAD_CTRL(0x4001b0a8),
};


static iomux_v3_cfg_t const misc_pads[] = {
	MX6_PAD_GPIO_2__GPIO1_IO02	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// CELL_WAKE
	MX6_PAD_GPIO_3__GPIO1_IO03	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// CELL_WDISABLE
	MX6_PAD_GPIO_4__GPIO1_IO04	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// CELL_USB_RESET
	MX6_PAD_GPIO_5__GPIO1_IO05	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// CELL_LED_WWAN
	MX6_PAD_GPIO_6__GPIO1_IO06	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// WIFI_RESET
	MX6_PAD_NANDF_D0__GPIO2_IO00	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// IMX6_IRQ
	MX6_PAD_NANDF_D1__GPIO2_IO01	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// SNAP_WAKE
	MX6_PAD_NANDF_D2__GPIO2_IO02	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// SNAP_RESET_N
	MX6_PAD_NANDF_D4__GPIO2_IO04	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// RTC_INT
	MX6_PAD_NANDF_D5__GPIO2_IO05	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// BT_ENABLE
	MX6_PAD_NANDF_D6__GPIO2_IO06	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// CELL_ENABLE
	MX6_PAD_NANDF_D7__GPIO2_IO07	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// WIFI_ENABLE
	MX6_PAD_RGMII_RXC__GPIO6_IO30   | MUX_PAD_CTRL(GPIO_PAD_CTRL),  // CELL_RESET
	MX6_PAD_DISP0_DAT8__IPU1_DISP0_DATA08 | MUX_PAD_CTRL(GPIO_PAD_CTRL), // TP118
}; 


/*
* Button pads
*/
static iomux_v3_cfg_t const button_pads[] = {
	MX6_PAD_DISP0_DAT0__GPIO4_IO21	| MUX_PAD_CTRL(BUTTON_PAD_CTRL), // Reset / Factory Default
	MX6_PAD_DISP0_DAT1__GPIO4_IO22	| MUX_PAD_CTRL(BUTTON_PAD_CTRL), // User Defined
	MX6_PAD_DISP0_DAT2__GPIO4_IO23	| MUX_PAD_CTRL(BUTTON_PAD_CTRL), // User Defined
};


/*
* LED pads 
*/
static iomux_v3_cfg_t const led_pads[] = {
	MX6_PAD_SD4_DAT0__GPIO2_IO08	| MUX_PAD_CTRL(LED_PAD_CTRL), // Bluetooth Red
	MX6_PAD_SD4_DAT1__GPIO2_IO09	| MUX_PAD_CTRL(LED_PAD_CTRL), // Bluetooth Green
	MX6_PAD_SD4_DAT2__GPIO2_IO10	| MUX_PAD_CTRL(LED_PAD_CTRL), // Wifi Red
	MX6_PAD_SD4_DAT3__GPIO2_IO11	| MUX_PAD_CTRL(LED_PAD_CTRL), // Wifi Green
	MX6_PAD_SD4_DAT4__GPIO2_IO12	| MUX_PAD_CTRL(LED_PAD_CTRL), // Cell Red
	MX6_PAD_SD4_DAT5__GPIO2_IO13	| MUX_PAD_CTRL(LED_PAD_CTRL), // Cell Green
};

/*
* SPI pads
*/
#ifdef CONFIG_MXC_SPI
iomux_v3_cfg_t const ecspi4_pads[] = {
	MX6_PAD_EIM_D20__ECSPI4_SS0	| MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D21__ECSPI4_SCLK	| MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D22__ECSPI4_MISO	| MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D28__ECSPI4_MOSI	| MUX_PAD_CTRL(SPI_PAD_CTRL),
};
#endif



/*
* USB pads - TODO
*/


iomux_v3_cfg_t const usb_pads[] = {
	MX6_PAD_GPIO_1__USB_OTG_ID		| MUX_PAD_CTRL(WEAK_PULLDOWN),
	MX6_PAD_GPIO_7__GPIO1_IO07      | MUX_PAD_CTRL(GPIO_PAD_CTRL),	// USB_B_SUSPEND
	MX6_PAD_GPIO_8__GPIO1_IO08	| MUX_PAD_CTRL(GPIO_PAD_CTRL),	// USB_B_RESET
};




/////////////////////////////////////////////////////////////////
//
//  SETUP IOMUX CALLS
//
/////////////////////////////////////////////////////////////////




static void setup_iomux_enet(void)
{
	printf("Entered setup_iomux_enet...\n");

	gpio_direction_output(IMX_GPIO_NR(2, 14), 0); // dp83848 PHY rst //
	imx_iomux_v3_setup_multiple_pads(enet_pads4, ARRAY_SIZE(enet_pads4));
	// The spec says it can complete in 1us, but..  really?
	udelay(50);
	gpio_set_value(IMX_GPIO_NR(2, 14), 1);

	printf("Exiting setup_iomux_enet...\n");
}




static void setup_iomux_uart(void)
{
	printf("Entered setup_iomux_uart...\n");

	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
	imx_iomux_v3_setup_multiple_pads(uart4_pads, ARRAY_SIZE(uart4_pads));
	imx_iomux_v3_setup_multiple_pads(uart5_pads, ARRAY_SIZE(uart5_pads));

	printf("Exiting setup_iomux_uart...\n");
}



/////////////////////////////////////////////////////////////////
//
//  BOARD SPECIFIC CALLS
//
/////////////////////////////////////////////////////////////////


#ifdef CONFIG_USB_EHCI_MX6
int board_ehci_hcd_init(int port)
{
	printf("Entered board_ehci_hcd_init:  %d...\n", port);


	imx_iomux_v3_setup_multiple_pads(usb_pads, ARRAY_SIZE(usb_pads));

	printf("Exiting board_ehci_hcd_init:  %d...\n", port);

	return 0;
}

int board_ehci_power(int port, int on)
{
	printf("Entered board_ehci_power:  %d...\n", port);

	if (port)
		return 0;

	//gpio_set_value(GP_USB_OTG_PWR, on);
	// we are not powering things on the OTG, 
	// just appearing as device rather than host

	printf("Exiting board_ehci_power:  %d...\n", port);

	return 0;
}

#endif // CONFIG_USB_EHCI_MX6

#ifdef CONFIG_FSL_ESDHC
struct fsl_esdhc_cfg usdhc_cfg[2] = {
	{USDHC2_BASE_ADDR},	
	{USDHC3_BASE_ADDR},
};

// the chip and wifi are non removable, just return 1
int board_mmc_getcd(struct mmc *mmc)
{
	printf("Entered board_mmc_getcd...\n");
	printf("Exiting board_mmc_getcd...\n");

	return 1;
}

int board_mmc_init(bd_t *bis)
{
	printf("Entered board_mmc_init... \n");

	s32 status = 0;
	u32 index = 0;

	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
	printf("initing sd...\n");
	imx_iomux_v3_setup_multiple_pads(usdhc3_pads, ARRAY_SIZE(usdhc3_pads));
	status = fsl_esdhc_initialize(bis, &usdhc_cfg[0]);

	printf("Exiting board_mmc_init... status: %u\n", status);

	return status;
}
#endif



#ifdef CONFIG_MXC_SPI
void setup_spi(void)
{
	printf("Entered e20.c: setup_spi...\n");
	imx_iomux_v3_setup_multiple_pads(ecspi4_pads,
					 ARRAY_SIZE(ecspi4_pads));
	printf("Exiting e20.c: setup_spi done...\n");
}
#endif

void reset_phy(void)
{
	gpio_direction_output(IMX_GPIO_NR(2, 14), 0); // dp83848 PHY rst //
	imx_iomux_v3_setup_multiple_pads(enet_pads4, ARRAY_SIZE(enet_pads4));
	// The spec says it can complete in 1us, but..  really?
	udelay(50);
	gpio_set_value(IMX_GPIO_NR(2, 14), 1);
}

int board_eth_init(bd_t *bis)
{

	printf("Entered e20.c: board_eth_init... \n");
	return 0;

	setup_iomux_enet();
	int ret = cpu_eth_init(bis);

	printf("ret: %d\n", ret);

	uint32_t base = IMX_FEC_BASE;
	struct mii_dev *bus = NULL;
	struct phy_device *phydev = NULL;
	//int ret;

	//setup_iomux_enet();

#ifdef CONFIG_FEC_MXC
	bus = fec_get_miibus(base, -1);
	printf("bus: %x\n", bus);

	if (!bus)
	{
		printf("No bus...\n");
		return 0;
	}
	// scan phy 0-15
	phydev = phy_find_by_mask(bus, (0xff << 0), PHY_INTERFACE_MODE_RMII);
	printf("phydev: %x\n", phydev);

	if (!phydev) {
		printf("No phydev...\n");
		free(bus);
		return 0;
	}
	printf("using phy at %d\n", phydev->addr);
	ret  = fec_probe(bis, -1, base, bus, phydev);
	if (ret) {
		printf("FEC MXC: %s:failed\n", __func__);
		free(phydev);
		free(bus);
	}
#endif


#ifdef CONFIG_MV_UDC
	// For otg ethernet
	usb_eth_initialize(bis);
#endif
	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);
	ret = cpu_eth_init(bis);

	return 0;
}

int board_phy_config(struct phy_device *phydev)
{
	return 0;
}

static void setup_buttons(void)
{
	printf("Entered e20.c: setup_buttons...\n");
	imx_iomux_v3_setup_multiple_pads(button_pads,
					 ARRAY_SIZE(button_pads));
	printf("Exiting e20.c: setup_buttons...\n");
}


int board_early_init_f(void)
{
	setup_iomux_uart();

	printf("In board_early_init_f, already called \n");
	printf("    setup_iomux_uart().\n");

	printf("Exiting board_early_init_f...");

	return 0;
}

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

int board_init(void)
{
	printf("Entered e20.c:  board_init...\n");
	struct iomuxc_base_regs *const iomuxc_regs
		= (struct iomuxc_base_regs *)IOMUXC_BASE_ADDR;

	clrsetbits_le32(&iomuxc_regs->gpr[1],
			IOMUXC_GPR1_OTG_ID_MASK,
			IOMUXC_GPR1_OTG_ID_GPIO1);

	imx_iomux_v3_setup_multiple_pads(misc_pads, ARRAY_SIZE(misc_pads));

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_MXC_SPI
	setup_spi();
#endif

	setup_i2c(2, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c3_pad_info);

#ifdef CONFIG_CMD_SATA
	setup_sata();
#endif

	printf("Exiting e20.c:  board_init...\n");
	return 0;
}

int checkboard(void)
{
	puts("Board: Synapse E20\n");

	return 0;
}



static int do_kbd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int numpressed = 0;
	return numpressed == 0;
}

U_BOOT_CMD(
	kbd, 1, 1, do_kbd,
	"Tests for keypresses, sets 'keybd' environment variable",
	"Returns 0 (true) to shell if key is pressed."
);

#undef CONFIG_PREBOOT
#ifdef CONFIG_PREBOOT
static char const kbd_magic_prefix[] = "key_magic";
static char const kbd_command_prefix[] = "key_cmd";

static void preboot_keys(void)
{
	int numpressed;
	char keypress[ARRAY_SIZE(buttons)+1];
	numpressed = read_keys(keypress);
	if (numpressed) {
		char *kbd_magic_keys = getenv("magic_keys");
		char *suffix;
		/*
		 * loop over all magic keys
		 */
		for (suffix = kbd_magic_keys; *suffix; ++suffix) {
			char *keys;
			char magic[sizeof(kbd_magic_prefix) + 1];
			sprintf(magic, "%s%c", kbd_magic_prefix, *suffix);
			keys = getenv(magic);
			if (keys) {
				if (!strcmp(keys, keypress))
					break;
			}
		}
		if (*suffix) {
			char cmd_name[sizeof(kbd_command_prefix) + 1];
			char *cmd;
			sprintf(cmd_name, "%s%c", kbd_command_prefix, *suffix);
			cmd = getenv(cmd_name);
			if (cmd) {
				setenv("preboot", cmd);
				return;
			}
		}
	}
}
#endif

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	//{ "nand", MAKE_CFGVAL(0x80, 0x02, 0x00, 0x00) },
	{"mmc0",	MAKE_CFGVAL(0x40, 0x30, 0x00, 0x00)},
	{"mmc1",	MAKE_CFGVAL(0x40, 0x38, 0x00, 0x00)},
	{NULL,		0},
};
#endif

int misc_init_r(void)
{
#ifdef CONFIG_PREBOOT
	preboot_keys();
#endif

#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	return 0;
}
