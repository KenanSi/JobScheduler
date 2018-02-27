# Job Scheduler Documents
Authur: Kenan Si
Date: 2018-02-26
Version: 0.0.1

## 0. Design and Motivation
This job scheduler is designed to help its users arrange and run jobs at scheduled time. It utilizes multi-thread technique to achieve concurrency. It is developed with c++, since it's efficient and has good support for multithreading. 

## 1. Structure Overview

The system contains four major parts: 

1. `JobEnv`, i.e. Job Environment, defines a ready-to-run wrapper for the user provided job. It contains all the information for the job to be run. It is composed of `Job`, `JobDataMap`, `Trigger`, and Communication tools. 

	1. `Job` is a generic template for any function that user provides. All user-defined job should derive `Job` class and implement its interface `run()`. 
	2. `JobDataMap` is defined as a generic parameter pack for the job. A `JobEnv` contains a `input_data_map`, a `output_data_map` and a `imported_data_map`. The `output_data_map` stores the final result produced by the job. The `input_data_map` stores the user specified input parameters for the job. The `imported_data_map` stores the result produced by its upstream job, which can also be used as input of the job.
	3. `Trigger` decides when the job should run, whether and how the job should be run periodically.
	4. Communication tools capture the dependency relationship between jobs. `Prerequisites` of the job specifies its upstream jobs, so that the job won't run until all of its prerequisites are filled. `JobListeners` of the job specifies its downstream jobs. After a job is done, it will notify its listeners by passing down the output to them and filling one of their `prerequisites`.

2. `Monitor`, `ExecutorPool` and *Task Queue* are the part that is responsible for running the scheduled jobs in multithreads. It utilizes the *Producer and Consumer* pattern.
	1. *Task Queue* holds a queue of tasks that should be run immediately. It has the type of `SyncQueue` with the template parameter as the smart pointer to `JobEnv`. `SyncQueue` is a synchronised queue, which differs from the usual queue in a way that it can be accessed by a bunch of threads without confliction. 
	2. `Monitor` always looks at the scheduled list of jobs, and check whether it is the time to run the earliest job on the list. If it is, the `Monitor` will pop out the earliest job from the scheduled list and push it into the *task queue*. So, `Monitor` is the *producer* of tasks in this structure. Since it should always keep an eye on the clock and the scheduled list, it runs in its own thread.
	3. `ExecutorPool` is a pool of `Executor`s. `Executor` is the *consumer* of the tasks. Once the task queue is not empty, it will take out one task and run it. Since every job should run on its scheduled time, it is very likely that several jobs run in parallel. Therefore, we use a pool of executors to run the jobs and each of the executors runs in their own thread.
	
3. `Scheduler` is the center of the system. It holds the scheduled list. It provides interfaces, like `addJob()`, `modifyJob()` and `cancelJob()`, that allow the user to manage the task schedule. 

4. `Console User Interface` is an interactive user interface that allow the user to access and use this application through console. *This part is still under development.*


## 2. Current Functionality

1. Add job to the schedule and run at scheduled time
2. Allow job dependency and data communication
3. Run the job with any specified parameter
4. Log the system info into text file

## 3. Example
The following piece of code is picked from `main()`. It shows how we create a Job and add it to schedule. In this piece of code, for each job, we first create and setup three components of the `JobEnv`. Then, we let `job_b` depends on `job_a`. Finally, we add all jobs into the `scheduler`. This example also demonstrates how the system reacts under the circumstances that one job's prerequisites are not all filled and that one job throw exception when running.

### Create Job and add it to scheduler

~~~c++
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
~~~

### Then we can first see how the system deals with job dependency.

The `job_a`'s implementation is the following. Basicly, it prints "Hello date" into specified output path, sleep for 5 seconds (mimic time consuming job), and add result to `output_data_map`.

~~~c++
	void run(const JobDataMap & input_data_map, JobDataMap & output_data_map) override
	{
		auto object_date = input_data_map.getDate("object_date");
		auto output_path = input_data_map.getString("output_path");
		std::ofstream fout(output_path);
		fout << "Hello, " << object_date << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds{5});
		output_data_map.setString("result_from_SayHelloJob", "Hello!");
	}
~~~

The `job_b`'s implementation is the following. Basicly, it first print the result got from its upstream, sleeps for 2 seconds and prints "Goodbye date" into specified output path.

~~~c++
	void run(const JobDataMap & input_data_map, JobDataMap & output_data_map) override
	{
		auto object_date = input_data_map.getDate("object_date");
		auto output_path = input_data_map.getString("output_path");
		std::ofstream fout(output_path);
		fout << "Get result from SayHelloJob: " 
			 << input_data_map.getString("result_from_SayHelloJob") << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds{2});
		fout << "GoodBye, " << object_date << std::endl;
	}
~~~

The following piece of output is selected from the system log.

```
2018-Feb-26 23:05:47	J001 is scheduled at 2018-Feb-26 23:05:48.
2018-Feb-26 23:05:47	J002 is scheduled at 2018-Feb-26 23:05:50.
2018-Feb-26 23:05:50	J002 didn't run. The upstream jobs are not done yet.
2018-Feb-26 23:05:50	J002 is scheduled at 2018-Feb-26 23:05:51.
2018-Feb-26 23:05:51	J002 didn't run. The upstream jobs are not done yet.
2018-Feb-26 23:05:51	J002 is scheduled at 2018-Feb-26 23:05:52.
2018-Feb-26 23:05:52	J002 didn't run. The upstream jobs are not done yet.
2018-Feb-26 23:05:52	J002 is scheduled at 2018-Feb-26 23:05:53.
2018-Feb-26 23:05:53	J002 didn't run. The upstream jobs are not done yet.
2018-Feb-26 23:05:53	J002 is scheduled at 2018-Feb-26 23:05:54.
2018-Feb-26 23:05:53	J001 done.
2018-Feb-26 23:05:56	J002 done.
```

From our setting, we know that `job_a`(JobID: J001) needs 5 seconds to finish while `job_b`(JobID: J002) is scheduled 2 seconds after `job_a`. So, when the system first try to run `job_b`, it will be blocked by the prerequisite check. Then the system will reschedule `job_b` 1 second later (user is allowed to change the frequency). One second later, the system will try to rerun `job_b`, but it's got blocked again. This process goes on until the `job_a` is done. Then `job_b` starts at `2018-Feb-26 23:05:54` and takes 2 seconds to finish.

### Finally, the system will log the error message and continue if some job throws exception.

The `job_c`'s implementation is the following. Basicly, it tries to print "Failed, date" into specified output path.

~~~c++
	void run(const JobDataMap & input_data_map, JobDataMap & output_data_map) override
	{
		auto object_date = input_data_map.getDate("object_date");
		auto output_path = input_data_map.getString("output_path");
		std::ofstream fout(output_path);
		fout << "Failed, " << object_date << std::endl;
	}
~~~
But since we did't add `object_date` and `output_path` into `job_c`'s `input_data_map`, it will throw `NoRequestedData` exception. The system would record this error into log and continue to run next scheduled job. In the system log, we can find the following selected output about `job_c`(JobID: J003).

```
2018-Feb-26 23:05:47	J003 is scheduled at 2018-Feb-26 23:05:52.
2018-Feb-26 23:05:52	J003 failed. Error message: No data called "object_date" found.
```

## 4. Future Developments
1. Console UI used for interacting with the user at run time.
2. The ability to rerun historical jobs mannually with provided parameter.
3. The flexibility to rerun jobs with different settings, like not notifying downstream jobs or running without prerequisite check.