NAME = plant_protection_program
# Add any modules for which you want to use your own code for assign7, rest will be drawn from library
MY_MODULES = dht11_sensor.o spi.o mcp3008.o water_sensor.o ftoa.o soil_sensor.o lcd_2004.o
#Put new .o files here; as long as .h in same .c file, fine

# This is the list of modules for building libmypi.a
LIBMYPI_MODULES = my_modules/timer.o my_modules/gpio.o my_modules/strings.o my_modules/printf.o my_modules/backtrace.o my_modules/malloc.o my_modules/keyboard.o my_modules/shell.o my_modules/fb.o my_modules/gl.o my_modules/console.o my_modules/gprof.o

CFLAGS  = -I$(CS107E)/include -g -Wall -Wpointer-arith
CFLAGS += -Og -std=c99 -ffreestanding
CFLAGS += -mapcs-frame -fno-omit-frame-pointer -mpoke-function-name
LDFLAGS = -nostdlib -T memmap -L. -L$(CS107E)/lib
LDLIBS  = -lpi -lgcc

all : $(NAME).bin $(MY_MODULES)

%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: %.o $(MY_MODULES) start.o cstart.o
	arm-none-eabi-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	arm-none-eabi-as $(ASFLAGS) $< -o $@

libmypi.a: $(LIBMYPI_MODULES) Makefile
	rm -f $@
	arm-none-eabi-ar cDr $@ $(filter %.o,$^)

%.list: %.o
	arm-none-eabi-objdump --no-show-raw-insn -d $< > $@

install: $(NAME).bin
	rpi-install.py -p $<

bonus: $(NAME)-bonus.bin
	rpi-install.py -p $<

# Note: link is now against local libmypi first
%-bonus.elf: %.o start.o cstart.o libmypi.a
	arm-none-eabi-gcc $(LDFLAGS) $(filter %.o,$^) -lmypi $(LDLIBS) -o $@

clean:
	rm -f *.o *.bin *.elf *.list *~ libmypi.a

.PHONY: all clean install test bonus

.PRECIOUS: %.elf %.o %.a

# empty recipe used to disable built-in rules for native build
%:%.c
%:%.o

define CS107E_ERROR_MESSAGE
ERROR - CS107E environment variable is not set.

Please set it to point to the `cs107e.github.io/cs107e` directory using the
command `export CS107E=<replace with path to your cs107e.github.io directory>/cs107e`.

To have this done automatically, add the above command to your shell
environment configuration file (e.g. ~/.bashrc)
endef

ifndef CS107E
$(error $(CS107E_ERROR_MESSAGE))
endif
