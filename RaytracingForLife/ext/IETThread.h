#pragma once
#include <iostream>
#include <thread>

class IETThread
{
public:
	IETThread();
	~IETThread();

	void start(); //schedules thread execution
	static void sleep(int ms);

protected:
	virtual void run() = 0;
};