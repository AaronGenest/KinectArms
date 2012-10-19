#ifndef NETWORK_H
#define NETWORK_H

#pragma comment(lib, "Ws2_32.lib")
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define DEFAULT_BUFLEN 32000
#define DEFAULT_PORT "27018"
#define CLIENT_PORT 27018
#define HEADERLENGTH 9
#define NAMELENGTH 4
#define SIZELENGTH 4
#define MAXPACKETLENGTH 10000


/*Packet Header:
*1 byte contrl code
*4 bytes sender/origin name(server/table name for identifying data and placing in buffers)
*4 bytes message length (data, unsigned long -> bytes)
*/

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iphlpapi.h>

#define CLOSEBUFFERLENGTH 50
#define MAXCONTROLCODE 10
#define RESENDWAITTIME 5//this is in seconds?
namespace NetworkManager
{
	enum ControlCodes{
		//sending 1 is data, everything above 0 is a message
		Subscribe = 5,
		Unsubscribe = 6,
		AckSubscribe = 7,
		SubscriptionDecline = 8,
		RequestServers = 9,
		AckRequestServers = 10,
		Data = 1,
		FirstFramePacket = 2,
		LastFramePacket = 3,
		DataAppend = 4,
	};

	struct Packet{
		char* name;
		char controlcode;
		char* data;
		unsigned long messagelength;
	};

	struct ServerData
	{//contains information on other servers / tables
		char* ServerName;
		bool subscribed;
		bool cansubscribed;
		bool connected;
		bool fullframereceived;
		bool startframe;
		unsigned long DataBufferLength;
		unsigned long DataBufferOffset;
		/*unsigned long DataBufferLengthOne;
		unsigned long DataBufferOffsetOne;
		unsigned long DataBufferLengthTwo;
		unsigned long DataBufferOffsetTwo;*/
		char* DataBuffer;
		//char* DataBufferOne;//this data buffer will contain frame information from a server
		//char* DataBufferTwo;//this data buffer will contain frame information from a server, these 2 buffers are for double bufering so that when you receive a entire frame you can play with the data while new data is read into the other frame
		//char* CurrentDataBuffer;//points to a entire frame
		//bool framereceived;

		char* BackupDataBuffer;//backupsize is headerlength + maxpacketsize, 
		//so that when you receive part of a packet at the end of your buffer, you place it in here, 
		//then on next receive you parse the remaining data combined with this buffer into a packet, then continue on.
		bool BackupUsed;
		unsigned long BackupOffset;
		unsigned long BackupPacketLength;

		Packet** PacketQueue;
		unsigned int packetqueuesize;
		static const unsigned int packetqueueincreaseinterval =5;
		unsigned int numpacketsinqueue;
	};

	//Setup functions
	int connect(const char* serverName);
	int connectiontest(int argc, char** argv);
	bool InitWinsock(void);//Initialize WinSock
	bool setupAddressInfo(struct addrinfo**, int,char*); // setup addrinfo struct for clients
	bool createClientSocket(struct addrinfo* info, SOCKET* Socket); //creates a client socket and attempts to connect it with the addrinfo supplied (which is for connecting to a server)
	bool createDefaultSocket(SOCKET* Socket);
	void SetSocketOptions(SOCKET* sock, unsigned long synch);

	void IncreaseServerDataList(void);


	bool initializeConnection(char*);//initializes the entire connection to setup a socket that can be used to send data to the server.
	void socketCleanup(void);// closes the socket and deinit winsock
	bool CloseConnection(void); // closes the socket when finished sending
	void freeServerDataList(void);
	void freeServerData(ServerData* );


	void GracefulShutdown(void);

	//Running functions
	int Send(char*, int ); //sends an array of data, returns the number of bytes sent.
	int ReceiveData(char*, int );
	bool ParseReceivedData(char* data, unsigned long datalength);
	bool ProcessDataFromServer(Packet* p);
	bool ProcessPacket(Packet* p);
	void ParseSavedPackets(void);
	bool SavePacket(Packet* p);


	int SendData(char*, unsigned long ); //sends an array with a data message (the server will interpret this as data).
	int SendSubscribe(char*,unsigned long);//sends a subscription message to your server indicating a desire to get data from table/server named in the arguments (and its length)
	int SendServerListRequest();
	bool ProcessSubscriptionDecline(Packet* );
	bool ProcessAckSubscription(Packet* );
	bool ProcessAckRequestServers(Packet* );
	void ResetServerFrameReceiving(void);

	ServerData* FindServerFromPacket(Packet* p);
	void ProcessServerDataBuffer(ServerData* sdata);
	void PlacePacketIntoServerQueue(ServerData* sdata, Packet* p);

	bool SendAndReceiveFrame(char* framedata, unsigned long framelength);
	void ProcessDataAppendPacket(Packet* );
	void ProcessLastFramePacket(Packet* );
	void ProcessFirstFramePacket(Packet* );

	int BreakupandSendData(char*, unsigned long);
	Packet* createPacket(char, char*,unsigned long);
	Packet* AllocatePacket(unsigned long datalength);
	Packet* AllocatePacket(void);
	Packet* ParsePacket(char* tempdata, unsigned long* length);
	Packet** BreakupDataIntoPackets(char* data, unsigned long length, unsigned long &numpackets);
	char** BreakupPacketsIntoByteArrays(Packet** , unsigned long );
	int SendFrame(char** packetsasbytes, Packet** packetlist, unsigned long length);
	char* convertPackettoArray(Packet* p);
	void destroyPacket(Packet*);
	void destroyPacketRepresentations(char** packetsasbytes, Packet** packetlist, unsigned long length);

	int SendPacket(Packet*);
	int SendFirstFramePacket(unsigned long );

	//HELPER
	char* ParseLongToByteArray(unsigned long);
	unsigned long ParseByteArrayToLong(char* );//the length should be of sizelength
	bool IsAlphabetic(char character);
	bool IsControlCode(char character);
}


#endif