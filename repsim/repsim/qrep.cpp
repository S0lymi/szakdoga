#include "qrep.h"

//QPair

QPair::QPair(Vector4cd * statek,double* agek,QMem * mem1,QMem * mem2)
{
	state = new Vector4cd;
	if (statek != NULL)
	{
		*state = *statek;
	}
	else
	{
		*state << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	}
	if (agek != NULL)
	{
		for (int i = 0; i < 2; i++) age[i] = agek[i];
	}
	else
	{
		for (int i = 0; i < 2; i++) age[i] = 0;
	}

	mem[0] = mem1;
	mem[1] = mem2;
}

QPair::~QPair()
{
	if (state != NULL)
	{
		delete state;
	}
}

void QPair::operator=(QPair *other)
{
	if (state != NULL)
	{
		*state = *(other->state);
	}
	else
	{
		state = new Vector4cd;
		*state = *(other->state);
	}
	for (int i = 0; i < 2; i++) age[i] = other->age[i];

}

void QPair::print()
{
	cout << "state:" << endl << *state << endl;
}

void QPair::agesync()
{
	if (age[1] > age[0])
	{
		age[0] = age[1];
	}
	else
	{
		age[1] = age[0];
	}
}




QMem::QMem(QPair * pairk, int pairindexk, double maxtimek)
{
	pair = pairk;
	pairindex = pairindexk;
	maxtime = maxtimek;

}

QMem::~QMem()
{
}