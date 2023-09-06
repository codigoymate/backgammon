SRC := $(wildcard src/*.c)
OBJ := $(addprefix obj/, $(notdir $(SRC:.c=.o)))

CFLAGS := -Wall -I include $(shell pkg-config --cflags gtk+-3.0)
LFLAGS := $(shell pkg-config --libs gtk+-3.0)

ifeq ($(OS), Windows_NT)
	BIN := bin/backgammon.exe
	RM := rmdir /s /q obj bin
else
	BIN := bin/backgammon
	RM := rm -rf obj/ bin/
endif

all: $(BIN)

$(BIN): $(OBJ) | bin
	gcc $(CFLAGS) $(OBJ) -o $(BIN) $(LFLAGS)

obj/%.o: src/%.c | obj
	gcc -MD $(CFLAGS) $< -o $@ -c

-include obj/*.d

obj:
	mkdir obj

bin:
	mkdir bin

.PHONY=clean
clean:
	$(RM)

ifeq ($(OS), Windows_NT)
run: $(BIN)
	$(BIN)
else
run: $(BIN)
	./$(BIN)
endif
