#==============================================================================#
# RADAR SYSTEM MAKEFILE
#==============================================================================#

MCU    = atmega328p
F_CPU  = 16000000UL
CC     = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -Os -Wall -DF_CPU=$(F_CPU)

TARGET = main
RADAR_CONIG := inc/radar_config.h
RADAR_HEALTH_CHECK_ENABLED := $(shell \
awk '$$1 == "#define" && $$2 == "RADAR_HEALTH_CHECK_ENABLED" {print $$3; exit}' $(RADAR_CONIG))

ifneq ($(RADAR_HEALTH_CHECK_ENABLED),0)
ifneq ($(RADAR_HEALTH_CHECK_ENABLED),1)
$(error RADAR_HEALTH_CHECK_ENABLED must be 0 or 1)
endif
endif
CFLAGS += -DRADAR_HEALTH_CHECK_ENABLED=$(RADAR_HEALTH_CHECK_ENABLED)

# SOURCE FILES #
SRC    = main.c driver/timer.c driver/uart.c driver/gpio.c driver/servo.c driver/icu.c src/sweep_control.c src/obstacle_detection.c app/radar_control.c src/send_data.c

# INCLUDE OPTIONAL HEALTH_CHECK ONLY WHEN ENABLED #
ifeq ($(RADAR_HEALTH_CHECK_ENABLED),1)
SRC += src/boot_health_check.c
endif

# This converts the .c list into a .o list
OBJ    = $(SRC:.c=.o)


#=========================================================================================================#
# BUILD
#=========================================================================================================#

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

# Include generated header dependencies
DEP = $(OBJ:.o=.d)
-include $(DEP)

#--------------------------------------------------------------------------------------------------------#
# clean
#--------------------------------------------------------------------------------------------------------#
clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex

#-------------------------------------------------------------------------------------------------------#
# Flash and serial monitor
#-------------------------------------------------------------------------------------------------------#
flash:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -b 115200 -U flash:w:main.hex

serial:
	picocom -b 9600 --echo /dev/ttyACM0

.PHONY: all clean flash serial