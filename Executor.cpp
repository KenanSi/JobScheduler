//
//  Executor.cpp
//  job_scheduler
//
//  Created by Kenan Si on 25/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#include "Executor.hpp"
#include "Scheduler.hpp"

void ExecutorPool::executor(SyncQueue<std::shared_ptr<JobEnv>> & queue,
					 Scheduler & scheduler, bool & active)
{
	while (active || queue.size() > 0)
	{
		auto job_env = queue.pop_front();
		job_env->runJob();
		scheduler.postprocess(job_env);
	}
}
