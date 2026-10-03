MCU    = atmega328p
F_CPU  = 16000000UL
CC     = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -Os -Wall -DF_CPU=$(F_CPU)

TARGET = main
SRC    = main.c driver/timer/timer.c driver/uart/uart.c driver/gpio/gpio.c driver/servo/servo.c src/sweep_control.c
# This converts the .c list into a .o list
OBJ    = $(SRC:.c=.o)

all: $(TARGET).hex

# Link objects to create the ELF
$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Convert ELF to HEX for flashing
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Compile C files into Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET).elf $(TARGET).hex

.PHONY: all clean

flash:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -b 115200 -U flash:w:main.hex

.serial:
	picocom -b 9600 --echo /dev/ttyACM0