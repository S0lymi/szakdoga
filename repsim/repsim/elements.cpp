#include "elements.h"


//EPR
EPR::EPR(Vector4cd * statek, double fidelityk, double ratek)
{
	state = new Vector4cd;
	rate = ratek;
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

QPair * EPR::generatep()
{
	QPair * p = new QPair;
	*(p->state) = *state;
	return p;
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
	aux << jointstate(0 + result * 4), jointstate(1 + result * 4), jointstate(2 + result * 4), jointstate(3 + result * 4); //choosing the new state
	*p1->state = tmat.inverse()*aux;
	*p1->state = *p1->state * (1/sqrt(p1->state->cwiseAbs2().sum())); // norming


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

//Kronecker

Matrix4cd Kronecker(Matrix2cd  m1, Matrix2cd  m2)
{
	Matrix4cd retmat;
	retmat << m1(0, 0)*m2(0, 0), m1(0, 0)*m2(0, 1), m1(0, 1)*m2(0, 0), m1(0, 1)*m2(0, 1),
		m1(0, 0)*m2(1, 0), m1(0, 0)*m2(1, 1), m1(0, 1)*m2(1, 0), m1(0, 1)*m2(1, 1),
		m1(1, 0)*m2(0, 0), m1(1, 0)*m2(0, 1), m1(1, 1)*m2(0, 0), m1(1, 1)*m2(0, 1),
		m1(1, 0)*m2(1, 0), m1(1, 0)*m2(1, 1), m1(1, 1)*m2(1, 0), m1(1, 1)*m2(1, 1);
	return retmat;
}

//NodestoCorrect

void NodestoCorrect(Node * parent, Node ** correctleft, Node **correctright, double * distleft, double * distright)
{
	// leftside
	double dleft = parent->prevdistleft;
	Node * left = parent->prevNodeleft;
	while (left->nextNode != NULL)//down to the EPR generating layer
	{
		dleft = dleft + left->prevdistleft;
		left = left->prevNodeleft;
	}
	dleft = dleft + left->prevdistleft;
	left = left->prevNodeleft;
	*correctleft = left;
	*distleft = dleft;
	//rightside
	double dright = parent->prevdistright;
	Node * right = parent->prevNoderight;
	while (right->nextNode != NULL) //down to EPR layer
	{
		dright = dright + right->prevdistright;
		right = right->prevNoderight;
	}
	dright = dright + right->prevdistright;
	right = right->prevNoderight;
	*correctright = right;
	*distright = dright;

}

int CondDeletePair(QPair * pair)
{
	if (pair->simstate[0] == 1 || pair->simstate[1] == 1)
	{
		delete pair;
	}
	return 0;
}

//Channel

Channel::Channel(double lengthk, double alengthk, Node * fromk,Node* tok, int modek)
{
	to = tok;
	from = fromk;
	length = lengthk;
	alength = alengthk;
	mode = modek;
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

int Channel::SendThrough(SimRoot * Sim, QPair * pair, int index)
{
	//generate random
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	double rand = dis(gen);
	if (mode == 1 || rand <= exp(-length / alength)) // success
	{
		double delay;
		delay = length * 5;
		SimItem * item = new SimItem;
		item->FuncToCall = bind(&Node::ReceiveFromCh, to, Sim, pair, index, this);
		item->extime = Sim->curtime + delay;
		item->name = "rcvch";
		Sim->Schedule(item);
		cout << " chsucc " << endl;
	}
	else //fail -> has to delete pair
	{
		pair->simstate[index] = 1;
		cout << " chfail " << endl;
	}

	return 0;
}

//Node

Node::Node(int memsizek, Node* prevnl , double prevdl, Node* prevnr, double prevdr, Node* nextn, double nextd, EPR *eprk, Channel *leftchk, Channel * rightchk)
{
	memleft = new QMem[memsizek];
	memright = new QMem[memsizek];
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
	type = 0;
	purification = NULL;
	targetfid = 0.98;
}

Node::~Node()
{
	delete[] memleft;
	delete[] memright;
}

int Node::CorrectAfterMeasure(QPair * pair, int result)
{
	Matrix2cd p1, z1, i1;
	i1 << 1, 0, 0, 1;
	p1 << 0, 1, 1, 0;
	z1 << 1, 0, 0, -1;
	if(result==0 )// I gate
	{

	}
	else if (result == 1) // Z gate
	{
		*pair->state = Kronecker(z1, i1) * (*pair->state);
	}
	else if (result == 2) // P gate
	{
		*pair->state = Kronecker(p1, i1) * (*pair->state);
	}
	else if (result == 3) // PZ gate
	{
		*pair->state = Kronecker(z1, i1)* Kronecker(p1, i1) * (*pair->state);
	}
	//signal that pair is ready to be measured
	pair->mem[0]->ReadytoMeasure = true;
	pair->mem[1]->ReadytoMeasure = true;

	//	cout << "rc mems: " << pair->mem[0] << " " << pair->mem[1] << endl;

	return 0;
}

int Node::Bellmeasure(SimRoot * Sim,QMem *m1,QMem* m2)
{
	if(m1->pair!=NULL && m2->pair!=NULL)
	{
		int res;
		if (m1->pairindex != 1)
		{
			m1->pair->SwapIndex();
		}
		if (m2->pairindex != 0)
		{
			m2->pair->SwapIndex();
		}
		m1->pair->agesync();
		m2->pair->agesync();
		res = measure.bmeasure(m1->pair, m2->pair);
		//after measure set
		if (m2->pair->age[0] > m1->pair->age[0])
		{
			m1->pair->age[0] = m2->pair->age[0];
		}
		m2->pair->mem[1]->pair = m1->pair;
		m1->pair->mem[1] = m2->pair->mem[1];
		m1->pair->agesync();
		//m1->pair->mem[0]->ReadytoMeasure = true;
		//m1->pair->mem[1]->ReadytoMeasure = true;
		//Schedule things
		// /*
		//result correction
		double delay = nextdist*5;
		SimItem * item = new SimItem;
		item->FuncToCall = bind(&Node::CorrectAfterMeasure, this, m1->pair, res);
		item->extime = Sim->curtime + delay;
		item->name = "rc";
		Sim->Schedule(item);
		//next operation
			SimItem * next = new SimItem;
			next->FuncToCall = bind(&Node::Updateformeasure, nextNode, Sim);
			next->extime = Sim->curtime + delay;
			next->name = "updms";
			Sim->Schedule(next);
		//check for more measures
			SimItem * check = new SimItem;
			check->FuncToCall = bind(&Node::Updateformeasure, this, Sim);
			check->extime = Sim->curtime;
			check->name = "updmst";
			Sim->Schedule(check);
		//*/
		//freeing memories
		delete m2->pair;
		m2->ReadytoMeasure = false;
		m1->ReadytoMeasure = false;
		m1->pair = NULL;
		m2->pair = NULL;
		m1->state = 0;
		m2->state = 0;

	}
	cout << endl << "Bellmeasure" << endl;
	return 0;
}

int Node::Updateformeasure(SimRoot * Sim)
{
	QMem *m1, *m2;
	m1 = NULL;
	m2 = NULL;
	//check memories ready for measure
	for (int i = 0; i < memsize && m1 == NULL; i++)
	{
		if (memleft[i].ReadytoMeasure && memleft[i].state == 2) m1 = &memleft[i];
	}
	for (int i = 0; i < memsize && m2 == NULL; i++)
	{
		if (memright[i].ReadytoMeasure && memright[i].state == 2) m2 = &memright[i];
	}

	bool pairs_ok =false;
	if (m1 != NULL && m2 != NULL)
	{
		if (m1->pair->mem[0] != NULL && m1->pair->mem[1] != NULL && m2->pair->mem[0] != NULL && m2->pair->mem[1] != NULL)
		{
			pairs_ok = (m1->pair->mem[0]->state == 2 && m1->pair->mem[1]->state == 2 && m2->pair->mem[0]->state == 2 && m2->pair->mem[1]->state == 2); // check the pairs' memory states
		}
	}

	if (m1 != NULL && m2 != NULL && pairs_ok) //Schedule measure
	{
		SimItem * item = new SimItem;
		item->FuncToCall = bind(&Node::Bellmeasure, this, Sim, m1, m2);
		item->extime = Sim->curtime;
		item->name = "bm";
		//schedule measure
		Sim->Schedule(item);
	}

	cout << endl << "Updateformeasure" << endl;
	return 0;
}

int Node::ReceiveFromCh(SimRoot * Sim, QPair * pair, int index, Channel * from)
{
	cout << endl << "Receive: " << pair << " sim: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;
	if (pair->simstate[0] != 1 && pair->simstate[1] != 1)
	{
		if (from->from->prevNoderight == this) // receive to leftside memories 
		{
			cout << "left";
			int check = 0;
			bool found = false;
			for (int i = 0; i < memsize && !found; i++)
			{
				if (memleft[i].pair == NULL)
				{
					found = true;
					check = i;
				}
			}
			if (found) // there's space for the qubit
			{
				cout << "0";
				memleft[check].pair = pair;
				memleft[check].pairindex = index;
				memleft[check].state = 1;
				pair->mem[index] = &memleft[check];
				pair->simstate[index] = 3;
				if (type == 0) // check whether this node is the bottom -> has to start measure chain
				{
					if (prevNodeleft->type == 1) memleft[check].ReadytoMeasure = true;
				}
				else memleft[check].ReadytoMeasure = false;
				//check if both sides of the pair have arrived
				if (pair->mem[0] != NULL && pair->mem[1] != NULL)
				{
					cout << "2" << "  " << pair->mem[0] << "  " << pair->mem[1] << endl;
					if (pair->mem[0]->state == 1 && pair->mem[1]->state == 1)
					{
						Node * leftnode;
						leftnode = from->from->prevNodeleft;
						double delay;
						delay = (from->from->leftch->length + from->from->rightch->length) * 5;
						SimItem *item = new SimItem;
						item->FuncToCall = bind(&Node::ReceiveFromChSuccess, this, Sim, pair, leftnode, this);
						item->extime = Sim->curtime + delay;
						item->name = "chrecscl";
						Sim->Schedule(item);
					}
				}
				cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

			}
			else // delete pair if we cant store it
			{
				//check if other half of the pair have arrived -> has to delete pair
				if (pair->simstate[!index] == 3)
				{
					if (pair->mem[0] != NULL) // correcting memories for other possibly received half of the pair
					{
						pair->mem[0]->pair = NULL;
						pair->mem[0]->ReadytoMeasure = false;
						pair->mem[0]->state = 0;
						pair->mem[0] = NULL;
					}
					if (pair->mem[1] != NULL)
					{
						pair->mem[1]->pair = NULL;
						pair->mem[1]->ReadytoMeasure = false;
						pair->mem[1]->state = 0;
						pair->mem[1] = NULL;
					}
					delete pair;
					cout << "pairdel2" << endl;
				}
				//other half not arrived yet->mark for delete
				else
				{
					pair->simstate[index] = 1;
					cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

				}
			}

		}
		if (from->from->prevNodeleft == this) // receive to rightside memories
		{
			cout << "right";
			int check = 0;
			bool found = false;
			for (int i = 0; i < memsize && !found; i++)
			{
				if (memright[i].pair == NULL)
				{
					found = true;
					check = i;
				}
			}
			if (found) // there's space for the qubit
			{
				cout << "1";
				memright[check].pair = pair;
				memright[check].pairindex = index;
				memright[check].state = 1;
				pair->mem[index] = &memright[check];
				pair->simstate[index] = 3;
				if (type == 0) // check whether this node is the bottom -> has to start measure chain
				{
					if (prevNoderight->type == 1) memright[check].ReadytoMeasure = true;
				}
				else memright[check].ReadytoMeasure = false;
				//check if both sides of the pair have arrived
				if (pair->mem[0] != NULL && pair->mem[1] != NULL)
				{
					cout << "3" << "  " << pair->mem[0] << "  " << pair->mem[1] << endl;
					if (pair->mem[0]->state == 1 && pair->mem[1]->state == 1)
					{
						Node * rightnode;
						rightnode = from->from->prevNoderight;
						double delay;
						delay = (from->from->leftch->length + from->from->rightch->length) * 5;
						SimItem *item = new SimItem;
						item->FuncToCall = bind(&Node::ReceiveFromChSuccess, this, Sim, pair, this, rightnode);
						item->extime = Sim->curtime + delay;
						item->name = "chrecscr";
						Sim->Schedule(item);
					}
				}
				cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

			}
			else // pair has to be deleted
			{
				//check if other half of the pair have arrived -> has to delete pair
				if (pair->simstate[!index] == 3)
				{
					if (pair->mem[0] != NULL) // correcting memories for other possibly received half of the pair
					{
						pair->mem[0]->pair = NULL;
						pair->mem[0]->ReadytoMeasure = false;
						pair->mem[0]->state = 0;
						pair->mem[0] = NULL;
					}
					if (pair->mem[1] != NULL)
					{
						pair->mem[1]->pair = NULL;
						pair->mem[1]->ReadytoMeasure = false;
						pair->mem[1]->state = 0;
						pair->mem[1] = NULL;
					}
					delete pair;
					cout << "pairdel2" << endl;
				}
				//other half not arrived yet->mark for delete
				else
				{
					pair->simstate[index] = 1;
					cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

				}
			}

		}
	}
	else
	{
		delete pair;
		cout << "delpair" << endl;
	}
	return 0;
}

int Node::ReceiveFromChSuccess(SimRoot * Sim, QPair * pair, Node * nodeleft, Node * noderight)
{
	pair->mem[0]->state = 2;
	pair->mem[1]->state = 2;
	//Update the nodes which hold the pair
	SimItem * left = new SimItem;
	left->FuncToCall = bind(&Node::Updateformeasure, nodeleft, Sim);
	left->extime = Sim->curtime;
	left->name = "updmsrcvl";
	Sim->Schedule(left);

	SimItem * right = new SimItem;
	right->FuncToCall = bind(&Node::Updateformeasure, noderight, Sim);
	right->extime = Sim->curtime;
	right->name = "updmsrcvr";
	Sim->Schedule(right);


	return 0;
}

int Node::GenEPR(SimRoot * Sim)
{
	QPair * pair = epr->generatep();
	//send left
	SimItem * left = new SimItem;
	left->FuncToCall = bind(&Channel::SendThrough, leftch, Sim, pair, 0);
	left->extime = Sim->curtime;
	left->name = "thrch";
	Sim->Schedule(left);
	// send right
	SimItem * right = new SimItem;
	right->FuncToCall = bind(&Channel::SendThrough, rightch, Sim, pair, 1);
	right->extime = Sim->curtime;
	right->name = "thrch";
	Sim->Schedule(right);
	//Schedule next generation
	/*
	SimItem * gen = new SimItem;
	gen->FuncToCall = bind(&Node::GenEPR, this, Sim);
	gen->extime = Sim->curtime + epr->rate;
	gen->name = "eprgen";
	Sim->Schedule(gen);
	//*/
	//Schedule conditional delayed deletion
	/*SimItem * del = new SimItem;
	del->FuncToCall = bind(CondDeletePair, pair);
	del->extime = Sim->curtime + (leftch->length + rightch->length) * 2;
	del->name = "delch";
	Sim->Schedule(del);
	//*/
	return 0;
}

Vector4cd Cheapstatefid(double fid)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned int> dist(0, 20000000000);
	std::srand(dist(gen));
	Vector4cd v1, vr;
	v1 << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	vr.setRandom();
	v1 = sqrt(fid)*v1;
	vr(0) = 0;
	vr(3) = 0;
	vr = (vr / sqrt(vr.cwiseAbs2().sum()))*sqrt(1 - fid);
	v1 = v1 + vr;

	return v1;
}

double Vec4Calcfid(Vector4cd state, Vector4cd target)
{
	return (state.adjoint()*(target*target.adjoint())*state).norm();
}

//
int DEJPurif(QPair * pair1, QPair * pair2)
{
	Matrix2cd A, B;
	A << 1, 0.0 - 1i, 0.0 - 1i, 1;
	B << 1, 0.0 + 1i, 0.0 + 1i, 1;
	A = (1 / sqrt(2))*A;
	B = (1 / sqrt(2))*B;
	Matrix4cd joint;
	Matrix4cd AB;
	AB = Kronecker(A, B);
	*pair1->state = AB *(*pair1->state);
	*pair2->state = AB *(*pair2->state);
	Matrix < complex<double>, 16, 16> CNOT2;
	CNOT2.setZero();
	//set CNOT2
	CNOT2(0, 0) = 1;
	CNOT2(1, 1) = 1;
	CNOT2(2, 2) = 1;
	CNOT2(3, 3) = 1;
	CNOT2(4, 5) = 1;
	CNOT2(5, 4) = 1;
	CNOT2(6, 7) = 1;
	CNOT2(7, 6) = 1;
	CNOT2(8, 10) = 1;
	CNOT2(9, 11) = 1;
	CNOT2(10, 8) = 1;
	CNOT2(11, 9) = 1;
	CNOT2(12, 15) = 1;
	CNOT2(13, 14) = 1;
	CNOT2(14, 13) = 1;
	CNOT2(15, 12) = 1;
	//cout << CNOT2;
	joint = (*pair2->state) *(pair1->state->transpose());
	VectorXcd jstate(16);
	for (int i = 0; i < 16; i++) jstate(i) = joint(i);
	jstate = (CNOT2 *jstate);
	//cout << endl << jstate << endl;
	//cout << endl << jstate.cwiseAbs2().sum() << endl;
	double success1 = abs2(jstate(0)) + abs2(jstate(4)) + abs2(jstate(8)) + abs2(jstate(12)) + abs2(jstate(3)) + abs2(jstate(7)) + abs2(jstate(11)) + abs2(jstate(15));
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	double rand = dis(gen);
	int result = 0;
	if (rand < success1)
	{
		for (int i = 0; i < 4; i++)
		{
			(*pair1->state)(i) = jstate(i * 4) + jstate(3 + (i * 4));
		}
		*pair1->state = *pair1->state * (1 / sqrt(pair1->state->cwiseAbs2().sum())); // norming
		result = 1;
	}
	else
	{
		result = 0;
	}
	if (result != 0) //purification successful -> throw away 1 pair
	{
		if (pair2->mem[0] != NULL) pair2->mem[0]->reset();
		if (pair2->mem[1] != NULL) pair2->mem[1]->reset();
		delete pair2;
	}
	else //purification not successful-> throw away both pairs
	{
		if (pair1->mem[0] != NULL) pair1->mem[0]->reset();
		if (pair1->mem[1] != NULL) pair1->mem[1]->reset();
		delete pair1;

		if (pair2->mem[0] != NULL) pair2->mem[0]->reset();
		if (pair2->mem[1] != NULL) pair2->mem[1]->reset();
		delete pair2;
	}
	return result;
}

int DEJ2Purif(QPair * pair1, QPair * pair2)
{
	Vector4cd vt1, vt2, vt3, vt4;
	vt1 << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	vt4 << 1 / sqrt(2), 0, 0, -1 / sqrt(2);
	vt3 << 0, 1 / sqrt(2), 1 / sqrt(2), 0;
	vt2 << 0, 1 / sqrt(2), -1 / sqrt(2), 0;
	Vector4d cp, tp, np;
	cp(0) = Vec4Calcfid(*pair1->state, vt1);
	cp(1) = Vec4Calcfid(*pair1->state, vt2);
	cp(2) = Vec4Calcfid(*pair1->state, vt3);
	cp(3) = Vec4Calcfid(*pair1->state, vt4);

	tp(0) = Vec4Calcfid(*pair2->state, vt1);
	tp(1) = Vec4Calcfid(*pair2->state, vt2);
	tp(2) = Vec4Calcfid(*pair2->state, vt3);
	tp(3) = Vec4Calcfid(*pair2->state, vt4);

	double N = (cp(0) + cp(1))*(tp(0) + tp(1)) + (cp(2) + cp(3))*(tp(2) + tp(3));
	np(0) = (cp(0)*tp(0) + cp(1)*tp(1)) / N;
	//cout << endl << "fid:  " << np(0) << endl;
	*pair1->state = Cheapstatefid(np(0));
	delete pair2;

	return 0;
}