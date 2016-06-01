#pragma once

#include <sqlite3.h>
#include <boost/optional.hpp>
#include <list>

#include "NoCopy.hpp"
#include "DS2CD2x32EventReceiver.hpp"

class DataBase : public NoCopy
{
	public:
		typedef uint64_t CameraId_t;

		struct CameraData_t
		{
			CameraId_t CameraId;
			Ip4 CameraIp;
			std::string AdminPassword;
		};

		typedef std::list<CameraData_t> CamerasData_t;

		class SQLite3Exception : public std::runtime_error
		{
			public:
				SQLite3Exception(const int Code);
				SQLite3Exception(const std::string & Text);
		};

	private:
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
		DataBase(const std::string & FilePath);
		~DataBase();

	private:
		sqlite3 * ConnectionHandle;

	private:
		static const std::string GetCamerasDataQuery;
		boost::optional<QueryBase> GetCamerasDataQueryBaseOpt;

		static const std::string StoreEventsQuery;
		boost::optional<QueryBase> StoreEventsQueryBaseOpt;

	public:
		CamerasData_t GetCamerasData();
		void StoreEvents(const CameraId_t CameraId, const DS2CD2x32EventReceiver::Events & Events);
};