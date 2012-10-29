#pragma once

// Unmanaged //
#include "KinectTable\Client.h"
#include "DataTypes\KinectDataParams.h"
#include "DataTypes\KinectData.h"

// Managed //
#include "KinectDataParams.h"
#include "KinectData.h"
#include "DataReadyEventArgs.h"

// Namespaces //
using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

namespace KinectTableNet
{
	
	//!! Can only get data from one method.  Either the GetData() function or the DataReady event!
	/// <summary>
	/// A connection to a remote or local Kinect.
	/// </summary>
	public ref class Client sealed// : public IDisposable
	{
	  
	  public:

		delegate void DataReadyHandler(Object^ sender, DataReadyEventArgs^ args);

		
		/// <summary>
		/// Is fired when new data is available.
		/// </summary>
		event DataReadyHandler^ DataReady
		{
		public:
			void add(DataReadyHandler^ dataReadyHandler)
			{
				dataReadyEvent += dataReadyHandler;
			}

			void remove(DataReadyHandler^ dataReadyHandler)
			{
				dataReadyEvent -= dataReadyHandler;
			}

		private:
			void raise(Object^ sender, DataReadyEventArgs^ eventArgs)
			{
				if(dataReadyEvent)
					dataReadyEvent->Invoke(sender, eventArgs);
			}
			
		}

		
		/// <summary>
		/// Gets the data parameters of the session.
		/// </summary>
		KinectDataParams^ GetParams()
		{
			::DataTypes::KinectDataParams dataParams;
			client.GetParams(dataParams);
			return gcnew KinectDataParams(dataParams);
		}

		/// <summary>
		/// Sets the data parameters of the session.
		/// </summary>
		void SetParams(KinectDataParams^ dataParams)
		{
			::DataTypes::KinectDataParams dataParamsU;
			dataParams->CopyTo(dataParamsU);
			client.SetParams(dataParamsU);
			return;
		}

		  
		/// <summary>
		/// Recalculates the table in the image.
		/// </summary>
		void RecalculateTable()
		{
			client.RecalculateTableCorners();
			return;
		}


		/// <summary>
		/// Gets the current data from the Kinect.  Does not block the current thread.
		/// </summary>
		/// <param name="data">The current data available.</param>
		/// <returns>Returns whether the data is different from the last time it was grabbed.</returns>
		bool GetData([Out] KinectData^% data)
		{
			return GetData(data, false);
		}

		/// <summary>
		/// Gets the current data from the Kinect.
		/// </summary>
		/// <param name="data">The current data available.</param>
		/// <param name="waitForNewData">Whether or not to wait until there has been new data.</param>
		/// <returns>Returns whether the data is different from the last time it was grabbed.</returns>
		bool GetData([Out] KinectData^% data, bool waitForNewData)
		{
			static DataTypes::KinectData dataU;
			bool frameChanged = client.GetData(dataU, waitForNewData);
			data = gcnew KinectData(dataU);
			return frameChanged;
		}


	  internal:

		Client(KinectTable::Client& client) : client(client), kinectData(*new DataTypes::KinectData()), syncContext(SynchronizationContext::Current), dataReadyEvent(nullptr)
		{

			// Get the synchronization context
			syncContext = SynchronizationContext::Current;


			// Start thread
			ThreadStart^ threadDelegate = gcnew ThreadStart(this, &Client::GetDataThreadStart);
			thread = gcnew Thread(threadDelegate);
			threadExit = false;
			thread->Start();

			return;
		}


		~Client()
		{
			threadExit = true;
			delete syncContext; //This is so that it exits in case it is waiting to send something
			thread->Join();

			delete &kinectData;
			return;
		}



		KinectTable::Client& GetClient() { return client; }


	  private:

		void GetDataThreadStart()
		{
			while(!threadExit)
			{
				if(!dataReadyEvent)
					continue;

				try
				{
					bool frameChanged = client.GetData(kinectData, true);
					Debug::Assert(frameChanged);

					
					SendOrPostCallback^ callback = gcnew SendOrPostCallback(this, &Client::FireDataReadyEvent);

					// Must make sure we block here so that the data doesn't change while the user is using the DataReadyEventArgs structure
					syncContext->Send(callback, nullptr);
				}
				catch(...)
				{
					// do nothing
				}
			}

			return;
		}

		void FireDataReadyEvent(Object^ obj)
		{
			DataReadyEventArgs^ eventArgs = gcnew DataReadyEventArgs(kinectData);
			DataReady(this, eventArgs);
			eventArgs->SetDataInvalid();

			return;
		}



		SynchronizationContext^ syncContext;
		
		Thread^ thread;
		bool threadExit;

		DataReadyHandler^ dataReadyEvent;

		KinectTable::Client& client;
		DataTypes::KinectData& kinectData;  // For DataReady event

	};

}
