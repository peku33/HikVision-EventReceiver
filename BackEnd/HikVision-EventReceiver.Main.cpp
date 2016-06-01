#include "DataBase.hpp"
#include "EPoll.hpp"
#include "DS2CD2x32EventReceiver.hpp"

#include <iostream>

/*
	The main application. Relays on SQLite3 database.
	Loads list of cameras and stores upcoming events.

	This application should be put in background and kept running
*/
int main()
{
	//Open database connetion
	DataBase DB("../DataBase/DataBase.sqlite3");

	//Load cameras list from database
	DataBase::CamerasData_t CamerasData = DB.GetCamerasData();
	if(CamerasData.empty())
	{
		std::cerr << "No Enabled cameras found in DataBase. Exiting." << std::endl;
		return 2;
	}

	//Here we dump list of cameras to console
	std::cout << "Cameras found: " << std::endl;
	for(const DataBase::CameraData_t & CameraData : CamerasData)
		std::cout << "\t#" << CameraData.CameraId << "\t" << CameraData.CameraIp.GetIpStr() << "\t" << CameraData.AdminPassword << std::endl;
	std::cout << "End of cameras list" << std::endl;

	//Application begin
	EPoll EP;
	std::list<DS2CD2x32EventReceiver> Receivers; //This holds DS2CD2x32EventReceiver objects

	for(const DataBase::CameraData_t & CameraData : CamerasData)
		Receivers.emplace_back(EP, CameraData.CameraIp, CameraData.AdminPassword, std::bind(&DataBase::StoreEvents, &DB, CameraData.CameraId, std::placeholders::_1));

	//Cameras listneres are initialized - we are ready to fire the main event loop

	std::cout << std::endl;
	std::cout << "Switching to main event loop (Enabling application). No news (here, in console) is good news. See you in database / front end ;)" << std::endl;

	EP.Main();

	std::cout << std::endl;
	std::cout << "Finalizing application, exiting gracefully ;)" << std::endl;

	return 0;
}