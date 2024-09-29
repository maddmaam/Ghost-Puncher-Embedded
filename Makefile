# Makefile for ENCE260 Assignment
# File created 2023/09/26
# Built up from M. P. Hayes of UCECE's Makefile template, which was dated 2010/09/12

# Definitions.
CC = avr-gcc
CFLAGS = -mmcu=atmega32u2 -Os -Wall -Wstrict-prototypes -Wextra -g -I. -I../../utils -I../../fonts -I../../drivers -I../../drivers/avr
OBJCOPY = avr-objcopy
SIZE = avr-size
DEL = rm
CLEAR = clear


# Default target.
all: game.out


# Compile: create object files from C source files.

# <> files are assumed to be stable, but I've listed them in comments in case that assumption changes

# D R I V E R .o  F I L E S #

# System includes <avr/wdt.h>
system.o: ../../drivers/avr/system.c ../../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

# Delay.h (no associated .c, it's just #defines) includes from <util/delay.h>
navswitch.o: ../../drivers/navswitch.c ../../drivers/navswitch.h ../../drivers/avr/system.h ../../drivers/avr/delay.h
	$(CC) -c $(CFLAGS) $< -o $@

# timer.h includes <avr/io.h>
timer.o: ../../drivers/avr/timer.c ../../drivers/avr/timer.h ../../drivers/avr/system.h
	$(CC) -c $(CFLAGS) $< -o $@

# pio.h includes <avr/io.h>
pio.o: ../../drivers/avr/pio.c ../../drivers/avr/pio.h
	$(CC) -c $(CFLAGS) $< -o $@



# System files from here are just dependencies of ir_uart & not used anywhere else. Seems to be a fairly intensive module :3

# the src file includes from <stdlib.h>
prescale.o: ../../drivers/avr/prescale.c ../../drivers/avr/system.h ../../drivers/avr/prescale.h
	$(CC) -c $(CFLAGS) $< -o $@

# the src file includes from <avr/io.h>
timer0.o: ../../drivers/avr/timer0.c ../../drivers/avr/system.h ../../drivers/avr/timer0.h ../../drivers/avr/bits.h ../../drivers/avr/prescale.h
	$(CC) -c $(CFLAGS) $< -o $@

# the src file includes from <avr/io.h>
usart1.o: ../../drivers/avr/usart1.c ../../drivers/avr/system.h ../../drivers/avr/usart1.h
	$(CC) -c $(CFLAGS) $< -o $@

ir_uart.o: ../../drivers/avr/ir_uart.c ../../drivers/avr/ir_uart.h ../../drivers/avr/system.h ../../drivers/avr/usart1.h ../../drivers/avr/timer0.h ../../drivers/avr/pio.h ../../drivers/avr/delay.h
	$(CC) -c $(CFLAGS) $< -o $@

# And system files from here are just dependencies of tinygl.o & not used anywhere else.

display.o: ../../drivers/display.c ../../drivers/avr/system.h ../../drivers/display.h ../../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

ledmat.o: ../../drivers/ledmat.c ../../drivers/avr/pio.h ../../drivers/avr/system.h ../../drivers/ledmat.h
	$(CC) -c $(CFLAGS) $< -o $@

font.o: ../../utils/font.c ../../drivers/avr/system.h ../../utils/font.h
	$(CC) -c $(CFLAGS) $< -o $@

# src file includes <string.h>
tinygl.o: ../../utils/tinygl.c ../../utils/tinygl.h ../../drivers/avr/system.h ../../drivers/display.h ../../utils/font.h
	$(CC) -c $(CFLAGS) $< -o $@
# End D R I V E R .o  F I L E S #


# G A M E .o  F I L E S #
display_matrix.o: display_matrix.c ../../drivers/avr/pio.h ../../utils/tinygl.h display_matrix.h sprites_and_fighters.h ../../fonts/font5x7_1.h
	$(CC) -c $(CFLAGS) $< -o $@

# the .h includes <stdint.h>
sprites_and_fighters.o: sprites_and_fighters.c sprites_and_fighters.h
	$(CC) -c $(CFLAGS) $< -o $@

user_input.o: user_input.c ../../drivers/navswitch.h user_input.h
	$(CC) -c $(CFLAGS) $< -o $@

# the .h includes <stdint.h>, the .c includes <stdlib>
communications.o: communications.c ../../drivers/avr/ir_uart.h game.h communications.h sprites_and_fighters.h
	$(CC) -c $(CFLAGS) $< -o $@

# the .c includes <avr/io>
scheduling.o: scheduling.c scheduling.h
	$(CC) -c $(CFLAGS) $< -o $@

# the .c includes <stdbool.h> and <stdint.h>
game.o: game.c ../../utils/tinygl.h display_matrix.h user_input.h communications.h game.h sprites_and_fighters.h scheduling.h
	$(CC) -c $(CFLAGS) $< -o $@

main.o: main.c ../../drivers/avr/system.h ../../drivers/avr/ir_uart.h display_matrix.h user_input.h communications.h scheduling.h game.h
	$(CC) -c $(CFLAGS) $< -o $@
# End G A M E .o  F I L E S #


# Link: create ELF output file from object files.
game.out: main.o game.o system.o navswitch.o timer.o pio.o prescale.o timer0.o usart1.o ir_uart.o display_matrix.o user_input.o scheduling.o communications.o sprites_and_fighters.o tinygl.o font.o display.o ledmat.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
	$(SIZE) $@


# Target: clean project. Added clear for convenience
.PHONY: clean
clean: 
	-$(DEL) *.o *.out *.hex; $(CLEAR)


# Target: program project.
.PHONY: program
program: game.out
	$(OBJCOPY) -O ihex game.out game.hex
	dfu-programmer atmega32u2 erase; dfu-programmer atmega32u2 flash game.hex; dfu-programmer atmega32u2 start