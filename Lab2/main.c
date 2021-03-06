#include <msp430.h> 
#include <msp430fr6989.h>

#define redLED BIT0     // Red LED at P1.0
#define greenLED BIT7   // Green LED at P9.7
#define BUT1 BIT1       // Button S1 at P1.1
#define BUT2 BIT2       // Button S2 at P1.2

/**
 * main.c
 */
void main(void) {
    volatile unsigned int i;    // unsigned int type
    volatile unsigned int j;
    WDTCTL = WDTPW | WDTHOLD;   // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Enable the GPIO pins

    // Configure and initialize LEDs
    P1DIR |= redLED;    // Direct pin as output
    P9DIR |= greenLED;  // Direct pin as output
    P1OUT &= ~redLED;   // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure buttons
    P1DIR &= ~BUT1; // Direct pin as input
    P1DIR &= ~BUT2; // Direct pin as input
    P1REN |= BUT1;  // Enable built-in resistor
    P1REN |= BUT2;  // Enable built-in resistor
    P1OUT |= BUT1;  // Set resistor as pull-up
    P1OUT |= BUT2;  // Set resistor as pull-up

    // Polling the button in an infinite loop
    for(;;) {
            // Check if both buttons are pressed
            if ((P1IN & (BUT1 | BUT2)) == 0) {
                for (i = 0; i < 10; i++) {
                    P1OUT ^= redLED;    // Toggle red LED
                    P9OUT ^= greenLED;  // Toggle green LED
                    for (j = 0; j < 60000; j++) {}
                }
            }
            // Check if BUT1 is pressed
            else if ((P1IN & BUT1) == 0) {
                P1OUT |= redLED;    // Turn red LED on
                P9OUT &= ~greenLED; // Turn green LED off
            }
            // Check if BUT2 is pressed
            else if ((P1IN & BUT2) == 0) {
                P9OUT |= greenLED;  // Turn green LED on
                P1OUT &= ~redLED;   // Turn red LED off
            }
            else {
                P1OUT &= ~redLED;   // Turn red LED off
                P9OUT &= ~greenLED; // Turn green LED off
            }
    }
}

