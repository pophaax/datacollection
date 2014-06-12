#include <iomanip>
#include "DBHandler.h"


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

	updateTable(sstm.str());
}


void DBHandler::insertDataLog(
        	int sc_command,

        	int rc_command,

        	int cc_dtw,
        	int cc_btw,
        	int cc_cts,
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

	string sqlstart = "INSERT INTO datalogs VALUES(NULL, NULL";
	stringstream sstm;

	sstm << sqlstart
		<< ", " << sc_command

		<< ", " << rc_command

		<< ", " << cc_dtw << ", " << cc_btw << ", " << cc_cts << ", " << cc_tack
		
		<< ", " << ws_buffersize << ", '" << ws_sensormodel << "', " << ws_direction
		<< ", " << ws_speed << ", " << ws_temperature

		<< ", " << rs_position

		<< ", " << ss_position

		<< ", '" << gps_timestamp << "', " << std::setprecision(6) << gps_latitude << ", " << gps_longitude
		<< ", " << gps_altitude << ", " << gps_speed << ", " << gps_heading
		<< ", " << gps_mode << ", " << gps_satellites

		<< ");";

	updateTable(sstm.str());
}


void DBHandler::insertErrorLog(string error) {
	string sqlstart = "INSERT INTO errorlogs VALUES(NULL, NULL";
	string result;
	stringstream sstm;
	sstm << sqlstart
		<< ", '" << error << "');";
	updateTable(sstm.str());
}


void DBHandler::insertWaypoint(
        	int routeid,

        	double latitude,
        	double longitude) {

	string sqlstart = "INSERT INTO waypoints VALUES(NULL";
	stringstream sstm;

	sstm << sqlstart
		<< ", " << routeid

		<< ", " << latitude << ", " << longitude
		<< ");";

	updateTable(sstm.str());
}

DBHandler::DBHandler(void) {
}

DBHandler::~DBHandler(void) {
}

void DBHandler::openDatabase(string fileName) {
	m_rc = sqlite3_open(fileName.c_str(), &m_db);

	if (m_rc) {
		stringstream errorStream;
		errorStream << "DBHandler::openDatabase(), " << sqlite3_errmsg(m_db);
		sqlite3_free(m_error);

		throw errorStream.str().c_str();
	}
}

void DBHandler::closeDatabase(void) {
	sqlite3_close(m_db);
}


void DBHandler::updateTable(string sqlINSERT) {
	m_rc = sqlite3_exec(m_db, sqlINSERT.c_str(), NULL, NULL, &m_error);

	if (m_rc) {
		stringstream errorStream;
		errorStream << "DBHandler::updateTable(), " << sqlite3_errmsg(m_db);
		sqlite3_free(m_error);

		throw errorStream.str().c_str();
	}
}

char** DBHandler::retriveFromTable(string sqlSELECT, int &rows,
		int &columns) {
	char **results = NULL;

	sqlite3_get_table(m_db, sqlSELECT.c_str(), &results, &rows, &columns,
			&m_error);

	if (m_rc) {
		stringstream errorStream;
		errorStream << "DBHandler::retrieveFromTable(), " << sqlite3_errmsg(m_db);
		sqlite3_free(m_error);

		throw errorStream.str().c_str();
	}

	return results;
}

string DBHandler::retriveCell(string table, string id, string column) {

 	stringstream sstm;
	sstm << "SELECT " << column << " FROM " << table << " WHERE id=" << id << ";";

	int rows, columns;
    char** results;
    results = retriveFromTable(sstm.str(), rows, columns);

    if (columns < 1) {
		stringstream errorStream;
		errorStream << "DBHandler::retriveCell(), no columns from Query: " << sstm.str();
    	throw errorStream.str().c_str();
    }

    if (rows < 1) {
		stringstream errorStream;
		errorStream << "DBHandler::retriveCell(), no rows from Query: " << sstm.str();
    	throw errorStream.str().c_str();
    }

    return results[1];
}
