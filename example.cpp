#include "DBHandler.h"

int main() {
	DBHandler db;
	db.openDatabase("asr.db");

        db.insertDataLog(
                0,

                0,

                0,
                0,
                0,
                0,

                0,
                "string ws_sensormodel",
                0,
                0,
                0,

                0,

                0,

                        "string gps_timestamp",
                        0.123456,
                        0.123456,
                        0.123456,
                        0.123456,
                        0.123456,
                        0,
                        0);

//        std::cout << db.retriveCell("configs", "1", "sc_commandrunning") << "\n";
}
