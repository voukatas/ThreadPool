/*
 * ThreadPool.hpp
 *
 *  Created on: May 24, 2020
 *      Author: voukatas
 */

#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#define DEBUG 1

#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <iostream>

class ThreadPool{

	using Job = std::function<void()>;

private:
	//keep track of threads for cleanup
	std::vector<std::thread> workers;
	std::queue<Job> jobQ;
	std::mutex jobQMutex;
	std::condition_variable jobQCv;
	bool stopExecution = false;

public:

	ThreadPool(std::size_t numOfWorkers){
		start(numOfWorkers);
	}

	~ThreadPool(){
		shutdown();
	}

	void addJob(Job job){
		//create local scope for lock and avoid race conditions
		{
			std::unique_lock<std::mutex> lock{jobQMutex};
		    //emplace creates a std::function<void()> and stores it in our queue
			jobQ.emplace(job);
		}

		//jobQ has a job so notify a thread
		jobQCv.notify_one();

	}

private:

	void start(std::size_t numOfWorkers){

		for(std::size_t i = 0; i < numOfWorkers; i++){
			//create and hold worker threads
			workers.emplace_back([=](){

				std::thread::id worker_id;//only for debugging

				if(DEBUG){
					worker_id = std::this_thread::get_id();
				}

				while(true){

					Job job;

					//critical section below
					//use a lock to protect the jobQ and a cv to avoid unnecessary cpu utilization
					{
						std::unique_lock<std::mutex> lock{jobQMutex};
						jobQCv.wait(lock, [=] { return !jobQ.empty() || stopExecution; });

						if( jobQ.empty() && stopExecution ){
							break;
						}

						//not sure if move or copy is faster,move needs to move the callable and empty the moved-from object.. needs further investigation
						//https://stackoverflow.com/questions/13680587/move-semantic-with-stdfunction
						job = std::move(jobQ.front());
						jobQ.pop();
					}
					//better execute the job out of scope in order to hold the lock less
					job();

					if(DEBUG){
						std::cout<<"Thread:"<<worker_id<<" finished"<<std::endl;
					}

				}
			});
		}
	}

	void shutdown(){

		if(DEBUG){
			//std::cout<<"Shutting down!!!!"<<std::endl;
		}

		{
			std::unique_lock<std::mutex> lock(jobQMutex);
			stopExecution = true;
		}

		jobQCv.notify_all();

		for(std::thread& worker: workers){
			worker.join();
		}
	}

};


#endif /* THREADPOOL_HPP_ */
