#include "DBHandler.h"

int main() {
	DBHandler db;
	db.openDatabase("asr.db");

	try {

		std::cout << "as string: " << db.retriveCell("configs", "1", "sc_ang_run") << "\n";
		std::cout << "as int: " << db.retriveCellAsInt("configs", "1", "sc_ang_run") << "\n";


		//db.insertMessageLog("gpstimezzz", "type", "msg1");
		//db.insertMessageLog("gpstimezzz", "type", "msg2");
		//db.insertMessageLog("gpstimezzz", "type", "msg3");

		//db.insertDataLog("gpstimezzz", 19.123456, 60.123456, 1.2, 120, 3, 10, 0, 0, 0, 10, 0, 0, 0, 0, 0.123456, 1, 1);
		//db.insertDataLog("gpstimezzz", 19.123456, 60.123456, 1.2, 120, 3, 10, 0, 0, 0, 10, 0, 0, 0, 0, 0.123456, 1, 1);
		//db.insertDataLog("gpstimezzz", 19.123456, 60.123456, 1.2, 120, 3, 10, 0, 0, 0, 10, 0, 0, 0, 0, 0.123456, 1, 1);

		/*	db.insertMessageLog("gpstimezzz", "type", "msg", 1);

			db.insertWaypoint(1, 19.123456, 60.123456);

			db.insertServer(1, "ship_name", "ship_pwd", "srv_addr");

			db.insertState(1, "cfg_rev", "route_rev", "wpt_rev", 1); */
		//db.insertMessageLog("gpstimezzz", "type", "msg2");
		//db.insertMessageLog("gpstimezzz", "type", "msg2");

		std::cout << db.getLogs() << "\n";
		db.clearTable("messages");
	} catch (const char * e) {
		std::cout << e << "\n";
	}

//        std::cout << db.retriveCell("configs", "1", "sc_commandrunning") << "\n";
}
