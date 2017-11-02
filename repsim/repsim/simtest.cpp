#include "qrep.h"
#include "elements.h"
#include <random>
typedef struct szam
{
	int sz;

	void print()
	{
		cout << endl << sz << endl;
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

int main()
{
	int i = 1;
	int j = 2;
	function<void()> func2 = bind(plusasd, &j, i);
	function<void()> func = bind(printint, j);
	func();
	func2();
	func();
	

/*
	EPR ep1;
	QPair p1, p2;
	p1 = &ep1.generate();
	p2 = &ep1.generate();
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
