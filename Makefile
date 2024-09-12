# Sample Makefile to build C programs and link them to sketch

# Change this to the path of the sketch directory if it is a a different path
SKETCH_PATH=sketch

SOURCES=$(wildcard *.c)
TARGETS=$(SOURCES:%.c=%)
GCC=gcc -I $(SKETCH_PATH) -L $(SKETCH_PATH)

.PHONY: default

default: $(TARGETS)

%: %.c
	$(GCC) $<  -lm -l sketch -o $@ && ./$@

clean:
	-rm -f $(TARGETS)
