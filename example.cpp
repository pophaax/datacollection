#include "DBHandler.h"

int main() {
	DBHandler db;
	db.openDatabase("asr.db");

	try {
		std::cout << "as string: " << db.retriveCell("configs", "1", "sc_ang_run") << "\n";
		std::cout << "as int: " << db.retriveCellAsInt("configs", "1", "sc_ang_run") << "\n";

		std::cout << db.getLogs() << "\n";
		db.clearTable("messages");
	} catch (const char * e) {
		std::cout << e << "\n";
	}

}
