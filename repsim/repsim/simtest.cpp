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

int EPRgen(EPR* epr, QPair* pair,SimRoot * Sim)
{
	*pair = &(epr->generate());
	return 1;
}

int main()
{
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
