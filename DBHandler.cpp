#include "DBHandler.h"

const char* DBHandler::m_configs =
			"CREATE TABLE configs (id INTEGER PRIMARY KEY AUTOINCREMENT,"
        	"sc_commandclosereach INTEGER,"
        	"sc_commandbeamreach INTEGER,"
        	"sc_commandbroadreach INTEGER,"
        	"sc_commandrunning INTEGER,"
        	"sc_anglebeamreach INTEGER,"
        	"sc_anglebroadreach INTEGER,"
        	"sc_anglerunnning INTEGER,"

        	"rc_commandextreme INTEGER,"
        	"rc_commandmedium INTEGER,"
        	"rc_commandsmall INTEGER,"
        	"rc_commandmidships INTEGER,"
        	"rc_anglemedium INTEGER,"
        	"rc_anglesmall INTEGER,"
        	"rc_anglemidships INTEGER,"

        	"cc_tackangle INTEGER,"
        	"cc_sectorangle INTEGER,"

        	"ws_sensormodel VARCHAR,"
        	"ws_portname VARCHAR,"
        	"ws_baudrate INTEGER,"

        	"mc_portname VARCHAR,"

        	"rs_channel INTEGER,"
        	"rs_speed INTEGER,"
        	"rs_acceleration INTEGER,"
        	"rs_limitmin INTEGER,"
        	"rs_limitmax INTEGER,"

        	"ss_channel INTEGER,"
        	"ss_speed INTEGER,"
        	"ss_acceleration INTEGER,"
        	"ss_limitmin INTEGER,"
        	"ss_limitmax INTEGER,"

        	"gps_portname VARCHAR,"
        	"gps_connectionname VARCHAR);";

void DBHandler::insertConfig(
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
        	string gps_connectionname) {

	string sqlstart = "INSERT INTO configs VALUES(NULL";
	string result;
	stringstream sstm;

	sstm << sqlstart
		<< ", " << sc_commandclosereach << ", " << sc_commandbeamreach << ", " << sc_commandbroadreach
		<< ", " << sc_commandrunning << ", " << sc_anglebeamreach << ", " << sc_anglebroadreach << ", " << sc_anglerunning
		
		<< ", " << rc_commandextreme << ", " << rc_commandmedium << ", " << rc_commandsmall << ", " << rc_commandmidships
		<< ", " << rc_anglemedium << ", " << rc_anglesmall << ", " << rc_anglemidships

		<< ", " << cc_tackangle << ", " << cc_sectorangle

		<< ", '" << ws_sensormodel << "', '" << ws_portname << "', " << ws_baudrate

		<< ", '" << mc_portname

		<< "', " << rs_channel << ", " << rs_speed << ", " << rs_acceleration
		<< ", " << rs_limitmin << ", " << rs_limitmax

		<< ", " << ss_channel << ", " << ss_speed << ", " << ss_acceleration
		<< ", " << ss_limitmin << ", " << ss_limitmax

		<< ", '" << gps_portname << "', '" << gps_connectionname

		<< "');";

	result = sstm.str();

	updateTable(result);
//	updateTable(createSQLsynchString(result));
}

const char* DBHandler::m_logs =
			"CREATE TABLE logs (id INTEGER PRIMARY KEY AUTOINCREMENT,"

			"logtime TIMESTAMP,"

        	"sc_command INTEGER,"

        	"rc_command INTEGER,"

        	"cc_dtw INTEGER,"
        	"cc_btw INTEGER,"
        	"cc_cts INTEGER,"
        	"cc_twd INTEGER,"
        	"cc_tack INTEGER,"

        	"ws_buffersize INTEGER,"
        	"ws_sensormodel VARCHAR,"
        	"ws_direction INTEGER,"
        	"ws_speed INTEGER,"
        	"ws_temperature INTEGER,"

        	"rs_position INTEGER,"

        	"ss_position INTEGER,"

			"gps_timestamp VARCHAR,"
			"gps_latitude DOUBLE,"
			"gps_longitude DOUBLE,"
			"gps_altitude DOUBLE,"
			"gps_speed DOUBLE,"
			"gps_heading DOUBLE,"
			"gps_mode INTEGER,"
			"gps_satellites INTEGER);";

void DBHandler::insertLog(
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
			int gps_satellites) {

	string sqlstart = "INSERT INTO logs VALUES(NULL, NULL";
	string result;
	stringstream sstm;

	sstm << sqlstart
		<< ", " << sc_command

		<< ", " << rc_command

		<< ", " << cc_dtw << ", " << cc_btw << ", " << cc_cts << ", " << cc_twd << ", " << cc_tack
		
		<< ", " << ws_buffersize << ", '" << ws_sensormodel << "', " << ws_direction
		<< ", " << ws_speed << "', " << ws_temperature

		<< ", " << rs_position

		<< ", " << ss_position

		<< ", '" << gps_timestamp << "', " << gps_latitude << ", '" << gps_longitude
		<< ", " << gps_altitude << ", " << gps_speed << ", '" << gps_heading
		<< ", " << gps_mode << ", " << gps_satellites

		<< ");";

	result = sstm.str();

	updateTable(result);
//	updateTable(createSQLsynchString(result));
}


const char* DBHandler::m_waypoints =
			"CREATE TABLE waypoints (id INTEGER PRIMARY KEY AUTOINCREMENT,"

			"routeid INTEGER,"

			"latitude DOUBLE,"
			"longitude DOUBLE);";

void DBHandler::insertWaypoint(
        	int routeid,

        	int latitude,
        	int longitude) {

	string sqlstart = "INSERT INTO waypoints VALUES(NULL";
	string result;
	stringstream sstm;

	sstm << sqlstart
		<< ", " << routeid

		<< ", " << latitude << ", " << longitude
		<< ");";

	result = sstm.str();

	updateTable(result);
//	updateTable(createSQLsynchString(result));
}

DBHandler::DBHandler(void) {
}

DBHandler::~DBHandler(void) {
}

bool DBHandler::openDatabase(void) {
	//char*error;
	//cout << "Opening MyDb.db ..." << endl;

	m_rc = sqlite3_open("DatCol.db", &m_db);

	if (m_rc) {
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(m_db)
				<< endl << endl;
		sqlite3_close (m_db);
		return false;
	}

	//return a bool
	else {
		cout << "Opened DatCol.db." << endl << endl;
		return true;
	}
}

void DBHandler::closeDatabase(void) {
	sqlite3_close (m_db);
	cout << endl << "Closed DatCol.db" << endl << endl;
}

bool DBHandler::createTables(void) {

	for (int i = 0; i < 3; i++) {
		if (i == 0)
			m_rc = sqlite3_exec(m_db, m_configs, NULL, NULL, &m_error);

		else if (i == 1)
			m_rc = sqlite3_exec(m_db, m_logs, NULL, NULL, &m_error);

		else if (i == 2)
			m_rc = sqlite3_exec(m_db, m_waypoints, NULL, NULL, &m_error);


		if (m_rc) {
			cerr << "Error executing SQLite3 statement: "
					<< sqlite3_errmsg(m_db) << endl << endl;
			sqlite3_free (m_error);

			return false;
		}

		else
			cout << "Created table no: " << i + 1 << endl << endl;
	}

	return true;
}

bool DBHandler::updateTable(string sqlINSERT) {
	m_rc = sqlite3_exec(m_db, sqlINSERT.c_str(), NULL, NULL, &m_error);

	if (m_rc) {
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(m_db)
				<< endl << endl;
		sqlite3_free (m_error);
		return false;
	} else {
		//cout << "Inserted a value into a given table." << endl << endl;
		return true;
	}
}

char** DBHandler::retriveFromTable(string sqlSELECT, int &rows,
		int &columns) {
	char **results = NULL;

	sqlite3_get_table(m_db, sqlSELECT.c_str(), &results, &rows, &columns,
			&m_error);

	if (m_rc) {
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(m_db)
				<< endl << endl;
		sqlite3_free (m_error);

		return NULL;
	}

	else
		return results;
}

/*void DBHandler::printResults(char **results, int rows, int columns) {
	// Display Table
	for (int rowCtr = 0; rowCtr <= rows; ++rowCtr) {
		for (int colCtr = 0; colCtr < columns; ++colCtr) {
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
		if (0 == rowCtr) {
			for (int colCtr = 0; colCtr < columns; ++colCtr) {
				cout.width(12);
				cout.setf(ios::left);
				cout << "------------ ";
			}
			cout << endl;
		}
	}
	cout << endl;
}

void DBHandler::printSelect(string sqlSelect) {
	char** result;
	int rows, columns;

	result = retriveFromTable(sqlSelect, rows, columns);
	printResults(result, rows, columns);
}


string DBHandler::createSQLsynchString(string sqlString) {
	string sqlstart = "INSERT INTO synch VALUES(NULL, \"";
	stringstream sstm;

	sstm << sqlstart << sqlString << "\", " << 0 << ");";

	return sstm.str();
}

void DBHandler::insertGPSdata(string gps_time, double latitude,
		double longitude, double altitude, double speed, double heading) {
	string sqlstart = "INSERT INTO gps VALUES(NULL,NULL, '";
	string result;
	stringstream sstm;

	sstm << sqlstart << gps_time << "', " << latitude << ", " << longitude
			<< ", " << altitude << ", " << speed << ", " << heading << ");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}

void DBHandler::insertCalculations(int offCourse, int steeringConstant,
		double CTS, double BWP, double DWP, bool TACK) {
	string sqlstart = "INSERT INTO calc VALUES(NULL, ";
	string result;
	stringstream sstm;

	sstm << sqlstart << offCourse << ", " << steeringConstant << ", " << CTS
			<< ", " << BWP << ", " << DWP << ", " << TACK << ");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}

void DBHandler::insertHeadingData(double hdt_heading, double gps_heading) {
	string sqlstart = "INSERT INTO head VALUES(NULL, ";
	string result;
	stringstream sstm;

	sstm << sqlstart << hdt_heading << ", " << gps_heading << ");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}

void DBHandler::insertWPdata(double wp_latitude, double wp_longitude) {
	string sqlstart = "INSERT INTO wp VALUES(NULL, ";
	string result;
	stringstream sstm;

	sstm << sqlstart << wp_latitude << ", " << wp_longitude << ");";
	result = sstm.str();

	updateTable(result);
	updateTable(createSQLsynchString(result));
}

char** DBHandler::retriveSynchData(int &rows, int &columns) {
	string sqlSelectString = "SELECT sql_strings FROM synch WHERE synched = 0";
	string sqlUpdateString = "UPDATE synch SET synched = 1 WHERE synched = 0";

	char** result = retriveFromTable(sqlSelectString, rows, columns);
	updateTable(sqlUpdateString);

	return result;
}
*/