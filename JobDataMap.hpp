//
//  JobDataMap.hpp
//  job_scheduler
//
//  Created by Kenan Si on 24/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef JobDataMap_hpp
#define JobDataMap_hpp

#include <string>
#include <map>
#include <exception>
#include <boost/date_time.hpp>

class JobException : public std::exception
{
};

class NoRequestedData : public JobException
{
private:
	std::string data_name;
public:
	NoRequestedData(std::string _data_name)
	: data_name(_data_name)
	{
	}
	const char* what() const noexcept override
	{
		return ("No data called \"" + data_name + "\" found.").c_str();
	}
};

class JobDataMap
{
private:
	using date = boost::gregorian::date;
	std::map<std::string, int> int_map;
	std::map<std::string, double> double_map;
	std::map<std::string, date> date_map;
	std::map<std::string, std::string> string_map;
public:
	int getInt(const std::string & int_name)
	{
		auto int_ptr = int_map.find(int_name);
		if (int_ptr != int_map.end())
			return int_ptr->second;
		else
			throw NoRequestedData(int_name);
	}
	double getDouble(const std::string & double_name) const
	{
		auto double_ptr = double_map.find(double_name);
		if (double_ptr != double_map.end())
			return double_ptr->second;
		else
			throw NoRequestedData(double_name);
	}
	date getDate(const std::string & date_name) const
	{
		auto date_ptr = date_map.find(date_name);
		if (date_ptr != date_map.end())
			return date_ptr->second;
		else
			throw NoRequestedData(date_name);
	}
	std::string getString(const std::string & string_name) const
	{
		auto string_ptr = string_map.find(string_name);
		if (string_ptr != string_map.end())
			return string_ptr->second;
		else
			throw NoRequestedData(string_name);
	}

	void setInt(const std::string & int_name, int int_data)
	{
		int_map[int_name] = int_data;
	}

	void setDouble(const std::string & double_name, double double_data)
	{
		double_map[double_name] = double_data;
	}

	void setDate(const std::string & date_name, date date_data)
	{
		date_map[date_name] = date_data;
	}

	void setString(const std::string & string_name, const std::string & string_data)
	{
		string_map[string_name] = string_data;
	}

	JobDataMap & combineAndOverwrite(const JobDataMap & other_data_map)
	{
		for (auto int_data : other_data_map.int_map)
			int_map[int_data.first] = int_data.second;
		for (auto double_data : other_data_map.double_map)
			double_map[double_data.first] = double_data.second;
		for (auto date_data : other_data_map.date_map)
			date_map[date_data.first] = date_data.second;
		for (auto string_data : other_data_map.string_map)
			string_map[string_data.first] = string_data.second;
		return *this;
	}

	void clear()
	{
		int_map.clear();
		double_map.clear();
		date_map.clear();
		string_map.clear();
	}
};

#endif /* JobDataMap_hpp */
