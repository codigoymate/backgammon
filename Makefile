SRC := $(wildcard src/*.c)
OBJ := $(addprefix obj/, $(notdir $(SRC:.c=.o)))

CFLAGS := -g -D BG_DEBUG -Wall -I include $(shell pkg-config --cflags gtk+-3.0)
LFLAGS := $(shell pkg-config --libs gtk+-3.0) -lm

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

transl_start:
	mkdir -p po/es/LC_MESSAGES
	mkdir -p po/fr/LC_MESSAGES
	xgettext --keyword=_ --add-comments --sort-output \
		-o po/backgammon.pot $(SRC) $(wildcard ui/*.glade)
	msginit --input=po/backgammon.pot --locale=es_ES.UTF-8 --output=po/es/backgammon.po
	msginit --input=po/backgammon.pot --locale=fr_FR.UTF-8 --output=po/fr/backgammon.po
	msgfmt --output-file=po/es/LC_MESSAGES/backgammon.mo po/es/backgammon.po
	msgfmt --output-file=po/fr/LC_MESSAGES/backgammon.mo po/fr/backgammon.po

transl_update:
	mkdir -p po/es/LC_MESSAGES
	mkdir -p po/fr/LC_MESSAGES
	xgettext --keyword=_ --add-comments --sort-output \
		-o po/backgammon.pot $(SRC) $(wildcard ui/*.glade)
	msgmerge --update po/es/backgammon.po po/backgammon.pot
	msgmerge --update po/fr/backgammon.po po/backgammon.pot
	msgfmt --output-file=po/es/LC_MESSAGES/backgammon.mo po/es/backgammon.po
	msgfmt --output-file=po/fr/LC_MESSAGES/backgammon.mo po/fr/backgammon.po
