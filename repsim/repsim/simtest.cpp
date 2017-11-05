#include "qrep.h"
#include "elements.h"
#include <random>
#include "Simulation.h"
class szam
{
public:
	int sz;

	void print()
	{
		cout << endl << sz << endl;
	}
	int recprint(SimRoot * Sim,double delay)
	{
		cout << endl << sz << endl;
		SimItem* item = new SimItem;
		item->FuncToCall = bind(&szam::recprint,this,Sim,delay);
		item->extime = Sim->curtime + delay;
		Sim->Schedule(item);
		//cout <<"curtime: "<< Sim->curtime << endl;
		return 1;
	}
	int printi()
	{
		cout << endl << sz << endl;
		return 1;
	}
};

void plusasd(int * i1, int i2)
{
	*i1 = *i1 + i2;
}
void printint(int i)
{
	cout << endl << i << endl;
}

int EPRgen(EPR* epr, Channel * ch1, Channel*ch2, SimRoot * Sim)
{
	QPair* pair = new QPair;
	*pair = &(epr->generate());
	return 1;
}

void Makepow2tree(Node nodes[], int min, int max, Node * parent,int pindex,double dist)
{
	if ((min % 2) != 0 || (max % 2) != 0) // got to the bottom of the tree
	{
		return;
	}

	int index = (min + max) / 2; //get the middle
	nodes[index].nextNode = parent;
	if (pindex > index) 
	{ 
		parent->prevNodeleft = &nodes[index]; 
		parent->prevdistleft = dist / 2;
	}
	if (pindex < index) 
	{ 
		parent->prevNoderight = &nodes[index]; 
		parent->prevdistright = dist / 2;
	}
	Makepow2tree(nodes, min, (min + max) / 2, &nodes[index], index, dist / 2); //make the leaves
	Makepow2tree(nodes, (min + max) / 2, max, &nodes[index], index, dist / 2);

}

void printNodeinf(Node* node)
{
	cout << endl << "parent:" << node->nextNode << endl << "self: " << node << endl << "left: " << node->prevNodeleft << "   dist: " << node->prevdistleft <<
		endl << "rigt: " << node->prevNoderight << "   dist: " << node->prevdistright << endl;
}


int main()
{
	Pair2Measure bellm;
	bellm.SetBellMeasure();

	Node n1;
	//cout << endl << *n1.measure.auxtransmat << endl << *n1.measure.transmat << endl;
	Node nodes[9];
	Node eprnodes[8];
	Channel channels[16];
	EPR std_epr;
	for (int i = 0; i < 16; i++)
	{
		channels[i].from = &eprnodes[i / 2];
		channels[i].to = &nodes[(i + 1) / 2];
		channels[i].length = 2;
		channels[i].alength = 1;
	}
	for (int i = 0; i < 8; i++)
	{
		eprnodes[i].prevNodeleft = &nodes[i];
		eprnodes[i].prevNoderight = &nodes[i + 1];
		eprnodes[i].leftch = &channels[i * 2];
		eprnodes[i].rightch = &channels[i * 2 + 1];
		eprnodes[i].epr = &std_epr;
	}
	Makepow2tree(nodes, 0, 8, &nodes[8], 8, 80);
	cout << "5" << endl;
	printNodeinf(&nodes[4]);
	cout << "7" << endl;
	printNodeinf(&nodes[6]);
	cout << "3" << endl;
	printNodeinf(&nodes[2]);


	/*
	int a = 0;
	SimRoot Sim;
	szam sz1, sz2,sz3;
	sz1.sz = 12;
	sz2.sz = 34;
	sz3.sz = 56;
	SimItem * item1 = new SimItem;
	item1->extime = 10;
	item1->FuncToCall = bind(&szam::printi, sz1);
	SimItem * item2 = new SimItem;
	item2->extime = 6;
	item2->FuncToCall = bind(&szam::printi, sz2);
	sz3.recprint(&Sim, 3);
	Sim.Schedule(item1);
	Sim.Schedule(item2);

	cin >> a;

	while (a != 1)
	{
		Sim.ExecuteNext();
		cin >> a;
	}
	*/
	


	/*
	int i = 1;
	int j = 2;
	function<void()> func2 = bind(plusasd, &j, i);
	function<void()> func = bind(printint, j);
	func();
	func2();
	func();



	EPR ep1;
	QPair p1, p2;
	p1 = &ep1.generate();
	p2 = &ep1.generate();
	*/
	/*
	p1.print();
	p2.print();
	Pair2Measure Bellm;
	Bellm.SetBellMeasure();
	Bellm.bmeasure(&p1, &p2);
	p1.print();
	p2.print();

	szam sz1;
	sz1.sz = 2;
	function<void()> fszam = bind(&szam::print, sz1);
	sz1.print();
	sz1.sz = sz1.sz + 1;
	sz1.print();
	fszam();
	*/
}
