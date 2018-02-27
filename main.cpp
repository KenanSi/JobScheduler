//
//  main.cpp
//  job_scheduler
//
//  Created by Kenan Si on 22/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#include <iostream>
#include "Scheduler.hpp"
#include "JobDataMap.hpp"
#include "DummyJob.hpp"
#include "Trigger.hpp"

int main(int argc, const char * argv[])
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	Scheduler scheduler;
	scheduler.start();

	// Create job from user provided dummy jobs
	auto job_a = std::make_shared<SayHelloJob>();
	auto job_b = std::make_shared<SayGoodbyeJob>();
	auto job_c = std::make_shared<FailedJob>();

	// Create input parameter pack for the jobs
	JobDataMap a_input;
	JobDataMap b_input;
	JobDataMap c_input;

	// Set input parameters
	a_input.setDate("object_date", date(from_string("2018-02-26")));
	a_input.setString("output_path", "job_a_output.txt");

	b_input.setDate("object_date", date(from_string("2018-02-26")));
	b_input.setString("output_path", "job_b_output.txt");

	// Create job triggers with provided start time
	Trigger trigger_a{second_clock::local_time() + seconds{1}};
	Trigger trigger_b{second_clock::local_time() + seconds{3}};
	Trigger trigger_c{second_clock::local_time() + seconds{5}};

	// Create job environments for jobs
	auto job_env_a = std::make_shared<JobEnv>("J001", job_a, a_input, trigger_a);
	auto job_env_b = std::make_shared<JobEnv>("J002", job_b, b_input, trigger_b);
	auto job_env_c = std::make_shared<JobEnv>("J003", job_c, c_input, trigger_c);

	// Form dependency between jobs. job_b dependends on job_a.
	job_env_b->addPrerequisite(job_env_a);
	job_env_a->addJobListener(job_env_b);

	// Add jobs to the schedule
	scheduler.addJob(job_env_a);
	scheduler.addJob(job_env_b);
	scheduler.addJob(job_env_c);

	std::this_thread::sleep_for(std::chrono::seconds{10});

	scheduler.shutDown();

	return 0;
}
