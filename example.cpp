#include "DBHandler.h"

int main() {
	DBHandler db;
	db.openDatabase();
	db.createTables();
	db.insertConfig(
			0, //int sc_commandclosereach,
        	1, //int sc_commandbeamreach,
        	2, //int sc_commandbroadreach,
        	3, //int sc_commandrunning,
        	0, //int sc_anglebeamreach,
        	1, //int sc_anglebroadreach,
        	2, //int sc_anglerunning,

        	0, //int rc_commandextreme,
        	1, //int rc_commandmedium,
        	2, //int rc_commandsmall,
        	3, //int rc_commandmidships,
        	0, //int rc_anglemedium,
        	1, //int rc_anglesmall,
        	2, //int rc_anglemidships,

        	10, //int cc_tackangle,
        	45, //int cc_sectorangle,

        	"WAINDSENRSOR", //string ws_sensormodel,
        	"/dev/ttySMTHING", //string ws_portname,
        	4600, //int ws_baudrate,

        	"/dev/ttyAGAIN", //string mc_portname,

        	0, //int rs_channel,
        	0, //int rs_speed,
        	0, //int rs_acceleration,
        	4000, //int rs_limitmin,
        	8000, //int rs_limitmax,

        	1, //int ss_channel,
        	0, //int ss_speed,
        	0, //int ss_acceleration,
        	4000, //int ss_limitmin,
        	8000, //int ss_limitmax,

        	"/dev/ttyGG", //string gps_portname,
        	"localhost"); //string gps_connectionname);
	
}
