#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include "ADC/ADC.h"
#include "Slave/Slave.h"
#include <avr/interrupt.h>

volatile uint8_t current_channel = 1;   // Canal inicial
volatile uint8_t adc_value1, adc_value2;

/************************ INTERRUPCIONES *******************/
// Rutina de interrupción del ADC
ISR(ADC_vect) {
	if (current_channel == 1) {
		adc_value1 = ADC; // Leer el valor alto del canal 1
		//adc_value1 |= (ADCL << 8); // Leer el valor bajo del canal 1 y combinarlo con el alto
		current_channel = 2;
		ADC2(); // Configurar ADC para el canal 2
	} else {
		adc_value2 = ADC; // Leer el valor alto del canal 2
		//adc_value2 |= (ADCL << 8); // Leer el valor bajo del canal 2 y combinarlo con el alto
		current_channel = 1;
		ADC1(); // Configurar ADC para el canal 1
	}
	habilitar_conversion(); // Iniciar la conversión ADC
}


int main(void) {
	SpiInit(SPI_SLAVE_SS, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOK_FIRST_EDGE);
	ADC_init(128);
	habilitar_conversion(); // Iniciar la primera conversión
	ADC1();
	SPCR |= (1<<SPIE); // Activar interrupción SPI
	sei(); // Habilitar interrupciones globales

	while (1) {
		uint8_t SpiValor = SPDR;
		if (SpiValor == 'c') {
			adc_value1 = adc_value1;
			SpiWrite(adc_value1); // Enviar el valor del canal 1
			
			_delay_ms(1);
		}
		if(SpiValor == 'd'){
			adc_value2 = adc_value2;
			SpiWrite(adc_value2); // Enviar el valor del canal 2
			
			_delay_ms(1);
		}
	}
}
