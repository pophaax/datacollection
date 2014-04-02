#include "DataCollect.h"

int main() {

	DataCollect dc;

	dc.openDatabase();
	dc.createTables();

	//inserting some test data into tables
	dc.insertGPSdata("18:04:2013 14:00", 12.4, 23.6, 2, 0.1, 2);
	dc.insertCalculations(12, 45, 3, 4, 5, 1);
	dc.insertHeadingData(12.4, 34.6);
	dc.insertWPdata(10, 20);

	//printing tables
	dc.printSelect("SELECT * FROM gps;");
	dc.printSelect("SELECT * FROM calc;");
	dc.printSelect("SELECT * FROM head;");
	dc.printSelect("SELECT * FROM wp;");
	dc.printSelect("SELECT * FROM synch;");

	dc.closeDatabase();

	return 0;
}
