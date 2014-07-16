/*
 * main.c
 *
 *  Created on: 27.04.2012
 *      Author: fork
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "spi.h"
#include "uart_interrupt.h"
#include "debug.h"

unsigned char debugLevel = DEBUG_DEBUG;
#define F_PWM 100                       // PWM-Frequenz in Hz
#define PWM_STEPS 255                   // PWM-Schritte pro Zyklus(1..255)
#define T_PWM (F_CPU/(F_PWM*PWM_STEPS)) // Systemtakte pro PWM-Takt
#if (T_PWM<(152+5))
#error T_PWM zu klein, F_CPU muss vergrössert werden oder F_PWM oder PWM_STEPS verkleinert werden
#endif

#if PWM_STEPS > 255
#error PWM_STEPS zu gross
#endif

volatile uint8_t pwm_setting[12]; // Einstellungen für die einzelnen PWM-Kanäle

ISR(TIMER1_COMPA_vect) {
	static uint8_t pwm_cnt = 0;
	uint8_t i = 0;
	uint16_t tmp = 0, j = 1;

	OCR1A += (uint16_t) T_PWM;

	for (; i < 12; i++) {
		if (pwm_setting[i] > pwm_cnt) {
			tmp |= j;
		}
		j <<= 1;
	}
	SPI_MasterTransmit(tmp); // PWMs aktualisieren
	if (pwm_cnt == (uint8_t) (PWM_STEPS - 1))
		pwm_cnt = 0;
	else
		pwm_cnt++;
}

int main(void) {

	//cli();
	debug_init();
	flash_twice_green();

	// start UART
	uart_init();

	SPI_MasterInitOutput();
	SPI_MasterTransmit(0x0000);

	TCCR1B = 1; // Timer läuft mit vollem Systemtakt
	TIMSK |= (1 << OCIE1A); // Interrupt freischalten

	// Interrupts global einschalten
	sei();

	uint8_t brightness = 0;
	uint8_t nextChannel = 0;

	log_info("HELLO THERE");

	while (1) {
		/*_delay_ms(1);
		 brightness++;
		 if (brightness == 255) {
		 brightness = 0;
		 nextChannel++;
		 }
		 for (uint8_t i = 0; i < 12; i++) {
		 pwm_setting[i] = brightness;
		 }*/

		int len = checkForNewCommand();
		if (len) {
#ifdef DEBUG
			log_debug3("length: ", len);

			for (int i = 0; i < len; i++) {
				uart_putc(cmd[i]);
			}
#endif

			if (cmd[0] == 0xFF) {
				process_cmd();
			}
		}

	}
	return 0;
}

void process_cmd() {
	pwm_setting[0] = cmd[9]; //  Blue  LEFT
	pwm_setting[1] = cmd[1]; //  Red   LEFT
	pwm_setting[2] = cmd[5]; //  Green LEFT
	pwm_setting[3] = cmd[10]; //  Blue  TOP
	pwm_setting[4] = cmd[2]; //  Red   TOP
	pwm_setting[5] = cmd[6]; //  Green TOP
	pwm_setting[6] = cmd[11]; //  Blue  RIGHT
	pwm_setting[7] = cmd[3]; //  Red   RIGHT
	pwm_setting[8] = cmd[7]; //  Green RIGHT
	pwm_setting[9] = cmd[12]; //  Blue  BOTTOM
	pwm_setting[10] = cmd[4]; // Red   BOTTOM
	pwm_setting[11] = cmd[8]; // Green BOTTOM

#ifdef DEBUG
	log_debug3("Blue  LEFT   ", pwm_setting[0]);
	log_debug3("Red   LEFT   ", pwm_setting[1]);
	log_debug3("Green LEFT   ", pwm_setting[2]);
	log_debug3("Blue  TOP    ", pwm_setting[3]);
	log_debug3("Red   TOP    ", pwm_setting[4]);
	log_debug3("Green TOP    ", pwm_setting[5]);
	log_debug3("Blue  RIGHT  ", pwm_setting[6]);
	log_debug3("Red   RIGHT  ", pwm_setting[7]);
	log_debug3("Green RIGHT  ", pwm_setting[8]);
	log_debug3("Blue  BOTTOM ", pwm_setting[9]);
	log_debug3("Red   BOTTOM ", pwm_setting[10]);
	log_debug3("Green BOTTOM ", pwm_setting[11]);
#endif
}
