# https://stackoverflow.com/a/25966957

BIN := bin
SRC := src
OBJ := objects

apps    :=
sources :=
objects :=
deps    :=

include $(SRC)/apps/module.mk

-include $(deps)

CC       := gcc
CPPFLAGS := -I. -I./src/lib -MMD -MP
CFLAGS   := -std=c23 -O3 -Wall -Wextra -pedantic-errors
LDFLAGS  :=
LDLIBS   :=

# Link apps
$(BIN)/%: $(OBJ)/%.o
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile objects
$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all
all: $(apps) $(objects)

.PHONY: clean
clean:
	$(RM) -r $(BIN)/* $(OBJ)/*
