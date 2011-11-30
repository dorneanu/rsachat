.EXPORT_ALL_VARIABLES:
		@echo "[-] ERROR: you need gnu make!"

CC = gcc
RM = rm -rf
MV = mv
CP = cp
MKDIR = mkdir

INCDIR = inc
SRCDIR = src

CFLAGS  = -W -Wall -Wextra -fmessage-length=0 -O2 -I $(INCDIR)
LDFLAGS = -lsqlite3 -lpthread -lcrypto

OBJS = $(SRCDIR)/args.o $(SRCDIR)/server.o $(SRCDIR)/client.o $(SRCDIR)/db.o \
	   $(SRCDIR)/key.o $(SRCDIR)/crypto.o

$(SRCDIR)%.o: %c
	$(CC) -c $@ $^ $< $(CFLAGS) 

default: RSAChat

RSAChat: $(OBJS) $(SRCDIR)/RSAChat.o
	$(CC) -o RSAChat $(SRCDIR)/RSAChat.o $(OBJS) $(LDFLAGS)

example: $(OBJS) $(SRCDIR)/example.o
	$(CC) -o example $(SRCDIR)/example.o $(OBJS) $(LDFLAGS)

clean:
	$(RM) RSAChat Example $(SRCDIR)/*.o 
