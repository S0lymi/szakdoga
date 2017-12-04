#pragma once

#include <iostream>
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

extern int pairsinmem;
extern int memsinmem;
extern int itemsinmem;
extern int measuresinmem;
extern int nodesinmem;
extern int eprsinmem;
extern int channelsinmem;
extern int withpurifpp;
extern double timelimit;

class QMem;
class Node;

class QPair
{
public:

	QMem * mem[2];
	Vector4cd *state;
	double age[2];
	int simstate[2];

	//konst
	QPair(Vector4cd *statek = NULL, double* agek = NULL, QMem * mem1 = NULL, QMem * mem2 = NULL, int simstate1 = 0, int simstate2 = 0);
	//dest
	~QPair();
	//operators
	void operator=(QPair *other);
	//other
	void print();
	void agesync();
	void SwapIndex();

};

class QMem
{
public:
	QPair * pair;
	int pairindex;
	double maxtime;
	bool ReadytoMeasure;
	double fid;
	int state;
	double rcvtime;
	Node* innode;
	bool inpurif;

	//konst
	QMem(QPair * pairk = NULL, int pairindexk = 0, double maxtime = 0, bool rdy = false, double fidk = 0 , int statek = 0);
	//dest
	~QMem();
	//other
	void print();
	void reset();

};

