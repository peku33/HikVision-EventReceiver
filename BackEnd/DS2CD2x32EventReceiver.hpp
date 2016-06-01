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

/**
 * @brief The CORE class - Camera event receiver.
 * @details This is extremly easy to use - just pass callback function to constructor. Everything else is done automatically inside.
 */
class DS2CD2x32EventReceiver
{
	/**
	 * @brief Event structure. Each bool describes one event reported by camera.
	 * @details When any of event changes DS2CD2x32EventReceiver's callback pushes reference to updated object. (For no this is always the same object, with updated fields, but you should'n relay on that fact)
	 */
	public:
		struct Events
		{
			public:
				/**
				 * @brief Constructs Events object with all events == false
				 */
				Events();

			public:
				bool operator == (const Events & Other) const;
				bool operator != (const Events & Other) const;

			public:
				//CameraFailure is not reported by camera. This is internal event reporting that something went wrong - connection failure / camera error / timeout / etc.
				bool CameraFailure;

				//Below are events reported by camera. See camera documentation (google for HikVision ISAPI IPMD) for detailed description. Notice: TamperingDetection is sometimes called 'shelteralam'.
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
		/**
		 * @brief Callback functional type. See constructor
		 */
		typedef std::function<void (const Events &)> OnEventsChange_f;

	public:
		/**
		 * @brief Constructor. Begins operation of event receiver.
		 * @details This is the only place you interact with this class. Events are reported by OnEventsChange() callback
		 * 
		 * @param EP EPoll event loop to register all Fd's in (timers, sockets, etc)
		 * @param CameraIp Ip4Port object representing camera ip to connect to. See Ip4Port constructor.
		 * @param AdminPassword Password for admin user to camera. This is default and non-deletable user account, the only one with access to AlertStream. In this application we do nothing more then receiving AlertStream
		 * @param OnEventsChange Callback function called when any of events is changed. Callback contains reference to current Events object.
		 */
		DS2CD2x32EventReceiver(EPoll & EP, const Ip4 & CameraIp, const std::string & AdminPassword, const OnEventsChange_f & OnEventsChange);

	private:
		EPoll & EP;
		const Ip4 CameraIp;
		const std::string AdminPassword;
		const OnEventsChange_f OnEventsChange;

	private:
		const Ip4Port CameraHTTPIpPort; //Just CameraIp + port 80

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