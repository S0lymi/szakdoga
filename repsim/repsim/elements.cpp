#include "elements.h"


//EPR
EPR::EPR(Vector4cd * statek, double fidelityk)
{
	state = new Vector4cd;
	fidelity = fidelityk;
	if (statek != NULL)
	{
		*state = *statek;
	}
	else
	{
		*state << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	}
}

EPR::~EPR()
{
	delete state;
}

void EPR::print()
{
	cout << "initstate:" << endl << *state << endl << "fidelity:" << endl << fidelity << endl;
}

QPair EPR::generate()
{
	return QPair(state);
}

//Pair2Measure

Pair2Measure::Pair2Measure(Matrix<complex<double>, 16, 16>* trmat, Matrix<complex<double>, 16, 16>* auxtrmat)
{
	auxtransmat = new Matrix<complex<double>, 16, 16>;
	transmat = new Matrix<complex<double>, 16, 16>;

	if (auxtrmat != NULL)
	{
		*auxtransmat = *auxtrmat;
	}
	else
	{
		*auxtransmat = Matrix<complex<double>, 16, 16>::Identity(16, 16);
	}
	if (trmat != NULL)
	{
		*transmat = *trmat;
	}
	else
	{
		*transmat= Matrix<complex<double>, 16, 16>::Identity(16, 16);
	}

}

Pair2Measure::~Pair2Measure()
{
	delete transmat;
	delete auxtransmat;
}

void Pair2Measure::SetBellMeasure()
{
	//setting auxtransmat

	Vector4i vt1, vt2;
	vt1 << 0, 0, 0, 0;
	vt2 << 0, 0, 0, 0;
	for (int k = 0; k < 16; k++)
	{
		for (int j = 3; j > 0; j--)
		{
			if (vt1(j) > 1)
			{
				vt1(j - 1) = vt1(j - 1) + 1;
				vt1(j) = 0;
			}

		}
		vt2(0) = vt1(1);
		vt2(1) = vt1(2);
		vt2(2) = vt1(0);
		vt2(3) = vt1(3);

		//cout << endl;
		//for (int i = 0; i<4; i++) cout << vt1(i) << " " << vt2(i) << endl;
		for (int i = 0; i < 16; i++)
		{
			auxtransmat->operator()(k, i) = (vt2(0) * 8 + vt2(1) * 4 + vt2(2) * 2 + vt2(3) == i);
		}

		vt1(3) = vt1(3) + 1;
	}





	//setting transmat
	Vector4cd bv[4];
	bv[0] << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	bv[1] << 1 / sqrt(2), 0, 0, -1 / sqrt(2);
	bv[2] << 0, 1 / sqrt(2), 1 / sqrt(2), 0;
	bv[3] << 0, 1 / sqrt(2), -1 / sqrt(2), 0;
	//cout << bv[0] * bv[0].transpose() << endl;

	Matrix4cd auxmat;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			auxmat = bv[i] * bv[j].transpose();
			for (int k = 0; k < 16; k++)
			{
				transmat->operator()(i * 4 + j, k) = auxmat(k);
			}
		}

	}
}

int Pair2Measure::bmeasure(QPair * p1, QPair * p2)
{
	int result;
	VectorXcd jointstate(16);
	Matrix4cd jointmat;
	//generate random
	std::random_device rd;  
	std::mt19937 gen(rd()); 
	std::uniform_real_distribution<> dis(0.0, 1.0);
	double rand = dis(gen);

	jointmat = *(p1->state) * (p2->state->transpose());
	for (int i = 0; i < 16; i++)
	{
		jointstate(i) = jointmat(i);
	}
	jointstate = *transmat * (*auxtransmat*jointstate);
	double m[4];
	for (int i = 0; i < 4; i++) m[i] = abs2(jointstate(0 + i * 4)) + abs2(jointstate(1 + i * 4)) + abs2(jointstate(2 + i * 4)) + abs2(jointstate(3 + i * 4));
	double chance=0;
	//cout << endl << "rand: " << rand << endl;
	for (int i = 0; i < 4; i++)
	{
		chance = chance + m[i];
		if (chance >= rand)
		{
			result = i;
			i = 4;
		}
	}
	//setting after measure states
	Vector4cd bv[4];
	bv[0] << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	bv[1] << 1 / sqrt(2), 0, 0, -1 / sqrt(2);
	bv[2] << 0, 1 / sqrt(2), 1 / sqrt(2), 0;
	bv[3] << 0, 1 / sqrt(2), -1 / sqrt(2), 0;

	*p2->state = bv[result];
	Matrix4cd tmat;
	tmat <<
		0.707, 0, 0, 0.707,
		0.707, 0, 0, -0.707,
		0, 0.707, 0.707, 0,
		0, 0.707, -0.707, 0;
	Vector4cd aux;
	aux << jointstate(0 + result * 4), jointstate(1 + result * 4), jointstate(2 + result * 4), jointstate(3 + result * 4);
	*p1->state = tmat.inverse()*aux;
	*p1->state = *p1->state * (1/sqrt(p1->state->cwiseAbs2().sum()));


	return result;
}

void Pair2Measure::print()
{
	cout << endl << "aux transformation matrix:" << endl << *auxtransmat << endl << "transformation matrix:" << endl << *transmat << endl;
}

//abs2

double abs2(complex<double> n)
{
	return n.real() * n.real()+n.imag()*n.imag();
}

//Channel

Channel::Channel(double lengthk, double alengthk, Node * fromk,Node* tok)
{
	to = tok;
	from = fromk;
	length = lengthk;
	alength = alengthk;
}

Channel::~Channel()
{
}

bool Channel::through(QPair * pair, int pairindex)
{
	//generate random
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	double rand = dis(gen);
	if (rand >= exp(-length / alength))
	{
		return false;
	}
	else
	{
		return true;
	}
}

//Node

Node::Node(int memsizek, Node* prevnl , double prevdl, Node* prevnr, double prevdr, Node* nextn, double nextd, EPR *eprk, Channel *leftchk, Channel * rightchk)
{
	mem = new QMem[memsizek];
	memsize = memsizek;
	measure.SetBellMeasure();
	prevNodeleft = prevnl;
	prevdistleft = prevdl;
	prevNoderight = prevnr;
	prevdistright = prevdr;
	nextNode = nextn;
	nextdist = nextd;
	epr = eprk;
	leftch = leftchk;
	rightch = rightchk;

}

Node::~Node()
{
	delete[] mem;
}
