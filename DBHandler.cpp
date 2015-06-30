#include "DBHandler.h"
#include "JSON.h"
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cstdio>


DBHandler::DBHandler(void) :
	m_db(NULL)
{
	m_latestDataLogId = 1;
}


DBHandler::~DBHandler(void) {
	closeDatabase();
}


void DBHandler::openDatabase(std::string fileName) {
	
	// check if file exists
	FILE* db_file = fopen(fileName.c_str(), "r");
	if (!db_file) {
		std::string error = "DBHandler::openDatabase(), " + fileName +
			" not found.";
		throw error.c_str();
	}
	fclose(db_file);

	m_rc = sqlite3_open(fileName.c_str(), &m_db);

	if (m_rc) {
		std::stringstream errorStream;
		errorStream << "DBHandler::openDatabase(), " << sqlite3_errmsg(m_db);

		throw errorStream.str().c_str();
	}
}


void DBHandler::closeDatabase(void) {
	sqlite3_close(m_db);
	m_db = NULL;
}


void DBHandler::insertDataLog(
	std::string gps_time,
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
	float ws_spd,
	float ws_tmp,
	int wpt_cur,
	int cps_head,
	int cps_pitch,
	int cps_roll) {

	std::stringstream sstm;

	sstm << "INSERT INTO datalogs VALUES(NULL"
		<< ", '" << gps_time
		<< "', " << std::setprecision(10) << gps_lat << ", " << gps_lon << ", " << gps_spd << ", " << gps_head << ", " << gps_sat
		<< ", " << sc_cmd << ", " << rc_cmd << ", " << ss_pos << ", " << rs_pos
		<< ", " << cc_dtw << ", " << cc_btw << ", " << cc_cts << ", " << cc_tack
		<< ", " << ws_dir << ", " << ws_spd << ", " << ws_tmp << ", " << wpt_cur
		<< ", " << cps_head << ", " << cps_pitch << ", " << cps_roll
		<< ");";

	std::stringstream outp;
	outp << "'" << gps_time << "', " << std::setprecision(10) << gps_lat << ", " << gps_lon << ", " << gps_spd << ", " << gps_head << ", " << gps_sat
	<< ", " << sc_cmd << ", " << rc_cmd << ", " << ss_pos << ", " << rs_pos
	<< ", " << cc_dtw << ", " << cc_btw << ", " << cc_cts << ", " << cc_tack
	<< ", " << ws_dir << ", " << ws_spd << ", " << ws_tmp << ", "  << wpt_cur
	<< ", " << cps_head << ", " << cps_pitch << ", " << cps_roll;
	printf("%s\n",outp.str().c_str());

	queryTable(sstm.str());
	m_latestDataLogId = sqlite3_last_insert_rowid(m_db);
}


void DBHandler::insertMessageLog(std::string gps_time, std::string type, std::string msg) {
	std::string result;
	std::stringstream sstm;
	sstm << "INSERT INTO messages VALUES(NULL"
		<< ", '" << gps_time << "', '" << type << "', '" << msg << "', " << (m_latestDataLogId)
		<< ");";
	queryTable(sstm.str());
}



bool DBHandler::revChanged(std::string toCheck, std::string serverRevs) {
	JSONDecode decoder;
	decoder.addJSON(serverRevs);
	std::string serverConfig, localConfig;
	if (decoder.hasNext()) {
		serverConfig = decoder.getData(toCheck);
	} else {
		throw ("DBHandler::revChanged(), coudn't find "+ toCheck + "in JSONdata.").c_str();
	}
	if (getTableIds("state").size() == 0) {
		return true;
	} else {
		localConfig = retriveCell("state", "1", toCheck);
	}

	if ( serverConfig.compare(localConfig) != 0 ) {
		return true;
	}
	return false;
}


void DBHandler::updateTable(std::string table, std::string data) {
	JSONDecode decoder;
	decoder.addJSON(data);
	std::vector<std::string> types = getColumnInfo("type", table);
	std::vector<std::string> columns = getColumnInfo("name", table);

	clearTable(table);
	while(decoder.hasNext()) {

		if (decoder.getSize() != columns.size()) {
			throw ("DBHandler::updateTable(), decoder and columns size mismatch for table: " + table).c_str();
		}

		std::string values = "";
		for (unsigned int i = 0; i < columns.size(); i++) {
			if (i > 0) {
				values += ", ";
			}
			if (types[i].compare("VARCHAR") == 0) {
				values = values + "'" + decoder.getData(columns[i]) + "'";
			} else {
				values += decoder.getData(columns[i]);
			}
		}
		queryTable("INSERT INTO " + table + " VALUES(" + values + ");");
	}
}


std::string DBHandler::retriveCell(std::string table, std::string id, std::string column) {
	std::string query = "SELECT " + column + " FROM " + table +" WHERE id=" + id + ";";

	int rows, columns;
    char** results;
    results = retriveFromTable(query, rows, columns);

    if (columns < 1) {
		std::stringstream errorStream;
		errorStream << "DBHandler::retriveCell(), no columns from Query: " << query;
    	throw errorStream.str().c_str();
    }

    if (rows < 1) {
		std::stringstream errorStream;
		errorStream << "DBHandler::retriveCell(), no rows from Query: " << query;
    	throw errorStream.str().c_str();
    }

    return results[1];
}


int DBHandler::retriveCellAsInt(std::string table, std::string id, std::string column) {
	try {
		return atoi(retriveCell(table, id, column).c_str());
	}
	catch (const char *  e) {
		std::cout << "exception thrown in retriveCellAsInt, is returned cell a int?  " << e << std::endl;
		return 0;
	}
	
}


void DBHandler::clearTable(std::string table) {
	queryTable("DELETE FROM " + table + ";");
}


std::string DBHandler::getLogs() {

	std::vector<std::string> logIds;
	logIds = getTableIds("datalogs");
	JSONArray datalogs;
	datalogs.setName("datalogs");
	std::vector<std::string> datalogColumns = getColumnInfo("name", "datalogs");

/*	for (unsigned int i = 0; i < logIds.size(); i++) {
		JSONData data;

		for (unsigned int j = 0; j < datalogColumns.size(); j++) {
			data.add(datalogColumns[j], retriveCell("datalogs", logIds[i], datalogColumns[j]));
		}

		vector<string> stateColumns = getColumnInfo("name", "state");
		for (unsigned int j = 0; j < stateColumns.size(); j++) {
			data.add(stateColumns[j], retriveCell("state", "1", stateColumns[j]));
		}

		JSONBlock block;
		block.add(data.toString());
		datalogs.add(block.toString());
	}

	vector<string> msgIds;
	msgIds = getTableIds("messages");
	JSONArray messages;
	messages.setName("messages");
	vector<string> messageColumns = getColumnInfo("name", "messages");

	for (unsigned int i = 0; i < msgIds.size(); i++) {
		JSONData data;
		for (unsigned int j = 0; j < messageColumns.size(); j++) {
			data.add(messageColumns[j], retriveCell("messages", msgIds[i], messageColumns[j]));
		}
		JSONBlock block;
		block.add(data.toString());
		messages.add(block.toString());
	}
*/
	std::stringstream lastId;
	lastId << m_latestDataLogId;
	JSONData data;
	for (unsigned int j = 0; j < datalogColumns.size(); j++) {
		data.add(datalogColumns[j], retriveCell("datalogs", lastId.str(), datalogColumns[j]));
	}
	JSONBlock block;
	block.add(data.toString());
	datalogs.add(block.toString());

	JSONBlock main;
	if(logIds.size() > 0)
		main.add(datalogs.toString());
/*	if(msgIds.size() > 0)
		main.add(messages.toString());
*/

	return main.toString();
}


void DBHandler::removeLogs(std::string lines) {
	JSONDecode decoder;
	decoder.addJSON(lines);
	while (decoder.hasNext()) {
std::cout << "tab: " << decoder.getData("tab") << ", id: " << decoder.getData("id") << "\n";
		queryTable("DELETE FROM " + decoder.getData("tab") + " WHERE id = " + decoder.getData("id") + ";");
	}
}


std::string DBHandler::getMinIdFromTable(std::string table) {
	int rows, columns;
    char** results;
    results = retriveFromTable("SELECT MIN(id) FROM " + table + ";", rows, columns);
    //std::cout << "result |" << rows << ":" << columns << "|" << results << std::endl;
    if (rows * columns < 1) {
    	return "";
    }

    if(results[1] == '\0') {
    	return "noll";
    } else {
    	return results[1];
    }
}


void DBHandler::deleteRow(std::string table, std::string id) {
	queryTable("DELETE FROM " + table + " WHERE id = " + id + ";");
}





////////////////////////////////////////////////////////////////////
// private helpers
////////////////////////////////////////////////////////////////////



void DBHandler::queryTable(std::string sqlINSERT) {
	if (m_db != NULL) {

		m_rc = sqlite3_exec(m_db, sqlINSERT.c_str(), NULL, NULL, &m_error);

		if (m_error != NULL) {
			std::stringstream errorStream;
			errorStream << "DBHandler::queryTable(), " << sqlite3_errmsg(m_db);
			sqlite3_free(m_error);

			throw errorStream.str().c_str();
		}
	}
	else {
		throw "DBHandler::queryTable(), no db connection";
	}
}


char** DBHandler::retriveFromTable(std::string sqlSELECT, int &rows, int &columns) {
	char **results = NULL;

	if (m_db != NULL) {

		sqlite3_get_table(m_db, sqlSELECT.c_str(), &results, &rows, &columns,
				&m_error);

		if (m_error != NULL) {
			std::stringstream errorStream;
			errorStream << "DBHandler::retrieveFromTable(), " << sqlite3_errmsg(m_db);
			sqlite3_free(m_error);

			throw errorStream.str().c_str();
		}
	}
	else {
		throw "DBHandler::retrieveFromTable(), no db connection";
	}

	return results;
}


std::vector<std::string> DBHandler::getTableIds(std::string table) {
	int rows, columns;
    char** results;
    results = retriveFromTable("SELECT id FROM " + table + ";", rows, columns);

    std::vector<std::string> ids;
    for (int i = 1; i <= rows; i++) {
    	ids.push_back(results[i]);
    }

    return ids;
}



std::vector<std::string> DBHandler::getColumnInfo(std::string info, std::string table) {
	int rows, columns;
    char** results;
    results = retriveFromTable("PRAGMA table_info(" + table + ");", rows, columns);
    std::vector<std::string> types;
    int infoIndex = 0;
    for (int i = 0; i < columns; i++) {
    	if (std::string(info).compare(results[i]) == 0) {
    		infoIndex = i;
    	}
    }

	for (int i = 1; i < rows+1; i++) {
		types.push_back(results[i * columns + infoIndex]);
	}
    return types;
}

void DBHandler::getWaypointFromTable(WaypointModel &waypointModel){

	waypointModel.id = getMinIdFromTable("waypoints");
	waypointModel.positionModel.latitude = atof(retriveCell("waypoints", waypointModel.id, "lat").c_str());
	waypointModel.positionModel.longitude = atof(retriveCell("waypoints", waypointModel.id, "lon").c_str());
	waypointModel.radius = retriveCellAsInt("waypoints",waypointModel.id, "radius");

}