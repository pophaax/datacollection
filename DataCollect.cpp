#include "DataCollect.h"


const char* DataCollect::m_gps = "CREATE TABLE gps (id INTEGER PRIMARY KEY AUTOINCREMENT, server_time TIMESTAMP, gps_time VARCHAR, latitude DOUBLE, longitude DOUBLE, altitude DOUBLE, speed DOUBLE, heading DOUBLE);";
const char* DataCollect::m_calc = "CREATE TABLE calc (id INTEGER PRIMARY KEY AUTOINCREMENT, offCourse INTEGER, steeringConstant INTEGER, cts DOUBLE, bwp DOUBLE, dwp DOUBLE, tack BOOL);";
const char* DataCollect::m_head = "CREATE TABLE head (id INTEGER PRIMARY KEY AUTOINCREMENT, hdt_heading DECIMAL, gps_heading DECIMAL);";
const char* DataCollect::m_wp = "CREATE TABLE wp (id INTEGER PRIMARY KEY AUTOINCREMENT, wp_latitude DECIMAL, wp_longitude DECIMAL);";
const char* DataCollect::m_synch = "CREATE TABLE synch (id INTEGER PRIMARY KEY AUTOINCREMENT, sql_strings TEXT, synched BOOL);";


DataCollect::DataCollect(void)
{
}


DataCollect::~DataCollect(void)
{
}


bool DataCollect::openDatabase(void)
{
	//char*error;
	//cout << "Opening MyDb.db ..." << endl;

	m_rc = sqlite3_open("DatCol.db", &m_db);

	if(m_rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(m_db) << endl << endl;
		sqlite3_close(m_db);
		return false;
	}

	//return a bool
	else
	{
		cout << "Opened DatCol.db." << endl << endl;
		return true;
	}
}


bool DataCollect::createTables(void)
{

	for (int i=0; i<5; i++)
	{
		if (i==0)
			m_rc = sqlite3_exec(m_db, m_gps, NULL, NULL, &m_error);

		else if (i==1)
			m_rc = sqlite3_exec(m_db, m_calc, NULL, NULL, &m_error);

		else if (i==2)
			m_rc = sqlite3_exec(m_db, m_head, NULL, NULL, &m_error);

		else if (i==3)
			m_rc = sqlite3_exec(m_db, m_wp, NULL, NULL, &m_error);

		else
			m_rc = sqlite3_exec(m_db, m_synch, NULL, NULL, &m_error);

		
		if (m_rc)
		{
			cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(m_db) << endl << endl;
			sqlite3_free(m_error);

			return false;
		}
	
		else
			cout << "Created table no: " << i+1 << endl << endl;	
	}

	return true;
}


bool DataCollect::updateTable(string sqlINSERT)
{
	m_rc = sqlite3_exec(m_db, sqlINSERT.c_str(), NULL, NULL, &m_error);

	if (m_rc)
   {
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(m_db) << endl << endl;
		sqlite3_free(m_error);
		return false;
   }
   else
   {
	   //cout << "Inserted a value into a given table." << endl << endl;
	   return true;
   }
}


char** DataCollect::retriveFromTable(string sqlSELECT, int &rows, int &columns)
{
	char **results = NULL;

	sqlite3_get_table(m_db, sqlSELECT.c_str(), &results, &rows, &columns, &m_error);

	if (m_rc)
   {
      cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(m_db) << endl << endl;
      sqlite3_free(m_error);

	  return NULL;
   }

	else
		return results;
}


void DataCollect::printResults(char **results, int rows, int columns)
{
	// Display Table
      for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
      {
         for (int colCtr = 0; colCtr < columns; ++colCtr)
         {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
 
            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
         }
 
         // End Line
         cout << endl;
 
         // Display Separator For Header
         if (0 == rowCtr)
         {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
               cout.width(12);
               cout.setf(ios::left);
               cout << "------------ ";
            }
            cout << endl;
         }
      }
	  cout << endl;
}


void DataCollect::printSelect(string sqlSelect)
{
	char** result;
	int rows, columns;

	result = retriveFromTable(sqlSelect, rows, columns);
	printResults(result, rows, columns);
}


void DataCollect::closeDatabase(void)
{
	sqlite3_close(m_db);
    cout << endl << "Closed DatCol.db" << endl << endl;
}


string DataCollect::createSQLsynchString(string sqlString)
{
	string sqlstart = "INSERT INTO synch VALUES(NULL, \"";
	stringstream sstm;

	sstm << sqlstart << sqlString << "\", " << 0 << ");";
	
	return sstm.str();
}


void DataCollect::insertGPSdata(string gps_time, double latitude, double longitude, double altitude, double speed, double heading)
{
	string sqlstart = "INSERT INTO gps VALUES(NULL,NULL, '";
	string result;
	stringstream sstm;
	
	sstm << sqlstart << gps_time << "', " << latitude << ", " << longitude << ", " << altitude << ", " << speed << ", " << heading <<");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}


void DataCollect::insertCalculations(int offCourse, int steeringConstant, double CTS, double BWP, double DWP, bool TACK)
{
	string sqlstart = "INSERT INTO calc VALUES(NULL, ";
	string result;
	stringstream sstm;
	
	sstm << sqlstart << offCourse << ", " << steeringConstant << ", " << CTS << ", " << BWP << ", " << DWP << ", " << TACK << ");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}


void DataCollect::insertHeadingData(double hdt_heading, double gps_heading)
{
	string sqlstart = "INSERT INTO head VALUES(NULL, ";
	string result;
	stringstream sstm;
	
	sstm << sqlstart << hdt_heading << ", " << gps_heading << ");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}


void DataCollect::insertWPdata(double wp_latitude, double wp_longitude)
{
	string sqlstart = "INSERT INTO wp VALUES(NULL, ";
	string result;
	stringstream sstm;
	
	sstm << sqlstart << wp_latitude << ", " << wp_longitude << ");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}


char** DataCollect::retriveSynchData(int &rows, int &columns)
{
	string sqlSelectString = "SELECT sql_strings FROM synch WHERE synched = 0";
	string sqlUpdateString = "UPDATE synch SET synched = 1 WHERE synched = 0";

	char** result = retriveFromTable(sqlSelectString, rows, columns);
	updateTable(sqlUpdateString);

	return result;
}