#include <Arduino.h>

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

// Pin Mapping
// D10 -> PB2  (System LED)
// D9  -> PB1  (PIR Sensor Input)
// D8  -> PB0  (Buzzer)
// D7  -> PD7  (LED)

const int buzzerPin = 8;   // D8 corresponds to PB0

int main(void)
{
    // Initialize Arduino core (required for tone() function)
    init();

    // Configure Outputs
    DDRB |= (1 << PB2);    // PB2 output (D10)
    DDRB |= (1 << PB0);    // PB0 output (D8)
    DDRD |= (1 << PD7);    // PD7 output (D7)

    // Configure Input
    DDRB &= ~(1 << PB1);   // PB1 input (D9)

    while (1)
    {
        // System LED ON
        PORTB |= (1 << PB2);

        _delay_ms(1000);

        // Read PIR sensor
        if (PINB & (1 << PB1))
        {
            // Turn ON LED
            PORTD |= (1 << PD7);

            // Use tone() for the buzzer on D8 (PB0)
            // 1000 Hz frequency
            tone(buzzerPin, 1000);

            _delay_ms(1000);
        }
        else
        {
            // Turn OFF LED
            PORTD &= ~(1 << PD7);

            // Stop the tone
            noTone(buzzerPin);
        }

        // System LED OFF
        PORTB &= ~(1 << PB2);

        _delay_ms(1000);
    }

    return 0;
}