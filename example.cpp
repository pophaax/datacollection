#include "DBHandler.h"

int main() {
	DBHandler db;
	db.openDatabase("asr.db");

	try {
		db.insertConfig(1, 4000, 5000, 6000, 7000, 40, 80, 120, 4000, 5000, 6000, 7000, 10, 20, 30,
					10, 5, "ws_modl", 5, "ws_port", 4600, 10, "mc_port", 0, 0, 0, 1, 0, 0);

		db.insertDataLog("gpstimezzz", 19.123456, 60.123456, 1.2, 120, 3, 10, 0, 0, 0,
					10, 0, 0, 0, 0, 0.123456, 1, 1);

		db.insertMessageLog("gpstimezzz", "type", "msg", 1);

		db.insertWaypoint(1, 19.123456, 60.123456);

		db.insertServer(1, "ship_name", "ship_pwd", "srv_addr");

		db.insertState(1, "cfg_rev", "route_rev", "wpt_rev", 1);
	
	} catch (const char * e) {
		std::cout << e << "\n";
	}
//        std::cout << db.retriveCell("configs", "1", "sc_commandrunning") << "\n";
}
