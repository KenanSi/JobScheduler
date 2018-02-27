//
//  DummyJob.hpp
//  job_scheduler
//
//  Created by Kenan Si on 26/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef DummyJob_hpp
#define DummyJob_hpp

#include "Job.hpp"
#include <fstream>
#include <chrono>
#include <thread>

class SayHelloJob : public Job
{
public:
	void run(const JobDataMap & input_data_map, JobDataMap & output_data_map) override
	{
		auto object_date = input_data_map.getDate("object_date");
		auto output_path = input_data_map.getString("output_path");
		std::ofstream fout(output_path);
		fout << "Hello, " << object_date << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds{5});
		output_data_map.setString("result_from_SayHelloJob", "Hello!");
	}
};

class SayGoodbyeJob : public Job
{
public:
	void run(const JobDataMap & input_data_map, JobDataMap & output_data_map) override
	{
		auto object_date = input_data_map.getDate("object_date");
		auto output_path = input_data_map.getString("output_path");
		std::ofstream fout(output_path);
		fout << "Get result from SayHelloJob: " << input_data_map.getString("result_from_SayHelloJob") << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds{2});
		fout << "GoodBye, " << object_date << std::endl;
	}
};

class FailedJob : public Job
{
public:
	void run(const JobDataMap & input_data_map, JobDataMap & output_data_map) override
	{
		auto object_date = input_data_map.getDate("object_date");
		auto output_path = input_data_map.getString("output_path");
		std::ofstream fout(output_path);
		fout << "Failed, " << object_date << std::endl;
	}
};

#endif /* DummyJob_hpp */
