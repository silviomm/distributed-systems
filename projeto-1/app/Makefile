# October 2006
# Prof.: Silvana Rossetto
#------------------------------------------------------------

VERSION = 1.0

SERVER = par_passivo.c
CLIENT = par_ativo.c
SRV = srv
CLI = cli

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
all:  $(OBJ)/mysocket.o $(BIN)/$(SRV) $(BIN)/$(CLI)

$(OBJ)/mysocket.o: $(SRC)/mysocket.c $(INC)/mysocket.h
	$(CC) -c $(CFLAGS) -o $@ $(SRC)/mysocket.c

$(BIN)/$(SRV): $(OBJ)/mysocket.o $(SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(SERVER) $(LFLAGS)

$(BIN)/$(CLI): $(OBJ)/mysocket.o $(CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(CLIENT)
# version
version:
	@echo $(VERSION) "(Draft Version!)"
# clean
clean:
	$(RM) -rf $(OBJ)/*.o  $(BIN)/*
