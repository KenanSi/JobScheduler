job_scheduler: main.cpp Executor.cpp
	g++ -std=c++14 -o job_scheduler main.cpp Executor.cpp -I. -I ~/boost -L ~/boost/stage/lib -l libboost_date_time.dylib
