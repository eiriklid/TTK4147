# program executable name
TARGET = miniproject

# compiler flags
CFLAGS = -g -Wall -D_GNU_SOURCE -std=c99

# linker flags

# list of sources
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	/home/student/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc $^ -o $@ -pthread

# compile
%.o : %.c
	/home/student/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc $(CFLAGS) -c -o $@ $<
	sudo cp $(TARGET) /export/nfs

# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)

copy:
	sudo cp $(TARGET) /export/nfs
