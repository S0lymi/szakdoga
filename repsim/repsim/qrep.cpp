#include "qrep.h"

int pairsinmem = 0;
int memsinmem = 0;
int itemsinmem = 0;
int measuresinmem = 0;
int nodesinmem = 0;
int eprsinmem = 0;
int channelsinmem =0;
int withpurifpp = 1;
double timelimit = 20000;
//QPair

QPair::QPair(Vector4cd * statek,double* agek,QMem * mem1,QMem * mem2, int simstate1, int simstate2)
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
	simstate[0] = simstate1;
	simstate[1] = simstate2;
	pairsinmem++;
}

QPair::~QPair()
{
	pairsinmem--;
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

void QPair::SwapIndex()
{
	QMem * auxm;
	double auxage;
	Matrix4cd swap;
	swap <<
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1;
	*state = swap* (*state); //this gates swaps represented bit order
	//setting indexes for swapped memories
	if (mem[0] != NULL) 
	{
		mem[0]->pairindex = 1;
	}
	if (mem[1] != NULL)
	{
		mem[1]->pairindex = 0;
	}
	//swapping stored memories
	auxm = mem[0];
	mem[0] = mem[1];
	mem[1] = auxm;
	//swapping stored age counters
	auxage = age[0];
	age[0] = age[1];
	age[1] = auxage;
}


//QMem

QMem::QMem(QPair * pairk, int pairindexk, double maxtimek, bool rdy, double fidk, int statek)
{
	pair = pairk;
	pairindex = pairindexk;
	maxtime = maxtimek;
	ReadytoMeasure = rdy;
	fid = fidk;
	state = statek;
	rcvtime = 0;
	innode = NULL;
	inpurif = false;

	memsinmem++;

}

QMem::~QMem()
{
	if (pair != NULL)
	{
		if (pair->mem[0] != this && pair->mem[0] != NULL)
		{
			pair->mem[0]->reset();
		}
		if (pair->mem[1] != this && pair->mem[1] != NULL)
		{
			pair->mem[1]->reset();
		}
		delete pair;
	}
	memsinmem--;
}

void QMem::print()
{
	cout << endl << "pair: " << pair << endl << "pairindex: " << pairindex << endl << "maxtime: " << maxtime << endl << "rdy: " << ReadytoMeasure << endl << "fid: " << fid << endl;
}

void QMem::reset()
{
	pair = NULL;
	ReadytoMeasure = false;
	state = 0;
	fid = 0;
	rcvtime = 0;
	inpurif = false;
}
