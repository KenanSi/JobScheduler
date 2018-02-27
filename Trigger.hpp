//
//  Trigger.hpp
//  job_scheduler
//
//  Created by Kenan Si on 24/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef Trigger_hpp
#define Trigger_hpp

#include <boost/date_time.hpp>

class Trigger
{
	using ptime = boost::posix_time::ptime;
	using seconds = boost::posix_time::seconds;
private:
	ptime start_time, end_time = ptime(boost::date_time::pos_infin);
	uint repeat_count = 1;
	seconds repeat_interval = seconds{0};
	seconds reschedule_inverval = seconds{1};
	bool active = true;

//	std::shared_ptr<JobEnv> job_env;
public:
	Trigger(std::string start_time_str)
	{
		start_time = ptime(boost::posix_time::time_from_string(start_time_str));
	}

	Trigger(ptime _start_time)
	{
		start_time = _start_time;
	}

	ptime getStartTime() const
	{
		return start_time;
	}

	void setStartTime(std::string time_str)
	{
		start_time = ptime(boost::posix_time::time_from_string(time_str));
	}

	bool isActive() const
	{
		return active;
	}

	void rescheduleTime()
	{
		start_time += reschedule_inverval;
	}

	void setupNextRun()
	{
		--repeat_count;
		if (!end_time.is_pos_infinity())
		{
			if (start_time + repeat_interval > end_time)
				active = false;
		} else
		{
			if (repeat_count == 0)
				active = false;
		}
		if (active)
		{
			start_time += repeat_interval;
		}
	}
};

#endif /* Trigger_hpp */
