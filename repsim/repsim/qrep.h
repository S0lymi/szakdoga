#pragma once

#include <iostream>
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

class QMem;

class QPair
{
public:

	QMem * mem[2];
	Vector4cd *state;
	double age[2];
	

	//konst
	QPair(Vector4cd *statek = NULL, double* agek = NULL,QMem * mem1=NULL,QMem * mem2=NULL);
	//dest
	~QPair();
	//operators
	void operator=(QPair *other);
	//other
	void print();
	void agesync();

};

class QMem
{
public:
	QPair * pair;
	int pairindex;
	double maxtime;

	//konst
	QMem(QPair * pairk = NULL, int pairindexk = 0, double maxtime = 0);
	//dest
	~QMem();

};
