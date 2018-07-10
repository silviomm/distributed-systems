# October 2006
# Prof.: Silvana Rossetto
#------------------------------------------------------------

VERSION = 1.0

SERVER = server.c
CLIENTE = cliente.c
PAR_ATIVO = par_ativo.c
PAR_PASSIVO = par_passivo_op_2.c
SRV = srv
CLI = cli
ATIV = ativ
PASV = pasv

#-------------------------------------------------------------
# the rest of the file should not change
INC = ../include
SRC = ../src
OBJ = ../obj
BIN = ../bin

CC = gcc -Wall
CFLAGS = -I$(INC)
LFLAGS = -lpthread
RM = rm

# make all
all:  $(OBJ)/mysocket.o $(BIN)/$(SRV) $(BIN)/$(CLI) $(BIN)/$(ATIV) $(BIN)/$(PASV)

$(OBJ)/mysocket.o: $(SRC)/mysocket.c $(INC)/mysocket.h
	$(CC) -c $(CFLAGS) -o $@ $(SRC)/mysocket.c

#server
$(BIN)/$(SRV): $(OBJ)/mysocket.o $(SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(SERVER) $(LFLAGS)

#cliente
$(BIN)/$(CLI): $(OBJ)/mysocket.o $(CLIENTE)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(CLIENTE) $(LFLAGS)

#par-ativo
$(BIN)/$(ATIV): $(OBJ)/mysocket.o $(PAR_ATIVO)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(PAR_ATIVO) $(LFLAGS)

#par-passivo
$(BIN)/$(PASV): $(OBJ)/mysocket.o $(PAR_PASSIVO)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(PAR_PASSIVO) $(LFLAGS)


# version
version:
	@echo $(VERSION) "(Draft Version!)"
# clean
clean:
	$(RM) -rf $(OBJ)/*.o  $(BIN)/*
