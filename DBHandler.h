#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__ //__DATACOLLECT_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "sqlite3.h"

using namespace std;

class DBHandler {

private:

	int m_rc;
	sqlite3 *m_db;
	char *m_error;
	int m_latestDataLogId;

	//execute INSERT query and add new row into table, the sql string will be also stored in a synch table
	void updateTable(string sqlINSERT);

	//retrive data from given table/tables, return value is a C++ 2D char array
	//rows and columns also return values (through a reference) about rows and columns in the result set
	char** retriveFromTable(string sqlSELECT, int &rows, int &columns);

public:
	void insertConfig(
		int id,
		int sc_cmd_clse,
		int sc_cmd_beam,
		int sc_cmd_brd,
		int sc_cmd_run,
		int sc_ang_beam,
		int sc_ang_brd,
		int sc_ang_run,
		int rc_cmd_xtrm,
		int rc_cmd_med,
		int rc_cmd_sml,
		int rc_cmd_mid,
		int rc_ang_med,
		int rc_ang_sml,
		int rc_ang_mid,
		int cc_ang_tack,
		int cc_ang_sect,
		string ws_modl,
		int ws_chan,
		string ws_port,
		int ws_baud,
		int ws_buff,
		string mc_port,
		int rs_chan,
		int rs_spd,
		int rs_acc,
		int ss_chan,
		int ss_spd,
		int ss_acc);

	void insertDataLog(
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
		int wpt_cur);

	void insertMessageLog(string gps_time, string type, string msg);

	void insertWaypoint(int id, double lat, double lon);

	void insertServer(int id, string boat_id, string boat_pwd, string srv_addr);

	void insertState(int id, string cfg_rev, string rte_rev, string wpt_rev, int wpt_cur);

	//constructor
	DBHandler(void);

	//destructor
	~DBHandler(void);

	//open database
	void openDatabase(string fileName);

	//close database
	void closeDatabase();

    //retrieve one value from a table as string
	string retriveCell(string table, string id, string column);

	vector<string> getTableIds(string table);

	void clearTable(string table);
};

#endif
