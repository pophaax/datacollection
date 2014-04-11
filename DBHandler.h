#ifndef __DATACOLLECT_H__
#define __DATACOLLECT_H__

#include <iostream>
#include <sstream>
#include "sqlite3.h"

using namespace std;

class DataCollect {

private:

	int m_rc;
	sqlite3 *m_db;
	char *m_error;
	static const char *m_gps;
	static const char *m_calc;
	static const char *m_head;
	static const char *m_wp;
	static const char *m_synch;

	//create INSERT string for adding to synch table
	string createSQLsynchString(string sqlString);

	//execute INSERT query and add new row into table, the sql string will be also stored in a synch table
	bool updateTable(string sqlINSERT);

	//retrive data from given table/tables, return value is a C++ 2D char array
	//rows and columns also return values (through a reference) about rows and columns in the result set
	char** retriveFromTable(string sqlSELECT, int &rows, int &columns);

public:

	//constructor
	DataCollect(void);

	//destructor
	~DataCollect(void);

	//open database
	bool openDatabase();

	//close database
	void closeDatabase();

	//create tables
	bool createTables();

	//print result set from sql query as a table
	void printSelect(string sqlSelect);

	//print results as a table from char** result set
	void printResults(char **results, int rows, int columns);

	//get a set of strings with SQL updates
	char** retriveSynchData(int &rows, int &columns);

	//insert into gps table
	void insertGPSdata(string gps_time, double latitude, double longitude,
			double altitude, double speed, double heading);

	//insert into calc table
	void insertCalculations(int offCourse, int steeringConstant, double CTS,
			double BWP, double DWP, bool TACK);

	//insert into head table
	void insertHeadingData(double hdt_heading, double gps_heading);

	//insert into waypoint table
	void insertWPdata(double wp_latitude, double wp_longitude);

};

#endif
