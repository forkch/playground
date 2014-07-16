/*
 * spi.c
 *
 *  Created on: 03.02.2012
 *      Author: fork
 */

#include "spi.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void SPI_MasterInitOutput(void) {

	//SS_PORT_DDR |= (1 << SS_GO) | (1 << SS_SHORT) | (1 << SS_DBG_LED);

	/* Set MOSI and SCK output, all others input */

	DDR_SPI |= (1 << DD_SS);
	DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK);

	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterInitInput(void) {

	//SS_PORT_DDR |= (1 << SS_SHORT);

	DDR_SPI |= (1 << DD_SS);
	/* Set SCK output */DDR_SPI |= (1 << DD_SCK);

	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPOL);
}

void SPI_MasterTransmit(uint16_t cData) {
	/* Start transmission */
	SPDR = (cData>>8);
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;
	/* Start transmission */
	SPDR = (cData);
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;
	DD_SS_PORT |= (1 << DD_SS);
	DD_SS_PORT &= ~(1 << DD_SS);

}

void SPI_MasterTransmitDebug(unsigned char cData) {
#ifdef DEVEL_BOARD
	cData ^= 0xF0;
#endif
	/* Start transmission */
	SPDR = (cData);
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;
	SS_PORT &= ~(1 << SS_DBG_LED);
	SS_PORT |= (1 << SS_DBG_LED);

}

void SPI_MasterTransmitGO(unsigned char cData) {
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;

	SS_PORT &= ~(1 << SS_GO);
	SS_PORT |= (1 << SS_GO);

}

unsigned char SPI_MasterReceiveShort() {

	SS_PORT &= ~(1 << SS_SHORT);
	SS_PORT |= (1 << SS_SHORT);

	// Send Dummy transmission for reading the data
	SPDR = 0x00;
	// Wait for transmission complete
	while (!(SPSR & (1 << SPIF)))
		;

	/* Wait for reception complete */
	while (!(SPSR & (1 << SPIF)))
		;

	return SPDR;

}
