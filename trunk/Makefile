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
LDFLAGS = -lsqlite3 -lpthread 

OBJS = $(SRCDIR)/RSAChat.o $(SRCDIR)/args.o $(SRCDIR)/server.o $(SRCDIR)/client.o \
	   $(SRCDIR)/db.o 

$(SRCDIR)%.o: %c
	$(CC) -c $@ $^ $< $(CFLAGS) 

default: RSAChat

RSAChat: $(OBJS)
	$(CC) -o RSAChat $(OBJS) $(LDFLAGS)

clean:
	$(RM) RSAChat $(SRCDIR)/*.o 
