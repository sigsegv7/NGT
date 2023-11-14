.SILENT:

override PROMPT := printf "%s\t\t%s\n"
override CFILES = $(shell find src/ -name "*.c")
override OBJECTS = $(CFILES:.c=.o)
override CC = gcc
override CFLAGS = -pedantic -Werror=implicit-function-declaration \
		 		  -Wno-multichar								  \
		 		  -Iinclude/

bin/ngt: $(OBJECTS)
	mkdir -p $(@D)
	gcc $(OBJECTS) -o $@

%.o: %.c
	$(PROMPT) " CC " $<
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)
