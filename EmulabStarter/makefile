CC=g++
CFLAGS=-lpthread
VPATH=starter:CommUtil:MVCTP
OBJ=EmulabStarter.o ConfigParser.o Logger.o mvctp.o Tester.o MVCTPComm.o MVCTPBuffer.o \
MulticastComm.o ReceiverCommExecClient.o SenderCommandClient.o StatusMonitor.o \
CommandExecClient.o StatusReportClient.o MVCTPManager.o RawSocketComm.o  NetInterfaceManager.o


all: emustarter

emustarter: $(OBJ)
	$(CC) -o $@ $(OBJ) -lpthread

EmulabStarter.o: ConfigParser.o Logger.o Tester.o

ConfigParser.o Logger.o: 

Tester.o: mvctp.o MulticastComm.o MVCTPComm.o ReceiverCommExecClient.o SenderCommandClient.o StatusMonitor.o CommandExecClient.o StatusReportClient.o MVCTPManager.o RawSocketComm.o NetInterfaceManager.o

SenderCommandClient.o ReceiverCommExecClient.o : CommandExecClient.o MVCTPComm.o

MVCTPComm.o: mvctp.o MulticastComm.o MVCTPManager.o MVCTPBuffer.o

MVCTPBuffer.o: mvctp.o MulticastComm.o

MulticastComm.o: mvctp.o  

MVCTPManager.o: mvctp.o RawSocketComm.o NetInterfaceManager.o 

RawSocketComm.o NetInterfaceManager.o: mvctp.o

mvctp.o:

StatusMonitor.o: CommandExecClient.o StatusReportClient.o

CommandExecClient.o StatusReportClient.o:


# Clean Action
clean:
	rm *.o


