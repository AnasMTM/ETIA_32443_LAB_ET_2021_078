#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define STATE_NORMAL      0
#define STATE_EMERGENCY   1
#define STATE_PEDESTRIAN  2
#define STATE_MAINTENANCE 3

volatile uint8_t emergency_flag = 0;
volatile uint8_t pedestrian_flag = 0;
volatile uint8_t maintenance_flag = 0;

void init_hardware(void);
void set_lights_normal_flow(uint8_t cycle);
void clear_all_lights(void);

int main(void) {
    init_hardware();
    sei();

    uint8_t current_state = STATE_NORMAL;
    uint8_t normal_cycle = 0; 
    uint16_t timer_counter = 0;

    while (1) {
        if (emergency_flag) {
            current_state = STATE_EMERGENCY;
        } else if (pedestrian_flag && current_state != STATE_EMERGENCY) {
            current_state = STATE_PEDESTRIAN;
        } else if (maintenance_flag && current_state == STATE_NORMAL) {
            current_state = STATE_MAINTENANCE;
        }

        switch (current_state) {
            
            case STATE_NORMAL:
                set_lights_normal_flow(normal_cycle);
                _delay_ms(100);
                timer_counter++;
                if (timer_counter >= 50) {
                    normal_cycle = !normal_cycle;
                    timer_counter = 0;
                }
                break;

            case STATE_EMERGENCY:
                clear_all_lights();
                PORTB |= (1 << PB0);
                PORTD |= (1 << PD7);
                
                for (int i = 0; i < 100; i++) {
                    _delay_ms(100);
                }
                
                emergency_flag = 0;
                timer_counter = 0;
                current_state = STATE_NORMAL;
                break;

            case STATE_PEDESTRIAN:
                PORTD |= (1 << PD6); 
                for (int i = 0; i < 50; i++) {
                    _delay_ms(100);
                    if (emergency_flag) break; 
                }

                if (!emergency_flag) {
                    clear_all_lights();
                    PORTD |= (1 << PD5);
                    PORTB |= (1 << PB1);
                    
                    for (int i = 0; i < 100; i++) {
                        _delay_ms(100);
                        if (emergency_flag) break;
                    }
                }

                pedestrian_flag = 0;
                timer_counter = 0;
                current_state = STATE_NORMAL;
                break;

            case STATE_MAINTENANCE:
                clear_all_lights();
                
                while (maintenance_flag) {
                    PORTD ^= (1 << PD6);
                    
                    for (int i = 0; i < 5; i++) {
                        _delay_ms(100);
                        
                        if (emergency_flag || pedestrian_flag) {
                            maintenance_flag = 0; 
                            break;
                        }
                    }
                }
                
                timer_counter = 0;
                current_state = STATE_NORMAL;
                break;
        }
    }
    return 0;
}

void init_hardware(void) {
    DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD7);
    DDRB |= (1 << PB0) | (1 << PB1);            

    DDRD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4));
    
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4);

    EICRA |= (1 << ISC01) | (1 << ISC11);
    EICRA &= ~((1 << ISC00) | (1 << ISC10));
    EIMSK |= (1 << INT0) | (1 << INT1);

    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT20);
}

void set_lights_normal_flow(uint8_t cycle) {
    clear_all_lights();
    if (cycle == 0) {
        PORTD |= (1 << PD5);
        PORTB |= (1 << PB1);
    } else {
        PORTD |= (1 << PD7);
        PORTB |= (1 << PB0);
    }
}

void clear_all_lights(void) {
    PORTD &= ~((1 << PD5) | (1 << PD6) | (1 << PD7));
    PORTB &= ~((1 << PB0) | (1 << PB1));
}


ISR(INT0_vect) {
    emergency_flag = 1;
}

ISR(INT1_vect) {
    pedestrian_flag = 1;
}

ISR(PCINT2_vect) {
    if (!(PIND & (1 << PD4))) {
        maintenance_flag = 1;
    }
}