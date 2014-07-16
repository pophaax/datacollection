#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__ //__DATACOLLECT_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "sqlite3.h"

using namespace std;

class DBHandler {

private:

	int m_rc;
	sqlite3 *m_db;
	char *m_error;
	int m_latestDataLogId;

	//execute INSERT query and add new row into table, the sql string will be also stored in a synch table
	void queryTable(string sqlINSERT);

	//retrive data from given table/tables, return value is a C++ 2D char array
	//rows and columns also return values (through a reference) about rows and columns in the result set
	char** retriveFromTable(string sqlSELECT, int &rows, int &columns);

	//gets the id column from a given table
	vector<string> getTableIds(string table);

	//gets information(for instance: name/datatype) about all columns
	vector<string> getColumnInfo(string info, string table);

public:

	void insertDataLog(
		string gps_time,
		double gps_lat,
		double gps_lon,
		double gps_spd,
		double gps_head,
		int gps_sat,
		int sc_cmd,
		int rc_cmd,
		int ss_pos,
		int rs_pos,
		double cc_dtw,
		double cc_btw,
		double cc_cts,
		int cc_tack,
		int ws_dir,
		double ws_spd,
		int ws_tmp,
		int wpt_cur);

	void insertMessageLog(string gps_time, string type, string msg);

	bool revChanged(string toCheck, string serverRevs);

	void updateTable(string table, string data);

	void clearTable(string table);

	//constructor
	DBHandler(void);

	//destructor
	~DBHandler(void);

	//open database
	void openDatabase(string fileName);

	//close database
	void closeDatabase();

    //retrieve one value from a table as string
	string retriveCell(string table, string id, string column);
	int retriveCellAsInt(string table, string id, string column);

	string getLogs();

	void removeLogs(string lines);

	string getMinIdFromTable(string table);
	void deleteRow(string table, string id);
};

#endif
