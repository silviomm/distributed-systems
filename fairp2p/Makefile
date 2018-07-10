
#Final P2P applications
PEER1 = p1.c
PEER2 = p2.c
P1 = p1
P2 = p2

#-------------------------------------------------------------
# the rest of the file should not change
CC = gcc -Wall
LFLAGS = -lpthread 
RM = rm

# make all
all:  fairp2p.o $(P1) $(P2) 

fairp2p.o: fairp2p.c fairp2p.h
	$(CC) -c -o $@ fairp2p.c $(LFLAGS)

$(P1): fairp2p.o $(PEER1)
	$(CC) -o $@ fairp2p.o $(PEER1) $(LFLAGS) 

$(P2): fairp2p.o $(PEER2)
	$(CC) -o $@ fairp2p.o $(PEER2) $(LFLAGS) 

# clean
clean:
	$(RM) fairp2p.o ./$(P1) ./$(P2)
