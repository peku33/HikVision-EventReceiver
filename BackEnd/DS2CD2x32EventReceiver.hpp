#pragma once

#include <chrono>
#include <functional>
#include <boost/optional.hpp>

#include "Ip4.hpp"
#include "Ip4Port.hpp"
#include "TcpClientPersistentSocket.hpp"
#include "TimerSingle.hpp"
#include "CircularExtractBuffer.hpp"

class EPoll;

class DS2CD2x32EventReceiver
{
	public:
		struct Events
		{
			public:
				Events();

			public:
				bool operator == (const Events & Other) const;
				bool operator != (const Events & Other) const;

			public:
				bool CameraFailure;
				bool VideoLoss;
				bool TamperingDetection;
				bool MotionDetection;
				bool LineDetection;
				bool FieldDetection;
		};

	//General settings
	private:
		//Full message has ~600 bytes. Socket input buffer has 1500.
		//In case only 599 bytes were transfered and then application catches lag and socket buffer fills up we would get buffer overlow (just got an exception from CircularExtractBuffer.Append())
		//So the safe size for this is 600b + 1500b = ~2100 (2048 is to low, i tried :D). To be absolutely safe - 4K is reasonable size.
		static const unsigned int InputBufferLength = 4096;

		static const TcpClientPersistentSocket::KeepAliveData_t AlertStreamSocketKeepAliveData;		//KeepAlive structure passed to Socket
		static const std::chrono::seconds AlertStreamSocketRecreateScheduleDuration;				//The time we wait between socket death / error and creating a new one
		static const std::chrono::milliseconds EventTimeoutDuration;								//Camera does not inform us if the event is finished. Instead we have to implement timeout

	public:
		typedef std::function<void (const Events &)> OnEventsChange_f;

	public:
		DS2CD2x32EventReceiver(EPoll & EP, const Ip4 & CameraIp, const std::string & AdminPassword, const OnEventsChange_f & OnEventsChange);

	private:
		EPoll & EP;
		const Ip4 CameraIp;
		const std::string AdminPassword;
		const OnEventsChange_f OnEventsChange;

	private:
		const Ip4Port CameraHTTPIpPort;

	private:
		TimerSingle AlertStreamSocketRecreateTimer;

		boost::optional<TcpClientPersistentSocket> AlertStreamSocketOpt;
		void AlertStreamSocketRecreate();

		void AlertStreamSocketOnConnected();
		void AlertStreamSocketOnNewData(const std::string & Data);
		void AlertStreamSocketOnError();

		CircularExtractBuffer InputBuffer;
		void OnXmlMessage(const std::string & Xml);

	private:
		TimerSingle VideoLossTimeoutTimer;
		TimerSingle TamperingDetectionTimeoutTimer;
		TimerSingle MotionDetectionTimeoutTimer;
		TimerSingle LineDetectionTimeoutTimer;
		TimerSingle FieldDetectionTimeoutTimer;

		void EventTimeoutTimer(bool * const StatePtr);

		Events PreviousEvents, CurrentEvents;
		void AnyEventChanged();
};