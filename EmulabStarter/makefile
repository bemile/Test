CC=g++
CFLAGS=-lpthread
VPATH=starter:CommUtil:MVCTP
OBJ=EmulabStarter.o ConfigInfo.o Logger.o mvctp.o Tester.o MVCTPComm.o \
SendBufferMgr.o ReceiveBufferMgr.o MVCTPBuffer.o MVCTPSender.o MVCTPReceiver.o \
MulticastComm.o RawSocketComm.o InetComm.o UdpComm.o NetInterfaceManager.o \
ReceiverCommExecClient.o SenderCommandClient.o StatusMonitor.o CommandExecClient.o StatusReportClient.o



all: emustarter

emustarter: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

EmulabStarter.o: ConfigInfo.o Logger.o Tester.o

ConfigInfo.o Logger.o: 

Tester.o: mvctp.o MulticastComm.o RawSocketComm.o InetComm.o MVCTPComm.o SendBufferMgr.o ReceiveBufferMgr.o MVCTPBuffer.o MVCTPSender.o MVCTPReceiver.o UdpComm.o ReceiverCommExecClient.o SenderCommandClient.o StatusMonitor.o CommandExecClient.o StatusReportClient.o NetInterfaceManager.o

SenderCommandClient.o : CommandExecClient.o MVCTPSender.o MVCTPComm.o

ReceiverCommExecClient.o : CommandExecClient.o MVCTPReceiver.o MVCTPComm.o

MVCTPSender.o: mvctp.o MVCTPComm.o SendBufferMgr.o 

MVCTPReceiver.o: mvctp.o MVCTPComm.o ReceiveBufferMgr.o 

MVCTPComm.o: mvctp.o MulticastComm.o RawSocketComm.o NetInterfaceManager.o

SendBufferMgr.o ReceiveBufferMgr.o: mvctp.o MVCTPBuffer.o UdpComm.o InetComm.o

MVCTPBuffer.o: mvctp.o

MulticastComm.o RawSocketComm.o: mvctp.o InetComm.o 

InetComm.o : mvctp.o

UdpComm.o NetInterfaceManager.o: mvctp.o

mvctp.o:

StatusMonitor.o: CommandExecClient.o StatusReportClient.o

CommandExecClient.o StatusReportClient.o:


# Clean Action
clean:
	rm *.o


