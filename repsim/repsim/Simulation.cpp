#include "Simulation.h"
#include "elements.h"
SimItem::SimItem()
{
	extime = 0;
	nextItem = NULL;
	prevItem = NULL;
	FuncToCall = NULL;
	name = " ";
	itemsinmem++;
}

SimItem::~SimItem()
{
	itemsinmem--;
}

SimRoot::SimRoot()
{
	curtime = 0;
	nextItem = NULL;
	diagnostics = 0;
}

SimRoot::~SimRoot()
{
	cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << endl;
	this->printlisttimes();
	//execute pair distribution  items, so there are no pairs left in system;
	SimItem * todo;
	todo = nextItem;
	int auxcount = 0;
	while (todo != NULL)
	{
		cout << endl << "asdasdadasdasdasdasdasdasdasdasd" << endl;
		if (todo->name == "rcvch" || todo->name == "thrch" || todo->name == "chrecscr" || todo->name == "chrecscr" || todo->name == "chrecscl")// these items have to be done in order to free the pairs
		{
			//spec case if todo is nextItem
			if (todo == nextItem)
			{
				curtime = nextItem->extime;
				nextItem->FuncToCall();
				SimItem *aux;
				aux = nextItem->nextItem;
				cout << "aux: " << aux << endl;
				delete nextItem;
				nextItem = aux;
				if (nextItem != NULL) nextItem->prevItem = NULL;
				todo = nextItem;
				cout << "spec nt:" << nextItem << endl;
			}
			else
			{
				curtime = todo->extime;
				todo->FuncToCall();
				SimItem *aux1, *aux2;
				aux1 = todo->nextItem;
				aux2 = todo->prevItem;
				delete todo;
				if (aux1 != NULL)
				{
					todo = aux1;

					todo->prevItem = aux2;
					if (aux2 != NULL) aux2->nextItem = todo;
					auxcount++;
					cout << " in ";
				}
				else
				{
					todo = aux1;
					if (aux2 != NULL) aux2->nextItem = todo;
					cout << "iels";
				}
			}
		}
		else
		{
			cout << " else ";
			todo = todo->nextItem;
		}
	}
	int count = 0;
	while (nextItem != NULL)
	{
		SimItem * nextnext=nextItem;
		while (nextnext->nextItem != NULL)
		{
			nextnext = nextnext->nextItem;
		}
		if (nextnext->prevItem != NULL)
		{
			nextnext->prevItem->nextItem = NULL;
		}
		else
		{
			nextItem = NULL;
		}
		//cout << "bbb" << endl;
		//if(nextnext->name == "rcvch" && nextnext->name == "thrch" && nextnext->name == "chrecscr" && nextnext->name == "chrecscr" && nextnext->name == "chrecscl")
		count++;
		delete nextnext;
	}
	cout << "delcount: " << count << endl << "auxcount: " << auxcount << endl;
	cout << "cccccccCCCCCCCCccccccccc" << endl;
}

void SimRoot::Schedule(SimItem * item)
{
	if (item != NULL)
	{

		//cout << endl << "item:" << item << endl;
		//cout << "itemex:" << item->extime << endl;
		if (nextItem != NULL) //list is not empty
		{
			if (item->extime < nextItem->extime) // item is to be inserted to the front of the list
			{
				item->nextItem = nextItem;
				item->prevItem = NULL;
				nextItem = item;
			}
			else //item is not to be inserted to the front of the list
			{
				SimItem * nextnext = nextItem;
				bool goon = true;
				while (nextnext->nextItem != NULL && goon) //while not at the end of list or found place
				{
					if (item->extime < nextnext->nextItem->extime) //item is before next item
					{
						goon = false;
					}
					else
					{
						nextnext = nextnext->nextItem;
					}
				}
				if (!goon) //item is before next item
				{
					item->nextItem = nextnext->nextItem;
					nextnext->nextItem->prevItem = item;
					item->prevItem = nextnext;
					nextnext->nextItem = item;
				}
				else //item is at the end of the list
				{
					nextnext->nextItem = item;
					nextnext->nextItem->prevItem = nextnext;
				}
			}
		}
		else
		{
			nextItem = item;
			nextItem->prevItem = NULL;
		}
	}
	else
	{
		cout << endl << "Tried to schedule NULL item!!" << endl;
	}
}

int SimRoot::ExecuteNext()
{
	if (nextItem != NULL)
	{
		curtime = nextItem->extime;
		nextItem->FuncToCall();
		SimItem *aux;
		aux = nextItem->nextItem;
		delete nextItem;
		nextItem = aux;
		if(nextItem!=NULL) nextItem->prevItem = NULL;
	}
	else
	{
		cout << endl << "Done! " << endl;
		cout << "curtime:" << curtime << endl;
		return 1;
	}
	//cout << "curtime:" << curtime << endl;
	return 0;
}

void SimRoot::printlisttimes()
{
	SimItem * next = nextItem;
	cout << endl << "curtime: " << curtime << endl;
	cout << "nexttimes: ";
	while (next != NULL)
	{
		cout << next->extime << " "<<next->name <<" ";
		next = next->nextItem;
	}
	cout << endl;

}
