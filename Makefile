# Definición de variables
CC = gcc
CFLAGS = -Wall -I -g

# Lista de archivos fuente y objetos
SRCS = p3.c List.c cmnds.c cmdList.c sigstrnum.c
OBJS = $(SRCS:.c=.o)

# Nombre del programa de salida
TARGET = p3

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Dependencias de los archivos de encabezado
p3.o: p3.c List.h cmnds.h cmdList.h
List.o: List.c List.h
cmnds.o: cmnds.c cmnds.h
cmdList.o: cmdList.c cmdList.h
sigstrnum.o: sigstrnum.c sigstrnum.h

# Lmpiar archivos temporales
clean:
	rm -f $(OBJS) $(TARGET)
