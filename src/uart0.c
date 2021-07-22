#include "uart0.h"
#include "gpio.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart0_init()
{
    register unsigned int r;

	/* Turn off UART0 */
	*UART0_CR = 0x0;

	/* Setup GPIO pins 14 and 15 */
	/* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0	*/
	 r = *GPFSEL1;
	 r &= ~( (7 << 12)|(7 << 15) ); //Clear bits 12-17 (gpio14, gpio15)
	 r |=  ( 4 << 12)|(4 << 15) ; //Set value 4 (select ALT5: UART1)
	 *GPFSEL1 = r;


	/* enable GPIO 14, 15 */
	*GPPUD = 0;            //No pull up/down control
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = (1 << 14)|(1 << 15); //enable clock for GPIO 14, 15
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	*GPPUDCLK0 = 0;        // flush GPIO setup

	/* Mask all interrupts. */
	*UART0_IMSC = 0;

	/* Clear pending interrupts. */
	*UART0_ICR = 0x7FF;


	/* Set integer & fractional part of baud rate. */
	/* Divider = UART_CLOCK/(16 * Baud)            */
	/* Integer part register = integer part of Divider */
	/* Fraction part register = (Fractional part * 64) + 0.5 */
	/* UART_CLOCK = 48MHz (old firmware it was 3MHz); Baud = 115200. */

	*UART0_IBRD = 2;       // 115200 baud
	*UART0_FBRD = 0xB;

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 stop bit */
	*UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;

	/* Enable UART0, receive, and transmit */
	*UART0_CR = 0x301;     // enable Tx, Rx, FIFO
}



/**
 * Send a character
 */
void uart0_sendc(unsigned char c) {

	 do {
	 asm volatile("nop");
	 //uart0_puts("Full\n");
	 } while (*UART0_FR & UART0_FR_TXFF ); // Wait until TX is not full (If full -> Keep looping)
	 /* write the character to the buffer */
	 *UART0_DR = c;
}

/**
 * Receive a character
 */
char uart0_getc() {
    char c = 0;

    /* Check Flags Register */
	/* Wait until Receive FIFO is not empty */
	 do {
		 //uart0_puts("Full\n");
		 asm volatile("nop");
	 } while (*UART0_FR & UART0_FR_RXFE ); //IF EMPTY -> THIS LOOP KEEP LOOPING

    /* read it and return */
	 c = (unsigned char)(*UART0_DR);

    /* convert carriage return to newline */
    return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart0_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n')
            uart0_sendc('\r');
        uart0_sendc(*s++);
    }
}
