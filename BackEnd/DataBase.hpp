#pragma once

#include <sqlite3.h>
#include <boost/optional.hpp>
#include <list>

#include "NoCopy.hpp"
#include "DS2CD2x32EventReceiver.hpp"

/**
 * @brief Class used for loading list of cameras and storing reported events
 */
class DataBase : public NoCopy
{
	public:
		//CameraId type (if someone wants to have more than 3'000'000 cameras :D)
		typedef uint64_t CameraId_t;

		/**
		 * @brief Structure containing all necessary data to construct event listener.
		 * @details This is retrived from database.
		 * 
		 */
		struct CameraData_t
		{
			CameraId_t CameraId;
			Ip4 CameraIp;
			std::string AdminPassword;
		};

		typedef std::list<CameraData_t> CamerasData_t;

	private:
		class SQLite3Exception : public std::runtime_error
		{
			public:
				SQLite3Exception(const int Code);
				SQLite3Exception(const std::string & Text);
		};

		class QueryBase : public NoCopy
		{
			public:
				QueryBase(DataBase & DB, const std::string & Query);
				~QueryBase();

			private:
				DataBase & DB;

			private:
				sqlite3_stmt * Statement;

			friend DataBase;
		};

		friend QueryBase;

	public:
		/**
		 * @brief Initializes connection to database. Checks for database and table existence.
		 * 
		 * @param FilePath Path to SQLite3 database data file
		 */
		DataBase(const std::string & FilePath);

		/**
		 * @brief Destructor. Performs cleanup and closes database connection.
		 */
		~DataBase();

	private:
		sqlite3 * ConnectionHandle;

	private:
		//We use optionals here to force late construction

		static const std::string GetCamerasDataQuery;
		boost::optional<QueryBase> GetCamerasDataQueryBaseOpt;

		static const std::string StoreEventsQuery;
		boost::optional<QueryBase> StoreEventsQueryBaseOpt;

	public:
		/**
		 * @brief Loads list of all Enabled cameras from DataBase
		 * @return List of cameras data
		 */
		CamerasData_t GetCamerasData();

		/**
		 * @brief Inserts new row into Events table in database.
		 * @details Uses databases time (CURRENT_TIMESTAMP) as Time
		 * 
		 * @param CameraId Id of camera reporting this event
		 * @param Events The event
		 */
		void StoreEvents(const CameraId_t CameraId, const DS2CD2x32EventReceiver::Events & Events);
};