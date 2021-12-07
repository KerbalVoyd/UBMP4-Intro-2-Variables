/*==============================================================================
	Project: LCD
	Version: 0.1				Date: January 24, 2017
	Target: CHRP3				Processor: PIC18F25K50

	This program demonstrates how to communicate with a character-mode LCD
	display over an 8-bit interface. Bi-directional communication is used.
==============================================================================*/

#include    "xc.h"              // XC compiler general include file

#include    "stdint.h"          // Integer definition
#include    "stdbool.h"         // Boolean (true/false) definition

#include    "UBMP4.h"            // User-created variables and functions

#define	LCDLINE1	0x80		// Line 1 starting address
#define LCDLINE2	0xC0		// Line 2 starting address
#define LCDCLR		0x01		// Clear Display, send cursor home
#define LCDHOME		0x02		// Cursor home, DDRAM unchanged
#define LCDDEC		0x04		// Cursor decrement mode
#define LCDINC		0x06		// Cursor increment mode
#define LCDOFF		0x08		// Display off
#define LCDON		0x0C		// Display on, cursor off
#define CURSON		0x0E		// Display on, cursor on
#define CURSBLINK	0x0F		// Display on, cursor on and blinking
#define CURSLEFT	0x10		// Move cursor left
#define CURSRIGHT	0x14		// Move cursor right
#define LCDLEFT		0x18		// Shift display left
#define LCDRIGHT	0x1C		// Shift display right
#define LCDFUNC4	0x28		// 2 line, 4-bit LCD initialization
#define LCDFUNC8	0x38		// 2-line, 8-bit LCD initialization
#define LCDCGRAM	0x40		// CGRAM starting address

void lcdInit(unsigned char);				// Initialize LCD port pins and LCD controller

void lcdBusy(void);				// Wait until LCD is not busy

void lcdCommand4(unsigned char);	// Send LCD control command

void lcdWrite4(unsigned char);	// Display LCD data

int main(void)
{
    // Initialize the oscillator
	OSC_config();

	// Initialize I/O and peripherals
    UBMP4_config();

	__delay_ms(45);				// LCD power-up delay
	
        // Configure LCD display for 4-bit mode
    lcdInit(LCDFUNC8); // Function set (LCD is in 8-bit mode)
    __delay_ms(4.2);
    lcdInit(LCDFUNC8); // Function set to 4 bit mode (still in 8-bit mode)
    __delay_us(102);
    lcdInit(LCDFUNC8);
    lcdInit(LCDFUNC4); // Complete function set 4-bit, 2 lines
    lcdCommand4(LCDFUNC4);
    lcdCommand4(LCDOFF);
    lcdCommand4(LCDCLR); // Clear display, cursor home
    lcdCommand4(LCDINC); // Entry mode set to increment
    lcdCommand4(CURSBLINK); // LCD on, cursor on and Blink
    
	while(1)
	{
        lcdWrite4('H');
        lcdWrite4('e');
        lcdWrite4('l');
        lcdWrite4('l');
        lcdWrite4('o');

		lcdCommand4(LCDLINE2);	// Move cursor to line 2
		lcdWrite4('W');
		lcdWrite4('o');
		lcdWrite4('r');
		lcdWrite4('l');
		lcdWrite4('d');
		lcdWrite4('!');
		SLEEP();

	}
}

void lcdInit(unsigned char data)	// Initialize LCD controller (8-bit mode)
{
	LCDRS = 0;
	LCDRW = 0;
	LATC = data;
	LCDE = 1;
	__delay_us(6);
	LCDE = 0;					// Follow this code with the appropriate delay
}

void lcdBusy(void)				// Wait while LCD controller is busy
{
	TRISC = 0b11111111;			// Set all PORTB pins as inputs
	TRISC = 0b11110000;			// Set PORTB pins as inputs (4-bit mode)
	LCDRS = 0;					// 0 = register mode
	LCDRW = 1;					// 1 = read mode
	__delay_us(6);
	LCDE = 1;
	__delay_us(6);
	while(LCDBUSY == 1);		// Check Busy flag
	LCDE = 0;
	__delay_us(6);
	TRISC = 0b00000000;			// Reset PORTB pins to outputs
}

void lcdCommand4(unsigned char data)	// Send LCD control command (4-bit mode)
{
	lcdBusy();					// Wait until LCD is not busy
	LATC = LATC & 0b00001111;	// Clear LCD data bits
	LATC = LATC | (data & 0b11110000);	// OR in upper data nybble
	LCDRS = 0;					// 0 = register mode
	LCDRW = 0;					// 0 = write mode
	LCDE = 1;					// Pulse Enable line for > 300 us
	__delay_us(6);
	LCDE = 0;
	__delay_us(6);
	data = data << 4;			// Shift data left 4 bits
	LATC = LATC & 0b00001111;	// Clear LCD data bits
	LATC = LATC | (data & 0b11110000);	// OR in lower data nybble
	LCDE = 1;					// Pulse Enable line for > 300 us
	__delay_us(6);
	LCDE = 0;
}

void lcdWrite4(unsigned char data)	// Display LCD data (4-bit mode)
{
	lcdBusy();					// Wait until LCD is not busy
	LATC = LATC & 0b00001111;	// Clear LCD data bits
	LATC = LATC | (data & 0b11110000);	// OR in upper data nybble
	LCDRS = 1;					// 0 = register mode
	LCDRW = 0;	  				// 0 = write mode
	LCDE = 1;					// Pulse Enable line for > 300 us
	__delay_us(6);
	LCDE = 0;
	__delay_us(6);
	data = data << 4;			// Shift data left 4 bits
	LATC =LATC & 0b00001111;	// Clear LCD data bits
	LATC = LATC | (data & 0b11110000);	// OR in lower data nybble
	LCDE = 1;					// Pulse Enable line for > 300 us
	__delay_us(6);
	LCDE = 0;
}