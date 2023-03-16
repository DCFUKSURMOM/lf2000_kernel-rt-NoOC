/*
 * (C) Copyright 2012
 * Daniel Lazzari Jr., LeapFrog, Inc., <dlazzari@leapfrog.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MACH_NXP3200_SPI_H_
#define _MACH_NXP3200_SPI_H_

struct spi_io_cs
{
	unsigned long pad;
	unsigned long func;
};

struct nxp3200_spi_plat_data
{
	unsigned	port;
	unsigned	irq;
	unsigned	io_clk;
	unsigned	io_rx;
	unsigned	io_tx;
	struct spi_io_cs*	io_cs;
	unsigned	cs_num;
	NX_SSPSPI_FORMAT	format;
	unsigned	clk_src;
	unsigned	clk_divisor;
	unsigned	clk_divcount;
	int		dma_tx_channel;
	int		dma_rx_channel;
};

#define CFG_SPI_CLK_SRC 1 /* PLL1 */
#define CFG_SPI_CLOCK_DIVISOR 3
#define CFG_SPI_CLOCK_DIVCOUNT 4
#define CFG_DMA_SPI_TX	6
#define CFG_DMA_SPI_RX	7

#endif /* _MACH_NXP3200_SPI_H_ */

