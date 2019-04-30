#pragma once
#include <chrono>
#include <iostream>
#include <string>
using namespace std;
class TimingUtil
{
	std::chrono::steady_clock::time_point _start;
	string								  _message;
public:
	TimingUtil(string const & message) :_start(chrono::steady_clock::now()),
		_message(message)
	{}

	~TimingUtil()
	{
	chrono::steady_clock::time_point finish = chrono::steady_clock::now();
	cout << _message << " took "
	<< chrono::duration_cast<chrono::nanoseconds>(finish - _start).count()
	<< " nanosec" << endl;
	}

	void Reset() {
		_start = chrono::steady_clock::now();
	}

	long long GetTime()
	{
		chrono::steady_clock::time_point finish = chrono::steady_clock::now();
		return chrono::duration_cast<chrono::nanoseconds>(finish - _start).count();
	}
};