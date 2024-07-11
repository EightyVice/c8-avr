/*
 * lcd.h
 *
 * Created: 9/13/2023 7:00:24 PM
 *  Author: Zeyad <EightyVice> Ahmed
 */ 



#ifndef LCD_IO
#define LCD_IO

typedef struct{
	/* Pin Numbers */
	uint8_t RS;	
	uint8_t RW;
	uint8_t E;
	
	volatile uint8_t* data;
} lcd_ctx_t;

void lcd_init(lcd_ctx_t* ctx, uint8_t RS,
				uint8_t RW,
				uint8_t E,
				volatile uint8_t* data);
void lcd_cmd(lcd_ctx_t* ctx, uint8_t cmd);
void lcd_data(lcd_ctx_t* ctx, uint8_t data);
void lcd_setgc(lcd_ctx_t* ctx, uint8_t slot, uint8_t* bytes);
void lcd_print(lcd_ctx_t* ctx, char* str);
void lcd_setxy(lcd_ctx_t* ctx, uint8_t x, uint8_t y);
#endif
