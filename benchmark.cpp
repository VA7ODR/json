#include "json.hpp"
#include <iostream>
#include <chrono>
#include <fstream>

using namespace std::chrono;

class ElapsedTimer
{
public:
	ElapsedTimer() {
		reset();
	}

	void reset() {
		t1 = high_resolution_clock::now();
	}

	double value(bool bReset = false){
		duration<double> time_span = duration_cast<duration<double> >(high_resolution_clock::now() - t1);
		double ret = time_span.count();
		if(bReset) {
			reset();
		}

		return ret;
	}

private:
	high_resolution_clock::time_point  t1;
};

void iterate (json::document & jDoc, json::document & jDoc2) {
	for (auto & row : jDoc["rows"]) {
		auto & doc = row["doc"];
		if (doc.exists("crash_data")) {
			json::value jTemp;
			jTemp["siteID"] = doc["siteID"];
			jTemp["version"] = doc["version"];
			jTemp["eventGroup"] = doc["eventGroup"];
			jTemp["eventType"] = doc["eventType"];
			jTemp["eventName"] = doc["eventName"];
			jTemp["eventTime"] = doc["eventTime"];
			jTemp["crash_data"] = doc["crash_data"];
			jDoc2.push_back(jTemp);
		}
	}
};

int main(int argc, char ** argv)
{
	ElapsedTimer timer;
	json::document jDoc;
	json::document jDoc2;
	std::cout << "Parsing " << argv[1] << "... ";
	std::cout.flush();

	bool bParsedOk = jDoc.parseFile(argv[1]);

	std::cout << "Took " << timer.value(true) << "s" << std::endl;

	if (!bParsedOk) {
		std::cout << "FAILED!" << std::endl;
		std::cerr << "Could not parse " << argv[1] << "." << std::endl;
		exit(-1);
	}
	std::cout << "Writing temp1.json... ";
	std::cout.flush();
	bool bWroteOk = jDoc.writeFile("temp1.json", true);
	if (!bWroteOk) {
		std::cout << "FAILED!" << std::endl;
		std::cerr << "Could not write temp1.json." << std::endl;
		exit(-1);
	}
	std::cout << "Took " << timer.value(true) << "s" << std::endl;
	std::cout << "Iterating " << argv[1] << "... ";
	std::cout.flush();
	iterate (jDoc, jDoc2);
	std::cout << "Took " << timer.value(true) << "s" << std::endl;

	std::cout << "Writing temp2.json... ";
	std::cout.flush();
	bWroteOk = jDoc2.writeFile("temp2.json", true);
	if (!bWroteOk) {
		std::cout << "FAILED!" << std::endl;
		std::cerr << "Could not write temp2.json." << std::endl;
		exit(-1);
	}
	std::cout << "Took " << timer.value(true) << "s" << std::endl;

	std::cout << "Writing temp3.json... ";
	std::cout.flush();

	std::ofstream out3("temp3.json");

	out3 << std::noskipws;
	out3 << jDoc;
	out3.close();
	std::cout << "Took " << timer.value(true) << "s" << std::endl;
	return 0;
}
