#pragma once

#include "qrep.h"
#include "elements.h"

class SimItem
{
public:
	double extime;
	SimItem * nextItem;
	SimItem * prevItem;
	function<int()> FuncToCall;

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
};
