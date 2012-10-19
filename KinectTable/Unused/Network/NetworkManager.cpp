#include "NetworkManager.h"
#include <time.h>


namespace NetworkManager
{
	//measure fps from table
	static long framecount = 0;
	static time_t lastcalltime= time(NULL);
	static time_t currenttime;
	static float waittime = 1.0;// in seconds


	static SOCKET ConnectSocket;
	static sockaddr ServerAddressInfo;
	static int ClientPort = CLIENT_PORT;
	static addrinfo addressinfo;
	const char CLIENT_NAME[] = "uoca";
	static ServerData** ServerDataList;
	static char* BackupDataBuffer;//backupsize is headerlength + maxpacketsize, 
		//so that when you receive part of a packet at the end of your buffer, you place it in here, 
		//then on next receive you parse the remaining data combined with this buffer into a packet, then continue on.
	static bool BackupUsed;
	static unsigned long BackupOffset;
	static unsigned long BackupPacketLength;
	static int NumServers=0;

	//HELPER GENERIC
	bool createDefaultSocket(SOCKET* Socket)
	{
		if(*Socket != 0)
			*Socket = INVALID_SOCKET;
		*Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		if (*Socket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			return false;
		}

		return true;
	}
	void SetSocketOptions(SOCKET* sock, unsigned long synch)
	{
		//Set to a-synchronous
		ioctlsocket(*sock,FIONBIO,&synch);
		/*int optval = 1;
		setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR,(char*) &optval, sizeof optval);*/
	}
	
	//SETUP
	bool setupAddressInfo(struct addrinfo **result,int port ,char* address)
	{
		int iResult;
		struct addrinfo hints;

		ZeroMemory(&hints, sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		
		//Convert numeric port to string (for port/service)
		char buffer[6];
		_itoa_s(port,buffer, 10);

		//Resolve the local address and port to be used by the server
		iResult = getaddrinfo(address, buffer, &hints, result);
		if (iResult != 0) {
			printf("getaddrinfo failed: %d\n", iResult);
			return false;
		}

	//	// Resolve the local address and port to be used by the server

	//	iResult = getaddrinfo(serverip, DEFAULT_PORT, &hints, result);
	//	if (iResult != 0) {
	//		printf("getaddrinfo failed: %d\n", iResult);
	//		return false;
	//	}

	//	char ipstr[INET6_ADDRSTRLEN];

	//	struct addrinfo *p;
	//	printf("IP addresses for %s:\n\n", serverip);

	//for(p = *result;p != NULL; p = p->ai_next) {
	//	void *addr;
	//	char *ipver;

	//	// get the pointer to the address itself,
	//	// different fields in IPv4 and IPv6:
	//	if (p->ai_family == AF_INET) { // IPv4
	//		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
	//		addr = &(ipv4->sin_addr);
	//		ipver = "IPv4";
	//	} else { // IPv6
	//		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
	//		addr = &(ipv6->sin6_addr);
	//		ipver = "IPv6";
	//	}

	//	// convert the IP to a string and print it:
	//	inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
	//	printf("  %s: %s\n", ipver, ipstr);
	//}




		return true;
	}
	bool createClientSocket(struct addrinfo* info, SOCKET* Socket)
	{
		int iResult = 0;
		struct addrinfo *ptr = NULL;
		for(ptr=info; ptr != NULL ;ptr=ptr->ai_next){
			//Create a SOCKET for connecting to server
			*Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (*Socket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				return false;
			}
	
			//sockaddr_in NewRemoteSin;
			
			// Connect to server.
			iResult = connect( *Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	
			if (iResult == SOCKET_ERROR) {
				closesocket(*Socket);
				printf("Couldnt connect, finding another address\n");
				*Socket = INVALID_SOCKET;
				continue;
			}

			//inet_ntop(ptr->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s)

			char* servip =  inet_ntoa(((struct sockaddr_in*) ptr->ai_addr)->sin_addr);
	
			printf("Connected to ip: %s!\n", servip);

			break;
		}
		return true;

	}
	bool InitWinsock(void)
	{
		WSADATA wsaData;
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
			return false;
		}

		return true;
	}
	bool initializeConnection(char* servername)
	{
		NetworkManager::ConnectSocket = INVALID_SOCKET;
		struct addrinfo *result = NULL;
		// Initialize Winsock
		if(!NetworkManager::InitWinsock())
		{
		  return false;
		}

		if(!NetworkManager::setupAddressInfo(&result, CLIENT_PORT,servername))
		{
			WSACleanup();
			return false;
		}

		if(!NetworkManager::createClientSocket(result, &ConnectSocket))
		{
			WSACleanup();
			freeaddrinfo(result);
			return false;
		}

		unsigned long synch = 1;
		SetSocketOptions(&NetworkManager::ConnectSocket, synch);

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			WSACleanup();
			return false;
		}

		NumServers =0;
		ServerDataList = NULL;
		return true;
	}

	void setupServerData(ServerData* sdata)
	{
		sdata->BackupOffset =0;
		sdata->BackupUsed = false;
		sdata->BackupPacketLength = 0;
		sdata->cansubscribed = true;
		sdata->connected = false;
		sdata->DataBufferLength =0;
		sdata->DataBufferOffset =0;
		sdata->fullframereceived = false;
		sdata->startframe = false;
		sdata->subscribed = false;
		sdata->BackupDataBuffer = NULL;
		sdata->DataBuffer = NULL;

		/*sdata->DataBufferOne = NULL;
		sdata->DataBufferTwo = NULL;
		sdata->CurrentDataBuffer = NULL;*/

		sdata->ServerName = NULL;
		sdata->PacketQueue = NULL;
	}
	void IncreaseServerDataList(void)
	{
		if(NumServers ==0)
		{
			//List is empty
			NumServers = 1;
			ServerDataList = (ServerData**)malloc(sizeof(ServerData*));
			ServerDataList[0] = (ServerData*)malloc(sizeof(ServerData));
			setupServerData(ServerDataList[0]);
			return;
		}

		NumServers++;
		ServerData** templist = (ServerData**)malloc(sizeof(ServerData) * NumServers);
		int i =0;
		for(i =0;i < NumServers-1;i++)
		{
			//copy points to old list, allocate area for new server data object
			templist[i] = ServerDataList[i];
		}
		templist[NumServers-1] = (ServerData*)malloc(sizeof(ServerData));
		setupServerData(templist[NumServers-1]);
		free(ServerDataList);
		ServerDataList = templist;
	}

	//RUNNING
	int SendSubscribe(char* tablename,unsigned long length)
	{
		int iResult = 0;
		//creates a packet with the controlcode of subscribe, and the table name in the data field (sets the data field length to the length of the name)
		Packet* packet = createPacket(Subscribe, tablename, length);
		iResult = SendPacket(packet);
		destroyPacket(packet);
		free(packet);
		return iResult;
	}
	int SendServerListRequest()
	{
		int iResult = 0;
		//creates a packet with the controlcode of subscribe, and the table name in the data field (sets the data field length to the length of the name)
		Packet* packet = createPacket(RequestServers, 0, 0);
		iResult = SendPacket(packet);
		destroyPacket(packet);
		free(packet);
		return iResult;
	}
	int SendData(char* data, unsigned long length)
	{
		int iResult = 0;
		if(length > MAXPACKETLENGTH)
		{
			//printf("Packets broken up!\n");
			iResult = BreakupandSendData(data,length);
		}else
		{
			Packet* packet = createPacket(Data, data, length);
			iResult = SendPacket(packet);
			printf("%d bytes sent in one packet!\n", iResult);
			destroyPacket(packet);
			free(packet);
		}
		if(iResult == -1)
		{
			printf("Send error!\n");
		}
		
		return iResult;
	}

	//running system functions (not to be called by api users)
	Packet* AllocatePacket(unsigned long datalength)
	{
		Packet* p = (Packet*)malloc(sizeof(Packet));
		if(p == NULL)
			return NULL;
		p->name = (char*)malloc(NAMELENGTH + 1);
		if(p->name == NULL)
		{
			free(p);
			return NULL;
		}

		if(datalength > 0)
		{
			p->messagelength = datalength;
			p->data = (char*)malloc(p->messagelength);
			if(p->data == NULL)
			{
				free(p->name);
				free(p);
				return NULL;
			}
		}else
		{
			p->messagelength = 0;
			p->data = NULL;
		}
		
		return p;
	}
	Packet* AllocatePacket(void)
	{
		Packet* p = (Packet*)malloc(sizeof(Packet));
		if(p == NULL)
			return NULL;
		p->name = (char*)malloc(NAMELENGTH + 1);
		if(p->name == NULL)
		{
			free(p);
			return NULL;
		}
		return p;
	}
	Packet* createPacket(char controlcode, char* data,unsigned long datalength)
	{
		Packet* p = AllocatePacket(datalength);
		if(p == NULL)
			return NULL;
		p->controlcode = controlcode;

		unsigned int i = 0;
		for(i = 0; i < datalength;i++)
		{
			p->data[i] = data[i];
		}
		
		p->messagelength = datalength;
		for(i = 0;i<NAMELENGTH;i++)
		{
			p->name[i] = CLIENT_NAME[i];
		}
		p->name[NAMELENGTH] = '\0';

		return p;
	}
	Packet* createPacket(char controlcode, char* data,unsigned long datalength, char* name)
	{
		Packet* p = AllocatePacket(datalength);
		if(p == NULL)
			return NULL;
		p->controlcode = controlcode;

		//copy data
		unsigned int i =0;
		for(i = 0; i < datalength;i++)
		{
			p->data[i] = data[i];
		}
		for(i = 0;i < NAMELENGTH;i++)
		{
			p->name[i] = name[i];
		}
		p->name[NAMELENGTH] = '\0';

		return p;
	}
	Packet* ParsePacket(char* tempdata, unsigned long* length)
	{
		unsigned long messageoffset = 0;
		Packet* p = AllocatePacket();
		if(p == NULL)
		{	//Memory allocation failed
			printf("Memory allocation failed!\n");
			return NULL;
		}

		p->controlcode = tempdata[messageoffset];
		messageoffset++;

		unsigned long i = 0;
		for(i = 0; i < NAMELENGTH;i++)
		{
			p->name[i] = tempdata[messageoffset + i];
		}
		p->name[NAMELENGTH] = '\0';

		messageoffset += NAMELENGTH;
		p->messagelength = ParseByteArrayToLong(tempdata + messageoffset);//Only looks at the first SIZELENGTH number of bytes for parsing the message size
		messageoffset += SIZELENGTH;

		//messageoffset + tempdata is now pointing at a data section
		p->data = (char*)malloc(p->messagelength);
		
		for( i = 0 ; i < p->messagelength;i++)
		{
			p->data[i] = tempdata[messageoffset];
			messageoffset++;
		}

		*length = messageoffset;
		return p;
	}
	int SendFirstFramePacket(unsigned long size)
	{
		//sends a packet saying that the next size bytes of data are to be appended to each other to receive an entire frame
		static unsigned long byteindex =0;//for sending partial packets
		static unsigned long bytessent =0;
		static char* firstframe;
		unsigned long bytestosend = 0;

		if(byteindex == 0)
		{
			//first attempt
			char* sizedata = ParseLongToByteArray(size);
			Packet* p = createPacket(FirstFramePacket, sizedata, SIZELENGTH);
			firstframe = convertPackettoArray(p);
			free(sizedata);
			destroyPacket(p);
			free(p);
		}

		bytessent = Send(firstframe + byteindex, HEADERLENGTH + SIZELENGTH - byteindex);//its a header with SIZELENGTH bytes of data (to be parsed later to determine how much they are sending)
		if(bytessent < 0)
			return bytessent;
		if(bytessent == HEADERLENGTH + SIZELENGTH)
		{
			//sent the entire packet!
			byteindex = 0;
			bytessent = 0;
			free(firstframe);
			return 1;
		}else
		{
			//didnt send the entire packet!
			byteindex += bytessent;
			return 0;
		}

		return false;
	}
	Packet** BreakupDataIntoPackets(char* data, unsigned long length, unsigned long &numpackets)
	{
		//Breaks up an array into n packets and returns a pointer to an array of packets to be dealt with.
		numpackets = length / MAXPACKETLENGTH + 1;
		Packet** packetlist = (Packet**)malloc(sizeof(Packet*) * numpackets);
		char* newdata = data;
		unsigned char controlcode = DataAppend;
		unsigned long packetsize =0;
		unsigned long packetcount =0;

	
		while(packetcount != numpackets)
		{
			if(packetcount == numpackets - 1)
			{
				//Last packet being sent
				controlcode = LastFramePacket;
				packetsize = length % MAXPACKETLENGTH;
			}
			else
			{	//maxlength packet to send
				controlcode = DataAppend;
				packetsize = MAXPACKETLENGTH;
			}
			//place a new packet into the list
			
			packetlist[packetcount] = createPacket(controlcode, newdata, packetsize);
			newdata += packetsize;
			packetcount++;
		}

		return packetlist;
	}
	char** BreakupPacketsIntoByteArrays(Packet** packetlist, unsigned long numpackets)
	{
		char** ByteArrayList = (char**)malloc(sizeof(char*) * numpackets);
		if(ByteArrayList == NULL)
			return NULL;

		unsigned long i =0;
		for(i=0;i < numpackets;i++)
		{
			ByteArrayList[i] = convertPackettoArray(packetlist[i]);
		}
		
		return ByteArrayList;
	}

	int SendFrame(char** packetsasbytes, Packet** packetlist, unsigned long length)
	{//this function is to be called in a unblocking environment, over and over until an entire list of packets has been sent (pass the same 3 parameters everytime!)
		static unsigned long packetindex =0;//for knowing which packets have been sent
		static unsigned long byteindex =0;//for sending partial packets
		static unsigned long bytessent =0;
		unsigned long bytestosend = 0;

		while(1)
		{//try to send all packets
			if(packetindex == length)
			{
				//we have now sent the entire list of packets
				packetindex = 0;
				byteindex = 0;
				bytessent = 0;
				return 1;
			}
			//continue to send stuff
			bytestosend = packetlist[packetindex]->messagelength + HEADERLENGTH - byteindex;
			bytessent = Send(packetsasbytes[packetindex] + byteindex, bytestosend);
			if(bytessent == -1)
			{
				//SEND ERROR!
				return -1;
			}
			if(bytessent != bytestosend)
			{
				//not everything was sent, prepare statics to send the rest next call(you need to pass the same variables)
				byteindex += bytessent;
				return 0;
			}else
			{
				//this entire packet was sent
				byteindex = 0;
				packetindex++;
			}
		}

		return 0;
	}
	int BreakupandSendData(char* data, unsigned long length)
	{
		int numberchunks = length / MAXPACKETLENGTH;
		Packet* newpacket;
		char* newdata;
		unsigned long newsize;
		int iResult = 0;
		unsigned char controlcode = Data;
		unsigned long sendcount = 0;

		if(!SendFirstFramePacket(length))
		{
			//something failed
			printf("sending first frame failed\n");
			return -1;
		}

		while(sendcount < length)
		{
			//while you havent sent the entire thing, break up the data into chunks and send them individually.
			if(length - sendcount > MAXPACKETLENGTH)
			{
				//there is enough data to pack into one max size packet
				newsize = MAXPACKETLENGTH;
				controlcode = DataAppend;//all data is to now be appeneded to the previously created array for containing all this data
			}else
			{
				//pack the remaining data into a smaller packet
				newsize = length - sendcount;

				controlcode = LastFramePacket;//the last packet dictates the end of data receiving for a single frame
			}

			newdata = data+sendcount;

			newpacket = createPacket(controlcode, newdata, newsize);
			iResult = SendPacket(newpacket);
			if(iResult == -1)
			{
				//Send error!
				printf("send packet error\n");
				destroyPacket(newpacket);
				free(newpacket);
				return -1;
			}else
			{
				if(iResult == 0)
				{
					printf("no bytes sent...\n");
				}
				if(iResult == HEADERLENGTH + newsize)
				{
					sendcount += newsize;				
				}
				else if(iResult > 9)
				{
					sendcount += (iResult - HEADERLENGTH);				
				}
			}
			destroyPacket(newpacket);
			free(newpacket);
		}


		return sendcount;
	}
	char* convertPackettoArray(Packet* p)
	{
		char* packet = (char*)malloc(p->messagelength + HEADERLENGTH);
		if(packet == NULL)
			return NULL;
		packet[0] = p->controlcode;
	
		packet[1] = p->name[0];
		packet[2] = p->name[1];
		packet[3] = p->name[2];
		packet[4] = p->name[3];
		
		char* bytearray = ParseLongToByteArray(p->messagelength);
		int i =0;
		for(i=0;i < SIZELENGTH;i++)
		{
			packet[5 + i] = bytearray[i];
		}
		free(bytearray);

		/*packet[5] = (int)((p->messagelength >> 24) & 0xFF);
		packet[6] = (int)((p->messagelength >> 16) & 0xFF);
		packet[7] = (int)((p->messagelength >> 8) & 0XFF);
		packet[8] = (int)((p->messagelength & 0XFF));*/

		//copy data into the messate data area
		for(unsigned int i = 0 ; i < p->messagelength;i++)
		{
			packet[i + HEADERLENGTH] = p->data[i];
		}
		return packet;
	}
	int SendPacket(Packet* p)
	{
		char* packet = convertPackettoArray(p);

		int iResult = Send(packet, HEADERLENGTH + p->messagelength);
		free(packet);
		if(iResult == -1)
		{
			//send packet error
			return -1;
		}

		return iResult;//returns number of bytes sent
	}
	void destroyPacketRepresentations(char** packetsasbytes, Packet** packetlist, unsigned long length)
	{
		unsigned long i =0;
		if(packetsasbytes!= NULL)
		{
			for(i=0;i<length;i++)
			{
				if(packetsasbytes[i] != NULL)
					free(packetsasbytes[i]);
			}
		}

		if(packetlist != NULL)
		{
			for(i=0;i<length;i++)
			{
				if(packetlist[i] != NULL)
					destroyPacket(packetlist[i]);
			}
		}
	}
	void destroyPacket(Packet* p )
	{
		if(p == NULL)
			return;
		if(p->data != NULL)
			free(p->data);
		if(p->name != NULL)
			free(p->name);
	}
	int Send(char* Input, int length)
	{
		int sendResult =0;
		sendResult = send(ConnectSocket, Input, length, 0);
		if (sendResult == SOCKET_ERROR) {
			int error = WSAGetLastError();
	
			if(error == WSAECONNRESET)
			{
				printf("Connection Reset\n");
				CloseConnection();
				return -1;
			}else if(error == WSAEWOULDBLOCK)
			{
				return 0;
			}
			printf("send failed with error: %d\n", error);
			return -1;
		}

		if(sendResult == 0){
			printf("nothing sent...\n");
			return 0;
		}

		//printf("Bytes Sent: %ld\n", sendResult);
		return sendResult;
	}
	bool SendAndReceiveFrame(char* framedata, unsigned long framelength)
	{
		ResetServerFrameReceiving();//set all frames to not received
		time_t sendtime = time(NULL);
		time_t currenttime= time(NULL);
		double waittime =0;
		bool sentandreceived = false;
		bool sent = false;
		int receivedcount = 0;
		bool* counted = (bool*)malloc(sizeof(bool) * NumServers);
		unsigned long numpackets = 0;
		int framesent = 0;
		Packet** packets = BreakupDataIntoPackets(framedata, framelength, numpackets);
		char** packetsasbytes = BreakupPacketsIntoByteArrays(packets,numpackets);
		bool firstframesent = false;

		ServerData* server;
		int i =0;
		for(i=0;i<NumServers;i++)
			counted[i] = false;

		char* output = (char*)malloc(DEFAULT_BUFLEN);
		while(!sentandreceived)
		{
			currenttime = time(NULL);
			if(!firstframesent)
			{
				//try to send first frame
				int firstframecheck = SendFirstFramePacket(framelength);
				if(firstframecheck == 0)
					firstframesent = false;
				else if(firstframecheck == 1)
					firstframesent = true;
				else
				{
					firstframecheck = false;
					//check for send errors!
					printf("Error sending first frame packet!\n");
					return false;
				}
			}

			if(!sent && firstframesent)
			{
				framesent = SendFrame(packetsasbytes, packets, numpackets);
				if(framesent == -1)
				{
					//send problems!?
					sent = false;
				}else if(framesent == 0)
					sent= false;
				else if(framesent == 1)
					sent = true;

				if(sent)
					sendtime = time(NULL);
			}
			

			waittime = difftime(currenttime, sendtime);
			if(waittime > RESENDWAITTIME)
			{
				printf("resetting send clock!\n");
				//dont reset received stuff as via tcp it will eventually get here?
				sent = false;
				firstframesent = false;
				sendtime = time(NULL);
			}
			
			//check if we have received a fullframe from every table we are subscribed to
			for(i=0;i < NumServers;i++)
			{
				server = ServerDataList[i];
				if(!counted[i] && server->fullframereceived)
				{
					receivedcount++;
					counted[i] = true;				
				}
			}
			
			int datalen = ReceiveData(output, DEFAULT_BUFLEN);
			if(datalen > 0)
				ParseReceivedData(output, datalen);
			if(receivedcount == NumServers && sent)
			{
				sentandreceived = true;
			}
		}

		//done with the packet list
		destroyPacketRepresentations(packetsasbytes, packets,numpackets);
		free(output);
		return true;
	}
	int ReceiveData(char* Output, int length)
	{
		int receiveResult = recv(ConnectSocket, Output, length, 0);
		if (receiveResult == SOCKET_ERROR) {
			int error = WSAGetLastError();
			if(error == WSAEWOULDBLOCK)
			{
				return 0;
			}else if(error == WSAECONNRESET)
			{
				socketCleanup();
				printf("connection to table lost...\n");
			}
			printf("Receive failed with error: %d\n", error);
			char h[5];
			gets(h);
			return -1;
		}
		return receiveResult;
	}

	int FindNextPacketInMemory(char* data, unsigned long &messageoffset, unsigned long messagelength, unsigned long &packetlength)
	{
		//messageoffset is where we are, and we will place where the begining of a packet is back into there.
		//things to look for to specify the begning of a packet:
		//a control code a code from 1 to MAXCONTROLCODE
		//4 bytes that are alphabetic
		//4 byets that when parsed are less then MAXPACKETLENGTH, to be placed into the packetlength
		
		while(messageoffset < messagelength)
		{
			//Check the case in which there is less then 9 bytes left, ie a partial packet might exist
			if(messagelength - messageoffset < HEADERLENGTH)
			{
				//Possible partial packet
				//check the first 5 bytes for controlcode / name header
				int i =0;
				for(i =0;i < HEADERLENGTH - 1 - NAMELENGTH;i++)
				{
					if(IsControlCode(data[messageoffset]))
					{
						int j = 1;
						bool foundpacket = true;
						for(j=1;j<NAMELENGTH+1;j++)
						{
							if(!IsAlphabetic(data[messageoffset + j]))
							{
								foundpacket = false;
								break;
							}
						}

						if(foundpacket)
							return 0;//a possible header has been found at messageoffset containing a name and a control code
					}

					messageoffset++;

				}

				//now search for just a control code in the last 4 bytes
				for(i=0;i < HEADERLENGTH - 1 - NAMELENGTH;i++)
				{
					if(IsControlCode(data[messageoffset]))
					{
						//possible small header left, only control code
						return 0;

					}else
						messageoffset++;
				}

				return -1;//nothing found, messageoffset is at the end of the list
			}else
				
			if(IsControlCode(data[messageoffset]))
			{
				int i = 1;
				bool possibleheader = true;
				for(i=1;i<NAMELENGTH+1;i++)
				{
					if(!IsAlphabetic(data[messageoffset + i]))
					{
						possibleheader = false;
					}
				}

				if(possibleheader == true)
				{
					//check if bytes 5-9 can be parsed into a long that makes sense:
					unsigned long possiblelength = ParseByteArrayToLong(data + messageoffset + 5);
					if(possiblelength <= MAXPACKETLENGTH)
					{
						//Found a packet(most likely)!
						packetlength = possiblelength;
						return 1;//data[messageoffset] will contain a packet
					}
				}
			}

			messageoffset++;
		}
		return -1;
	}

	void ParseSavedPackets(void)
	{
		//go through each servers packetqueue and process:
		int i =0;
		ServerData* server;
		for(i=0;i<NumServers;i++)
		{
			server = ServerDataList[i];
			if(server->numpacketsinqueue > 0)
			{
				unsigned int j =0;
				for(j =0;j <server->numpacketsinqueue;j++)
				{
					ProcessPacket(server->PacketQueue[j]);
				}
				server->numpacketsinqueue =0;
			}
		}
	}

	bool ParseReceivedData(char* data, unsigned long datalength)
	{
		//parse and process messages that have been saved:
		ParseSavedPackets();

		unsigned long messageoffset = 0;
		char* tempdata = data;

		if(BackupUsed)
		{
			//printf("using backup!\n");
			if(BackupOffset < HEADERLENGTH)
			{
				//The partial data contains part of a header, read in enough data to make a complete header, then continue copying more data based on the packet length
				unsigned int remainingheaderdata = HEADERLENGTH - BackupOffset;
				unsigned int i=0;
				//read in the first few bytes to make a compete header
				for(i=0;i<remainingheaderdata;i++)
				{
					BackupDataBuffer[BackupOffset++] = tempdata[messageoffset++];
				}

				BackupPacketLength = ParseByteArrayToLong(BackupDataBuffer + 1 + NAMELENGTH);
			}
			//now treat it the same as if we read in a bunch of data already
			while(BackupOffset < BackupPacketLength+ HEADERLENGTH)
			{
				BackupDataBuffer[BackupOffset++] = tempdata[messageoffset++];
			}

			//now parse the backup buffer's packet and reset it before dealing with future packets
			unsigned long packetlength =0;
			Packet* p = ParsePacket(BackupDataBuffer, &packetlength);
			if(p == NULL || packetlength != BackupOffset)
			{
				printf("read in packet oddly?\n");
				//??
				//return false;
			}
			ProcessPacket(p);
			//reset the backup buffer, so its data can be overwritten when more partial packets are received.
			BackupOffset =0;
			BackupUsed = false;
			BackupPacketLength =0;
			free(BackupDataBuffer);
		}

		
		while(messageoffset < datalength)
		{
			unsigned long packetlength =0;
			int returncode = FindNextPacketInMemory(tempdata, messageoffset, datalength, packetlength);
			//This is a pretty damn messy if statement block. it basically checks how much data is left in the buffer, 
			//if there is less then a header then it places the data into a backupbuffer, 
			//if the data is larger then a max size packet it will read it in normally, 
			//if the data is between thse values it checks the packet header for how much data should be in the data section, 
			//then determines if its just a small packet that can be parsed now or a partial packet to be looked at on the next receive.
			//data is placed in a backupbuffer that will be checked on the next receive and parsing call.
			unsigned long dataleftinbuffer = datalength - messageoffset;
			if(dataleftinbuffer > 0)
			{
				//still data to be read
				//check if there is a small packet still able to be read
				if(dataleftinbuffer < HEADERLENGTH)
				{
					//incomplete header! copy into databuffer
					BackupDataBuffer = (char*)malloc((HEADERLENGTH + MAXPACKETLENGTH ));//setup the array
					BackupUsed = true;
					BackupOffset = dataleftinbuffer;
					datalength = 0;
					//copy the small data into the buffer
					unsigned int i =0;
					for(i =0;i < dataleftinbuffer;i++)
					{
						BackupDataBuffer[i] = tempdata[i + messageoffset];
					}
					
				}else if(dataleftinbuffer < MAXPACKETLENGTH + HEADERLENGTH)
				{
					//perhaps a small packet left in buffer?
					//get the packet size:
					unsigned long remainingpacketlength = ParseByteArrayToLong(tempdata + messageoffset + 1 + NAMELENGTH);//the SIZELENGTH bytes are offset by 1(control code)+the length of the name(NAMELENGTH)
					if(remainingpacketlength + HEADERLENGTH > dataleftinbuffer)
					{

						//this packet is partial
						BackupDataBuffer = (char*)malloc((HEADERLENGTH + MAXPACKETLENGTH ));//setup the array
						BackupUsed = true;
						BackupOffset = dataleftinbuffer;
						datalength = 0;
						BackupPacketLength = remainingpacketlength;
						//copy all partial data to buffer
						unsigned int i =0;
						for(i =0;i < dataleftinbuffer;i++)
						{
							BackupDataBuffer[i] = tempdata[i+ messageoffset];
						}
					}else
					{
						//full small packet still in buffer
						unsigned long packetlength = 0;
						Packet* p = ParsePacket(tempdata+ messageoffset, &packetlength);
						if(p == NULL)
						{
							return false;
						}
						messageoffset += packetlength;
			
						ProcessPacket(p);
						continue;
					}
				}else
				{
					//full length packet left
					unsigned long packetlength = 0;
					Packet* p = ParsePacket(tempdata+ messageoffset, &packetlength);
					if(p == NULL)
					{
						return false;
					}
					messageoffset += packetlength;
					ProcessPacket(p);
					continue;
				}
			}

		}

		return true;
	}

	//Process messages /data from the server
	bool ProcessDataFromServer(Packet* p)
	{
		//Do something with the data from the server(which is determined from the name field of the packet)
		//print it for now:
		char name[NAMELENGTH+1];
		unsigned int i =0;
		for(i = 0 ; i < NAMELENGTH;i++)
		{
			name[i] = p->name[i];
		}
		name[NAMELENGTH] = '\0';

		//perceive data as a string for displaying...
		char* data = (char*)malloc(p->messagelength + 1);
		for(i =0;i < p->messagelength;i++)
		{
			data[i] = p->data[i];
		}
		data[p->messagelength] = '\0';
		printf("Data received from table: %s:\n%s\n", name, data);
		free(data);
		return true;
	}
	bool ProcessPacket(Packet* p)
	{
		bool destroypacket = true;
		bool freepacketptr = true;

		switch(p->controlcode)
		{
		case Data:
			ProcessDataFromServer(p);
			break;
		case FirstFramePacket:
			if(SavePacket(p))
			{
				destroypacket = false;
			}else
				ProcessFirstFramePacket(p);
			break;
		case LastFramePacket:
			if(SavePacket(p))
			{
				destroypacket = false;			
			}else
				ProcessLastFramePacket(p);
			break;
		case DataAppend:
			if(SavePacket(p))
			{
				destroypacket = false;			
			}else
				ProcessDataAppendPacket(p);
			break;
		case AckSubscribe:
			ProcessAckSubscription(p);
			break;
		case SubscriptionDecline:
			ProcessSubscriptionDecline(p);
			break;
		case AckRequestServers:
			ProcessAckRequestServers(p);
			break;
		default:
			return false;
		};

		if(destroypacket)
			destroyPacket(p);
		if(freepacketptr)
			free(p);

		return true;
	}
	bool ProcessSubscriptionDecline(Packet* p)
	{
		//couldnt subscribe to the server either due to connection issue or privacy issue

		return true;
	}
	bool ProcessAckSubscription(Packet* p)
	{
		//You are now subscribed to the server(with name in the packet), setup data buffers!
		printf("Subscribed! creating server information and lists\n");
		IncreaseServerDataList();//allocates space for a new connection
		//fillout the newest object with the new subscribed server informaiton

		//fill out name
		ServerData* sdata = ServerDataList[NumServers -1];
		sdata->ServerName = (char*)malloc(NAMELENGTH + 1);
		int i=0;
		for(i =0 ; i < NAMELENGTH;i++)
		{
			sdata->ServerName[i] = p->name[i];
		}
		sdata->ServerName[NAMELENGTH] = '\0';

		sdata->subscribed = true;
		sdata->cansubscribed = true;
		sdata->connected = true;
		sdata->BackupUsed = false;
		sdata->BackupOffset = 0;
		sdata->BackupPacketLength = 0;

		sdata->PacketQueue = (Packet**)malloc(sdata->packetqueueincreaseinterval * sizeof(Packet*));
		sdata->packetqueuesize = sdata->packetqueueincreaseinterval;
		sdata->numpacketsinqueue =0;

		return true;
	}
	bool ProcessAckRequestServers(Packet* p)
	{
		//the data filed of this packet contains a list of names that the server can connect and subscribe to.
		
		unsigned int numberservers = 1;// at least one in the list (otherwise return false)
		unsigned long i =0;
		if(p->messagelength < NAMELENGTH)
		{
			if(p->messagelength ==0)
				printf("No servers to connect to!\n");
			return false;		
		}
		for(i =0;i < p->messagelength;i++)
		{
			if(p->data[i] == ',')
			{
				numberservers++;
			}
		}

		char** namelist = (char**)malloc(sizeof(char*)*numberservers);
		unsigned long nameoffset = 0;
		unsigned long j =0;
		for(i =0;i < numberservers;i++)
		{
			namelist[i] = (char*)malloc(sizeof(char)*(NAMELENGTH + 1));
			for(j =0;j < NAMELENGTH;j++)
			{
				namelist[i][j] = p->data[nameoffset];
				nameoffset++;
			}
			namelist[i][NAMELENGTH] = '\0';
			nameoffset++;//to account for the comma seperating them
		}

		//REDO - do something with this list of names, print for now:
		for(i = 0;i < numberservers;i++)
		{
			printf("Server: %s\n", namelist[i]);
			//free(namelist[i]);
		}

		for(i =0;i < numberservers;i++)
		{
			free(namelist[i]);
		}

		free(namelist);
		return true;
	}
	ServerData* FindServerFromPacket(Packet* p)
	{
		ServerData* server;
		int i=0;
		for(i =0;i < NumServers;i++)
		{
			server = ServerDataList[i];
			if(strcmp(server->ServerName, p->name)==0)
			{
				//found the server sending us data
				return server;
			}
		}
		return NULL;
	}
	
	void PlacePacketIntoServerQueue(ServerData* sdata, Packet* p)
	{
		if(sdata->numpacketsinqueue == sdata->packetqueuesize)
		{
		//first increase this servers queue
			Packet** tempdata = (Packet**)malloc(sdata->packetqueuesize + sdata->packetqueueincreaseinterval);
			unsigned int i =0;
			for(i=0;i< sdata->packetqueuesize;i++)
			{
				//copy pointers
				tempdata[i] = sdata->PacketQueue[i];
			}

			free(sdata->PacketQueue);
			sdata->PacketQueue = tempdata;
			sdata->packetqueuesize += sdata->packetqueueincreaseinterval;
		}

		//place packet into next available pointer
		sdata->PacketQueue[sdata->numpacketsinqueue] = p;
		sdata->numpacketsinqueue++;
	}
	bool SavePacket(Packet* p)
	{
		//Determins if we should place this packet into a queue to be processed later
		ServerData* server = FindServerFromPacket(p);
		if(server == NULL)
			return false;
		if(server->fullframereceived)
		{
			//place this packet into the servers packet queue to be used later
			PlacePacketIntoServerQueue(server,p);
			return true;
		}

		return false;
	}
	void ResetServerFrameReceiving(void)
	{
		int i =0;
		for(i=0;i<NumServers;i++)
		{
			ServerDataList[i]->fullframereceived = false;
		}
	}

	void ProcessDataAppendPacket(Packet* p)
	{
		ServerData* sdata = FindServerFromPacket(p);
		if(sdata == NULL || !sdata->startframe)
			return;
		if(sdata->DataBufferOffset >= sdata->DataBufferLength)
		{
			//More data has been packed in somehow....
			return;
		}

		unsigned long i =0;
		if(p->messagelength + sdata->DataBufferOffset > sdata->DataBufferLength)
		{
			//extra data???
			for(i =0;i + sdata->DataBufferOffset< sdata->DataBufferLength;i++)
			{
				sdata->DataBuffer[sdata->DataBufferOffset + i] = p->data[i];
			}
			sdata->DataBufferOffset = sdata->DataBufferLength;
		}

		for(i =0;i < p->messagelength;i++)
		{
			sdata->DataBuffer[sdata->DataBufferOffset + i] = p->data[i];
		}
		sdata->DataBufferOffset += p->messagelength;
	}
	void ProcessLastFramePacket(Packet* p)
	{
		ServerData* sdata = FindServerFromPacket(p);
		if(sdata == NULL || !sdata->startframe)
			return;
		if(sdata->DataBufferOffset > sdata->DataBufferLength)
		{
			//More data has been packed in somehow....Return or so something with the potentially corrupt data?
			printf("what?\n");
			return;
		}

		unsigned long i =0;
		if(p->messagelength + sdata->DataBufferOffset > sdata->DataBufferLength)
		{
			//extra data???
			for(i =0;i + sdata->DataBufferOffset< sdata->DataBufferLength;i++)
			{
				sdata->DataBuffer[sdata->DataBufferOffset + i] = p->data[i];
			}
			sdata->DataBufferOffset = sdata->DataBufferLength;
		}

		for(i =0;i < p->messagelength && i + sdata->DataBufferOffset < sdata->DataBufferLength;i++)
		{
			sdata->DataBuffer[sdata->DataBufferOffset + i] = p->data[i];
		}
		sdata->DataBufferOffset = sdata->DataBufferLength;//buffer full o frame data!

		sdata->startframe = false;
		sdata->fullframereceived = true;
		//Do something with this data!
		ProcessServerDataBuffer(sdata);
	}
	void ProcessFirstFramePacket(Packet* p)
	{
		//find which server is giving us this data
		ServerData* sdata = FindServerFromPacket(p);

		if(sdata == NULL)
			return;

		char* bytearray = (char*)malloc(SIZELENGTH);
		unsigned int i =0;
		for(i =0;i < SIZELENGTH && i <p->messagelength;i++)
		{
			bytearray[i] = p->data[i];
		}

		unsigned long totalmsglength = ParseByteArrayToLong(bytearray);
		free(bytearray);

		
		//first frame packets data contains a length for all packets data combined
		sdata->startframe = true;
		sdata->DataBufferLength = totalmsglength;
		sdata->DataBuffer = (char*)malloc(totalmsglength);
		sdata->DataBufferOffset =0;
	}
	void ProcessServerDataBuffer(ServerData* sdata)
	{
		//Do something with this servers frame before displaying / doing anything with it?
		//printf("full buffer received from server: %s! length: %lu\n", sdata->ServerName, sdata->DataBufferLength);
		//char inp[5];
		//gets(inp);
	}


	//CLEANUP
	void socketCleanup(void)
	{
		closesocket(NetworkManager::ConnectSocket);
		WSACleanup();
	}
	bool CloseConnection(void)
	{
		int iResult = shutdown(NetworkManager::ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			NetworkManager::socketCleanup();
			return false;
		}

		return true;
	}
	void GracefulShutdown(void)
	{
		char closebuffer[CLOSEBUFFERLENGTH];
		int iResult = 0;
		
		if(!CloseConnection())
		{
			return;
		}

		// Receive until the peer closes the connection
		do {
			//iResult = recv(NetworkManager::ConnectSocket, closebuffer, CLOSEBUFFERLENGTH, 0);
			iResult = NetworkManager::ReceiveData(closebuffer, CLOSEBUFFERLENGTH);

			if ( iResult > 0 ){
				printf("Bytes received: %d\n", iResult);
				if(iResult < CLOSEBUFFERLENGTH)
				{
					closebuffer[iResult] = '\0';
				}
				printf("Buffer: %s\n", closebuffer);			
			}
			else if ( iResult == 0 )
				printf("Connection closed\n");
			else
				printf("recv failed with error: %d\n", WSAGetLastError());

		} while( iResult > 0 );

		freeServerDataList();

		return;
	}
	void freeServerDataList(void)
	{
		int i =0;
		for(i =0;i < NumServers;i++)
		{
			freeServerData(ServerDataList[i]);
			free(ServerDataList[i]);
		}

		free(ServerDataList);
		
	}
	void freeServerData(ServerData* sdata)
	{
		sdata->BackupOffset =0;
		sdata->BackupPacketLength =0;
		sdata->BackupUsed = false;
		sdata->cansubscribed = false;
		sdata->connected = false;
		sdata->DataBufferLength =0;
		sdata->DataBufferOffset =0;
		sdata->subscribed = false;
		free(sdata->ServerName);
		free(sdata->BackupDataBuffer);
		free(sdata->DataBuffer);
	}

	int connectiontest(int argc, char** argv)
	{
		int iResult;
		
		//Validate the parameters
		if (argc != 2) {
			printf("usage: %s server-name\n", argv[0]);
			system("PAUSE");
			return 1;
		}

		if(!NetworkManager::initializeConnection(argv[1]))
		{
			printf("Error initializing connection to server!\n");
			system("PAUSE");
			return 1;
		}


		char Key[] = "exit";
		char Key2[] = "SUB";
		char Key3[] = "REQ";
		char Key4[] = "wait";
		char Key5[]= "framesend";
		char Key6[] = "send";

		char Input[50];
		while(1){
			printf ("Send a Message. send exit to quit.\n:");
			gets (Input);
			if(strcmp(Input, Key)==0)
			{
				 printf("Key entered!\n exiting...\n");
				 break;
			}else if(strcmp(Input, Key2)==0)
			{
				iResult = SendSubscribe("robp", 4);
				printf("Subscription sent!\n");

			}else if(strcmp(Input, Key3)==0)
			{
				iResult = SendServerListRequest();
				printf("Server List Request sent!\n");
				Sleep(1000);
				char* output = (char*)malloc(DEFAULT_BUFLEN);
				int datalen = ReceiveData(output, DEFAULT_BUFLEN);
				printf("received data: %d\n", datalen);
				if(datalen >= HEADERLENGTH)
				{
					ParseReceivedData(output, datalen);
				}
					
			}else if(strcmp(Input, Key4)==0)
			{
				bool done = false;
				while(!done)
				{
					char* output = (char*)malloc(DEFAULT_BUFLEN);
					int datalen = ReceiveData(output, DEFAULT_BUFLEN);
					if(datalen >= HEADERLENGTH)
					{
						printf("received data: %d\n", datalen);
						ParseReceivedData(output, datalen);
					}
					if(datalen == 0)
						done = true;
				}
			}else if(strcmp(Input, Key5)==0)
			{
				unsigned long fakelength = 50000;
				while(1){
					char* fakedata = (char*)malloc(fakelength);
					if(fakedata == NULL)
					{
						printf("Cannot allocate %lu bytes !", fakelength);
					}else
					{
						memset(fakedata, 52, fakelength / 4);
						memset(fakedata + fakelength / 4, 76, fakelength / 4);
						memset(fakedata+ fakelength / 2, 44, fakelength / 4);
						memset(fakedata+ 3*fakelength / 4, 91, fakelength / 4);
						fakedata[fakelength -1] = 'Z';
						SendAndReceiveFrame(fakedata, fakelength);
						framecount++;
						currenttime = time(NULL);
						double timewaited = difftime(currenttime,lastcalltime);
						if(timewaited > (double)waittime)
						{
							system("CLS");
							printf("%lu frames in %f seconds, frames per second: %f\n",framecount,timewaited, (float)framecount / timewaited);
							lastcalltime = time(NULL);
							framecount =0;
						}
						free(fakedata);
						//printf("Do something with received frame...\n");
					}
				}

			}else if(strcmp(Input, Key6)==0)
			{
				unsigned long fakelength = 50000;
				char* fakedata = (char*)malloc(fakelength);
				if(fakedata == NULL)
				{
					printf("Cannot allocate %lu bytes !", fakelength);
				}else
				{
					memset(fakedata, '4', fakelength / 4);
					memset(fakedata + fakelength / 4, 'a', fakelength / 4);
					memset(fakedata+ fakelength / 2, '&', fakelength / 4);
					memset(fakedata+ 3*fakelength / 4, 'P', fakelength / 4);
					fakedata[fakelength -1] = 'Z';
					SendData(fakedata, fakelength);
					free(fakedata);
					printf("fakedata sent\n");
				}
			}
			else
			{

				iResult = NetworkManager::SendData(Input, (int)strlen(Input));
				if(iResult == SOCKET_ERROR)
				{
					NetworkManager::socketCleanup();
					system("PAUSE");
					return 1;
				}

			printf("Message send: %s, length: %d\n", Input, (int)strlen(Input));
			}
		}

		NetworkManager::GracefulShutdown();
		return 0;
	}
	int connect(const char* serverName){return 0;}

	//HELPER
	char* ParseLongToByteArray(unsigned long longnumber)
	{
		char* bytearray = (char*)malloc(SIZELENGTH);
		int i=0;
		for(i=0;i < SIZELENGTH;i++)
		{
			bytearray[i] = (unsigned char)(longnumber >> (24 - (i*8)) & 0xFF);
			
		}
		return bytearray;
	}
	unsigned long ParseByteArrayToLong(char* bytearray)
	{
		//Size needs to be SIZELENGTH!
		unsigned long longnumber = 0;
		int i =0;
		
		for(i =0;i < SIZELENGTH;i++)
		{
			longnumber += (unsigned long)(((unsigned char)(bytearray[i])) << (24 - 8*i));
		}
		return longnumber;
	}
	bool IsAlphabetic(char character)
	{
		if((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z'))
			return true;

		return false;
	}
	bool IsControlCode(char character)
	{
		if(character < MAXCONTROLCODE && character >= 1)
			return true;

		return false;
	}
}

