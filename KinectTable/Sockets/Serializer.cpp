//---------------------------------------------------------------------------
// Module Definition
//---------------------------------------------------------------------------

#define MOD_SOCKETS

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

// Header
#include "Serializer.h"

// Project


// Standard C++


// Standard C
#include <Windows.h>


//---------------------------------------------------------------------------
// Namespaces
//---------------------------------------------------------------------------



namespace Sockets
{


//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------


Serializer::Serializer(const SOCKET& socket) : socket(socket)
{
	return;
}

Errno Serializer::SendData(const char data[], unsigned long length)
{
	//!!
	assert(data[0] >= 0 && data[0] <= 6);

	// Break up the data and send them off as packets

	unsigned long neededPackets = length / (Packet::maxDataLength+1);
	assert(neededPackets <= 0xFF);  // Make sure value doesn't rail
	unsigned char packetNum = (char)(length / (Packet::maxDataLength+1));
	size_t bytesSent = 0;
	int ret = 0;
	while(length > 0)
	{
		// Create packet
		sendPacket.PacketNum() = packetNum;
		const unsigned int packetLength = (length < Packet::maxDataLength) ? length : Packet::maxDataLength;
		memcpy(sendPacket.Data(), &data[bytesSent], packetLength);  //!!Could also just send as separate command!


		// Convert packet to network byte order
		assert(sizeof(packetLength) == sizeof(long));
		sendPacket.DataLength() = htonl(packetLength);

		
		// Send bytes
		ret = Serializer::SendDataLowLevel((const char*)&sendPacket, sendPacket.headerSize + packetLength);
		if(ret < 0)
			return ret;

		// Update values
		packetNum--;
		length -= packetLength;
		bytesSent += packetLength;

	}
	
	return 0;
}


Errno Serializer::ReceiveData(char data[], unsigned long& length)
{

	int ret;
	unsigned long recvBytes = 0;

	//Gather packets
	while(true)
	{

		ret = packetizer.FindPacket(recvPacket);

		switch(ret)
		{
			// Found packet
			case 0:
			{

				//Convert length back to host byte order
				recvPacket.DataLength() = ntohl(recvPacket.DataLength());

				// Give packet data to user
				assert(length >= recvPacket.DataLength() + recvBytes);
				memcpy(&data[recvBytes], recvPacket.Data(), recvPacket.DataLength());
				recvBytes += recvPacket.DataLength();

				//Check if we have all the data needed
				if(recvPacket.PacketNum() == 0)
				{
					//!!
					assert(data[0] >= 0 && data[0] <= 6);
					return 0;
				}

				break;
			}


			// Need more data
			case -1:
			{
				//Get data
				int bufferSize = recvBuffer.maxSize;
				ret = Serializer::RecvDataLowLevel(recvBuffer.data, bufferSize);
				if(ret < 0)
					return -1;

				recvBuffer.size = bufferSize;

				packetizer.AddBytes(recvBuffer.data, recvBuffer.size);
				break;
			}

			// Error occurred
			default: return -1;
		}

	}

	// We shouldn't get here
	assert(false);
	return 0;
}




/*
Errno ReceiveData(const SOCKET& socket, char id, const char data[], size_t& length)
{
	
	//Check queue for received data

	//If not data, wait for it

	//If data, copy data over and return

	return -1;
}
*/



//---------------------------------------------------------------------------
// Private Methods
//---------------------------------------------------------------------------

int Serializer::SendDataLowLevel(const char data[], int length)
{

	// Send the data
	int sendResult = send(socket, data, length, 0);
	if (sendResult == SOCKET_ERROR)
	{

		int error = WSAGetLastError();
		switch(error)
		{
			case WSAECONNRESET: return -1;
			case WSAEWOULDBLOCK: return 0; //!!
			default: return -1;
		}
	}

	if(sendResult == 0)
		return -1;

	return 0;
}


int Serializer::RecvDataLowLevel(char data[], int& length)
{

	// Receive data
	unsigned long receiveResult = recv(socket, data, length, 0);
	if (receiveResult == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		switch(error)
		{
			case WSAEWOULDBLOCK: return 0;
			case WSAECONNRESET: return -1;
			default: return -1;
		}
	}

	length = receiveResult;
	return 0;
}


/*
void ReceiveThread()
{

	const int length = 1023;
	static char bytes[length];

	while(!threadExit)
	{
		
		ReceiveBytes(socket, bytes, length);

		//Store in array

		//Check array for any complete packets

		//Add any complete packets to output data queue
		
	}

	return;
}
*/

/*
void ConvertPacketToBytes(const Packet& packet, char data[], int& length)
{

	
	unsigned long byteNum = 0;

	// Id
	assert(sizeof(packet.id) == 1);
	data[0] = packet.id;
	byteNum++;

	// Packet Num
	assert(sizeof(packet.packetNum) == 1);
	data[1] = packet.packetNum;
	byteNum++;

	// Data length
	assert(sizeof(packet.length) == sizeof(long));
	unsigned long networkLength = htonl(packet.length);
	memcpy(&data[2], &networkLength, sizeof(networkLength));
	byteNum += sizeof(networkLength);

	// Data
	memcpy(&data[byteNum], packet.data, packet.length);
	byteNum += packet.length;


	length = byteNum;
	return;
}
*/
/*
int ConvertBytesToPacket(const char data[], int length, Packet& userPacket)
{

	static Packet packet;
	static int packetByteNum = 0;

	// See the packet structure as an array of bytes
	typedef char (&PacketByteArray)[sizeof(packet)];
	PacketByteArray& packetBytes = (PacketByteArray)packet;

	// Loop through incoming bytes
	for(int i=0; i<length; i++)
	{
		// Copy new byte over
		packetBytes[packetByteNum] = data[i];
		packetByteNum++;

		//Check if we are finished a packet
		if(packetByteNum > packet.headerSize)
		{
			if(packetByteNum > packet.headerSize + packet.length)
			{
				// Give packet to user
				memcpy(&userPacket, &packet, packet.headerSize + packet.length);
				return 0;
			}
		}
		
	}

	return -1;
}
*/
/*
int ReceiveBytes(const SOCKET& socket, char data[], int length)
{

	int receiveResult = recv(socket, data, length, 0);
	if (receiveResult == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if(error == WSAEWOULDBLOCK)
		{
			return 0;
		}else if(error == WSAECONNRESET)
		{
			//!!socketCleanup();
			printf("connection to table lost...\n");
		}
		printf("Receive failed with error: %d\n", error);
		return -1;
	}

	return receiveResult;
}


int SendBytes(const SOCKET& socket, const char data[], int length)
{

	int sendResult =0;
	sendResult = send(socket, data, length, 0);
	if (sendResult == SOCKET_ERROR) {
		int error = WSAGetLastError();
	
		if(error == WSAECONNRESET)
		{
			printf("Connection Reset\n");

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
*/





/*
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
*/

/*
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
*/


}
