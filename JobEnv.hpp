//
//  JobEnv.hpp
//  job_scheduler
//
//  Created by Kenan Si on 24/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef JobEnv_hpp
#define JobEnv_hpp


#include <memory>
#include <string>
#include <list>
#include <exception>
#include <functional>
#include "JobDataMap.hpp"
#include "Job.hpp"
#include "Trigger.hpp"

class JobEnvException : public std::exception
{};

class PrerequisiteNotFilled : public JobEnvException
{};

class JobEnv
{
public:
	enum JOB_STATUS {NOT_START, RUNNING, WAITING_FOR_UPSTREAM, DONE, FAILED};

private:
	std::string job_id;
	std::string job_name;
	JOB_STATUS status;
	JobDataMap input_data_map;
	JobDataMap imported_data_map;
	JobDataMap output_data_map;
	std::shared_ptr<Job> job;

	bool check_prerequisite = true;
	std::map<std::string, bool> preprequisites;

	std::list<std::shared_ptr<JobEnv>> job_listeners;
	
	std::string error_message;

	Trigger trigger;

public:

	JobEnv() = delete;
	JobEnv(std::string _job_id, std::shared_ptr<Job> _job, JobDataMap _input_data_map, Trigger _trigger) :
	job_id(_job_id), job(_job), input_data_map(_input_data_map), trigger(_trigger)
	{
		status = NOT_START;
	}

	auto getId() const
	{
		return job_id;
	}

	auto getScheduledTime() const
	{
		return trigger.getStartTime();
	}

	auto getErrorMessage()
	{
		return error_message;
	}

	void reinitializeEnv()
	{
		status = NOT_START;
		if (check_prerequisite)
		{
			for (auto preprequisite : preprequisites)
				preprequisite.second = false;
			imported_data_map.clear();
		}
	}

	void addPrerequisite(const std::shared_ptr<JobEnv> & job_env)
	{
		preprequisites[job_env->getId()] = false;
	}

	void finishPrerequisite(const std::string & job_id)
	{
		preprequisites[job_id] = true;
	}

	void importDataMap(const JobDataMap & data_map)
	{
		imported_data_map.combineAndOverwrite(data_map);
	}

	void setPrerequisiteCheck(bool check)
	{
		check_prerequisite = check;
	}

	void addJobListener(const std::shared_ptr<JobEnv> & job_env)
	{
		job_listeners.push_back(job_env);
	}

	void notifyListeners() const
	{
		for (auto job_listener : job_listeners)
		{
			job_listener->finishPrerequisite(job_id);
			job_listener->importDataMap(output_data_map);
		}
	}
	JOB_STATUS getStatus()
	{
		return status;
	}

	bool itsTime()
	{
		return (trigger.getStartTime() <= boost::posix_time::second_clock::local_time());
	}

	bool needsReschedule() const
	{
		return trigger.isActive();
	}

	void runJob()
	{
		if (check_prerequisite)
			for (auto preprequisite : preprequisites)
				if (!preprequisite.second)
				{
					status = WAITING_FOR_UPSTREAM;
					// throw PrerequisiteNotFilled();
					trigger.rescheduleTime();
					return;
				}
		try
		{
			status = RUNNING;
			job->run(imported_data_map.combineAndOverwrite(input_data_map), output_data_map);
			status = DONE;
			notifyListeners();
			trigger.setupNextRun();
		}
		catch(const JobException & job_exception)
		{
			status = FAILED;
			error_message = job_exception.what();
			trigger.setupNextRun();
		}
	}

};

#endif /* JobEnv_hpp */
