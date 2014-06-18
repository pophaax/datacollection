#include "DBHandler.h"

int main() {
	DBHandler db;
	db.openDatabase("asr.db");

	try {
	/*	db.insertConfig(1,
						7424,
						6600,
						6200,
						5824,
						120,
						80,
						40,
						7616,
						7000,
						6500,
						5984,
						60,
						30,
						10,
						45,
						5,
						"CV7",
						5,
						"/dev/ttyAMA0",
						4800,
						10,
						"/dev/ttyACM0",
						0,
						0,
						0,
						1,
						0,
						0); */
	db.getTableIds("messages");

	db.insertMessageLog("gpstimezzz", "type", "msg1", 1);
	db.insertMessageLog("gpstimezzz", "type", "msg2", 1);
	db.insertMessageLog("gpstimezzz", "type", "msg3", 1);

	/*	db.insertDataLog("gpstimezzz", 19.123456, 60.123456, 1.2, 120, 3, 10, 0, 0, 0,
					10, 0, 0, 0, 0, 0.123456, 1, 1);

		db.insertMessageLog("gpstimezzz", "type", "msg", 1);

		db.insertWaypoint(1, 19.123456, 60.123456);

		db.insertServer(1, "ship_name", "ship_pwd", "srv_addr");

		db.insertState(1, "cfg_rev", "route_rev", "wpt_rev", 1); */
	db.clearTable("messages");
	db.insertMessageLog("gpstimezzz", "type", "msg2", 1);
	} catch (const char * e) {
		std::cout << e << "\n";
	}
//        std::cout << db.retriveCell("configs", "1", "sc_commandrunning") << "\n";
}
