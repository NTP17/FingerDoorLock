#define BAUD_RATE 57600
#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "i2c_master.h"
#include "liquid_crystal_i2c.h"

void USART_Init(unsigned int baud);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);

unsigned char gen_img();
unsigned char img2tz();
unsigned char finger_search();

void buzzer_wrong();
void buzzer_correct();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS);
	uint8_t wrongs = 0;
	USART_Init(BAUD_RATE);
	lq_turnOnBacklight(&device);
	
	DDRB |= (1 << PB2);
	DDRB |= (1 << PB1);

	PORTB |= (1 << PB1);
	_delay_ms(100);
	PORTB &= ~(1 << PB1);

	while (1)
	{
		PORTB &= ~(1 << PB2);
		do {
			gen_img();
			lq_clear(&device);
			lq_print(&device, "Insert finger");
		} while (gen_img() != 0x00);
			
		lq_clear(&device);
			
		img2tz();
			
		if (finger_search() == 0x00) {
			lq_clear(&device);
			lq_print(&device, "Found a match");
			buzzer_correct();
			PORTB |= (1 << PB2);
			_delay_ms(5000);
			wrongs = 0;
		} else if (wrongs >= 4) {
			uint8_t cooldown = 30;
			lq_clear(&device);
			lq_print(&device, "Try again in 30s");
			buzzer_wrong();
			while (cooldown > 0) {
				char str[3];
				sprintf(str, "%02u", cooldown);
				lq_setCursor(&device, 0, 13);
				lq_print(&device, str);
				_delay_ms(1000);
				cooldown--;
			}
			wrongs = 0;
		} else {
			wrongs++;
			lq_clear(&device);
			lq_print(&device, "No match");
			buzzer_wrong();
			_delay_ms(1000);
		}
		
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void buzzer_wrong() {
	PORTB |= (1 << PB1);
	_delay_ms(50);
	PORTB &= ~(1 << PB1);
	_delay_ms(50);
	PORTB |= (1 << PB1);
	_delay_ms(50);
	PORTB &= ~(1 << PB1);
	_delay_ms(50);
	PORTB |= (1 << PB1);
	_delay_ms(50);
	PORTB &= ~(1 << PB1);
}

void buzzer_correct() {
	PORTB |= (1 << PB1);
	_delay_ms(500);
	PORTB &= ~(1 << PB1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USART_Init(unsigned int baud)
{
	// Set baud rate
	unsigned int ubrr = (F_CPU / (16UL * baud)) - 1;
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr);
	
	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// Set frame format: 8 data bits, 1 stop bit, no parity
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(uint8_t data)
{
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}
	// Put data into buffer, sends the data
	UDR0 = data;
}

uint8_t USART_Receive(void)
{
	// Wait for data to be received
	while (!(UCSR0A & (1 << RXC0)))
	{
	}
	// Get and return received data from buffer
	return UDR0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned char gen_img() {
	USART_Transmit(0xEF);
	USART_Transmit(0x01);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0x01);
	USART_Transmit(0x00);
	USART_Transmit(0x03);
	USART_Transmit(0x01);
	USART_Transmit(0x00);
	USART_Transmit(0x05);
	
	unsigned char byte0 = USART_Receive();
	unsigned char byte1 = USART_Receive();
	unsigned char byte2 = USART_Receive();
	unsigned char byte3 = USART_Receive();
	unsigned char byte4 = USART_Receive();
	unsigned char byte5 = USART_Receive();
	unsigned char byte6 = USART_Receive();
	unsigned char byte7 = USART_Receive();
	unsigned char byte8 = USART_Receive();
	unsigned char byte9 = USART_Receive();
	unsigned char byte10 = USART_Receive();
	unsigned char byte11 = USART_Receive();
	
	return byte9;
}

unsigned char img2tz() {
	USART_Transmit(0xEF);
	USART_Transmit(0x01);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0x01);
	USART_Transmit(0x00);
	USART_Transmit(0x04);
	USART_Transmit(0x02);
	USART_Transmit(0x01);
	USART_Transmit(0x00);
	USART_Transmit(0x08);	

	unsigned char byte0 = USART_Receive();
	unsigned char byte1 = USART_Receive();
	unsigned char byte2 = USART_Receive();
	unsigned char byte3 = USART_Receive();
	unsigned char byte4 = USART_Receive();
	unsigned char byte5 = USART_Receive();
	unsigned char byte6 = USART_Receive();
	unsigned char byte7 = USART_Receive();
	unsigned char byte8 = USART_Receive();
	unsigned char byte9 = USART_Receive();
	unsigned char byte10 = USART_Receive();
	unsigned char byte11 = USART_Receive();
	
	return byte9;
}

unsigned char finger_search() {
	USART_Transmit(0xEF);
	USART_Transmit(0x01);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0xFF);
	USART_Transmit(0x01);
	USART_Transmit(0x00);
	USART_Transmit(0x08);
	USART_Transmit(0x04);
	USART_Transmit(0x01);
	USART_Transmit(0x00);
	USART_Transmit(0x00);
	USART_Transmit(0x00);
	USART_Transmit(0x7F);
	USART_Transmit(0x00);
	USART_Transmit(0x8D);

	unsigned char byte0 = USART_Receive();
	unsigned char byte1 = USART_Receive();
	unsigned char byte2 = USART_Receive();
	unsigned char byte3 = USART_Receive();
	unsigned char byte4 = USART_Receive();
	unsigned char byte5 = USART_Receive();
	unsigned char byte6 = USART_Receive();
	unsigned char byte7 = USART_Receive();
	unsigned char byte8 = USART_Receive();
	unsigned char byte9 = USART_Receive();
	unsigned char byte10 = USART_Receive();
	unsigned char byte11 = USART_Receive();
	unsigned char byte12 = USART_Receive();
	unsigned char byte13 = USART_Receive();
	unsigned char byte14 = USART_Receive();
	unsigned char byte15 = USART_Receive();
	
	return byte9;
}