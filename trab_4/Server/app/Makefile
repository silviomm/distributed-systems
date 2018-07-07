VERSION = 1.0
SERVER   = srv.cpp
PAS = pas
ATI = ati
USERS = users
SRV = serv
#-------------------------------------------------------------
# the rest of the file should not change
INC = ../include
SRC = ../src
OBJ = ../obj
BIN = ../bin

CC = g++ -std=c++11
CFLAGS = -I$(INC)
LFLAGS = -lpthread

RM = rm

# make all
all:  $(OBJ)/mysocket.o $(BIN)/$(SRV) $(BIN)/$(PAS)

$(OBJ)/mysocket.o: $(SRC)/mysocket.cpp $(INC)/mysocket.h $(INC)/structs.h
	$(CC) -c $(CFLAGS) -o $@ $(SRC)/mysocket.cpp

$(BIN)/$(SRV): $(OBJ)/mysocket.o $(SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(SERVER) $(LFLAGS)

version:
	@echo $(VERSION) "(Draft Version!)"
# clean
clean:
	$(RM) -rf $(OBJ)/*.o  $(BIN)/*
