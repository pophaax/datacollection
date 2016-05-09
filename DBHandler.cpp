#include "DBHandler.h"
#include "JSON.h"
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cstdio>
#include "models/SystemStateModel.h"
#include "models/WaypointModel.h"
#include "models/PositionModel.h"


DBHandler::DBHandler(std::string filePath) :
	m_filePath(filePath)
{
	m_latestDataLogId = 1;
}


DBHandler::~DBHandler(void) {

}

std::string DBHandler::getRowAsJson(std::string select, std::string table, std::string key, std::string id, Json& json) {
	int rows = 0, columns = 0;
	std::vector<std::string> values;
	std::vector<std::string> columnNames;
	char** results;

	try {
		results = retrieveFromTable("SELECT " + select + " FROM " + table + " WHERE ID = " + id + ";", rows, columns);
	} catch(const char * error) {
		std::cout << "error in DBHandler::getRowAsJson: " << error << std::endl;
	}

	for(int i = 0; i < columns*2; ++i) {
		if(i < columns)
			columnNames.push_back(results[i]);
		else
			values.push_back(results[i]);
	}

	if(values.size() != columnNames.size()) return "";

	Json jsonEntry;

	for(auto i = 0; i < values.size(); i++) {
		jsonEntry[columnNames.at(i)] = values.at(i);
	}
	json[key] = Json::array({jsonEntry});

	values.clear();
	columnNames.clear();
	return json.dump();
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
		localConfig = retrieveCell("state", "1", toCheck);
	}

	if ( serverConfig.compare(localConfig) != 0 ) {
		return true;
	}
	return false;
}


void DBHandler::updateTable(std::string table, std::string data) {

		std::vector<std::string> columns = getColumnInfo("name", table);
		Json json = Json::parse(data);

		std::stringstream ss;

		//start at i = 1 to skip the id
		ss << "SET ";
		for (auto i = 1; i < json.size(); i++) {
			ss << columns.at(i) << " = " << json[columns.at(i)] << ",";
		}
		std::string values = ss.str();
		values = values.substr(0, values.size()-1);

		int id = json["id"];

		try {
			queryTable("UPDATE " + table + " " + values + " WHERE ID = " + std::to_string(id) + ";");
		}
		catch( const char * error) {
			m_logger.error(std::string("Error in DBHandler::updateTable" + std::string(error)));
			// throw std::string("Error in DBHandler::updateTable" + std::string(error)).c_str();
		}
}


std::string DBHandler::retrieveCell(std::string table, std::string id, std::string column) {
	std::string query = "SELECT " + column + " FROM " + table +" WHERE id=" + id + ";";

	int rows, columns;
    char** results;
    results = retrieveFromTable(query, rows, columns);

    if (columns < 1) {
		std::stringstream errorStream;
		errorStream << "DBHandler::retrieveCell(), no columns from Query: " << query;
    	throw errorStream.str().c_str();
    }

    if (rows < 1) {
		std::stringstream errorStream;
		errorStream << "DBHandler::retrieveCell(), no rows from Query: " << query;
    	throw errorStream.str().c_str();
    }

    return results[1];
}

void DBHandler::updateConfigs(std::string configs) {
	/*TODO*/
	// Json json = Json::parse(configs);
	//
	// std::vector<std::string> vector;
	//
	// for (auto i : Json::iterator_wrapper(json))  {
	// 	std::cout << i.key() << std::endl;
	// 	// vector.push_back(i.key());
	// }

}


int DBHandler::retrieveCellAsInt(std::string table, std::string id, std::string column) {
	try {
		return atoi(retrieveCell(table, id, column).c_str());
	}
	catch (const char *  e) {
		std::cout << "exception thrown in retrieveCellAsInt, is returned cell a int?  " << e << std::endl;
		return 0;
	}

}


void DBHandler::clearTable(std::string table) {
	queryTable("DELETE FROM " + table + ";");
}

int DBHandler::getRows(std::string table) {
	int columns, rows;
	retrieveFromTable("SELECT * FROM " + table + ";", rows, columns);
	return rows;
}

std::string DBHandler::getLogs() {
	//Get IDs related to  the current system_datalogs_id
	std::string courseCalculationId = retrieveCell("system_datalogs",std::to_string(m_latestDataLogId),"course_calculation_id");
	std::string windsensorId = retrieveCell("system_datalogs",std::to_string(m_latestDataLogId),"windsensor_id");
	std::string compassModelId = retrieveCell("system_datalogs",std::to_string(m_latestDataLogId),"compass_id");
	std::string gpsId = retrieveCell("system_datalogs",std::to_string(m_latestDataLogId),"gps_id");
	// Get required fields from datalogs

	Json json;
	//create json string
	try {
		getRowAsJson("id,sail_command_sail_state,rudder_command_rudder_state,sail_servo_position,rudder_servo_position,waypoint_id,true_wind_direction_calc",
									"system_datalogs","system_datalogs",std::to_string(m_latestDataLogId),json);
		getRowAsJson("time,latitude,longitude,speed,heading,satellites_used",
									"gps_datalogs","gps_datalogs",gpsId,json);
		getRowAsJson("distance_to_waypoint,bearing_to_waypoint,course_to_steer,tack,going_starboard",
									"course_calculation_datalogs", "course_calculation_datalogs",courseCalculationId,json);
		getRowAsJson("heading,pitch,roll",
									"compass_datalogs","compass_datalogs",compassModelId,json);
		getRowAsJson("direction,speed,temperature",
								"windsensor_datalogs","windsensor_datalogs",windsensorId,json);
		} catch(const char * error) {
			m_logger.error(error);
		}
	return json.dump();
}



void DBHandler::removeLogs(std::string lines) {
	// JSONDecode decoder;
	// decoder.addJSON(lines);
	// while (decoder.hasNext()) {
	// 	// std::cout << "tab: " << decoder.getData("tab") << ", id: " << decoder.getData("id_system") << "\n";
	// 	queryTable("DELETE FROM system_datalogs WHERE id = " + decoder.getData("id_system") + ";");
	// }
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
		i = retrieveCell("waypoint_index", waypoint_id, "i");
		j = retrieveCell("waypoint_index", waypoint_id, "j");
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

std::string DBHandler::getWaypoints() {
	int rows = 0;
	Json json;
	std::string wp = "waypoint_";
	try {
		rows = getRows("waypoints");
		for(auto i = 1; i < rows; ++i) {
			getRowAsJson("id,latitude,longitude,radius","waypoints",wp+std::to_string(i),std::to_string(i),json);
		}
		getRowAsJson("id,latitude,longitude,radius","waypoints",wp+std::to_string(rows),std::to_string(rows),json);
	} catch (const char * error) {
		m_logger.error("error in DBHandler::getWaypoints()");
		std::stringstream ss;
		ss << "error in DBHandler::getWaypoints() : " << error;
		throw ss.str();
	}

	return json.dump();
}

void DBHandler::clearDatalogTables() {
	clearTable("system_datalogs");
	clearTable("compass_datalogs");
	clearTable("course_calculation_datalogs");
	clearTable("gps_datalogs");
	clearTable("windsensor_datalogs");

}


////////////////////////////////////////////////////////////////////
// private helpers
////////////////////////////////////////////////////////////////////

sqlite3* DBHandler::openDatabase() {
	sqlite3* connection;
	int resultcode = 0;
	// check if file exists
	FILE* db_file = fopen(m_filePath.c_str(), "r");
	if (!db_file) {
		std::string error = "DBHandler::openDatabase(), " + m_filePath +
			" not found.";
		throw error.c_str();
	}
	fclose(db_file);

	do {
		resultcode = sqlite3_open(m_filePath.c_str(), &connection);
	} while(resultcode == SQLITE_BUSY);

	if (resultcode) {
		std::stringstream errorStream;
		errorStream << "DBHandler::openDatabase(), " << sqlite3_errmsg(connection);

		throw errorStream.str().c_str();
	}

	return connection;
}


void DBHandler::closeDatabase(sqlite3* connection) {

	if(connection != NULL) {
		sqlite3_close(connection);
		connection = NULL;
	} else {
		throw "DBHandler::closeDatabase() : connection is already null";
	}
}

int DBHandler::insertLog(std::string table, std::string values) {
	std::stringstream ss;
	ss << "INSERT INTO " << table << " VALUES(NULL, " << values << ");";

	// std::cout << ss.str() << std::endl;

	try {
		sqlite3* db = openDatabase();

		queryTableWithOpenDatabase(ss.str(), db);
		m_latestDataLogId = sqlite3_last_insert_rowid(db);

		closeDatabase(db);
	} catch(const char * error) {
		std::cout << "error in DBHandler::insertLog: " << error << std::endl;
	}
	return m_latestDataLogId;
}

void DBHandler::queryTable(std::string sqlINSERT) {
	sqlite3* db = openDatabase();

	if (db != NULL) {
		int resultcode = 0;

		do {
			resultcode = sqlite3_exec(db, sqlINSERT.c_str(), NULL, NULL, &m_error);
		} while(resultcode == SQLITE_BUSY);

		if (m_error != NULL) {
			std::stringstream errorStream;
			errorStream << "DBHandler::queryTable(), " << sqlite3_errmsg(db);
			sqlite3_free(m_error);

			throw errorStream.str().c_str();
		}
	}
	else {
		throw "DBHandler::queryTable(), no db connection";
	}

	closeDatabase(db);
}

void DBHandler::queryTableWithOpenDatabase(std::string sqlINSERT, sqlite3* db) {
	if (db != NULL) {
		int resultcode = 0;

		do {
			resultcode = sqlite3_exec(db, sqlINSERT.c_str(), NULL, NULL, &m_error);
		} while(resultcode == SQLITE_BUSY);

		if (m_error != NULL) {
			std::stringstream errorStream;
			errorStream << "DBHandler::queryTable(), " << sqlite3_errmsg(db);
			sqlite3_free(m_error);

			throw errorStream.str().c_str();
		}
	}
	else {
		throw "DBHandler::queryTable(), no db connection";
	}
}

char** DBHandler::retrieveFromTable(std::string sqlSELECT, int &rows, int &columns) {
	sqlite3* db = openDatabase();
	char **results = NULL;

	if (db != NULL) {
		int resultcode = 0;

		do {
			resultcode = sqlite3_get_table(db, sqlSELECT.c_str(), &results, &rows, &columns, &m_error);
		} while(resultcode == SQLITE_BUSY);

		if (m_error != NULL) {
			std::stringstream errorStream;
			errorStream << "DBHandler::retrieveFromTable(), " << sqlite3_errmsg(db);
			sqlite3_free(m_error);
			std::cout << errorStream.str().c_str() << std::endl;
			throw errorStream.str().c_str();
		}
	}
	else {
		throw "DBHandler::retrieveFromTable(), no db connection";
	}

	closeDatabase(db);
	return results;
}

std::vector<std::string> DBHandler::getTableIds(std::string table) {
		int rows, columns;
    char** results;
    results = retrieveFromTable("SELECT id FROM " + table + ";", rows, columns);

    std::vector<std::string> ids;
    for (int i = 1; i <= rows; i++) {
    	ids.push_back(results[i]);
    }

    return ids;
}

std::vector<std::string> DBHandler::getColumnInfo(std::string info, std::string table) {
	int rows, columns;
    char** results;
    results = retrieveFromTable("PRAGMA table_info(" + table + ");", rows, columns);
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
    results = retrieveFromTable("SELECT MIN(id) FROM waypoints WHERE harvested = 0;", rows, columns);
    //std::cout << "result |" << rows << ":" << columns << "|" << results << std::endl;
    if (rows * columns < 1 || results[1] == '\0') {
    	waypointModel.id = "";
    }
    else {
    	waypointModel.id = results[1];
    }

	if(!waypointModel.id.empty())
	{
		waypointModel.positionModel.latitude = atof(retrieveCell("waypoints", waypointModel.id, "latitude").c_str());
		waypointModel.positionModel.longitude = atof(retrieveCell("waypoints", waypointModel.id, "longitude").c_str());
		waypointModel.radius = retrieveCellAsInt("waypoints", waypointModel.id, "radius");
		waypointModel.declination = retrieveCellAsInt("waypoints", waypointModel.id, "declination");

		results = retrieveFromTable("SELECT time FROM waypoint_stationary WHERE id = " +
			waypointModel.id + ";", rows, columns);

		if (rows * columns < 1 || results[1] == '\0') {
			waypointModel.time = 0;
		}
		else {
			waypointModel.time = retrieveCellAsInt("waypoint_stationary", waypointModel.id, "time");
		}
	}

}


void DBHandler::changeOneValue(std::string table, std::string id,std::string newValue, std::string colName){

	queryTable("UPDATE " + table + " SET "+ colName + " = "+ newValue +" WHERE id = " + id +";");

}

std::string DBHandler::getMinIdFromTable(std::string table) {
	int rows, columns;
    char** results;
    results = retrieveFromTable("SELECT MIN(id) FROM " + table + ";", rows, columns);
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

std::string DBHandler::formatRowToJson(std::string key,std::vector<std::string> values, std::vector<std::string> columnNames) {
	JSONArray datalogs;
	datalogs.setName(key);

	JSONData data;

	for (auto i = 0; i < values.size(); i++) {
		data.add(columnNames.at(i),values.at(i));
	}

	JSONBlock block;
	block.add(data.toString());
	datalogs.add(block.toString());

	return datalogs.toString();
}
