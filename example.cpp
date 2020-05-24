/*
 * example.cpp
 *
 *  Created on: May 24, 2020
 *      Author: voukatas
 */

#include <array>
//#include <iostream>
#include <sstream>
#include <string>

#include "ThreadPool.hpp"




template <typename T>
std::vector<T> factorize(T x);

void factor(unsigned long long x);


//find the factors of each of the 6 big numbers to demonstrate the threads that are running.


int main(){

	std::size_t numOfThreads =  std::thread::hardware_concurrency();

    if(numOfThreads < 1){
    	numOfThreads = 1;
    }

    ThreadPool workers{numOfThreads};

    std::cout<<"Number of Threads:"<<numOfThreads<<"\n"<<std::endl;



    std::array<unsigned long long, 6> numbers{ 10000000004, 10000000002, 10000000003,10000000004, 10000000004, 10000000004 };


    for(auto number : numbers){

    	//in case of number modification inside the function the mutable kw should be used
    	workers.addJob([=] () { factor(number); });
    }


//    while(true){
//
//    	std::function<void()> func {[](){std::cout<<"Hello World!"<<std::endl;}};
//
//    }

    //char x;
    //std::cout<<"\nWaiting for threads to calculate and shutdown the threadpool gracefully"<<std::endl;
    //std::cin>>x;

}

template <typename T>
std::vector<T> factorize(T x) {

	std::vector<T> result{ 1 };
	for(T candidate = 2; candidate <= x; candidate++) {
		if(x % candidate == 0) {
			result.push_back(candidate);
			x /= candidate;
			candidate = 1;
		}
	}
  return result;
}

void factor(unsigned long long x) {

	std::vector<unsigned long long> factors;

    factors = factorize(x);
    std::stringstream ss;
    ss << " Number: " << x << " factors: ";
    for(auto factor : factors){
    	ss << factor << " ";
    }
    ss << "\n";
    std::cout<< ss.str();
}
