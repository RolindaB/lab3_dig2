#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "Spi_master/Spi_M.h"
#include "UART/UART.h"

volatile uint8_t valorSpi1, valorSpi2;
volatile char buffer[16];
int valorSpi1b, valorSpi2b;
uint8_t inicio = 1;
int received_value = 0;
char input_buffer[4]; // Buffer para almacenar la cadena recibida

void setUp();
void enviarValoresUART();
void IntCont();
void lecturaADC();
//inicialización
void setUp() {
	// Configura PC4 y PC5 como salidas
	DDRC |= (1 << DDC4) | (1 << DDC5);
	// Configura PD2 a PD7 como salidas
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);
	// Inicializa los LEDs apagados
	PORTC &= ~((1 << PORTC4) | (1 << PORTC5));
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7));
	SpiInit(SPI_MASTER_OSC_DIV128, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOK_FIRST_EDGE);
	UART_init(9600); // Asegúrate de inicializar UART
}
//uso de uart a consola
void enviarValoresUART() {
	valorSpi2b = (int)(valorSpi2);
	valorSpi1b = (int)(valorSpi1);
	UART_send_string("\n\r");
	snprintf(buffer, sizeof(buffer), "P1: %d \r\n", valorSpi1b);
	UART_send_string(buffer);
	_delay_ms(1);
	snprintf(buffer, sizeof(buffer), "P2: %d \r\n", valorSpi2b);
	UART_send_string(buffer);
}
//leer adc
void lecturaADC() {
	PORTB &= ~(1<<PORTB2); // SLAVE SELECT LOW
	SpiWrite('c'); // Suponiendo que 'c' se representa como 0xC0
	valorSpi1 = SpiRead();
	_delay_ms(10);
	SpiWrite('d'); // Suponiendo que 'd' se representa como 0xD0
	valorSpi2 = SpiRead();
	_delay_ms(10);
	PORTB |= (1<<PORTB2); // SLAVE SELECT HIGH
}
//dependiendo de lo que quiera el usario opciones

void IntCont() {

	if (UART_available()) {
		char comando = UART_receive();
		if (comando == 'm') {
			enviarValoresUART();
			_delay_ms(100); // Esperar un tiempo para permitir la actualización del ADC
			UART_send_string("\nIngrese 'm' para observar el ADC.\r\n");
			UART_send_string("Ingrese 'n' para el contador.\r\n");
			IntCont();
		} else if (comando == 'n'){
				contNUm();
				_delay_ms(100); // Esperar un tiempo para permitir la actualización del ADC
				UART_send_string("\nIngrese 'm' para observar el ADC.\r\n");
				UART_send_string("Ingrese 'n' para el contador.\r\n");
				IntCont();
		}else {
			UART_send_string("\nEl comando ingresado no se reconoce\n");
		}
	}
}
/********************Mape0 de leds **************************************/

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

/*******************Contador uart***************************************/
void contNUm(){
	UART_send_string("\n\r");
	// Mensaje inicial para el usuario
	UART_send_string("Por favor, ingresa un número (0-255): ");
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
		//UART_send_string("\nPor favor, ingresa un número (0-255): ");
		
}

/********************PROGRAMA PRINCIPAL*********************************/
int main(void) {
	setUp();
	sei(); // Habilitar interrupciones globales

	while (1) {
		if (inicio == 1) {
			_delay_ms(100); // Esperar un tiempo para permitir la actualización del ADC
			UART_send_string("\nIngrese 'm' para observar el ADC.\r\n");
			UART_send_string("Ingrese 'n' para el contador.\r\n");
			lecturaADC();
			_delay_ms(300);
			IntCont();
			inicio = 0;
			} else {
			_delay_ms(500); // Esperar un tiempo para permitir la actualización del ADC
			IntCont(); // Verificar y procesar los comandos UART
		}
		lecturaADC(); // Leer ADC y actualizar valores
		_delay_ms(300);
	}
}
