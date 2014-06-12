#include <iomanip>
#include "DBHandler.h"


void DBHandler::insertConfig(
	int id,
	int sc_cmd_clse,
	int sc_cmd_beam,
	int sc_cmd_brd,
	int sc_cmd_run,
	int sc_ang_beam,
	int sc_ang_brd,
	int sc_ang_run,

	int rc_cmd_xtrm,
	int rc_cmd_med,
	int rc_cmd_sml,
	int rc_cmd_mid,
	int rc_ang_med,
	int rc_ang_sml,
	int rc_ang_mid,

	int cc_ang_tack,
	int cc_ang_sect,

	string ws_modl,
	int ws_chan,
	string ws_port,
	int ws_baud,
	int ws_buff,

	string mc_port,

	int rs_chan,
	int rs_spd,
	int rs_acc,
	int ss_chan,
	int ss_spd,
	int ss_acc) {

	string sqlstart = "INSERT INTO configs VALUES(";
	stringstream sstm;

	sstm << sqlstart
		<< id
		<< ", " << sc_cmd_clse << ", " << sc_cmd_beam << ", " << sc_cmd_brd << ", " << sc_cmd_run
		<< ", " << sc_ang_beam << ", " << sc_ang_brd << ", " << sc_ang_run
		
		<< ", " << rc_cmd_xtrm << ", " << rc_cmd_med << ", " << rc_cmd_sml << ", " << rc_cmd_mid
		<< ", " << rc_ang_med << ", " << rc_ang_sml << ", " << rc_ang_mid

		<< ", " << cc_ang_tack << ", " << cc_ang_sect

		<< ", '" << ws_modl << "', " << ws_chan << ", '" << ws_port << "', " << ws_baud << ", "  << ws_buff

		<< ", '" << mc_port

		<< "', " << rs_chan << ", " << rs_spd << ", " << rs_acc
		<< ", " << ss_chan << ", " << ss_spd << ", " << ss_acc
		<< ");";

	updateTable(sstm.str());
}


void DBHandler::insertDataLog(
	string gps_time,
	double gps_lat,
	double gps_long,
	double gps_spd,
	double gps_head,
	int gps_sat,
	int sc_cmd,
	int rc_cmd,
	int ss_pos,
	int rs_pos,
	int cc_dtw,
	int cc_btw,
	int cc_cts,
	int cc_tack,
	int ws_dir,
	double ws_spd,
	int ws_tmp,
	string cfg_rev,
	string route_rev,
	int wpt_cur) {

	string sqlstart = "INSERT INTO datalogs VALUES(NULL";
	stringstream sstm;

	sstm << sqlstart
		<< ", '" << gps_time

		<< "', " << std::setprecision(10) << gps_lat << ", " << gps_long << ", " << gps_spd << ", " << gps_head << ", " << gps_sat

		<< ", " << sc_cmd << ", " << rc_cmd << ", " << ss_pos << ", " << rs_pos

		<< ", " << cc_dtw << ", " << cc_btw << ", " << cc_cts << ", " << cc_tack

		<< ", " << ws_dir << ", " << ws_spd << ", " << ws_tmp

		<< ", '" << cfg_rev << "', '" << route_rev << "', " << wpt_cur
		<< ");";

	updateTable(sstm.str());
}


void DBHandler::insertMessageLog(string gps_time, string type, string msg) {
	string sqlstart = "INSERT INTO messagelogs VALUES(NULL";
	string result;
	stringstream sstm;
	sstm << sqlstart
		<< ", '" << gps_time << "', '" << type << "', '" << msg
		<< "');";
	updateTable(sstm.str());
}


void DBHandler::insertWaypoint(int id, double lat, double lon) {
	string sqlstart = "INSERT INTO waypoints VALUES(";
	stringstream sstm;
	sstm << sqlstart
		<< id
		<< ", " << std::setprecision(10) << lat << ", " << lon
		<< ");";
	updateTable(sstm.str());
}


void DBHandler::insertServer(int id, string ship_name, string ship_pwd, string srv_addr) {
	string sqlstart = "INSERT INTO server VALUES(";
	stringstream sstm;
	sstm << sqlstart
		<< id
		<< ", '" << ship_name
		<< "', '" << ship_pwd << "', '" << srv_addr
		<< "');";
	updateTable(sstm.str());
}


void DBHandler::insertState(int id, string cfg_rev, string route_rev, int wpt_cur) {
	string sqlstart = "INSERT INTO state VALUES(";
	stringstream sstm;
	sstm << sqlstart
		<< id
		<< ", '" << cfg_rev << "', '" << route_rev << "', " << wpt_cur
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
