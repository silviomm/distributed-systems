# October 2006
# Prof.: Silvana Rossetto
#------------------------------------------------------------

VERSION = 1.1

PUB = pub.c
SUB = sub.c
P = pub
C = sub

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
all:  $(OBJ)/mysocket.o $(BIN)/$(P) $(BIN)/$(C)

$(OBJ)/mysocket.o: $(SRC)/mysocket.c $(INC)/mysocket.h
	$(CC) -c $(CFLAGS) -o $@ $(SRC)/mysocket.c

$(BIN)/$(P): $(OBJ)/mysocket.o $(PUB)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(PUB) $(LFLAGS)

$(BIN)/$(C): $(OBJ)/mysocket.o $(SUB)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(SUB)

# version
version:
	@echo $(VERSION) "(Draft Version!)"
# clean
clean:
	$(RM) -rf $(OBJ)/*.o  $(BIN)/*
