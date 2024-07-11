# Makefile
#
# targets:
#   all:    compiles the source code
#   test:   tests the isp connection to the mcu
#   flash:  writes compiled hex file to the mcu's flash memory
#   fuse:   writes the fuse bytes to the MCU
#   disasm: disassembles the code for debugging
#   clean:  removes all .hex, .elf, and .o files in the source code and library directories

# parameters (change this stuff accordingly)
# project name
PRJ = chip8
# avr mcu
MCU = atmega32
# mcu clock frequency
# CLK = 16000000
CLK = 8000000UL
# avr programmer (and port if necessary)
# e.g. PRG = usbtiny -or- PRG = arduino -P /dev/tty.usbmodem411
PRG = usbtiny
# fuse values for avr: low, high, and extended
# see http://www.engbedded.com/fusecalc/ for other MCUs and options
LFU = 
HFU = 
EFU = 

# output folder
BIN := bin

OUT := $(BIN)/$(PRJ)

# program source files (not including external libraries)
SRC_DIR = .

# where to look for external libraries (consisting of .c/.cpp files and .h files)
# e.g. EXT = ../../Eye2See ../../YouSART
EXT_DIR := mcal mcal/gpio mcal/spi mcal/usart
EXT_SRC_DIR := $(foreach dir, $(EXT_DIR), $(dir)/src)
EXT_HEADER_DIR := $(foreach dir, $(EXT_DIR), $(dir)/include)



#################################################################################################
# \/ stuff nobody needs to worry about until such time that worrying about it is appropriate \/ #
#################################################################################################

# include path
INCLUDE := $(foreach dir, $(EXT_HEADER_DIR), -I$(dir))
# c flags
CFLAGS    = -Wall -flto -Os -DF_CPU=$(CLK) -mmcu=$(MCU) #$(INCLUDE)
# any aditional flags for c++
CPPFLAGS =

# executables
AVRDUDE = avrdude -c $(PRG) -p $(MCU)
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size --format=avr --mcu=$(MCU)
CC      = avr-gcc

# generate list of objects
CFILES    := $(wildcard $(SRC_DIR)/*.c)
CPPFILES  := $(wildcard $(SRC_DIR)/*.cpp)
EXTC     := $(foreach dir, $(EXT_SRC_DIR), $(wildcard $(dir)/*.c))
EXTCPP   := $(foreach dir, $(EXT_SRC_DIR), $(wildcard $(dir)/*.cpp))
OBJ       = $(CFILES:.c=.o) $(EXTC:.c=.o) $(CPPFILES:.cpp=.o) $(EXTCPP:.cpp=.o)

# user targets
# compile all files
all: $(OUT).hex

# test programmer connectivity
test:
	$(AVRDUDE) -v

# flash program to mcu
flash: all
	$(AVRDUDE) -U flash:w:$(OUT).hex:i

# write fuses to mcu
fuse:
	$(AVRDUDE) -U lfuse:w:$(LFU):m -U hfuse:w:$(HFU):m -U efuse:w:$(EFU):m

# generate disassembly files for debugging
disasm: $(OUT).elf
	$(OBJDUMP) -d $(OUT).elf

# remove compiled files
clean:
	rm -rf $(BIN) *.hex *.elf *.o $(OBJ)
	

# other targets
# objects from c files
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# objects from c++ files
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# elf file
$(OUT).elf: $(OBJ) | $(BIN)
	$(CC) $(CFLAGS) -o $(OUT).elf $(OBJ)

# hex file
$(OUT).hex: $(OUT).elf
	$(OBJCOPY) -j .text -j .data -O ihex $(OUT).elf $(OUT).hex
	$(SIZE) $(OUT).elf

$(BIN) : 
	mkdir -p $(BIN)