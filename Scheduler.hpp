//
//  Scheduler.hpp
//  job_scheduler
//
//  Created by Kenan Si on 24/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef Scheduler_hpp
#define Scheduler_hpp

#include <queue>
#include <fstream>
#include "JobEnv.hpp"
#include "Executor.hpp"
#include "Monitor.hpp"
#include "SyncQueue.hpp"

class Scheduler
{
//	using time_point = std::chrono::steady_clock::time_point<std::chrono::steady_clock>;
private:
	ScheduleQueue scheduled_jobs;
	SyncQueue<std::shared_ptr<JobEnv>> queue;
	ExecutorPool executor_pool;
	Monitor monitor;
	std::ofstream log;
	bool take_log = true;

public:
	void start()
	{
		log = std::ofstream("log.txt");
		if (take_log)
			log << boost::posix_time::second_clock::local_time() << "\tJob scheduler starts.\n";
		monitor.start(std::ref(scheduled_jobs), std::ref(queue));
		if (take_log)
			log << boost::posix_time::second_clock::local_time() << "\tSchedule monitor starts.\n";
		executor_pool.start(2, std::ref(queue), *this);
		if (take_log)
			log << boost::posix_time::second_clock::local_time() << "\tExecutor pool starts.\n";
	}

	void postprocess(std::shared_ptr<JobEnv> job_env)
	{
		switch (job_env->getStatus())
		{
			case JobEnv::DONE:
				if (take_log)
					log << boost::posix_time::second_clock::local_time() << '\t'
						<< job_env->getId() << " done.\n";
			break;
			case JobEnv::FAILED:
				if (take_log)
					log << boost::posix_time::second_clock::local_time() << '\t'
						<< job_env->getId() << " failed. Error message: "
						<< job_env->getErrorMessage() << std::endl;
			break;
			case JobEnv::WAITING_FOR_UPSTREAM:
				if (take_log)
					log << boost::posix_time::second_clock::local_time() << '\t'
						<< job_env->getId() << " didn't run. The upstream jobs are not done yet.\n";
			break;
		}
		if (job_env->needsReschedule())
			addJob(job_env);
	}

	void addJob(std::shared_ptr<JobEnv> job_env)
	{
		scheduled_jobs.push(job_env);
		if (take_log)
			log << boost::posix_time::second_clock::local_time() << '\t'
				<< job_env->getId() << " is scheduled at "
				<< job_env->getScheduledTime() << ".\n";
	}

	void shutDown()
	{
		if (take_log)
			log << boost::posix_time::second_clock::local_time() << '\t'
				<< "Scheduler is shutting down.\n";

		if (take_log)
			log << boost::posix_time::second_clock::local_time() << '\t'
				<< "Schedule monitor is shutting down.\n";
		monitor.shutDown();
		
		if (take_log)
			log << boost::posix_time::second_clock::local_time() << '\t'
				<< "Schedule monitor shut down.\n";

		if (take_log)
			log << boost::posix_time::second_clock::local_time() << '\t'
				<< "Executor pool is shutting down.\n";
		executor_pool.shutDown();
		if (take_log)
			log << boost::posix_time::second_clock::local_time() << '\t'
				<< "Executor pool shut down.\n";

		if (take_log)
			log << boost::posix_time::second_clock::local_time() << '\t'
				<< "Scheduler shut down.\n";
	}
};

#endif /* Scheduler_hpp */
