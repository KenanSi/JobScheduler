//
//  Job.hpp
//  job_scheduler
//
//  Created by Kenan Si on 22/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef Job_hpp
#define Job_hpp

#include "JobDataMap.hpp"

class Job
{
public:
	virtual void run(const JobDataMap & input_data_map, JobDataMap & output_data_map) = 0;
};

#endif /* Job_hpp */
