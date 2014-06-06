#include "DBHandler.h"

int main() {
	DBHandler db;
	db.openDatabase("asr.db");
	db.insertConfig(
		7424, //int sc_commandclosereach,
        	6600, //int sc_commandbeamreach,
        	6200, //int sc_commandbroadreach,
        	5824, //int sc_commandrunning,
        	120, //int sc_anglebeamreach,
        	80, //int sc_anglebroadreach,
        	40, //int sc_anglerunning,

        	7616, //int rc_commandextreme,
        	7000, //int rc_commandmedium,
        	6500, //int rc_commandsmall,
        	5984, //int rc_commandmidships,
        	60, //int rc_anglemedium,
        	30, //int rc_anglesmall,
        	10, //int rc_anglemidships,

        	10, //int cc_tackangle,
        	45, //int cc_sectorangle,

        	"CV7", //string ws_sensormodel,
        	"/dev/ttyAMA0", //string ws_portname,
        	4800, //int ws_baudrate,

        	"/dev/ttyACM0", //string mc_portname,

        	0, //int rs_channel,
        	0, //int rs_speed,
        	0, //int rs_acceleration,
        	1088, //int rs_limitmin,
        	1904, //int rs_limitmax,

        	1, //int ss_channel,
        	0, //int ss_speed,
        	0, //int ss_acceleration,
        	1456, //int ss_limitmin,
        	1856, //int ss_limitmax,

        	"/dev/ttyUSB0", //string gps_portname,
        	"localhost"); //string gps_connectionname);

        db.insertWaypoint(1, 60.767208, 19.823913);

//        std::cout << db.retriveCell("configs", "1", "sc_commandrunning") << "\n";
}
