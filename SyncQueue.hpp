//
//  SyncQueue.hpp
//  job_scheduler
//
//  Created by Kenan Si on 26/02/2018.
//  Copyright © 2018 Kenan Si. All rights reserved.
//

#ifndef SyncQueue_hpp
#define SyncQueue_hpp

#include <queue>
#include <mutex>
#include <thread>

// Synchronizing queue
template <typename T>
class SyncQueue
{
private:
	std::queue<T> _queue;
	std::mutex _mutex;
	std::condition_variable _empty_condition;

public:
	void push_back(const T & t)
	{
		std::unique_lock<std::mutex> ul(_mutex);
		_queue.push(t);
		ul.unlock();
		_empty_condition.notify_one();
	}

	// Dequeuing function with lock and notification mechanism
	T pop_front()
	{
		std::unique_lock<std::mutex> ul(_mutex);

		// Wait if the queue is empty
		_empty_condition.wait(ul, [this](){ return !_queue.empty(); });

		// Now the queue is not empty, start service for a client
		T result = _queue.front();
		_queue.pop();
		return result;
	}

	std::size_t size()
	{
		std::lock_guard<std::mutex> lg(_mutex);
		return _queue.size();
	}

};
#endif /* SyncQueue_hpp */
