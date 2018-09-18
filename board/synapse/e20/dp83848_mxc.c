/*
 * National Semiconductor DP83848 PHY Driver for TI DaVinci
 * (TMS320DM644x) based boards.
 *
 * Copyright (C) 2007 Sergey Kubushyn <ksi@koi8.net>
 *
 * --------------------------------------------------------
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <net.h>
#include <dp83848.h>
#include <miiphy.h>
#include "emac_defs.h"
//#include "../../../drivers/net/davinci_emac.h"
#include "../../../drivers/net/fec_mxc.h"
#include <phy.h>

//#ifdef CONFIG_DRIVER_TI_EMAC

#ifdef CONFIG_CMD_NET


int dp83848_is_phy_connected(int phy_addr)
{
	//u_int16_t	id1, id2;
	int id1, id2;

	// <chris code> - dp83848_mxc.c:32:2: warning: passing argument 3 of ‘bus->read’ makes integer from pointer without a cast [enabled by default]
	//struct mii_dev *bus = fec_get_miibus(IMX_FEC_BASE, -1);
	struct mii_dev *bus = mdio_get_current_dev();
	printf("mdio_get_current_dev = %lu\n", bus);
	//int fec_phy_read(struct mii_dev *bus, int phyAddr, int dev_addr, int regAddr)

	id1 = bus->read(bus, phy_addr, 0, DP83848_PHYID1_REG);
	/*if (id1)
		return 0;*/

	id2 = bus->read(bus, phy_addr, 1, DP83848_PHYID2_REG);
	/*if (id2)
		return 0;*/
/*
	id3 = bus->read(bus, phy_addr, 2, DP83848_PHYID1_REG);
	if (id3)
		return 0;

	id4 = bus->read(bus, phy_addr, 3, DP83848_PHYID2_REG);
	if (id4)
		return 0;
*/
	if ((id1 == DP83848_PHYID1_OUI) && (id2 == DP83848_PHYID2_OUI))
	{
		printf("return code = 1!\n");
		return(1);
	}
	// </chris code>

	/*
	if (!davinci_eth_phy_read(phy_addr, DP83848_PHYID1_REG, &id1))
		return(0);
	if (!davinci_eth_phy_read(phy_addr, DP83848_PHYID2_REG, &id2))
		return(0);

	if ((id1 == DP83848_PHYID1_OUI) && (id2 == DP83848_PHYID2_OUI))
		return(1);
	*/

	printf("return code = 0\n");
	return(0);
}

int dp83848_get_link_speed(int phy_addr)
{
	u_int16_t		tmp;
	//volatile emac_regs*	emac = (emac_regs *)EMAC_BASE_ADDR;

	//struct mii_dev *bus = fec_get_miibus(IMX_FEC_BASE, -1);
	struct mii_dev *bus = mdio_get_current_dev();
	printf("mdio_get_current_dev = %lu\n", bus);

	tmp = bus->read(bus, phy_addr, 0, DP83848_STAT_REG);
	printf("STAT_REG: %x\n", tmp);

	if (!(tmp & DP83848_LINK_STATUS))	/* link up? */
		return(0);

	tmp = bus->read(bus, phy_addr, 0, DP83848_PHY_STAT_REG);
	printf("PHY_STAT_REG: %x\n", tmp);

	if (tmp & DP83848_DUPLEX) {
		bus->phymap[0]->duplex = 1;
	}
	else {
		bus->phymap[0]->duplex = 0;
	}

	if ( tmp & DP83848_SPEED ) {
		bus->phymap[0]->speed = SPEED_100;
	}
	else {
		bus->phymap[0]->speed = SPEED_10;
	}

	printf("speed and duplex: %d, %d \n", bus->phymap[0]->speed, bus->phymap[0]->duplex);

	/* Speed doesn't matter, there is no setting for it in EMAC... */
	/*if (tmp & DP83848_DUPLEX) {
		
		/* set DM644x EMAC for Full Duplex  */
		//emac->MACCONTROL = EMAC_MACCONTROL_MIIEN_ENABLE |
		//	EMAC_MACCONTROL_FULLDUPLEX_ENABLE;
	//} else {
		/*set DM644x EMAC for Half Duplex  */
		//emac->MACCONTROL = EMAC_MACCONTROL_MIIEN_ENABLE;
	//}*/

	return(1);
}


int dp83848_init_phy(int phy_addr)
{
	int	ret = 1;
	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);
	printf("Entered dp83848_init_phy, calling mdio_get_current_dev...\n");

	//struct mii_dev *bus = mdio_get_current_dev();

	//printf("mdio_get_current_dev = %lu\n", bus);

	if (!dp83848_get_link_speed(phy_addr)) {
		/* Try another time */
		printf("get link speed failed, trying again.\n");
		udelay(100000);
		ret = dp83848_get_link_speed(phy_addr);
		printf("tried second time, ret = %d\n", ret);
	}

	/* Disable PHY Interrupts */
	//bus->write(bus, phy_addr, 0, DP83848_PHY_INTR_CTRL_REG, 0);
	//printf("Disabled phys.\n", ret);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);

	int i = 0;
	for (i = 0; i < 16; i++)
	{
		printf("Checking if phyaddr %i is connected: ", i);
		if(dp83848_is_phy_connected(i)) {
			printf(" YES \n"); 
			printf(" attempting to auto negotiate...\n");
			int linkspeed = dp83848_auto_negotiate(i);
			printf(" link_speed: %d\n", linkspeed);
			break; 
		}
		else
			printf(" no \n");
	}

	return(ret);
}


int dp83848_auto_negotiate(int phy_addr)
{
	u_int16_t	tmp;

	printf("called auto_negotiate\n");

	//struct mii_dev *bus = fec_get_miibus(IMX_FEC_BASE, -1);
	struct mii_dev *bus = mdio_get_current_dev();

	//if (!bus->read(bus, phy_addr, &tmp, DP83848_CTL_REG))
	//	return(0);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);

	tmp = bus->read(bus, phy_addr, 0, DP83848_CTL_REG);
	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);
	printf("  Read CTL_REG: %x\n", tmp);

	if(!tmp)
		return 0;

	/* Restart Auto_negotiation  */
	tmp &= ~DP83848_AUTONEG;	/* remove autonegotiation enable */
	tmp |= DP83848_ISOLATE;		/* Electrically isolate PHY */

	bus->write(bus, phy_addr, 0, DP83848_CTL_REG, tmp);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);

	/* Set the Auto_negotiation Advertisement Register
	 * MII advertising for Next page, 100BaseTxFD and HD,
	 * 10BaseTFD and HD, IEEE 802.3
	 */
	tmp = DP83848_NP | DP83848_TX_FDX | DP83848_TX_HDX |
		DP83848_10_FDX | DP83848_10_HDX | DP83848_AN_IEEE_802_3;

	bus->write(bus, phy_addr, 0, DP83848_ANA_REG, tmp);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);

	/* Read Control Register */
	//if (!bus->read(bus, phy_addr, &tmp, DP83848_CTL_REG))
	tmp = bus->read(bus, phy_addr, 0, DP83848_CTL_REG);
	printf("  Read CTL_REG: %x\n", tmp);

	if(!tmp)
		return(0);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);

	tmp |= DP83848_SPEED_SELECT | DP83848_AUTONEG | DP83848_DUPLEX_MODE;

	bus->write(bus, phy_addr, 0, DP83848_CTL_REG, tmp);

	/* Restart Auto_negotiation  */
	tmp |= DP83848_RESTART_AUTONEG;

	bus->write(bus, phy_addr, 0, DP83848_CTL_REG, tmp);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);

	/*check AutoNegotiate complete */
	udelay(10000);

	tmp = bus->read(bus, phy_addr, 0, DP83848_STAT_REG);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);
	printf("  Read STAT_REG: %x\n", tmp);

	if (!(tmp & DP83848_AUTONEG_COMP))
		return(0);

	printf("%s - [%d]:%s \n", __FILE__, __LINE__, __func__);

	return (dp83848_get_link_speed(phy_addr));
}

#endif	/* CONFIG_CMD_NET */

//#endif	/* CONFIG_DRIVER_ETHER */
