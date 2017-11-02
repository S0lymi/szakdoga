#pragma once

#include "qrep.h"
#include <random>



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

	//konst
	Channel(double lengthk, double alengthk);

	//dest
	~Channel();

	//other
	bool through(QPair * pair, int pairindex);

};


double abs2(complex<double> n);
