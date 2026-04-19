# ==============================================================================
# Makefile para o Projeto DTW - T1 Sistemas Embarcados
# ==============================================================================

CC = gcc
# Flags: Avisos no máximo, otimização nível 2, e padrão C99 estrito
CFLAGS = -Wall -Wextra -Wpedantic -O2 -std=c99
TARGET = dtw_app

# Arquivos fonte e objetos
SRCS = main.c dtw.c
OBJS = $(SRCS:.c=.o)
HEADERS = dtw.h

# Regra principal (o que roda quando digitar 'make')
all: $(TARGET)

# Como "linkar" os objetos para criar o executável final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Como compilar cada .c em um .o separadamente
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os binários gerados (make clean)
clean:
	rm -f $(OBJS) $(TARGET)

# Declara que 'all' e 'clean' não são arquivos físicos
.PHONY: all clean