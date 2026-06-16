#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

// -------------------- Pin Definitions --------------------
#define TRIG PD3
#define ECHO PD4

#define GREEN_LED PB0
#define YELLOW_LED PB1
#define RED_LED PB2

#define RESET_BTN PD2

// -------------------- Parking Variables --------------------
const uint8_t initial_capacity = 10;

uint8_t vehicles_entered = 0;
uint8_t available_slots = 10;

// State change detection
uint8_t previous_vehicle_state = 0;

// ---------------------------------------------------------
// Generate trigger pulse
// ---------------------------------------------------------
void ultrasonic_trigger()
{
    PORTD &= ~(1 << TRIG);
    _delay_us(2);

    PORTD |= (1 << TRIG);
    _delay_us(10);

    PORTD &= ~(1 << TRIG);
}

// ---------------------------------------------------------
// Measure Echo pulse width
// ---------------------------------------------------------
uint32_t ultrasonic_read()
{
    uint32_t count = 0;

    ultrasonic_trigger();

    // Wait for echo HIGH
    while (!(PIND & (1 << ECHO)));

    // Measure HIGH duration
    while (PIND & (1 << ECHO))
    {
        count++;
        _delay_us(1);
    }

    return count;
}

// ---------------------------------------------------------
// Calculate distance (cm)
// ---------------------------------------------------------
uint16_t get_distance_cm()
{
    uint32_t pulse_width = ultrasonic_read();

    uint16_t distance = pulse_width / 58;

    return distance;
}

// ---------------------------------------------------------
// LED Status Update
// ---------------------------------------------------------
void update_leds()
{
    PORTB &= ~((1 << GREEN_LED) |
               (1 << YELLOW_LED) |
               (1 << RED_LED));

    if (available_slots > (initial_capacity / 2))
    {
        PORTB |= (1 << GREEN_LED);
    }
    else if (available_slots > 0)
    {
        PORTB |= (1 << YELLOW_LED);
    }
    else
    {
        PORTB |= (1 << RED_LED);
    }
}

// ---------------------------------------------------------
// Main Program
// ---------------------------------------------------------
int main()
{
    // LEDs as outputs
    DDRB |= (1 << GREEN_LED) |
            (1 << YELLOW_LED) |
            (1 << RED_LED);

    // Trigger output
    DDRD |= (1 << TRIG);

    // Echo input
    DDRD &= ~(1 << ECHO);

    // Button input
    DDRD &= ~(1 << RESET_BTN);

    // Enable internal pull-up for button
    PORTD |= (1 << RESET_BTN);

    update_leds();

    while (1)
    {
        // -------- Reset Button --------
        if (!(PIND & (1 << RESET_BTN)))
        {
            vehicles_entered = 0;
            available_slots = initial_capacity;

            update_leds();

            _delay_ms(300);
        }

        // -------- Vehicle Detection --------
        uint16_t distance = get_distance_cm();

        uint8_t current_vehicle_state = 0;

        // Vehicle detected if within 15 cm
        if (distance < 15)
        {
            current_vehicle_state = 1;
        }

        // State change detection
        if (current_vehicle_state == 1 &&
            previous_vehicle_state == 0)
        {
            if (vehicles_entered < initial_capacity)
            {
                vehicles_entered++;

                available_slots =
                    initial_capacity - vehicles_entered;

                update_leds();
            }
        }

        previous_vehicle_state = current_vehicle_state;

        _delay_ms(100);
    }
}