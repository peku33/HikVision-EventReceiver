#include "DS2CD2x32EventReceiver.hpp"

#include <iostream>

#include "StringTools.hpp"

DS2CD2x32EventReceiver::Events::Events():
	CameraFailure(false), VideoLoss(false), TamperingDetection(false), MotionDetection(false), LineDetection(false), FieldDetection(false)
{

}

bool DS2CD2x32EventReceiver::Events::operator == (const Events & Other) const
{
	return
		(CameraFailure == Other.CameraFailure) &&
		(VideoLoss == Other.VideoLoss) &&
		(TamperingDetection == Other.TamperingDetection) &&
		(MotionDetection == Other.MotionDetection) &&
		(LineDetection == Other.LineDetection) &&
		(FieldDetection == Other.FieldDetection);
}
bool DS2CD2x32EventReceiver::Events::operator != (const Events & Other) const
{
	return !(*this == Other);
}

const TcpClientPersistentSocket::KeepAliveData_t DS2CD2x32EventReceiver::AlertStreamSocketKeepAliveData = {5, 5, 1};
const std::chrono::seconds DS2CD2x32EventReceiver::AlertStreamSocketRecreateScheduleDuration = std::chrono::seconds(5);
const std::chrono::milliseconds DS2CD2x32EventReceiver::EventTimeoutDuration = std::chrono::milliseconds(1250);

DS2CD2x32EventReceiver::DS2CD2x32EventReceiver(EPoll & EP, const Ip4 & CameraIp, const std::string & AdminPassword, const OnEventsChange_f & OnEventsChange):
	EP(EP), CameraIp(CameraIp), AdminPassword(AdminPassword), OnEventsChange(OnEventsChange),
	CameraHTTPIpPort(CameraIp, 80),

	AlertStreamSocketRecreateTimer(EP, AlertStreamSocketRecreateScheduleDuration, std::bind(&DS2CD2x32EventReceiver::AlertStreamSocketRecreate, this)),
	InputBuffer(InputBufferLength, "<EventNotificationAlert version=\"1.0\" xmlns=\"http://www.hikvision.com/ver20/XMLSchema\">", "</EventNotificationAlert>"),

	VideoLossTimeoutTimer				(EP, EventTimeoutDuration, std::bind(&DS2CD2x32EventReceiver::EventTimeoutTimer, this, &CurrentEvents.VideoLoss)),
	TamperingDetectionTimeoutTimer		(EP, EventTimeoutDuration, std::bind(&DS2CD2x32EventReceiver::EventTimeoutTimer, this, &CurrentEvents.TamperingDetection)),
	MotionDetectionTimeoutTimer			(EP, EventTimeoutDuration, std::bind(&DS2CD2x32EventReceiver::EventTimeoutTimer, this, &CurrentEvents.MotionDetection)),
	LineDetectionTimeoutTimer			(EP, EventTimeoutDuration, std::bind(&DS2CD2x32EventReceiver::EventTimeoutTimer, this, &CurrentEvents.LineDetection)),
	FieldDetectionTimeoutTimer			(EP, EventTimeoutDuration, std::bind(&DS2CD2x32EventReceiver::EventTimeoutTimer, this, &CurrentEvents.FieldDetection))
{
	AlertStreamSocketRecreate();
}

void DS2CD2x32EventReceiver::AlertStreamSocketRecreate()
{
	CurrentEvents.CameraFailure = true;
	AnyEventChanged();

	//To prevent inifinite waiting on connect()
	AlertStreamSocketRecreateTimer.Restart();

	#ifdef DEBUG
		std::cout << "DS2CD2x32EventReceiver::AlertStreamSocketRecreate()" << std::endl;
	#endif

	try
	{
		AlertStreamSocketOpt.emplace(
			EP, CameraHTTPIpPort,
			std::bind(&DS2CD2x32EventReceiver::AlertStreamSocketOnConnected, this),
			std::bind(&DS2CD2x32EventReceiver::AlertStreamSocketOnNewData, this, std::placeholders::_1),
			std::bind(&DS2CD2x32EventReceiver::AlertStreamSocketOnError, this),
			AlertStreamSocketKeepAliveData
		);
	}
	catch(const std::exception & E)
	{
		std::cerr << "DS2CD2x32EventReceiver::AlertStreamSocketRecreate(): " << E.what() << std::endl;
	}
}

void DS2CD2x32EventReceiver::AlertStreamSocketOnConnected()
{
	#ifdef DEBUG
		std::cerr << "DS2CD2x32EventReceiver::AlertStreamSocketOnConnected()" << std::endl;
	#endif

	const std::string GetAlertStreamRequest = std::string() + "GET /ISAPI/Event/notification/alertStream HTTP/1.1\r\nAuthorization: Basic " + StringTools::Base64Encode("admin:" + AdminPassword) + "\r\n\r\n";
	(*AlertStreamSocketOpt).Send(GetAlertStreamRequest);
}
void DS2CD2x32EventReceiver::AlertStreamSocketOnNewData(const std::string & Data)
{
	InputBuffer.Append(Data);
	const std::string Xml = InputBuffer.Extract();
	if(Xml.empty())
		return;
	
	OnXmlMessage(Xml);
}
void DS2CD2x32EventReceiver::AlertStreamSocketOnError()
{
	std::cerr << "DS2CD2x32EventReceiver::AlertStreamSocketOnError()" << std::endl;
	
	CurrentEvents.CameraFailure = true;
	AnyEventChanged();

	AlertStreamSocketOpt = boost::none; //This deletes TcpPersistenClientSocket object
	AlertStreamSocketRecreateTimer.Restart();
}

void DS2CD2x32EventReceiver::OnXmlMessage(const std::string & Xml)
{
		const std::string EventType = StringTools::ExtractFromBetween(Xml, "<eventType>", "</eventType>");
		if(EventType.empty())
		{
			std::cerr << "DS2CD2x32EventReceiver::OnXmlMessage(): " << "Cannot extract EventType" << std::endl;
			return;
		}
		
		const std::string EventStateStr = StringTools::ExtractFromBetween(Xml, "<eventState>", "</eventState>");
		if(EventStateStr.empty())
		{
			std::cerr << "DS2CD2x32EventReceiver::OnXmlMessage(): " << "Cannot extract EventStateStr" << std::endl;
			return;
		}
		
		const bool EventState = EventStateStr == "active";
		
		//We restart the no-data timer because something was received
		AlertStreamSocketRecreateTimer.Restart();
		CurrentEvents.CameraFailure = false;

		TimerSingle * TimerPtr = nullptr;
		bool * StatePtr = nullptr;
		
		//Please don't ask why cameras event names are not consitent with actual event names.
		//Those names are created by chineese developers, I tried to standarize them somehow
		if(EventType == "videoloss")
		{
			TimerPtr = &VideoLossTimeoutTimer;
			StatePtr = &CurrentEvents.VideoLoss;
		}
		else if(EventType == "shelteralarm")
		{
			TimerPtr = &TamperingDetectionTimeoutTimer;
			StatePtr = &CurrentEvents.TamperingDetection;
		}
		else if(EventType == "VMD")
		{
			TimerPtr = &MotionDetectionTimeoutTimer;
			StatePtr = &CurrentEvents.MotionDetection;
		}
		else if(EventType == "linedetection")
		{
			TimerPtr = &LineDetectionTimeoutTimer;
			StatePtr = &CurrentEvents.LineDetection;
		}
		else if(EventType == "fielddetection")
		{
			TimerPtr = &FieldDetectionTimeoutTimer;
			StatePtr = &CurrentEvents.FieldDetection;
		}
		else
		{
			std::cerr << "DS2CD2x32EventReceiver::OnXmlMessage(): " << "Unknown EventType: " << EventType << std::endl;
		}
		
		if(TimerPtr && StatePtr)
		{
			if(EventState)
			{
				TimerPtr->Restart();
				*StatePtr = true;
			}
			else
			{
				TimerPtr->Stop();
				*StatePtr = false;
			}
		}
		
		//We call AnyEventChanged because CameraFailure is always touched. Since AnyEventChanged will check for actual change - nothing can go wrong here.
		AnyEventChanged();
}

void DS2CD2x32EventReceiver::EventTimeoutTimer(bool * const StatePtr)
{
	*StatePtr = false;
	AnyEventChanged();
}

void DS2CD2x32EventReceiver::AnyEventChanged()
{
	if(PreviousEvents == CurrentEvents)
		return;

	OnEventsChange(CurrentEvents);

	PreviousEvents = CurrentEvents;
}