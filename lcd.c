/*
 * lcd.c
 *
 * Created: 9/13/2023 7:29:38 PM
 *  Author: 8yvic
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

#define BIT_OFF(b, i)	b &= ~(1 << i)
#define BIT_ON(b, i)	b |= 1 << i

void lcd_init(lcd_ctx_t* ctx, uint8_t RS,
				uint8_t RW,
				uint8_t E,
				volatile uint8_t* data)
{
	ctx->RS = RS;
	ctx->RW = RW;
	ctx->E = E;		
	ctx->data = data;
	
	DDRC = 0xFF;
	DDRD = 0xFF;	
	
	BIT_OFF(PORTC, ctx->E);
	_delay_us(2000);			// wait for init
	lcd_cmd(ctx, 0x38);			// init LCD 2  line 5 x 7
	lcd_cmd(ctx, 0x0C);			// display on, cursor on
	lcd_cmd(ctx, 0x01);			// clear LCD
	
	_delay_us(2000);		// wait
	lcd_cmd(ctx, 0x06);			// shift cursor right		
}

void lcd_enable(lcd_ctx_t* ctx){
	BIT_ON(PORTC, ctx->E);		// EN = 1
	_delay_us(1);
	BIT_OFF(PORTC, ctx->E);		// EN = 0
	_delay_us(100);
}		
		
void lcd_cmd(lcd_ctx_t* ctx, uint8_t cmd){
	*(ctx->data) = cmd;
	
	BIT_OFF(PORTC, ctx->RS);		// RS = 0
	BIT_OFF(PORTC, ctx->RW);		// RW = 0
	
	lcd_enable(ctx);
}

void lcd_data(lcd_ctx_t* ctx, uint8_t data){
	*(ctx->data) = data;
	
	BIT_ON(PORTC, ctx->RS);		// RS = 1
	BIT_OFF(PORTC, ctx->RW);		// RW = 0
	BIT_ON(PORTC, ctx->E);		// EN = 1
	
	lcd_enable(ctx);
}

void lcd_print(lcd_ctx_t* ctx, char* str){
	uint8_t i = 0;
	while(str[i] != '\0'){
		lcd_data(ctx, str[i]);
		i++;
	}
}

void lcd_setxy(lcd_ctx_t* ctx, uint8_t x, uint8_t y){
	uint8_t line[] = {0x80, 0xC0, 0x94, 0xD4};
	lcd_cmd(ctx, line[x] + y);
	_delay_us(100);
}

void lcd_setgc(lcd_ctx_t* ctx, uint8_t slot, uint8_t* bytes){
	if(slot > 8)
		return;

	lcd_cmd(ctx, 0x40 + (slot * 8));

	for (uint8_t i = 0; i < 8; i++)
		lcd_data(ctx, bytes[i]);
	
}