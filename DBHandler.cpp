#include "DBHandler.h"
#include "JSON.h"
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cstdio>
#include "models/SystemStateModel.h"
#include "models/WaypointModel.h"
#include "models/PositionModel.h"


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
	SystemStateModel systemState,
	int sail_servo_position,
	int rudder_servo_position,
	double course_calculation_distance_to_waypoint,
	double course_calculation_bearing_to_waypoint,
	double course_calculation_course_to_steer,
	bool course_calculation_tack,
	bool course_calculation_going_starboard,
	int waypoint_id,
	double true_wind_direction_calc) {

	if(m_db == NULL) {
		throw "DBHandler::insertDataLog(), no db connection";
	}

	std::stringstream gpsValues;
	std::stringstream courseCalculationValues;
	std::stringstream compassModelValues;
	std::stringstream systemValues;
	std::stringstream windsensorValues;

	gpsValues << std::setprecision(10) << "'"
		<< systemState.gpsModel.timestamp << "', "
		<< systemState.gpsModel.positionModel.latitude << ", "
		<< systemState.gpsModel.positionModel.longitude << ", "
		<< systemState.gpsModel.speed << ", "
		<< systemState.gpsModel.heading << ", "
		<< systemState.gpsModel.satellitesUsed;

	courseCalculationValues << std::setprecision(10)
		<< course_calculation_distance_to_waypoint << ", "
		<< course_calculation_bearing_to_waypoint << ", "
		<< course_calculation_course_to_steer << ", "
		<< course_calculation_tack << ", "
		<< course_calculation_going_starboard;

	compassModelValues << std::setprecision(10)
		<< systemState.compassModel.heading << ", "
		<< systemState.compassModel.pitch << ", "
		<< systemState.compassModel.roll;

	windsensorValues << std::setprecision(10)
		<< systemState.windsensorModel.direction << ", "
		<< systemState.windsensorModel.speed << ", "
		<< systemState.windsensorModel.temperature;
	printf("GPS GMT + 3: %s GPS UTC: %s\n",systemState.gpsModel.timestamp.c_str(),systemState.gpsModel.utc_timestamp.c_str());
	int windsensorId = insertLog("windsensor_datalogs",windsensorValues.str());
	int gpsId = insertLog("gps_datalogs",gpsValues.str());
	int courceCalculationId = insertLog("course_calculation_datalogs",courseCalculationValues.str());
	int compassModelId = insertLog("compass_datalogs",compassModelValues.str());

	systemValues << std::setprecision(10)
		<< gpsId << ", "
		<< courceCalculationId << ", "
		<< windsensorId << ", "
		<< compassModelId << ", "
		<< systemState.sail << ", "
		<< systemState.rudder << ", "
		<< sail_servo_position << ", "
		<< rudder_servo_position << ", "
		<< waypoint_id << ", "
		<< true_wind_direction_calc;
	m_latestDataLogId = insertLog("system_datalogs", systemValues.str());
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

std::string DBHandler::getLogs() {
	std::vector<std::string> values;
	std::vector<std::string> columnNames;
	//Get IDs related to  the current system_datalogs_id
	std::string courseCalculationId = retriveCell("system_datalogs",std::to_string(m_latestDataLogId),"course_calculation_id");
	std::string windsensorId = retriveCell("system_datalogs",std::to_string(m_latestDataLogId),"windsensor_id");
	std::string compassModelId = retriveCell("system_datalogs",std::to_string(m_latestDataLogId),"compass_id");
	std::string gpsId = retriveCell("system_datalogs",std::to_string(m_latestDataLogId),"gps_id");
	// Get required fields from datalogs

	std::stringstream ss;

	try {
		ss << getDataLogRow("id,sail_command_sail_state,rudder_command_rudder_state,sail_servo_position,rudder_servo_position,waypoint_id,true_wind_direction_calc",
									"system_datalogs",std::to_string(m_latestDataLogId),values,columnNames) << ","
		<< getDataLogRow("time,latitude,longitude,speed,heading,satellites_used",
									"gps_datalogs",gpsId,values,columnNames) << ","
		<< getDataLogRow("distance_to_waypoint,bearing_to_waypoint,course_to_steer,tack,going_starboard",
									"course_calculation_datalogs", courseCalculationId,values,columnNames) << ","
		<< getDataLogRow("heading,pitch,roll",
									"compass_datalogs",compassModelId,values,columnNames) << ","
		<< getDataLogRow("direction,speed,temperature",
								"windsensor_datalogs",windsensorId,values,columnNames);
		} catch(const char * error) {
			std::cout << error << std::endl;
			m_logger.error(error);
		}

	// getDataLogs(std::to_string(m_latestDataLogId),values,columnNames);

	// JSON data formatting
	// JSONArray datalogs;
	// datalogs.setName("datalogs");
	//
	// JSONData data;
	//
	// for (auto i = 0; i < values.size(); i++) {
	// 	data.add(columnNames.at(i),values.at(i));
	// }
	//
	// JSONBlock block;
	// block.add(data.toString());
	// datalogs.add(block.toString());
	//
	// JSONBlock main;
	// main.add(datalogs.toString());

	// std::string json = formatDatalogsToJson("systemDatalogs",values,columnNames);
	// std::cout << json << std::endl;
	JSONBlock main;
	main.add(ss.str());
	std::cout << main.toString() << std::endl;
	return main.toString();
}


void DBHandler::removeLogs(std::string lines) {
	JSONDecode decoder;
	decoder.addJSON(lines);
	while (decoder.hasNext()) {
		// std::cout << "tab: " << decoder.getData("tab") << ", id: " << decoder.getData("id_system") << "\n";
		queryTable("DELETE FROM system_datalogs WHERE id = " + decoder.getData("id_system") + ";");
	}
}


void DBHandler::deleteRow(std::string table, std::string id) {
	queryTable("DELETE FROM " + table + " WHERE id = " + id + ";");
}

void DBHandler::insert(std::string table, std::string fields, std::string values)
{
	queryTable("INSERT INTO " + table + "(" + fields +
		") VALUES(" + values + ");");
}

void DBHandler::insertScan(std::string waypoint_id, PositionModel position, float temperature, std::string timestamp)
{
	//std::string waypoint_id = getMinIdFromTable("waypoints");

	std::string i = "null", j = "null";

	try {
		i = retriveCell("waypoint_index", waypoint_id, "i");
		j = retriveCell("waypoint_index", waypoint_id, "j");
	} catch (const char * error) {
		m_logger.error(error);
	}

	std::ostringstream fields;
	fields << "waypoint_id,"
		<< "time_UTC,"
		<< "latitude,"
		<< "longitude,"
		<< "air_temperature,"
		<< "i,"
		<< "j";

	std::ostringstream values;
	values << waypoint_id << ",'"
		<< timestamp << "',"
		<< position.latitude << ","
		<< position.longitude << ","
		<< temperature << ","
		<< i << ","
		<< j;

	insert("scanning_measurements", fields.str(), values.str());
}


////////////////////////////////////////////////////////////////////
// private helpers
////////////////////////////////////////////////////////////////////

std::string DBHandler::getDataLogRow(std::string select, std::string table, std::string id ,std::vector<std::string> &values, std::vector<std::string> &columnNames) {
	int rows = 0, columns = 0;
	char** results;

	try {
		results = retriveFromTable("SELECT " + select + " FROM " + table + " WHERE ID = " + id + ";", rows, columns);
	} catch(const char * error) {
		std::cout << "error in DBHandler::getDataLogRow: " << error << std::endl;
	}

	for(int i = 0; i < columns*2; ++i) {
		if(i < columns)
			columnNames.push_back(results[i]);
		else
			values.push_back(results[i]);
	}
	std::string result = formatDatalogsToJson(table,values, columnNames);
	values.clear();
	columnNames.clear();
	return result;
}

int DBHandler::insertLog(std::string table, std::string values) {
	std::stringstream ss;
	ss << "INSERT INTO " << table << " VALUES(NULL, " << values << ");";

	// std::cout << ss.str() << std::endl;

	try {
		queryTable(ss.str());
		m_latestDataLogId = sqlite3_last_insert_rowid(m_db);
	} catch(const char * error) {
		std::cout << "error in DBHandler::insertLog: " << error << std::endl;
	}
	return m_latestDataLogId;
}

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

		sqlite3_get_table(m_db, sqlSELECT.c_str(), &results, &rows, &columns, &m_error);

		if (m_error != NULL) {
			std::stringstream errorStream;
			errorStream << "DBHandler::retrieveFromTable(), " << sqlite3_errmsg(m_db);
			sqlite3_free(m_error);
			std::cout << errorStream.str().c_str() << std::endl;
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

	int rows, columns;
    char** results;
    results = retriveFromTable("SELECT MIN(id) FROM waypoints WHERE harvested = 0;", rows, columns);
    //std::cout << "result |" << rows << ":" << columns << "|" << results << std::endl;
    if (rows * columns < 1 || results[1] == '\0') {
    	waypointModel.id = "";
    }
    else {
    	waypointModel.id = results[1];
    }

	if(!waypointModel.id.empty())
	{
		waypointModel.positionModel.latitude = atof(retriveCell("waypoints", waypointModel.id, "lat").c_str());
		waypointModel.positionModel.longitude = atof(retriveCell("waypoints", waypointModel.id, "lon").c_str());
		waypointModel.radius = retriveCellAsInt("waypoints", waypointModel.id, "radius");
		waypointModel.declination = retriveCellAsInt("waypoints", waypointModel.id, "declination");

		results = retriveFromTable("SELECT time FROM waypoint_stationary WHERE id = " +
			waypointModel.id + ";", rows, columns);

		if (rows * columns < 1 || results[1] == '\0') {
			waypointModel.time = 0;
		}
		else {
			waypointModel.time = retriveCellAsInt("waypoint_stationary", waypointModel.id, "time");
		}
	}

}


void DBHandler::changeOneValue(std::string table, std::string id,std::string newValue, std::string colName){

	queryTable("UPDATE " + table + " SET "+ colName + " = "+ newValue +" WHERE id = " + id +";");

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
    	return "";
    } else {
    	return results[1];
    }
}

std::string DBHandler::formatDatalogsToJson(std::string logName,std::vector<std::string> values, std::vector<std::string> columnNames) {
	JSONArray datalogs;
	datalogs.setName(logName);

	JSONData data;

	for (auto i = 0; i < values.size(); i++) {
		data.add(columnNames.at(i),values.at(i));
	}

	JSONBlock block;
	block.add(data.toString());
	datalogs.add(block.toString());

	return datalogs.toString();
}
