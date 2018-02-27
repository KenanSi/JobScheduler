//
//  Executor.hpp
//  job_scheduler
//
//  Created by Kenan Si on 25/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef Executor_hpp
#define Executor_hpp

#include <iostream>
#include <chrono>
#include <list>

//#include "Scheduler.hpp"
#include "JobEnv.hpp"
#include "SyncQueue.hpp"

class Scheduler;

class ExecutorPool
{
	bool active = true;
	std::list<std::thread> execution_threads;


	static void executor(SyncQueue<std::shared_ptr<JobEnv>> & queue, 
						 Scheduler & scheduler, bool & active);

public:

	void start(uint _num_executors,
						SyncQueue<std::shared_ptr<JobEnv>> & queue, 
						Scheduler & scheduler)
	{
		for (int i = 0; i < _num_executors; ++i)
			execution_threads.emplace_back(&executor, std::ref(queue), std::ref(scheduler), std::ref(active));
	}

	void shutDown()
	{
		active = false;
		for (auto eth_ptr = execution_threads.begin(); eth_ptr != execution_threads.end(); ++eth_ptr)
			eth_ptr->detach();
	}
};

#endif /* Executor_hpp */
