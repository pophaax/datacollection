#ifndef __JSON_H__
#define __JSON_H__

#include <string>
#include <vector>
#include <map>


/******************************************
 *
 *	IEncoder interface
 *
 ******************************************/

class IEncoder {
	
	public:
		virtual ~IEncoder() {}
		virtual std::string toString() = 0;
};

/******************************************
 *
 *	JSONBLOCK
 *
 ******************************************/

class JSONBlock : public IEncoder {

	public:
		JSONBlock();
		~JSONBlock();

		void setName(std::string name);
		void add(std::string component);
		std::string toString();

	private:
		std::string m_name;
		std::vector<std::string> m_block;
};

/******************************************
 *
 *	JSONARRAY
 *
 ******************************************/

class JSONArray : public IEncoder {

	public:
		JSONArray();
		~JSONArray();

		void setName(std::string name);
		void add(std::string block);
		std::string toString();

	private:
		std::string m_name;
		std::vector<std::string> m_array;
};

/******************************************
 *
 *	JSONDATA
 *
 ******************************************/

class JSONData : public IEncoder {

	public:
		JSONData();
		~JSONData();

		void add(std::string name, std::string value);
		void add(std::string name, int value);
		void add(std::string name, float value);
		void add(std::string name, double value);
		std::string toString();

	private:
		std::vector<std::string> m_data;
};

/******************************************
 *
 *	JSONDECODER
 *
 ******************************************/

class JSONDecode {

private:
	std::vector< std::map<std::string, std::string> > data;
	unsigned int current;

public:
	JSONDecode();
	~JSONDecode();

	void addJSON(std::string json);
	bool hasNext();
	std::string getData(std::string id);
	int getSize();
	bool isEmpty();
};

#endif
