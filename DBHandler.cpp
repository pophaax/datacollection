#include "DBHandler.h"
#include "JSON.h"
#include <iomanip>
#include <string>
#include <cstdlib>



void DBHandler::insertDataLog(
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
	int wpt_cur) {

	string sqlstart = "INSERT INTO datalogs VALUES(NULL";
	stringstream sstm;

	sstm << sqlstart
		<< ", '" << gps_time
		<< "', " << std::setprecision(10) << gps_lat << ", " << gps_lon << ", " << gps_spd << ", " << gps_head << ", " << gps_sat
		<< ", " << sc_cmd << ", " << rc_cmd << ", " << ss_pos << ", " << rs_pos
		<< ", " << cc_dtw << ", " << cc_btw << ", " << cc_cts << ", " << cc_tack
		<< ", " << ws_dir << ", " << ws_spd << ", " << ws_tmp
		<< ", "  << wpt_cur
		<< ");";

	queryTable(sstm.str());
	m_latestDataLogId = sqlite3_last_insert_rowid(m_db);
}


void DBHandler::insertMessageLog(string gps_time, string type, string msg) {
	string sqlstart = "INSERT INTO messages VALUES(NULL";
	string result;
	stringstream sstm;
	sstm << sqlstart
		<< ", '" << gps_time << "', '" << type << "', '" << msg << "', " << (m_latestDataLogId)
		<< ");";
	queryTable(sstm.str());
}



bool DBHandler::revChanged(string toCheck, string serverRevs) {
	JSONDecode decoder;
	decoder.addJSON(serverRevs);
	string serverConfig, localConfig;
	if (decoder.hasNext()) {
		serverConfig = decoder.getData(toCheck);
	} else {
		throw "DBHandler::configChanged(), coudn't find "+ toCheck + "in JSONdata.";
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


void DBHandler::updateTable(string table, string data) {
	JSONDecode decoder;
	decoder.addJSON(data);
	vector<string> types = getColumnInfo("type", table);
	vector<string> columns = getColumnInfo("name", table);

	clearTable(table);
	while(decoder.hasNext()) {

		if (decoder.getSize() != columns.size()) {
			throw ("DBHandler::updateTable(), decoder and columns size mismatch for table: " + table).c_str();
		}

		std::string values = "";
		for (int i = 0; i < columns.size(); i++) {
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

vector<string> DBHandler::getColumnInfo(string info, string table) {
	int rows, columns;
    char** results;
    results = retriveFromTable("PRAGMA table_info(" + table + ");", rows, columns);
    vector<string> types;
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


DBHandler::DBHandler(void) {
	m_latestDataLogId = 1;
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


void DBHandler::queryTable(string sqlINSERT) {
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


vector<string> DBHandler::getTableIds(string table) {

 	stringstream sstm;
	sstm << "SELECT id FROM " << table << ";";

	int rows, columns;
    char** results;
    results = retriveFromTable(sstm.str(), rows, columns);

    vector<string> ids;
    for (int i = 1; i <= rows; i++) {
    	ids.push_back(results[i]);
    }

    return ids;
}


void DBHandler::clearTable(string table) {
	stringstream sstm;
	sstm << "DELETE FROM " << table << ";";
	queryTable(sstm.str());
}


string DBHandler::getLogs() {

	vector<string> logIds;
	logIds = getTableIds("datalogs");
	JSONArray datalogs;
	datalogs.setName("datalogs");
	vector<string> datalogColumns = getColumnInfo("name", "datalogs");

	for (unsigned int i = 0; i < logIds.size(); i++) {
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

	JSONBlock main;
	if(logIds.size() > 0)
		main.add(datalogs.toString());
	if(msgIds.size() > 0)
		main.add(messages.toString());

	return main.toString();
}


void DBHandler::removeLogs(string lines) {
	JSONDecode decoder;
	decoder.addJSON(lines);
	while (decoder.hasNext()) {
		queryTable("DELETE FROM " + decoder.getData("tab") + " WHERE id = " + decoder.getData("id") + ";");
	}
}


string DBHandler::getMinIdFromTable(string table) {
 	stringstream sstm;
	sstm << "SELECT MIN(id) FROM " << table << ";";

	int rows, columns;
    char** results;
    results = retriveFromTable(sstm.str(), rows, columns);

    if (columns < 1) {
		stringstream errorStream;
		errorStream << "DBHandler::getMinIdFromtable(), no columns from Query: " << sstm.str();
    	throw errorStream.str().c_str();
    }

    if (rows < 1) {
		stringstream errorStream;
		errorStream << "DBHandler::getMinIdFromtable(), no rows from Query: " << sstm.str();
    	throw errorStream.str().c_str();
    }

    return results[1];

}


void DBHandler::deleteRow(string table, string id) {
	queryTable("DELETE FROM " + table + " WHERE id = " + id + ";");
}
