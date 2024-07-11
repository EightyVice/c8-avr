#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"
#include "c8.h"

#define pinRS	5
#define pinRW	6
#define pinE	7

lcd_ctx_t lcd;
c8_t c8;

void clr() {

}

#define bb "%c%c%c%c%c%c%c%c"
#define _b(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


void drw(size_t X, size_t Y, char n) {
	PORTC |= 1 << 2;
	lcd_setxy(&lcd, 0, 0);
	

	/* Drawing */
	for (size_t w = 0; w < 32; w++)
	{
		for (size_t h = 0; h < 16; h++)
		{
			uint8_t b = c8.VRAM[h][w];
			
		}	
	}
	
	
	_delay_ms(20);
	PORTC &= ~(1 << 2);
}


uint8_t rom[] = {
	0x60, 0x00, 0x61, 0x00, 0xA2, 0x22, 0xC2, 0x01, 0x32, 0x01, 0xA2, 0x1E, 0xD0, 0x14, 0x70, 0x04,
	0x30, 0x40, 0x12, 0x04, 0x60, 0x00, 0x71, 0x04, 0x31, 0x20, 0x12, 0x04, 0x12, 0x1C, 0x80, 0x40,
	0x20, 0x10, 0x20, 0x40, 0x80, 0x10
};

int main(void)
{
	lcd_init(&lcd, pinRS, pinRW, pinE, &PORTD);
 

	c8_init(&c8);
	c8.onClear = clr;
	c8.onDraw = drw;

	memcpy(&c8.memory[0x200], rom, sizeof(rom));


	char addr[8];
	memset(addr, 0, 8); 

	while(c8.PC < (0x200 + sizeof(rom))) 
	{ 
		c8_fetch(&c8);
		lcd_setxy(&lcd, 0, 0);
		_delay_ms(20);   

		sprintf(addr, " %X", c8.IR);
		lcd_print(&lcd, addr);
		_delay_ms(20);
		c8_execute(&c8);
	}
	while(1);
	
}