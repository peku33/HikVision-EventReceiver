#include "EPoll.hpp"
#include "DS2CD2x32EventReceiver.hpp"

#include <ctime>
#include <iostream>
#include <assert.h>

/*
	Standalone event-dump application. After starting - prints events to console when any of event changes.
	Usage: ./DS2CD2x32EventReceiver_Standalone_Console CameraIp AdminPassword

	Outputs - Console-formatted text in columns:
		Date 					- date of this event
		CameraFailure 			- "CameraFailure" if CameraFailure event is present, empty otherwise
		VideoLoss 				- as above
		TamperingDetection 		- as above
		MotionDetection 		- as above
		LineDetection 			- as above
		FieldDetection 			- as above
*/
int main(int ArgC, char ** ArgV)
{
	if(ArgC != 3)
	{
		std::cout << "Usage: " << ArgV[0] << " CameraIp AdminPassword" << std::endl;
		return 1;
	}

	const std::string CameraIpStr(ArgV[1]);
	const Ip4 CameraIp(CameraIpStr);

	const std::string AdminPassword(ArgV[2]);

	EPoll EP;

	//User-readable messages
	static const std::string CameraFailureString 		= "CameraFailure";
	static const std::string VideoLossString 			= "VideoLoss";
	static const std::string TamperingDetectionString 	= "TamperingDetection";
	static const std::string MotionDetectionString 		= "MotionDetection";
	static const std::string LineDetectionString 		= "LineDetection";
	static const std::string FieldDetectionString 		= "FieldDetection";

	static const std::string NoCameraFailureString 		= std::string(CameraFailureString.length(), ' ');
	static const std::string NoVideoLossString 			= std::string(VideoLossString.length(), ' ');
	static const std::string NoTamperingDetectionString = std::string(TamperingDetectionString.length(), ' ');
	static const std::string NoMotionDetectionString 	= std::string(MotionDetectionString.length(), ' ');
	static const std::string NoLineDetectionString 		= std::string(LineDetectionString.length(), ' ');
	static const std::string NoFieldDetectionString 	= std::string(FieldDetectionString.length(), ' ');

	DS2CD2x32EventReceiver ER(EP, CameraIp, AdminPassword, [] (const DS2CD2x32EventReceiver::Events & Events)
	{
		const time_t Now = std::time(nullptr);
		const tm * NowTm = std::localtime(&Now);
		char TimeStr[64];
		strftime(TimeStr, sizeof(TimeStr), "%c", NowTm);

		std::cout
			<< "| " << TimeStr << " |"
			<< "| " << (Events.CameraFailure ? CameraFailureString : NoCameraFailureString) << " |"
			<< "| " << (Events.VideoLoss ? VideoLossString : NoVideoLossString) << " |"
			<< "| " << (Events.TamperingDetection ? TamperingDetectionString : NoTamperingDetectionString) << " |"
			<< "| " << (Events.MotionDetection ? MotionDetectionString : NoMotionDetectionString) << " |"
			<< "| " << (Events.LineDetection ? LineDetectionString : NoLineDetectionString) << " |"
			<< "| " << (Events.FieldDetection ? FieldDetectionString : NoFieldDetectionString) << " |"
		<< std::endl;

	});

	EP.Main();
	return 0;
}