#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__ //__DATACOLLECT_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "logger/Logger.h"

class SystemStateModel;
class WaypointModel;
class PositionModel;

class DBHandler {

private:

	int m_rc;
	sqlite3 *m_db;
	char *m_error;
	int m_latestDataLogId;
	int m_latestWaypointId;
	Logger m_logger;
	std::string m_filePath;

	//execute INSERT query and add new row into table
	void queryTable(std::string sqlINSERT);

	//returns table row in Json format, uses table as key
	std::string getRowAsJson(std::string select, std::string table, std::string key, std::string id);

	//retrive data from given table/tables, return value is a C 2D char array
	//rows and columns also return values (through a reference) about rows and columns in the result set
	char** retriveFromTable(std::string sqlSELECT, int &rows, int &columns);

	//gets the id column from a given table
	std::vector<std::string> getTableIds(std::string table);

	//gets information(for instance: name/datatype) about all columns
	std::vector<std::string> getColumnInfo(std::string info, std::string table);

	//help function used in insertDataLog
	int insertLog(std::string table, std::string values);

	//formats datarows to json using
	std::string formatRowToJson(std::string key,std::vector<std::string> values, std::vector<std::string> columnNames);

public:

	DBHandler(std::string filePath);
	~DBHandler(void);

	void openDatabase(std::string fileName);
	void closeDatabase();

	void insertDataLog(
		SystemStateModel systemState,
		int sail_servo_position,
		int rudder_servo_position,
		double course_calculation_distance_to_waypoint,
		double course_calculation_bearing_to_waypoint,
		double course_calculation_course_to_steer,
		bool course_calculation_tack,
		bool course_calculation_going_starboard,
		int waypoint_id,
		double true_wind_direction_calc);

	void insertMessageLog(std::string gps_time, std::string type, std::string msg);

	bool revChanged(std::string toCheck, std::string serverRevs);

	void updateTable(std::string table, std::string data);

	void clearTable(std::string table);

  //retrieve one value from a table as string
	std::string retriveCell(std::string table, std::string id, std::string column);

  //retrieve one value from a table as integer
	int retriveCellAsInt(std::string table, std::string id, std::string column);

	// returns first row in datalogs as JSON
	std::string getLogs();

	void removeLogs(std::string lines);

	std::string getMinIdFromTable(std::string table);

	void deleteRow(std::string table, std::string id);

	void getWaypointFromTable(WaypointModel &waypointModel);

	void insert(std::string table, std::string fields, std::string values);

	// inserts area scanning measurements into db
	void insertScan(std::string waypoint_id, PositionModel position, float temperature, std::string timestamp);

	void changeOneValue(std::string table, std::string id, std::string newValue, std::string colName);

	//returns total rows from table
	int getRows(std::string table);

	//returns waypoints
	std::string getWaypoints();

	void clearDatalogTables();
};

#endif
