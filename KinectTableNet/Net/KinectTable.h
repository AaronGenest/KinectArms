#pragma once

// Unmanaged
#include "Global\KinectTable_c.h"
#include "DataTypes\Other.h"
#include "KinectTable\Client.h"
#include <string.h>

// Managed
#include "SessionParameters.h"
#include "Client.h"


using namespace System;
using namespace System::Runtime::InteropServices;

namespace KinectTableNet
{
	/// <summary>
    /// Creates new KinectTable sessions.  (Start here to use KinectTable)
    /// </summary>
	public ref class KinectTable abstract sealed
	{

	  public:

		/// <summary>
        /// Attempts to create a private session with a local Kinect sensor.
        /// </summary>
        /// <param name="sessionParameters">The session parameters.</param>
        /// <returns>Returns the connected client.</returns>
		static Client^ ConnectLocal(SessionParameters^ sessionParameters)
		{
			DataTypes::SessionParameters sessionParametersU;
			sessionParameters->CopyTo(sessionParametersU);
			::KinectTable::Client* client = KinectTableConnectLocalPrivate(&sessionParametersU);
			if(client == NULL)
				throw -1;

			return gcnew Client(*client);
		}
	
	
		/// <summary>
        /// Attempts to create a public session with a local Kinect sensor.
        /// </summary>
        /// <param name="sessionParameters">The session parameters.</param>
		/// <param name="localAddress">The local address other computers can use to connect with the Kinect remotely.</param>
        /// <returns>Returns the connected client.</returns>
		static Client^ ConnectLocal(SessionParameters^ sessionParameters, String^ localAddress)
		{
			// Get local address string
			char localAddressU[255];
			{
				IntPtr localAddressPtr = Marshal::StringToHGlobalAnsi(localAddress);
				strcpy(localAddressU, (char*)localAddressPtr.ToPointer());
				Marshal::FreeHGlobal(localAddressPtr);
			}
			
			
			DataTypes::SessionParameters sessionParametersU;
			sessionParameters->CopyTo(sessionParametersU);
			::KinectTable::Client* client = KinectTableConnectLocalPublic(&sessionParametersU, localAddressU);
			if(client == NULL)
				throw -1;

			return gcnew Client(*client);
		}


		/// <summary>
        /// Attempts to create a private session with a remote Kinect sensor.
        /// </summary>
        /// <param name="sessionParameters">The session parameters.</param>
		/// <param name="localAddress">The address of the remote Kinect.</param>
        /// <returns>Returns the connected client.</returns>
		static Client^ ConnectRemote(SessionParameters^ sessionParameters, String^ remoteAddress)
		{
			// Get local address string
			char remoteAddressU[255];
			{
				IntPtr remoteAddressPtr = Marshal::StringToHGlobalAnsi(remoteAddress);
				strcpy(remoteAddressU, (char*)remoteAddressPtr.ToPointer());
				Marshal::FreeHGlobal(remoteAddressPtr);
			}
			
			
			DataTypes::SessionParameters sessionParametersU;
			sessionParameters->CopyTo(sessionParametersU);
			::KinectTable::Client* client = KinectTableConnectRemote(&sessionParametersU, remoteAddressU);
			if(client == NULL)
				throw -1;

			return gcnew Client(*client);
		}

		/// <summary>
		/// Disconnects the connection with the Kinect sensor.
		/// </summary>
		static void Disconnect(Client^ client)
		{
			::KinectTable::Client& clientU = client->GetClient();

			delete client;

			//!!KinectTableDisconnect(&(client->GetClient()));
			return;
		}

		
	};

}
