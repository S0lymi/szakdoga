#pragma once

#include "qrep.h"
#include <random>

class Node;

class EPR
{
public:
	double fidelity;
	Vector4cd *state;

	//konst
	EPR(Vector4cd * statek = NULL, double fidelityk = 1);
	//dest
	~EPR();
	//other
	void print();
		
	QPair generate();
};
class Pair2Measure
{
public:
	Matrix <complex <double>, 16, 16> * transmat;
	Matrix <complex <double>, 16, 16> * auxtransmat;
	//konst
	Pair2Measure(Matrix <complex <double>, 16, 16> * trmat=NULL, Matrix <complex <double>, 16, 16> * auxtrmat=NULL);
	//dest
	~Pair2Measure();
	//other
	void SetBellMeasure();
	int bmeasure(QPair* p1, QPair* p2);
	void print();

};

class Channel
{
public:
	double alength;
	double length;
	Node * from;
	Node * to;
	//konst
	Channel(double lengthk=0, double alengthk=0,Node* fromk=NULL, Node* to=NULL);

	//dest
	~Channel();

	//other
	bool through(QPair * pair, int pairindex);

};


class Node
{
public:
	int memsize;
	QMem * mem;
	Pair2Measure measure;
	Node * prevNodeleft;
	double prevdistleft;
	Node * prevNoderight;
	double prevdistright;
	Node * nextNode;
	double nextdist;
	EPR * epr;
	Channel* leftch;
	Channel* rightch;

	//konst
	Node(int memsizek=10,Node* prevnl=NULL,double prevdl=0, Node* prevnr = NULL, double prevdr = 0,Node* nextn=NULL,double nextd=0,EPR *eprk=NULL,Channel *leftchk=NULL, Channel * rightchk=NULL);
	//dest
	~Node();

};


double abs2(complex<double> n);
