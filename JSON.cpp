#include "JSON.h"
#include <sstream>
#include <iostream>

/******************************************
 *
 *	JSONBLOCK
 *
 ******************************************/

JSONBlock::JSONBlock() {
	m_name = "";
}

JSONBlock::~JSONBlock() {}

void JSONBlock::setName(string name) {
	m_name = name;
}

void JSONBlock::add(string component) {
	m_block.push_back(component);
}

string JSONBlock::toString() {
	if(m_block.empty()) {
		return "";
	}

	stringstream sstm;
	sstm << "{";
	if(m_name.length() != 0) {
		sstm << "\"" << m_name << "\":";	
	}
	for(unsigned int i = 0; i < m_block.size(); i++) {
		if(i>0){
			sstm << ",";
		}
		sstm << m_block.at(i);
	}
	sstm << "}";

	return sstm.str();
}

/******************************************
 *
 *	JSONARRAY
 *
 ******************************************/


JSONArray::JSONArray() {
	m_name = "";
}

JSONArray::~JSONArray() {}

void JSONArray::setName(string name) {
	m_name = name;
}

void JSONArray::add(string block) {
	m_array.push_back(block);

}

string JSONArray::toString() {
	if(m_array.empty()) {
		return "";
	}

	stringstream sstm;
	if(m_name.length() != 0) {
		sstm << "\"" << m_name << "\":";	
	}
	sstm << "[";
	for(unsigned int i = 0; i < m_array.size(); i++) {
		if(i>0){
			sstm << ",";
		}
		sstm << m_array.at(i);
	}
	sstm << "]";

	return sstm.str();
}

/******************************************
 *
 *	JSONDATA
 *
 ******************************************/

JSONData::JSONData() {}
JSONData::~JSONData() {}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void JSONData::add(string name, string value) {;
	stringstream sstm;
	sstm << "\"" << name << "\":" << "\"" << value << "\"";
	string result = sstm.str();
	while(replace(result, " ", "%26"));
	m_data.push_back(result);
}

void JSONData::add(string name, int value) {
	stringstream sstm;
	sstm << "\"" << name << "\":" << value;
	m_data.push_back(sstm.str());
}

void JSONData::add(string name, float value) {
	stringstream sstm;
	sstm << "\"" << name << "\":" << value;
	m_data.push_back(sstm.str());
}

void JSONData::add(string name, double value) {
	stringstream sstm;
	sstm << "\"" << name << "\":" << value;
	m_data.push_back(sstm.str());
}

string JSONData::toString() {
	if(m_data.empty()) {
		return "";
	}

	stringstream sstm;
	for(unsigned int i = 0; i < m_data.size(); i++) {
		if(i>0){
			sstm << ",";
		}
		sstm << m_data.at(i);
	}

	return sstm.str();
}
