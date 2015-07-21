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
	Logger m_logger;

	//execute INSERT query and add new row into table
	void queryTable(std::string sqlINSERT);

	//retrive data from given table/tables, return value is a C 2D char array
	//rows and columns also return values (through a reference) about rows and columns in the result set
	char** retriveFromTable(std::string sqlSELECT, int &rows, int &columns);

	//gets the id column from a given table
	std::vector<std::string> getTableIds(std::string table);

	//gets information(for instance: name/datatype) about all columns
	std::vector<std::string> getColumnInfo(std::string info, std::string table);

public:

	DBHandler(void);
	~DBHandler(void);

	void openDatabase(std::string fileName);
	void closeDatabase();

	void insertDataLog(
		SystemStateModel systemState,
		int ss_pos,
		int rs_pos,
		double cc_dtw,
		double cc_btw,
		double cc_cts,
		bool cc_tack,
		bool cc_goingStarboard,
		int wpt_cur);

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
	void insertScan(PositionModel position, float temperature);

	void changeOneValue(std::string table, std::string id, std::string newValue, std::string colName);


};

#endif
