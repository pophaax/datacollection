#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__ //__DATACOLLECT_H__

#include <iostream>
#include <sstream>
#include <string>
#include "sqlite3.h"

using namespace std;

class DBHandler {

private:

	int m_rc;
	sqlite3 *m_db;
	char *m_error;
	static const char *m_configs;
	static const char *m_dataLogs;
	static const char *m_waypoints;
	static const char *m_errorLogs;

	//create INSERT string for adding to synch table
	string createSQLsynchString(string sqlString);

	//execute INSERT query and add new row into table, the sql string will be also stored in a synch table
	bool updateTable(string sqlINSERT);

	//retrive data from given table/tables, return value is a C++ 2D char array
	//rows and columns also return values (through a reference) about rows and columns in the result set
	char** retriveFromTable(string sqlSELECT, int &rows, int &columns);

public:
	void insertConfig(
			int sc_commandclosereach,
        	int sc_commandbeamreach,
        	int sc_commandbroadreach,
        	int sc_commandrunning,
        	int sc_anglebeamreach,
        	int sc_anglebroadreach,
        	int sc_anglerunning,

        	int rc_commandextreme,
        	int rc_commandmedium,
        	int rc_commandsmall,
        	int rc_commandmidships,
        	int rc_anglemedium,
        	int rc_anglesmall,
        	int rc_anglemidships,

        	int cc_tackangle,
        	int cc_sectorangle,

        	string ws_sensormodel,
        	string ws_portname,
        	int ws_baudrate,

        	string mc_portname,

        	int rs_channel,
        	int rs_speed,
        	int rs_acceleration,
        	int rs_limitmin,
        	int rs_limitmax,

        	int ss_channel,
        	int ss_speed,
        	int ss_acceleration,
        	int ss_limitmin,
        	int ss_limitmax,

        	string gps_portname,
        	string gps_connectionname);

	void insertDataLog(
        	int sc_command,

        	int rc_command,

        	int cc_dtw,
        	int cc_btw,
        	int cc_cts,
        	int cc_twd,
        	int cc_tack,

        	int ws_buffersize,
        	string ws_sensormodel,
        	int ws_direction,
        	int ws_speed,
        	int ws_temperature,

        	int rs_position,

        	int ss_position,

			string gps_timestamp,
			double gps_latitude,
			double gps_longitude,
			double gps_altitude,
			double gps_speed,
			double gps_heading,
			int gps_mode,
			int gps_satellites);

	void insertErrorLog(string error);

	void insertWaypoint(int routeid, double latitude, double longitude);

	//constructor
	DBHandler(void);

	//destructor
	~DBHandler(void);

	//open database
	bool openDatabase(string fileName);

	//close database
	void closeDatabase();

	//create tables
	bool createTables();

	string retriveCell(string table, string id, string column);

};

#endif
