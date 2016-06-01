#include "DataBase.hpp"

DataBase::SQLite3Exception::SQLite3Exception(const int Code): std::runtime_error(sqlite3_errstr(Code)) {}
DataBase::SQLite3Exception::SQLite3Exception(const std::string & Text): std::runtime_error(Text) {}

DataBase::QueryBase::QueryBase(DataBase & DB, const std::string & Query):
	DB(DB), Statement(nullptr)
{
	const int Prepare = sqlite3_prepare_v2(DB.ConnectionHandle, Query.c_str(), Query.size() + 1, &Statement, nullptr);
	if(Prepare != SQLITE_OK)
	{
		sqlite3_finalize(Statement);
		throw SQLite3Exception(Prepare);
	}
}
DataBase::QueryBase::~QueryBase()
{
	const int Finalize = sqlite3_finalize(Statement);
	if(Finalize != SQLITE_OK)
		throw SQLite3Exception(Finalize);
}

const std::string DataBase::StoreEventsQuery = "INSERT INTO Events (CameraId, Time, EventCameraFailure, EventVideoLoss, EventTamperingDetection, EventMotionDetection, EventLineDetection, EventFieldDetection) VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?, ?, ?, ?)";
const std::string DataBase::GetCamerasDataQuery = "SELECT CameraId, Ip, AdminPassword FROM Cameras WHERE Enabled=1";

DataBase::DataBase(const std::string & FilePath):
	ConnectionHandle(nullptr)
{
	const int Open = sqlite3_open_v2(FilePath.c_str(), &ConnectionHandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);
	if(Open != SQLITE_OK)
	{
		sqlite3_close(ConnectionHandle);
		throw SQLite3Exception(Open);
	}

	//In case somebody tries to write to the database while we use it.
	//Just a nasty workaround, keep scrolling
	sqlite3_busy_timeout(ConnectionHandle, 1000);

	const int PragmaForeignKeys = sqlite3_exec(ConnectionHandle, "PRAGMA foreign_keys = 1", NULL, NULL, NULL);
	if(PragmaForeignKeys != SQLITE_OK)
		throw SQLite3Exception(PragmaForeignKeys);

	GetCamerasDataQueryBaseOpt.emplace(*this, GetCamerasDataQuery);
	StoreEventsQueryBaseOpt.emplace(*this, StoreEventsQuery);
}
DataBase::~DataBase()
{
	StoreEventsQueryBaseOpt = boost::none;
	GetCamerasDataQueryBaseOpt = boost::none;

	const int Close = sqlite3_close(ConnectionHandle);
	if(Close != SQLITE_OK)
		throw SQLite3Exception(Close);
}

DataBase::CamerasData_t DataBase::GetCamerasData()
{
	if(sqlite3_column_count((*GetCamerasDataQueryBaseOpt).Statement) != 3)
		throw SQLite3Exception("sqlite3_column_count((*GetCamerasDataQueryBaseOpt).Statement) != 3");


	CamerasData_t CamerasData;

	while(true)
	{
		const int Step = sqlite3_step((*GetCamerasDataQueryBaseOpt).Statement);

		if(Step == SQLITE_DONE)
			break;

		if(Step != SQLITE_ROW)
			throw SQLite3Exception(Step);

		//CameraId
		CameraId_t CameraId = sqlite3_column_int64((*GetCamerasDataQueryBaseOpt).Statement, 0);

		//CameraIp
		const char * CameraIpCStr = (const char *) sqlite3_column_text((*GetCamerasDataQueryBaseOpt).Statement, 1);
		std::string CameraIpStr = CameraIpCStr ? std::string(CameraIpCStr) : std::string();
		Ip4 CameraIp(CameraIpStr);

		//AdminPassword
		const char * AdminPasswordCStr = (const char *) sqlite3_column_text((*GetCamerasDataQueryBaseOpt).Statement, 2);
		std::string AdminPassword = AdminPasswordCStr ? std::string(AdminPasswordCStr) : std::string();

		CamerasData.emplace_back(CameraData_t{std::move(CameraId), std::move(CameraIp), std::move(AdminPassword)});
	}

	//Reset
	const int Reset = sqlite3_reset((*StoreEventsQueryBaseOpt).Statement);
	if(Reset != SQLITE_OK)
		throw SQLite3Exception(Reset);

	return CamerasData;
}

void DataBase::StoreEvents(const CameraId_t CameraId, const DS2CD2x32EventReceiver::Events & Events)
{
	if(sqlite3_bind_parameter_count((*StoreEventsQueryBaseOpt).Statement) != 7)
		throw SQLite3Exception("sqlite3_bind_parameter_count((*StoreEventsQueryBaseOpt).Statement) != 7");

	//CameraId
	const int CameraIdResult = sqlite3_bind_int64((*StoreEventsQueryBaseOpt).Statement, 1, CameraId);
	if(CameraIdResult != SQLITE_OK)
		throw SQLite3Exception(CameraIdResult);

	//EventCameraFailure
	const int EventCameraFailureResult = sqlite3_bind_int((*StoreEventsQueryBaseOpt).Statement, 2, Events.CameraFailure);
	if(EventCameraFailureResult != SQLITE_OK)
		throw SQLite3Exception(EventCameraFailureResult);

	//EventVideoLoss
	const int EventVideoLossResult = sqlite3_bind_int((*StoreEventsQueryBaseOpt).Statement, 3, Events.VideoLoss);
	if(EventVideoLossResult != SQLITE_OK)
		throw SQLite3Exception(EventVideoLossResult);

	//EventTamperingDetection
	const int EventTamperingDetectionResult = sqlite3_bind_int((*StoreEventsQueryBaseOpt).Statement, 4, Events.TamperingDetection);
	if(EventTamperingDetectionResult != SQLITE_OK)
		throw SQLite3Exception(EventTamperingDetectionResult);

	//EventMotionDetection
	const int EventMotionDetectionResult = sqlite3_bind_int((*StoreEventsQueryBaseOpt).Statement, 5, Events.MotionDetection);
	if(EventMotionDetectionResult != SQLITE_OK)
		throw SQLite3Exception(EventMotionDetectionResult);

	//EventLineDetection
	const int EventLineDetectionResult = sqlite3_bind_int((*StoreEventsQueryBaseOpt).Statement, 6, Events.LineDetection);
	if(EventLineDetectionResult != SQLITE_OK)
		throw SQLite3Exception(EventLineDetectionResult);

	//EventFieldDetection
	const int EventFieldDetectionResult = sqlite3_bind_int((*StoreEventsQueryBaseOpt).Statement, 7, Events.FieldDetection);
	if(EventFieldDetectionResult != SQLITE_OK)
		throw SQLite3Exception(EventFieldDetectionResult);

	//Execute
	const int Step = sqlite3_step((*StoreEventsQueryBaseOpt).Statement);
	if(Step != SQLITE_DONE)
		throw SQLite3Exception(Step);

	//Reset
	const int Reset = sqlite3_reset((*StoreEventsQueryBaseOpt).Statement);
	if(Reset != SQLITE_OK)
		throw SQLite3Exception(Reset);
}