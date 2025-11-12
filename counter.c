#include <xc.h>
#define _XTAL_FREQ 125000                     // Set at 125 KHz

// CONFIG1
#pragma config FOSC  = INTRC_NOCLKOUT         // High speed internal oscillator
#pragma config WDTE  = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP    = OFF
#pragma config CPD   = OFF
#pragma config BOREN = OFF
#pragma config IESO  = ON
#pragma config FCMEN = ON
#pragma config LVP   = OFF

// CONFIG2
#pragma config BOR4V = BOR40V
#pragma config WRT   = OFF

unsigned char general_counter     = 0;        // Counts from 0 to 99
unsigned char counter_units       = 0;
unsigned char counter_tens        = 0;

unsigned char display_digits_units[10] = 
{
    0x3F,                                     // 0
    0x06,                                     // 1
    0x5B,                                     // 2
    0x4F,                                     // 3
    0x66,                                     // 4
    0x6D,                                     // 5
    0x7D,                                     // 6
    0x07,                                     // 7
    0x7F,                                     // 8
    0x6F                                      // 9
};

unsigned char display_digits_tens[10] = 
{
    0x3F,                                     // 0
    0x06,                                     // 1
    0x5B,                                     // 2
    0x4F,                                     // 3
    0x66,                                     // 4
    0x6D,                                     // 5
    0x7D,                                     // 6
    0x07,                                     // 7
    0x7F,                                     // 8
    0x6F                                      // 9
};

void setRegisters()
{
    OSCCON = 0b00010000;                      // Set at 125 KHz
    while (OSCCONbits.HTS == 0);

    TRISBbits.TRISB0 = 1;                     // START button (increment counter)
    ANSELHbits.ANS12 = 0;                     // RB0 as digital input

    TRISBbits.TRISB1 = 1;                     // STOP button (pause counter)
    ANSELHbits.ANS10 = 0;                     // RB1 as digital input

    TRISBbits.TRISB2 = 1;                     // RESET button (reset counter to 0)
    ANSELHbits.ANS8  = 0;                     // RB2 as digital input

    INTCONbits.GIE  = 1;                      // Enable Global Interrupt
    INTCONbits.INTE = 1;                      // Enable External Interrupt

    INTCONbits.T0IE        = 0;               // Disable Timer0 interrupt 
    OPTION_REGbits.INTEDG  = 1;               // Interrupt on rising edge of INT pin  

/*******************************************************
 * Delay time (Tdelay) calculation:
 * 
 * Tdelay = (4 / Fosc) * Prescaler * (256 - TMR0)
 *
 * Tdelay = (4/125,000)*128*(256 - 0)
 * Tdelay = 1.048576 s
 *
 * Where:
 *  Fosc      = Oscillator frequency (Hz)
 *  Prescaler = Assigned prescale value
 *  TMR0      = Initial value of Timer0 register
 *******************************************************/

    OPTION_REGbits.T0CS = 0;                  // TMR0 Clock Source (Internal clock Fosc/4)
    OPTION_REGbits.PSA  = 0;                  // Prescaler assigned to TMR0
    OPTION_REGbits.PS0  = 1;
    OPTION_REGbits.PS1  = 1;
    OPTION_REGbits.PS2  = 1;                  // Prescaler 1:128
    
    TRISC = 0x00;
    TRISD = 0x00;
}

void initRegisters()
{
    INTCONbits.INTF = 0;
    INTCONbits.T0IF = 0;
    PORTC = 0x00;
    PORTD = 0x00;
    TMR0  = 0x00;
}

void __interrupt() ISR(void)
{
    if (INTCONbits.INTF)
    {
        INTCONbits.T0IE = 1;
    }

    if (INTCONbits.TMR0IF)
    {
        counter_units++;
        general_counter++;

        if (general_counter % 10 == 0)
        {
            counter_tens++; 
            if (counter_tens > 9)
            {
                counter_tens = 0;
            }

            if (general_counter == 100)
            {
                general_counter = 0;
            }
        }

        if (counter_units > 9)
        {
            counter_units = 0;
        }
    }

    INTCONbits.T0IF = 0;
    INTCONbits.INTF = 0;
}

void routine_to_reset_counter()
{
    TMR0              = 0x00;                 // Reset Timer0
    INTCONbits.T0IF   = 0;                    // Clear Timer0 interrupt flag
    INTCONbits.T0IE   = 0;                    // Disable Timer0 interrupt
    INTCONbits.INTF   = 0;                    // Clear external interrupt flag (RB0)
    INTCONbits.INTE   = 1;                    // Enable external interrupt again (START)
    
    general_counter   = 0;                    // Reset main counter
    counter_units     = 0;                    // Reset units counter
    counter_tens      = 0;                    // Reset tens counter
}

void routine_to_stop_counter()
{
    INTCONbits.T0IE = 0;                      // Stop Timer0
    INTCONbits.T0IF = 0;
    INTCONbits.INTF = 0;
}

void main(void)
{
    setRegisters();
    initRegisters();
    
    while (1)
    {
        PORTD = display_digits_units[counter_units];
        PORTC = display_digits_tens[counter_tens];
        
        if (PORTBbits.RB1)                    // STOP button pressed
        {
            routine_to_stop_counter();
        }

        if (PORTBbits.RB2)                    // RESET button pressed
        {
            routine_to_reset_counter(); 
        }
    }
}