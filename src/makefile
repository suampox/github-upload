IDIR =../headers
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=../obj

_DEPS = definiciones.h abb.h sistema_entrada.h tabla_simbolos.h errores.h analizador_lexico.h analizador_sintactico.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = abb.o sistema_entrada.o tabla_simbolos.o errores.o analizador_lexico.o analizador_sintactico.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -Wall -g -c -o $@ $< $(CFLAGS)

compiladores_p1: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 




