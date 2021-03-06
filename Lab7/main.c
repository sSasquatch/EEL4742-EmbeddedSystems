#include <msp430.h> 

#define redLED BIT0     // Red at P1.0
#define BUT1 BIT1       // Button S1 at Port 1.1

void config_ACLK_to_32KHz_crystal();

static volatile int status = 0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;       // Enable GPIO pins

    P1DIR |= redLED;
    P1OUT &= ~redLED;

    // Configuring buttons with interrupt
    P1DIR &= ~BUT1;    // 0: input
    P1REN |= BUT1;     // 1: enable built-in resistors
    P1OUT |= BUT1;     // 1: built-in resistor is pulled up to Vcc
    P1IE |= BUT1;      // 1: enable interrupts
    P1IES |= BUT1;     // 1: interrupt on falling edge
    P1IFG &= ~BUT1;    // 0: clear the interrupt flags

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Configure Channel 0
    //TA0CCR1 = 24576 - 1;    // @ 8 kHz --> 3 seconds
    //TA0CCTL1 |= CCIE;
    //TA0CCTL1 &= ~CCIFG;

    // Configure timer
    TA0CTL = TASSEL_1 | ID_2 | MC_2 | TACLR;

    // Enable the global interrupt bit (call an intrinsic function)
    _enable_interrupt();

    _low_power_mode_3();

    return 0;
}

// ISR of Channel 0 (A1 vector)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void T0A1_ISR() {
    //if ((TA0CCTL1 & CCIFG) != 0) {
        P1OUT &= ~redLED;
        //TA0CTL ^= MC_1;
        //TA0R = 0;
        status = 0;
        TA0CCTL1 &= ~CCIE;
    //}
}

//*******************************
#pragma vector = PORT1_VECTOR   // Write the vector name
__interrupt void Port1_ISR() {
    if (status == 0) {
        TA0CCR1 = TA0R + 24576;
        TA0CCTL1 |= CCIE;
        TA0CCTL1 &= ~CCIFG;
        P1OUT |= redLED;
        status = 1;
    }
    else {
        //P1OUT |= redLED;
        //TA0CCTL1 &= ~CCIFG;
        //status = 0;
    }
    //TA0CTL ^= MC_1;
    P1IFG &= ~BUT1;
}

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz

    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do {
        CSCTL5 &= ~LFXTOFFG;    // Local fault flag
        SFRIFG1 &= ~OFIFG;      // Global fault flag
    } while((CSCTL5 & LFXTOFFG) != 0);

    CSCTL0_H = 0;   // Lock CS registers
    return;
}
