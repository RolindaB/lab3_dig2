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

void setUp();
void enviarValoresUART();
void IntCont();
void lecturaADC();

void setUp() {
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);
	PORTD &= ~((1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7));
	PORTC |= (1<<PORTC5)|(1<<PORTC6);
	PORTC &= ~((1<<PORTC5)|(1<<PORTC6));
	SpiInit(SPI_MASTER_OSC_DIV128, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOK_FIRST_EDGE);
	UART_init(9600); // Asegúrate de inicializar UART
}

void enviarValoresUART() {
	valorSpi2b = (int)(valorSpi2);
	valorSpi1b = (int)(valorSpi1);
	UART_send_string("\n");
	snprintf(buffer, sizeof(buffer), "P1: %d \r\n", valorSpi1b);
	UART_send_string(buffer);
	_delay_ms(1);
	snprintf(buffer, sizeof(buffer), "P2: %d \r\n", valorSpi2b);
	UART_send_string(buffer);
}

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

void IntCont() {
	if (UART_available()) {
		char comando = UART_receive();
		if (comando == 'm') {
			enviarValoresUART();
			} else {
			UART_send_string("\nEl comando ingresado no se reconoce\n");
		}
	}
}

// Rutina de interrupción del UART
ISR(USART_RX_vect) {
	uart_buffer[uart_head] = UDR0; // Leer el carácter recibido del registro UDR0
	uart_head = (uart_head + 1) % sizeof(uart_buffer); // Actualizar el índice de la cabeza del buffer
	uart_buffer[uart_head] = '\0'; // Terminador de cadena
}

/********************PROGRAMA PRINCIPAL****************************************************/
int main(void) {
	setUp();
	sei(); // Habilitar interrupciones globales

	while (1) {
		if (inicio == 1) {
			_delay_ms(100); // Esperar un tiempo para permitir la actualización del ADC
			UART_send_string("\nIngrese 'm' para observar el ADC.\r\n");
			UART_send_string("Ingrese 'algun numero' para el contador.\r\n");
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
