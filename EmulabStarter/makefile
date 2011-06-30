CC=g++
CFLAGS=-lpthread
VPATH=starter:CommUtil:MVCTP
OBJ=EmulabStarter.o ConfigParser.o Logger.o Tester.o MulticastComm.o \
MVCReceiver.o MVCSender.o ReceiverCommExecClient.o SenderCommandClient.o \
StatusMonitor.o CommandExecClient.o StatusReportClient.o MVCTPManager.o RawSocketComm.o 

all: emustarter

emustarter: $(OBJ)
	$(CC) -o $@ $(OBJ) -lpthread

EmulabStarter.o: ConfigParser.o Logger.o Tester.o

ConfigParser.o Logger.o: 

Tester.o: MulticastComm.o MVCReceiver.o MVCSender.o ReceiverCommExecClient.o SenderCommandClient.o StatusMonitor.o CommandExecClient.o StatusReportClient.o MVCTPManager.o RawSocketComm.o

MulticastComm.o :

MVCSender.o: SenderCommandClient.o

MVCReceiver.o : ReceiverCommExecClient.o

SenderCommandClient.o ReceiverCommExecClient.o : CommandExecClient.o

StatusMonitor.o: CommandExecClient.o StatusReportClient.o

CommandExecClient.o StatusReportClient.o:

MVCTPManager.o: RawSocketComm.o

RawSocketComm.o:

clean:
	rm *.o


