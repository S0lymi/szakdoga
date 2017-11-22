#pragma once

#include "qrep.h"
#include <random>
#include "Simulation.h"

class Node;

class EPR
{
public:
	double fidelity;
	Vector4cd *state;
	double rate;

	//konst
	EPR(Vector4cd * statek = NULL, double fidelityk = 1,double ratek = 10);
	//dest
	~EPR();
	//other
	void print();
		
	QPair generate();
	QPair * generatep();
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
	int mode;
	//konst
	Channel(double lengthk = 0, double alengthk = 0, Node* fromk = NULL, Node* to = NULL, int modek = 0);

	//dest
	~Channel();

	//other
	bool through(QPair * pair, int pairindex);

	int SendThrough(SimRoot * Sim, QPair * pair, int index);

};


class Node
{
public:
	int memsize;
	QMem * memleft;
	QMem * memright;
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
	int type;
	function<int(QMem*,int,double)> purification;
	double targetfid;
	int epratonce;
	//konst
	Node(int memsizek=10,Node* prevnl=NULL,double prevdl=0, Node* prevnr = NULL, double prevdr = 0,Node* nextn=NULL,double nextd=0,EPR *eprk=NULL,Channel *leftchk=NULL, Channel * rightchk=NULL);
	//dest
	~Node();
	//other
	int ChangeMemsize(int newmemsize);

	int CorrectAfterMeasure(QPair * pair, int result);// local operation to get the desired Bell state after measure

	int Bellmeasure(SimRoot * Sim, QMem *m1, QMem* m2); // does the Bell-measure

	int Updateformeasure(SimRoot * Sim);

	int ReceiveFromCh(SimRoot * Sim, QPair * pair, int index, Channel * from);

	int ReceiveFromChSuccess(SimRoot *Sim, QPair * pair, Node * nodeleft, Node * noderight);

	int GenEPR(SimRoot * Sim);

	int Purify(SimRoot * Sim);

	int printmemstates();
};


double abs2(complex<double> n);

Matrix4cd Kronecker(Matrix2cd  m1, Matrix2cd  m2);

void NodestoCorrect(Node * parent, Node ** correctleft, Node **correctright, double * distleft, double * distright);

int CondDeletePair(QPair* pair);

Vector4cd Cheapstatefid(double fid);

double Vec4Calcfid(Vector4cd state, Vector4cd target);

int DEJPurif(QPair * pair1, QPair * pair2);

int DEJ2Purif(QPair * pair1, QPair * pair2);

int GreedyBU_DEJPurif(QMem * mem, int memsize, double targetfid);

void FidSortMems(QMem ** mems, int size);

double Vec4Calcstdfid(Vector4cd state);