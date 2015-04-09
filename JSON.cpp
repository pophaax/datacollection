#include "JSON.h"
#include <sstream>
#include <iostream>

/******************************************
 *
 *	Help functions
 *
 ******************************************/

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

/******************************************
 *
 *	JSONBLOCK
 *
 ******************************************/

JSONBlock::JSONBlock() {
	m_name = "";
}

JSONBlock::~JSONBlock() {}

void JSONBlock::setName(std::string name) {
	m_name = name;
}

void JSONBlock::add(std::string component) {
	m_block.push_back(component);
}

std::string JSONBlock::toString() {
	if(m_block.empty()) {
		return "";
	}

	std::stringstream sstm;
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

void JSONArray::setName(std::string name) {
	m_name = name;
}

void JSONArray::add(std::string block) {
	m_array.push_back(block);

}

std::string JSONArray::toString() {
	if(m_array.empty()) {
		return "";
	}

	std::stringstream sstm;
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

void JSONData::add(std::string name, std::string value) {;
	std::stringstream sstm;
	sstm << "\"" << name << "\":" << "\"" << value << "\"";
	std::string result = sstm.str();
	while(replace(result, " ", "%26"));
	m_data.push_back(result);
}

void JSONData::add(std::string name, int value) {
	std::stringstream sstm;
	sstm << "\"" << name << "\":" << value;
	m_data.push_back(sstm.str());
}

void JSONData::add(std::string name, float value) {
	std::stringstream sstm;
	sstm << "\"" << name << "\":" << value;
	m_data.push_back(sstm.str());
}

void JSONData::add(std::string name, double value) {
	std::stringstream sstm;
	sstm << "\"" << name << "\":" << value;
	m_data.push_back(sstm.str());
}

std::string JSONData::toString() {
	if(m_data.empty()) {
		return "";
	}

	std::stringstream sstm;
	for(unsigned int i = 0; i < m_data.size(); i++) {
		if(i>0){
			sstm << ",";
		}
		sstm << m_data.at(i);
	}

	return sstm.str();
}

/******************************************
 *
 *	JSONDECODER
 *
 ******************************************/

JSONDecode::JSONDecode() {
	current = 0;
}
JSONDecode::~JSONDecode() {}

void JSONDecode::addJSON(std::string json) {
	replace(json,"[{","");
	replace(json,"}]","");
	while(replace(json,"\\/","/"));
	while(replace(json,"\"",""));
	while(replace(json,"},{","&"));

	std::string atok;
	std::string etok;
	std::string id;
	std::string value;
	std::stringstream astrm(json);

	while(getline(astrm, atok, '&') ) {
		std::stringstream estrm(atok);
		std::map<std::string, std::string> tmp;
		while(getline(estrm, etok, ',') ) {
			std::stringstream dstrm(etok);
			getline(dstrm, id, ':');
			getline(dstrm, value, ':');

			tmp.insert(std::pair<std::string,std::string>(id,value));
		}
		data.push_back(tmp);
	}
}

bool JSONDecode::hasNext() {
	bool result = false;
	if(data.size() > current) {
		current++;
		result = true;
	}
	return result;
}

std::string JSONDecode::getData(std::string id) {
	if(data.empty()) {
		return "";
	}
	return data.at(current-1)[id];
}

int JSONDecode::getSize() {
	return data.at(current-1).size();
}

bool JSONDecode::isEmpty() {
	return data.empty();
}