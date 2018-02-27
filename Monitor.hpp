//
//  Monitor.hpp
//  job_scheduler
//
//  Created by Kenan Si on 26/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef Monitor_hpp
#define Monitor_hpp

#include <queue>
#include <boost/date_time.hpp>
#include "SyncQueue.hpp"
#include "JobEnv.hpp"

class JobEnvPtrComparator
{
public:
	bool operator()(const std::shared_ptr<JobEnv> & first_job,
					const std::shared_ptr<JobEnv> & second_job)
	{
		return first_job->getScheduledTime() > second_job->getScheduledTime();
	}
};

using ScheduleQueue =
std::priority_queue
<
	std::shared_ptr<JobEnv>,
	std::vector<std::shared_ptr<JobEnv>>,
	JobEnvPtrComparator
>;

class Monitor
{
private:
	bool active = true;
	std::shared_ptr<std::thread> monitor_thread;

	static void monitor(ScheduleQueue & scheduled_jobs,
						SyncQueue<std::shared_ptr<JobEnv>> & queue,
						bool & active)
	{
		while (active || scheduled_jobs.size() > 0)
		{
			if (!scheduled_jobs.empty())
			{
				if (scheduled_jobs.top()->itsTime())
				{
					queue.push_back(scheduled_jobs.top());
					scheduled_jobs.pop();
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

public:
	void start(ScheduleQueue & scheduled_jobs,
			   SyncQueue<std::shared_ptr<JobEnv>> & queue)
	{
		monitor_thread = std::make_shared<std::thread>(&monitor,
													   std::ref(scheduled_jobs),
													   std::ref(queue),
													   std::ref(active));
	}

	void shutDown()
	{
		active = false;
		monitor_thread->join();
	}

};
#endif /* Monitor_hpp */
