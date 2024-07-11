#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct  {
	// RAM
	uint8_t memory[1024];

	// Stack
	uint16_t stack[16];
    uint8_t sp;

	// Registers
	uint8_t V[16];
	uint16_t I;
	uint16_t PC;
	uint8_t DT;
	uint8_t ST;
    uint16_t IR;

	// VRAM (Display)
	uint8_t VRAM[16][32]; // 64 x 32

    // Callbacks
    void(*onClear)();
    void(*onDraw)(size_t X, size_t Y, char tile);
} c8_t;
#define struct_size sizeof(c8_t)
void c8_init(c8_t* c8) {
	// Zero Memory
	memset(c8, 0x00, sizeof(c8_t));

    uint8_t font_map[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memcpy(c8->memory, font_map, sizeof(font_map));

	// Set Entry Point
	c8->PC = 0x200;
}

void c8_loadrom(c8_t* c8, char* file){
	FILE* rom_file = fopen(file, "rb");
	
	size_t sz = 0;
	fseek(rom_file, 0, SEEK_END);
	sz = ftell(rom_file);
	fseek(rom_file, 0, SEEK_SET);

    // maximum size for ROM
    if(sz < 3584){
    	fread(&c8->memory[0x200], 1, sz, rom_file);
	    fclose(rom_file);
    }
}

void c8_fetch(c8_t* c8) {
    c8->IR = ((c8->memory[c8->PC] << 8) | (c8->memory[c8->PC + 1]));
    c8->PC += 2;
}




void c8_execute(c8_t* c8) {
    // opcode is on the format of 0xOXYN or 0xONNN or 0xOXKK
    //nnn or addr - A 12 - bit value, the lowest 12 bits of the instruction
    //n or nibble - A 4 - bit value, the lowest 4 bits of the instruction
    //x - A 4 - bit value, the lower 4 bits of the high byte of the instruction
    //y - A 4 - bit value, the upper 4 bits of the low byte of the instruction
    //kk or byte - An 8 - bit value, the lowest 8 bits of the instruction

    uint16_t ir = c8->IR;

    uint8_t     o   = (ir & 0xF000) >> 12;
    uint8_t     x   = (ir & 0x0F00) >> 8;
    uint8_t     y   = (ir & 0x00F0) >> 4;
    uint8_t     n   = ir & 0x000F;
    uint8_t     kk  = ir & 0x00FF;
    uint16_t    nnn = ir & 0x0FFF;

#define Vx c8->V[x]
#define Vy c8->V[y]
#define VF c8->V[0xF]
#define PC c8->PC
#define I c8->I
#define ST c8->ST
#define DT c8->DT


    /*
    printf("o: %x ", o);
    printf("x: %x ", x);
    printf("y: %x ", y);
    printf("n: %x ", n);
    printf("nnn: %x \n", nnn);
    */
    switch (o) {
        case 0: {
            switch (n) {
                case 0x0: c8->onClear(); break;
                case 0xE: /*todo*/ break;
                default:  break;
            }
        } break;

        case 0x1: PC = nnn; break;
        case 0x2:
            c8->stack[c8->sp] = PC;
            c8->sp++;
            PC = nnn;

            break;
        case 0x3: if (Vx == kk) PC += 2; break;
        case 0x4: if (Vx != kk) PC += 2; break;
        case 0x5: if (Vx == Vy) PC += 2; break;
        case 0x6: Vx = kk;  break;
        case 0x7: Vx += kk;  break;

        case 0x8: {
            switch (n) {
                case 0x0: Vx = Vy;  break;
                case 0x1: Vx |= Vy; break;
                case 0x2: Vx &= Vy; break;
                case 0x3: Vx ^= Vy; break;
                case 0x4: VF = (Vx + Vy > 255 ? 1 : 0);  Vx += Vy; break;
                case 0x5: VF = (Vx > Vy ? 1 : 0);  Vx -= Vy; break;
                case 0x6: VF = Vx & 1;  Vx >>= 1; break;
                case 0x7: VF = (Vy > Vx ? 1 : 0);  Vx = Vy - Vx; break;
                case 0x8: VF = Vx & 1;  Vx <<= 1; break;
            }
            break;
        }

        case 0x9: if (Vx != Vy) PC += 2; break;
        case 0xA: I = nnn; break;
        case 0xB: PC = c8->V[0] + nnn; break;
        case 0xC: Vx = (rand() % 255) & kk; break;
        case 0xD: {
            for (size_t h = 0; h < n; h++)
            {
                char byte = c8->memory[I + h];
                for (size_t w = 0; w < 8; w++)
                {
                   c8->VRAM[h][w] = byte;
                }
            }
            c8->onDraw(x, y, n);            
        }break;

        case 0xE: {/*todo*/
            switch (kk) {
                case 0x9E:
                    // .. .. .. .. //
                    break;
                case 0xA1:
                    // .. .. .. .. // 
                    break;
            }
        }break;

        case 0xF: {
            switch (kk) {
                case 0x07: Vx = DT; break;
                case 0x15: DT = Vx; break;
                case 0x18: ST = Vx; break;
                case 0x1E: I = I + Vx; break;
                case 0x55: for (size_t i = 0; i < 0xF; i++) c8->memory[I + i] = c8->V[i]; break;
                case 0x65: for (size_t i = 0; i < 0xF; i++) c8->V[i] = c8->memory[I + i]; break;
            }
            break;
        } 
        default:
            puts("WRONG");
            break;
    }
#undef Vx 
#undef Vy 
#undef VF 
#undef PC 
#undef I
#undef ST 
#undef DT 

}
