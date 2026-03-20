#define F_CPU 16000000UL  // Define CPU frequency as 16 MHz for delay calculations
#include <avr/io.h>       // AVR input/output definitions
#include <util/delay.h>   // Delay functions (_delay_ms)
#include <avr/interrupt.h> // Interrupt handling
#include <stdbool.h>       // Boolean data type support (true/false)

// Define the pins connected to the buttons
#define BUTTON_S1 PD2  // Button S1 connected to PD2 (External Interrupt INT0)
#define BUTTON_S2 PD3  // Button S2 connected to PD3 (External Interrupt INT1)
#define BUTTON_S3 PD4  // Button S3 connected to PD4 (Pin Change Interrupt PCINT20)

// Define LED patterns for PORTD (PD5, PD6, PD7) over 11 steps
const uint8_t pd_leds[11] = {
    (1 << PD5) | (1 << PD6) | (1 << PD7), // Step 0: All three LEDs on
    (1 << PD6) | (1 << PD7),               // Step 1: PD6, PD7 on
    (1 << PD7),                             // Step 2: PD7 only
    0,                                      // Step 3: All off
    0,                                      // Step 4: All off
    0,                                      // Step 5: All off
    0,                                      // Step 6: All off
    0,                                      // Step 7: All off
    (1 << PD7),                             // Step 8: PD7 only
    (1 << PD6) | (1 << PD7),               // Step 9: PD6, PD7 on
    (1 << PD5) | (1 << PD6) | (1 << PD7)  // Step 10: All three on
};

// Define LED patterns for PORTB (PB0-PB4) over 11 steps
const uint8_t pb_leds[11] = {
    0,                                                          // Step 0: All off
    (1 << PB0),                                                 // Step 1: PB0 on
    (1 << PB0) | (1 << PB1),                                   // Step 2: PB0, PB1 on
    (1 << PB0) | (1 << PB1) | (1 << PB2),                     // Step 3: PB0-PB2 on
    (1 << PB1) | (1 << PB2) | (1 << PB3),                     // Step 4: PB1-PB3 on
    (1 << PB2) | (1 << PB3) | (1 << PB4),                     // Step 5: PB2-PB4 on
    (1 << PB1) | (1 << PB2) | (1 << PB3),                     // Step 6: PB1-PB3 on
    (1 << PB0) | (1 << PB1) | (1 << PB2),                     // Step 7: PB0-PB2 on
    (1 << PB0) | (1 << PB1),                                   // Step 8: PB0, PB1 on
    (1 << PB0),                                                 // Step 9: PB0 only
    0                                                           // Step 10: All off
};

// Volatile flags to be used in ISR and main loop
volatile bool disable_outer_leds = false;  // Flag to disable PD5 and PB4
volatile bool start_sequence      = false; // Flag to start LED sequence
volatile uint16_t delay_time      = 500;   // Delay time between LED steps (default 500ms)

// Setup function to configure I/O and interrupts
void setup() {
    // Configure LED pins as output
    DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD7);                              // PD5-PD7 as output
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);  // PB0-PB4 as output

    // Configure button pins as input
    DDRD  &= ~((1 << BUTTON_S1) | (1 << BUTTON_S2) | (1 << BUTTON_S3));        // PD2-PD4 as input
    PORTD |=  (1 << BUTTON_S1)  | (1 << BUTTON_S2)  | (1 << BUTTON_S3);        // Enable internal pull-up resistors

    // Configure external interrupts on falling edge
    EICRA |= (1 << ISC01);                    // INT0 (PD2) triggers on falling edge
    EICRA |= (1 << ISC11);                    // INT1 (PD3) triggers on falling edge
    EIMSK |= (1 << INT0) | (1 << INT1);       // Enable INT0 and INT1

    // Configure pin change interrupt for PD4 (PCINT20)
    PCICR  |= (1 << PCIE2);                   // Enable Pin Change Interrupt for PORTD
    PCMSK2 |= (1 << PCINT20);                 // Enable interrupt on PCINT20 (PD4)

    sei(); // Enable global interrupts
}

int main(void) {
    setup(); // Initialize all hardware configurations

    while (1) {
        // Check if the start_sequence flag is set
        if (start_sequence) {
            start_sequence = false; // Reset flag so sequence only plays once

            for (uint8_t cycle = 0; cycle < 3; cycle++) {  // Repeat sequence 3 times
                for (uint8_t i = 0; i < 11; i++) {         // Go through each step of the pattern
                    uint8_t pd_mask = pd_leds[i];           // Get current LED pattern for PORTD
                    uint8_t pb_mask = pb_leds[i];           // Get current LED pattern for PORTB

                    // If S3 was pressed, disable outermost LEDs (PD5, PB4)
                    if (disable_outer_leds) {
                        pd_mask &= ~(1 << PD5);  // Clear PD5 bit
                        pb_mask &= ~(1 << PB4);  // Clear PB4 bit
                    }

                    // Clear all LEDs before lighting the new pattern
                    PORTD &= ~((1 << PD5) | (1 << PD6) | (1 << PD7));                             // Turn off PD LEDs
                    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4)); // Turn off PB LEDs

                    // Output the current LED pattern
                    PORTD |= pd_mask; // Turn on PD LEDs
                    PORTB |= pb_mask; // Turn on PB LEDs

                    // Wait for the specified delay time
                    for (uint16_t d = 0; d < delay_time; d += 10) {
                        _delay_ms(10); // Delay in small chunks for flexibility
                    }
                }
            }

            // Turn off all LEDs after completing the sequence
            PORTD &= ~((1 << PD5) | (1 << PD6) | (1 << PD7));
            PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4));
        }
    }
}

// Interrupt Service Routine for Button S1 (INT0)
ISR(INT0_vect) {
    _delay_ms(50); // Debounce delay
    if (!(PIND & (1 << BUTTON_S1))) {  // Check if button is still pressed (active low)
        delay_time     = 500;          // Set slow delay
        start_sequence = true;         // Start the LED sequence
    }
}

// Interrupt Service Routine for Button S2 (INT1)
ISR(INT1_vect) {
    _delay_ms(50); // Debounce delay
    if (!(PIND & (1 << BUTTON_S2))) {  // Check if button is still pressed
        delay_time     = 200;          // Set fast delay
        start_sequence = true;         // Start the LED sequence
    }
}

// Pin Change Interrupt for Button S3
ISR(PCINT2_vect) {
    if (!(PIND & (1 << BUTTON_S3))) {  // Check if button is pressed
        disable_outer_leds = true;      // Disable outer LEDs on future sequences
    }
}
