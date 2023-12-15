DAY ?= 10
YEAR ?= 2023

DIST := ./dist
INCLUDE := ./include
INPUT := ./include

FLAGS := -I$(INCLUDE)
LIBS := -lcollections -lm

INPUT := $(YEAR)/input_$(shell printf %02d $(DAY)).txt
SOURCE := $(YEAR)/day_$(shell printf %02d $(DAY))
TARGET := $(DIST)/$(SOURCE)

all: $(DIST)/$(YEAR) $(YEAR) $(INPUT) $(TARGET)

$(DIST)/$(YEAR) $(YEAR):
	mkdir -p "$@"

$(INPUT):
	./input_dl.sh $(DAY) $(YEAR)

$(TARGET): $(SOURCE).c
	gcc -g -o $@ $< $(FLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(DIST)
