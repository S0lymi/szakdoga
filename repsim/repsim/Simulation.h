#pragma once

#include "qrep.h"


class SimItem
{
public:
	double extime;
	SimItem * nextItem;
	SimItem * prevItem;
	function<int()> FuncToCall;
	string name;

	//konst
	SimItem();
	//dest
	~SimItem();
	//other
};
class SimRoot
{
public:
	double curtime;
	SimItem * nextItem;

	//konst
	SimRoot();
	//dest
	~SimRoot();
	//other
	void Schedule(SimItem * item);
	int ExecuteNext();
	void printlisttimes();
};

//purfcation