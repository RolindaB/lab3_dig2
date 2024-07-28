#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// Define el valor máximo para el contador (8 bits)
#define MAX_VALUE 255

void GPIO_init(void) {
	// Configura PC4 y PC5 como salidas
	DDRC |= (1 << DDC4) | (1 << DDC5);

	// Configura PD2 a PD7 como salidas
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);

	// Inicializa los LEDs apagados
	PORTC &= ~((1 << PORTC4) | (1 << PORTC5));
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7));
}

void refreshPort(uint8_t value) {
	// Actualiza cada bit en PORTD y PORTC según el valor recibido
	if (value & 0b00000001) {  // Bit 0
		PORTC |= (1 << PORTC4);
		} else {
		PORTC &= ~(1 << PORTC4);
	}

	if (value & 0b00000010) {  // Bit 1
		PORTC |= (1 << PORTC5);
		} else {
		PORTC &= ~(1 << PORTC5);
	}

	if (value & 0b00000100) {  // Bit 2
		PORTD |= (1 << PORTD2);
		} else {
		PORTD &= ~(1 << PORTD2);
	}

	if (value & 0b00001000) {  // Bit 3
		PORTD |= (1 << PORTD3);
		} else {
		PORTD &= ~(1 << PORTD3);
	}

	if (value & 0b00010000) {  // Bit 4
		PORTD |= (1 << PORTD4);
		} else {
		PORTD &= ~(1 << PORTD4);
	}

	if (value & 0b00100000) {  // Bit 5
		PORTD |= (1 << PORTD5);
		} else {
		PORTD &= ~(1 << PORTD5);
	}

	if (value & 0b01000000) {  // Bit 6
		PORTD |= (1 << PORTD6);
		} else {
		PORTD &= ~(1 << PORTD6);
	}

	if (value & 0b10000000) {  // Bit 7
		PORTD |= (1 << PORTD7);
		} else {
		PORTD &= ~(1 << PORTD7);
	}
}

int main(void) {
	uint8_t test_values[] = {0, 1, 3, 7, 15, 31, 63, 127, 255};
	uint8_t i;

	// Inicializa GPIO
	GPIO_init();

	// Prueba diferentes valores para verificar el funcionamiento de los LEDs
	while (1) {
		for (i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
			refreshPort(test_values[i]);
			_delay_ms(1000); // Espera 1 segundo para ver el resultado
		}
	}

	return 0;
}
