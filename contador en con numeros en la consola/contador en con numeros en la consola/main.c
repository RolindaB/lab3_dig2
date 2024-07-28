#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "UART/UART.h"

int received_value = 0;
char input_buffer[4]; // Buffer para almacenar la cadena recibida

// Configura los pines
void GPIO_init(void) {
	// Configura PC4 y PC5 como salidas
	DDRC |= (1 << DDC4) | (1 << DDC5);

	// Configura PD2 a PD7 como salidas
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);

	// Inicializa los LEDs apagados
	PORTC &= ~((1 << PORTC4) | (1 << PORTC5));
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7));
}

// Actualiza los LEDs según el valor recibido
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
	// Inicializa GPIO y UART
	GPIO_init();
	UART_init(9600); // Inicializa UART con baud rate de 9600
	sei(); // Habilita las interrupciones globales

	// Mensaje inicial para el usuario
	UART_send_string("Por favor, ingresa un número (0-255): ");

	while (1) {
		// Lee una cadena de caracteres desde UART
		UART_receive_string(input_buffer, sizeof(input_buffer));

		// Envía un salto de línea antes de imprimir el mensaje recibido
		UART_send('\n');
		UART_send('\r'); // Retorno de carro para asegurar la posición del cursor al inicio de la línea

		UART_send_string("Cadena recibida: ");
		UART_send_string(input_buffer);
		UART_send('\n');
		// Convierte la cadena a un número entero
		received_value = atoi(input_buffer);
		// Envía un salto de línea antes de imprimir el mensaje recibido
		UART_send('\n');
		UART_send('\r'); // Retorno de carro para asegurar la posición del cursor al inicio de la línea
		// Envía un salto de línea antes de imprimir el valor entero
		UART_send_string("Valor recibido como entero: ");
		UART_send_number(received_value);
		// Envía un salto de línea antes de imprimir el mensaje recibido
		UART_send('\n');
		UART_send('\r'); // Retorno de carro para asegurar la posición del cursor al inicio de la línea


		// Verifica si el número está en el rango permitido
		if (received_value >= 0 && received_value <= 255) {
			// Actualiza los LEDs según el valor recibido
			refreshPort(received_value);
			} else {
			// Maneja valores fuera del rango permitido
			UART_send_string("Valor no válido. Por favor, ingresa un número entre 0 y 255.\n");
		}

		// Solicita al usuario que ingrese otro número
		UART_send_string("\nPor favor, ingresa un número (0-255): ");
	}

	return 0;
}