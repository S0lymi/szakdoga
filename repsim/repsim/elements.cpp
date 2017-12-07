#include "elements.h"


//EPR
EPR::EPR(Vector4cd * statek, double fidelityk, double ratek)
{
	eprsinmem++;

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
	eprsinmem--;

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
	measuresinmem++;
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
	measuresinmem--;
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
	channelsinmem++;


	to = tok;
	from = fromk;
	length = lengthk;
	alength = alengthk;
	mode = modek;
}

Channel::~Channel()
{
	channelsinmem--;
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
		//if (Sim->diagnostics != 0) cout << " chsucc " << endl;
	}
	else //fail -> has to delete pair
	{
		pair->simstate[index] = 1;
		//if (Sim->diagnostics != 0) cout << " chfail " << endl;
		if (pair->simstate[0] == 1 && pair->simstate[1] == 1) // both channels failed( no rcvch to delete pair-> has to delete)
		{
			delete pair;
		}
	}

	return 0;
}

//Node

Node::Node(int memsizek, Node* prevnl , double prevdl, Node* prevnr, double prevdr, Node* nextn, double nextd, EPR *eprk, Channel *leftchk, Channel * rightchk)
{
	nodesinmem++;

	memleft = new QMem[memsizek];
	memright = new QMem[memsizek];
	memsize = memsizek;
	for (int i = 0; i < memsize; i++) 
	{
		memleft[i].innode = this;
	}
	for (int i = 0; i < memsize; i++)
	{
		memright[i].innode = this;
	}
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
	epratonce = 1;
	physNodeleft = NULL;
	physNoderight = NULL;
	physdistleft = 0;
	physdistright = 0;
}

Node::~Node()
{
	nodesinmem--;

	delete[] memleft;
	delete[] memright;
}

int Node::ChangeMemsize(int newmemsize)
{
	if (memleft == NULL) return 0;
	if (memright == NULL) return 0;
	delete[] memleft;
	delete[] memright;
	memsize = newmemsize;
	memleft = new QMem[memsize];
	memright = new QMem[memsize];
	for (int i = 0; i < memsize; i++)
	{
		memleft[i].innode = this;
	}
	for (int i = 0; i < memsize; i++)
	{
		memright[i].innode = this;
	}
	return 1;
}

int Node::CorrectAfterMeasure(QPair * pair, int result)
{
	//cout << "correct";
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
	else if (result == 2) // X gate
	{
		*pair->state = Kronecker(p1, i1) * (*pair->state);
	}
	else if (result == 3) // ZX gate
	{
		*pair->state = Kronecker(z1, i1)* Kronecker(p1, i1) * (*pair->state);
	}
	//signal that pair is ready to be measured
	pair->mem[0]->ReadytoMeasure = true;
	pair->mem[1]->ReadytoMeasure = true;
	double fidelity = Vec4Calcstdfid(*pair->state);
	pair->mem[0]->fid = fidelity;
	pair->mem[1]->fid = fidelity;
	//	cout << "rc mems: " << pair->mem[0] << " " << pair->mem[1] << endl;

	return 0;
}

int Node::Bellmeasure(SimRoot * Sim,QMem *m1,QMem* m2)
{
	//cout << endl << "BellMeasure" << endl;
	if (m1->pair != NULL && m2->pair != NULL)
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
		//cout << "BM done" << endl;
		//set states for next measure in line
		if (nextNode->prevNoderight != NULL)
		{
			if (nextNode->prevNoderight == this) //leftside is next
			{
				m1->pair->mem[!m1->pairindex]->state = 3;
			}
		}

		if (nextNode->prevNodeleft != NULL)
		{
			if (nextNode->prevNodeleft == this) //rightside is next
			{
				m2->pair->mem[!m2->pairindex]->state = 3;
			}
		}
		//after measure set
		if (m2->pair->age[0] > m1->pair->age[0])
		{
			m1->pair->age[0] = m2->pair->age[0];
		}
		m2->pair->mem[1]->pair = m1->pair;
		m1->pair->mem[1] = m2->pair->mem[1];
		m1->pair->agesync();
		m1->pair->mem[0]->ReadytoMeasure = false; //Not ready until result corrected
		m1->pair->mem[1]->ReadytoMeasure = false;
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
		m1->reset();
		m2->reset();

	}
	//if (Sim->diagnostics != 0) cout << endl << "Bellmeasure" << endl;
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
		if (memleft[i].ReadytoMeasure && memleft[i].state == 3 && memleft[i].fid >= targetfid && !memleft[i].inpurif) m1 = &memleft[i];
	}
	for (int i = 0; i < memsize && m2 == NULL; i++)
	{
		if (memright[i].ReadytoMeasure && memright[i].state == 3 && memright[i].fid >= targetfid && !memright[i].inpurif) m2 = &memright[i];
	}

	bool pairs_ok =false;
	if (m1 != NULL && m2 != NULL)
	{
		if (m1->pair->mem[0] != NULL && m1->pair->mem[1] != NULL && m2->pair->mem[0] != NULL && m2->pair->mem[1] != NULL)
		{
			pairs_ok = (m1->pair->mem[0]->state >= 2 && m1->pair->mem[1]->state >= 2 && m2->pair->mem[0]->state >= 2 && m2->pair->mem[1]->state >= 2); // check the pairs' memory states
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
	//Schedule purification if memories are close to full, and there are pairs that can be purified
	//checking left memories
	int left_freemems=0;
	int left_other_freemems=memsize;
	int left_mems_to_purif=0;
	for (int i = 0; i < memsize; i++)
	{
		if (memleft[i].pair == NULL) left_freemems = left_freemems + 1;
		if (memleft[i].fid < targetfid && memleft[i].ReadytoMeasure && memleft[i].state == 3) left_mems_to_purif = left_mems_to_purif + 1;
	}
	if (prevNodeleft != NULL) // check if its the bottom, then get available memories from the neigbouring node too
	{
		if (prevNodeleft->type == 1)
		{
			left_other_freemems = 0;
			for (int i = 0; i < prevNodeleft->prevNodeleft->memsize; i++)
			{
				if (prevNodeleft->prevNodeleft->memright[i].pair == NULL) left_other_freemems = left_other_freemems + 1;
			}
		}
	}
	if (left_other_freemems < left_freemems) left_freemems = left_other_freemems;

	//checking right memories
	int right_freemems = 0;
	int right_mems_to_purif = 0;
	int right_other_freemems=memsize;
	for (int i = 0; i < memsize; i++)
	{
		if (memright[i].pair == NULL) right_freemems = right_freemems + 1;
		if (memright[i].fid < targetfid && memright[i].ReadytoMeasure && memright[i].state == 3) right_mems_to_purif = right_mems_to_purif + 1;
	}
	if (prevNoderight != NULL) // check if its the bottom, then get available memories from the neigbouring node too
	{
		if (prevNoderight->type == 1)
		{
			right_other_freemems = 0;
			for (int i = 0; i < prevNoderight->prevNoderight->memsize; i++)
			{
				if (prevNoderight->prevNoderight->memleft[i].pair == NULL) right_other_freemems = right_other_freemems + 1;
			}
		}
	}
	if (right_other_freemems < right_freemems) right_freemems = right_other_freemems;
	int in=0;
	//cout << endl << left_freemems << "  " << left_mems_to_purif << "  " << Checkforpurif(memleft, memsize, targetfid) << endl;
	//cin >> in;
	if(left_freemems < 2 && left_mems_to_purif>1 && Checkforpurif(memleft,memsize,targetfid)) //check if leftside should purify
	{ 	//Schedule purification
		if (withpurifpp == 1)
		{
			QMem** reserved = ReserveMemsforPurif(memleft, memsize);
			QMem* distmem = NULL;
			for (int i = 0; i < memsize && distmem == NULL; i++)
			{
				if (reserved[i] != NULL) distmem = reserved[i];
			}
			//cout << endl << distmem->pair->mem[0]->innode<<"   "<< distmem->pair->mem[1]->innode << endl;
			double delay = GetDistofNodes(distmem->pair->mem[0]->innode, distmem->pair->mem[1]->innode) *5;
			//cout << endl << "delay: " << delay << endl;
			//cin >> in;
			SimItem *purifl = new SimItem;
			purifl->FuncToCall = bind(this->purification, Sim, reserved, memsize, targetfid);
			purifl->extime = Sim->curtime+delay;
			purifl->name = "purleft";
			Sim->Schedule(purifl);
			//check again after purification
			SimItem * check = new SimItem;
			check->FuncToCall = bind(&Node::Updateformeasure, this, Sim);
			check->extime = Sim->curtime+delay;
			check->name = "updpurt";
			Sim->Schedule(check);
		}
		else
		{
			QMem ** reserved = ReserveMemsforPurif(memleft, memsize);
			SimItem *purifl = new SimItem;
			purifl->FuncToCall = bind(this->purification, Sim, reserved, memsize, targetfid);
			purifl->extime = Sim->curtime;
			purifl->name = "purleft";
			Sim->Schedule(purifl);
			//check again after purification
			SimItem * check = new SimItem;
			check->FuncToCall = bind(&Node::Updateformeasure, this, Sim);
			check->extime = Sim->curtime;
			check->name = "updpurt";
			Sim->Schedule(check);
		}
		
	}
	if (right_freemems < 2 && right_mems_to_purif>1 && Checkforpurif(memright, memsize, targetfid)) // check if rightside should purify
	{
		if (withpurifpp == 1)
		{
			QMem** reserved = ReserveMemsforPurif(memright, memsize);
			QMem* distmem = NULL;
			for (int i = 0; i < memsize && distmem == NULL; i++)
			{
				if (reserved[i] != NULL) distmem = reserved[i];
			}

			double delay = GetDistofNodes(distmem->pair->mem[0]->innode, distmem->pair->mem[1]->innode) * 5;
			//Schedule purification
			SimItem *purifr = new SimItem;
			purifr->FuncToCall = bind(this->purification, Sim, reserved, memsize, targetfid);
			purifr->extime = Sim->curtime + delay;
			purifr->name = "purright";
			Sim->Schedule(purifr);
			//check again after purification
			SimItem * check = new SimItem;
			check->FuncToCall = bind(&Node::Updateformeasure, this, Sim);
			check->extime = Sim->curtime + delay;
			check->name = "updpurt";
			Sim->Schedule(check);
		}
		else
		{
			QMem **reserved = ReserveMemsforPurif(memright, memsize);
			//Schedule purification
			SimItem *purifr = new SimItem;
			purifr->FuncToCall = bind(this->purification, Sim, reserved, memsize, targetfid);
			purifr->extime = Sim->curtime;
			purifr->name = "purright";
			Sim->Schedule(purifr);
			//check again after purification
			SimItem * check = new SimItem;
			check->FuncToCall = bind(&Node::Updateformeasure, this, Sim);
			check->extime = Sim->curtime;
			check->name = "updpurt";
			Sim->Schedule(check);
		}
	}

	if (Sim->diagnostics != 0) cout << endl << "Updateformeasure" << endl;
	if (Sim->diagnostics != 0) cout << "leftfree: " << left_freemems << "  rightfree:  " << right_freemems << endl;
	return 0;
}

int Node::ReceiveFromCh(SimRoot * Sim, QPair * pair, int index, Channel * from)
{
	if (Sim->diagnostics != 0) cout << endl << "Receive: " << pair << " sim: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;
	if (pair->simstate[0] != 1 && pair->simstate[1] != 1)
	{
		if (from->from->prevNoderight == this) // receive to leftside memories 
		{
			if (Sim->diagnostics != 0) cout << "left";
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
				if (Sim->diagnostics != 0) cout << "0";
				memleft[check].pair = pair;
				memleft[check].pairindex = index;
				memleft[check].state = 1;
				memleft[check].rcvtime = Sim->curtime;
				pair->mem[index] = &memleft[check];
				pair->simstate[index] = 3;
				if (type != 1) // check whether this node is the bottom -> has to start measure chain
				{
					if (prevNodeleft != NULL)
					{
						if (prevNodeleft->type == 1) memleft[check].ReadytoMeasure = true;
					}
				}
				else
				{
					memleft[check].ReadytoMeasure = false;
				}
				//check if both sides of the pair have arrived
				if (pair->mem[0] != NULL && pair->mem[1] != NULL)
				{
					if (Sim->diagnostics != 0) cout << "2" << "  " << pair->mem[0] << "  " << pair->mem[1] << endl;
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
				if (Sim->diagnostics != 0) cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

			}
			else // delete pair if we cant store it
			{
				//check if other half of the pair have arrived -> has to delete pair
				if (pair->simstate[!index] == 3)
				{
					if (pair->mem[0] != NULL) // correcting memories for other possibly received half of the pair
					{
						pair->mem[0]->reset();
						pair->mem[0] = NULL;
					}
					if (pair->mem[1] != NULL)
					{
						pair->mem[1]->reset();
						pair->mem[1] = NULL;
					}
					delete pair;
					if (Sim->diagnostics != 0) cout << "pairdel2" << endl;
				}
				//other half not arrived yet->mark for delete
				else
				{
					pair->simstate[index] = 1;
					if (Sim->diagnostics != 0) cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

				}
			}

		}
		if (from->from->prevNodeleft == this) // receive to rightside memories
		{
			if (Sim->diagnostics != 0) cout << "right";
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
				if (Sim->diagnostics != 0) cout << "1";
				memright[check].pair = pair;
				memright[check].pairindex = index;
				memright[check].state = 1;
				memright[check].rcvtime = Sim->curtime;
				pair->mem[index] = &memright[check];
				pair->simstate[index] = 3;
				if (type != 1) // check whether this node is the bottom -> has to start measure chain
				{
					if (prevNoderight != NULL)
					{
						if (prevNoderight->type == 1) memright[check].ReadytoMeasure = true;
					}
				}
				else memright[check].ReadytoMeasure = false;
				//check if both sides of the pair have arrived
				if (pair->mem[0] != NULL && pair->mem[1] != NULL)
				{
					if (Sim->diagnostics != 0) cout << "3" << "  " << pair->mem[0] << "  " << pair->mem[1] << endl;
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
				if (Sim->diagnostics != 0) cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

			}
			else // pair has to be deleted
			{
				//check if other half of the pair have arrived -> has to delete pair
				if (pair->simstate[!index] == 3)
				{
					if (pair->mem[0] != NULL) // correcting memories for other possibly received half of the pair
					{
						pair->mem[0]->reset();
						pair->mem[0] = NULL;
					}
					if (pair->mem[1] != NULL)
					{
						pair->mem[1]->reset();
						pair->mem[1] = NULL;
					}
					delete pair;
					if (Sim->diagnostics != 0) cout << "pairdel2" << endl;
				}
				//other half not arrived yet->mark for delete
				else
				{
					pair->simstate[index] = 1;
					if (Sim->diagnostics != 0) cout << endl << "Simafter: " << pair->simstate[0] << "  " << pair->simstate[1] << endl;

				}
			}

		}
	}
	else
	{
		delete pair;
		if(Sim->diagnostics != 0) cout << "delpair" << endl;
	}
	return 0;
}

int Node::ReceiveFromChSuccess(SimRoot * Sim, QPair * pair, Node * nodeleft, Node * noderight)
{
	pair->mem[0]->state = 2;
	pair->mem[1]->state = 2;


	double fidelity = Vec4Calcstdfid(*pair->state);
	pair->mem[0]->fid = fidelity;
	pair->mem[1]->fid = fidelity;
	//set memory states according to place in chain
	if (nodeleft->type != 1) // check whether this node is the bottom -> has to start measure chain
	{
		if (nodeleft->prevNoderight != NULL)
		{
			if (nodeleft->prevNoderight->type == 1)
			{
				for (int i = 0; i < nodeleft->memsize; i++)//search for leftside memory of pair
				{
					if (pair == nodeleft->memright[i].pair)
					{
						nodeleft->memright[i].state = 3;
					}
				}
			}
		}
	}

	if (noderight->type != 1) // check whether this node is the bottom -> has to start measure chain
	{
		if (noderight->prevNodeleft != NULL)
		{
			if (noderight->prevNodeleft->type == 1)
			{
				for (int i = 0; i < noderight->memsize; i++)//search for leftside memory of pair
				{
					if (pair == noderight->memleft[i].pair)
					{
						noderight->memleft[i].state = 3;
					}
				}
			}
		}
	}

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

	SimItem * right;
	SimItem * left;
	QPair * pair;
	SimItem * gen;
	for (int i = 0; i < epratonce; i++)
	{
		pair = epr->generatep();
		*pair->state = Cheapstatefid(epr->fidelity);
		//send left
		left = new SimItem;
		left->FuncToCall = bind(&Channel::SendThrough, leftch, Sim, pair, 0);
		left->extime = Sim->curtime;
		left->name = "thrch";
		Sim->Schedule(left);
		// send right
		right = new SimItem;
		right->FuncToCall = bind(&Channel::SendThrough, rightch, Sim, pair, 1);
		right->extime = Sim->curtime;
		right->name = "thrch";
		Sim->Schedule(right);
		//Schedule next generation
		///*
		//*/
		//Schedule conditional delayed deletion
		/*SimItem * del = new SimItem;
		del->FuncToCall = bind(CondDeletePair, pair);
		del->extime = Sim->curtime + (leftch->length + rightch->length) * 2;
		del->name = "delch";
		Sim->Schedule(del);
		//*/
	}
	//generate again
	gen = new SimItem;
	gen->FuncToCall = bind(&Node::GenEPR, this, Sim);
	gen->extime = Sim->curtime + epr->rate;
	gen->name = "eprgen";
	Sim->Schedule(gen);
	return 0;
}

int Node::Purify(SimRoot * Sim)
{
	//call own purification method
	//purification(this);

	//Schedule Update
	SimItem* update = new SimItem;

	return 0;
}

int Node::printmemstates()
{
	cout << endl << "leftmems: " << endl;
	for (int i = 0; i < memsize; i++)
	{
		cout << " fid: " << memleft[i].fid << " state: " << memleft[i].state << " " << memleft[i].ReadytoMeasure << "    ";
		if (i % 2 == 1) cout << endl;
	}
	cout << endl << "rightmems: " << endl;
	for (int i = 0; i < memsize; i++)
	{
		cout << " fid: " << memright[i].fid << " state: " << memright[i].state << " " << memright[i].ReadytoMeasure << "    ";
		if (i % 2 == 1) cout << endl;
	}
	return 0;
}

//Other stuff

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
	*pair1->state = Cheapstatefid(np(0));
	delete pair2;

	return 0;
}

int GreedyBU_DEJPurif(SimRoot * Sim, QMem * mem, int memsize, double targetfid)
{
	//check if there's need for purification
	int freemems = 0;
	int mems_to_purif = 0;
	for (int i = 0; i < memsize; i++)
	{
		if (mem[i].pair == NULL) freemems = freemems + 1;
		if (mem[i].fid < targetfid && mem[i].ReadytoMeasure && mem[i].state == 3) mems_to_purif = mems_to_purif + 1;
	}
	if (mems_to_purif > 1)
	{

		QMem ** toPurif = new QMem*[memsize];
		QMem ** holdtoPurif = new QMem*[memsize];
		int j = 0;
		for (int i = 0; i < memsize; i++) toPurif[i] = NULL;
		for (int i = 0; i < memsize; i++) holdtoPurif[i] = NULL;
		//collect pairs to be purified between the same stations
		int k = 0;
		bool purifok=false;
		Node * n1 = NULL;
		Node * n2 = NULL;
		for (int k = 0; k < memsize && !purifok; k++) // check if the pairs to purify are between the same nodes or not;
		{
			bool skip = false;
			j = 0;
			for (int i = 0; i < memsize; i++) toPurif[i] = NULL;
			if (mem[k].pair != NULL)
			{
				if (mem[k].pair->mem[0] != NULL)
				{
					n1 = mem[k].pair->mem[0]->innode;
				}
				else
				{
					skip = true;
				}
				if (mem[k].pair->mem[1] != NULL)
				{
					n2 = mem[k].pair->mem[1]->innode;
				}
				else
				{
					skip = true;
				}
			}
			else
			{
				skip = true;
			}
			for (int i = 0; i < memsize && !skip; i++)
			{
				if (mem[i].fid < targetfid && mem[i].ReadytoMeasure && mem[i].state == 3 && mem[i].pair->mem[0]->innode == n1 && mem[i].pair->mem[1]->innode == n2)
				{
					toPurif[j] = &mem[i];
					j++;
				}
			}
			if (j > 1 && !skip)
			{
				purifok = true;
			}
		}
		//save findings
		for (int i = 0; i < memsize; i++)
		{
			holdtoPurif[i] = toPurif[i];
		}
		//Purify while there are pairs to be purified
		//find the smallest indexes
		QMem *m1, *m2, *aux;
		m1 = NULL;
		m2 = NULL;
		//find m1
		for (int i = 0; i < memsize; i++)
		{
			if (toPurif[i] != NULL)
			{
				if (m1 == NULL)
				{
					m1 = toPurif[i];
					toPurif[i] = NULL;
				}
				else if (m1->fid > toPurif[i]->fid) //found lower fid->swap
				{
					aux = m1;
					m1 = toPurif[i];
					toPurif[i] = aux;
				}
			}
		}
		//find m2
		for (int i = 0; i < memsize; i++)
		{
			if (toPurif[i] != NULL)
			{
				if (m2 == NULL)
				{
					m2 = toPurif[i];
					toPurif[i] = NULL;
				}
				else if (m2->fid > toPurif[i]->fid) //found lower fid->swap
				{
					aux = m2;
					m2 = toPurif[i];
					toPurif[i] = aux;
				}
			}
		}
		//purify until there's nothing left to purify
		while (m1 != NULL && m2 != NULL)
		{
			int result = 0;
			//purify
			result = DEJPurif(m2->pair, m1->pair);

			m1 = NULL;
			//find m1 again
			for (int i = 0; i < memsize; i++)
			{
				if (toPurif[i] != NULL)
				{
					if (m1 == NULL)
					{
						m1 = toPurif[i];
						toPurif[i] = NULL;
					}
					else if (m1->fid > toPurif[i]->fid) //found lower fid->swap
					{
						aux = m1;
						m1 = toPurif[i];
						toPurif[i] = aux;
					}
				}
			}
			if (result == 1) // Purification succesfull -> put m2 back to toPurif(only if fid < target)
			{
				double fidelity = Vec4Calcstdfid(*m2->pair->state);
				m2->pair->mem[0]->fid = fidelity; //nochecks!
				m2->pair->mem[1]->fid = fidelity;
				// 	cout << "m2fid:  "<<m2->fid << endl;
				if (m2->fid >= targetfid) m2 = NULL; // wont swap m2 back if its fid is high enough
			}
			else
			{
				m2 = NULL;
			}
			//find m2 again
			for (int i = 0; i < memsize; i++)
			{
				if (toPurif[i] != NULL)
				{
					if (m2 == NULL)
					{
						m2 = toPurif[i];
						toPurif[i] = NULL;
					}
					else if (m2->fid > toPurif[i]->fid) //found lower fid->swap
					{
						aux = m2;
						m2 = toPurif[i];
						toPurif[i] = aux;
					}
				}
			}
		}

		// cout << endl << "yesyesyesyes" << endl;
		// set mems available after purification ends;
		for (int i = 0; i < memsize; i++)
		{
			if (holdtoPurif[i] != NULL)
			{
				if (holdtoPurif[i]->pair != NULL)
				{
					holdtoPurif[i]->pair->mem[0]->inpurif = false;
					holdtoPurif[i]->pair->mem[1]->inpurif = false;
				}
			}
		}

		delete[] holdtoPurif;
		delete[] toPurif;
		return 1;
	}
	else
	{
		//cout << endl << "nononono" << endl;
		return 0;
	}
}

int GreedyBU_DEJPurif2(SimRoot * Sim, QMem ** toPurif, int memsize, double targetfid)
{
		// set mem states back to normal
	for (int i = 0; i < memsize; i++)
	{
		if (toPurif[i] != NULL)
		{
			toPurif[i]->pair->mem[0]->inpurif = false;
			toPurif[i]->pair->mem[1]->inpurif = false;
			//cout << "doned" << endl;
		}
	}

		int j = 0;
		//collect pairs to be purified between the same stations
		int k = 0;
		bool purifok = false;
		Node * n1 = NULL;
		Node * n2 = NULL;
		//save findings
		//Purify while there are pairs to be purified
		//find the smallest indexes
		QMem *m1, *m2, *aux;
		m1 = NULL;
		m2 = NULL;
		//find m1
		for (int i = 0; i < memsize; i++)
		{
			if (toPurif[i] != NULL)
			{
				if (m1 == NULL)
				{
					m1 = toPurif[i];
					toPurif[i] = NULL;
				}
				else if (m1->fid > toPurif[i]->fid) //found lower fid->swap
				{
					aux = m1;
					m1 = toPurif[i];
					toPurif[i] = aux;
				}
			}
		}
		//find m2
		for (int i = 0; i < memsize; i++)
		{
			if (toPurif[i] != NULL)
			{
				if (m2 == NULL)
				{
					m2 = toPurif[i];
					toPurif[i] = NULL;
				}
				else if (m2->fid > toPurif[i]->fid) //found lower fid->swap
				{
					aux = m2;
					m2 = toPurif[i];
					toPurif[i] = aux;
				}
			}
		}
		//purify until there's nothing left to purify
		while (m1 != NULL && m2 != NULL)
		{
			int result = 0;
			//purify
			result = DEJPurif(m2->pair, m1->pair);

			m1 = NULL;
			//find m1 again
			for (int i = 0; i < memsize; i++)
			{
				if (toPurif[i] != NULL)
				{
					if (m1 == NULL)
					{
						m1 = toPurif[i];
						toPurif[i] = NULL;
					}
					else if (m1->fid > toPurif[i]->fid) //found lower fid->swap
					{
						aux = m1;
						m1 = toPurif[i];
						toPurif[i] = aux;
					}
				}
			}
			if (result == 1) // Purification succesfull -> put m2 back to toPurif(only if fid < target)
			{
				double fidelity = Vec4Calcstdfid(*m2->pair->state);
				m2->pair->mem[0]->fid = fidelity; //nochecks!
				m2->pair->mem[1]->fid = fidelity;
				// 	cout << "m2fid:  "<<m2->fid << endl;
				if (m2->fid >= targetfid) m2 = NULL; // wont swap m2 back if its fid is high enough
			}
			else
			{
				m2 = NULL;
			}
			//find m2 again
			for (int i = 0; i < memsize; i++)
			{
				if (toPurif[i] != NULL)
				{
					if (m2 == NULL)
					{
						m2 = toPurif[i];
						toPurif[i] = NULL;
					}
					else if (m2->fid > toPurif[i]->fid) //found lower fid->swap
					{
						aux = m2;
						m2 = toPurif[i];
						toPurif[i] = aux;
					}
				}
			}
		}


		delete[] toPurif;
		return 1;
}

int GreedyTD_DEJPurif(SimRoot * Sim, QMem ** toPurif, int memsize, double targetfid)
{
	// set mem states back to normal
	for (int i = 0; i < memsize; i++)
	{
		if (toPurif[i] != NULL)
		{
			toPurif[i]->pair->mem[0]->inpurif = false;
			toPurif[i]->pair->mem[1]->inpurif = false;
			//cout << "doned" << endl;
		}
	}

	int j = 0;
	//collect pairs to be purified between the same stations
	int k = 0;
	bool purifok = false;
	Node * n1 = NULL;
	Node * n2 = NULL;
	//save findings
	//Purify while there are pairs to be purified
	//find the smallest indexes
	QMem *m1, *m2, *aux;
	m1 = NULL;
	m2 = NULL;
	//find m1
	for (int i = 0; i < memsize; i++)
	{
		if (toPurif[i] != NULL)
		{
			if (m1 == NULL)
			{
				m1 = toPurif[i];
				toPurif[i] = NULL;
			}
			else if (m1->fid < toPurif[i]->fid) //found higher fid->swap
			{
				aux = m1;
				m1 = toPurif[i];
				toPurif[i] = aux;
			}
		}
	}
	//find m2
	for (int i = 0; i < memsize; i++)
	{
		if (toPurif[i] != NULL)
		{
			if (m2 == NULL)
			{
				m2 = toPurif[i];
				toPurif[i] = NULL;
			}
			else if (m2->fid < toPurif[i]->fid) //found higher fid->swap
			{
				aux = m2;
				m2 = toPurif[i];
				toPurif[i] = aux;
			}
		}
	}
	//purify until there's nothing left to purify
	while (m1 != NULL && m2 != NULL)
	{
		int result = 0;
		//purify
		result = DEJPurif(m2->pair, m1->pair);

		m1 = NULL;
		//find m1 again
		for (int i = 0; i < memsize; i++)
		{
			if (toPurif[i] != NULL)
			{
				if (m1 == NULL)
				{
					m1 = toPurif[i];
					toPurif[i] = NULL;
				}
				else if (m1->fid < toPurif[i]->fid) //found higher fid->swap
				{
					aux = m1;
					m1 = toPurif[i];
					toPurif[i] = aux;
				}
			}
		}
		if (result == 1) // Purification succesfull -> put m2 back to toPurif(only if fid < target)
		{
			double fidelity = Vec4Calcstdfid(*m2->pair->state);
			m2->pair->mem[0]->fid = fidelity; //nochecks!
			m2->pair->mem[1]->fid = fidelity;
			// 	cout << "m2fid:  "<<m2->fid << endl;
			if (m2->fid >= targetfid) m2 = NULL; // wont swap m2 back if its fid is high enough
		}
		else
		{
			m2 = NULL;
		}
		//find m2 again
		for (int i = 0; i < memsize; i++)
		{
			if (toPurif[i] != NULL)
			{
				if (m2 == NULL)
				{
					m2 = toPurif[i];
					toPurif[i] = NULL;
				}
				else if (m2->fid < toPurif[i]->fid) //found higher fid->swap
				{
					aux = m2;
					m2 = toPurif[i];
					toPurif[i] = aux;
				}
			}
		}
	}


	delete[] toPurif;
	return 1;
}

double Vec4Calcstdfid(Vector4cd state)
{
	Vector4cd target;
	target << 1 / sqrt(2), 0, 0, 1 / sqrt(2);

	return (state.adjoint()*(target*target.adjoint())*state).norm();
}

bool Checkforpurif(QMem * mem, int memsize, double targetfid)
{

	int j = 0;
	//collect pairs to be purified between the same stations
	int k = 0;
	bool purifok = false;
	Node * n1 = NULL;
	Node * n2 = NULL;
	for (int k = 0; k < memsize && !purifok; k++) // check if the pairs to purify are between the same nodes or not;
	{
		bool skip = false;
		j = 0;
		if (mem[k].pair != NULL)
		{
			if (mem[k].pair->mem[0] != NULL)
			{
				n1 = mem[k].pair->mem[0]->innode;
			}
			else
			{
				skip = true;
			}
			if (mem[k].pair->mem[1] != NULL)
			{
				n2 = mem[k].pair->mem[1]->innode;
			}
			else
			{
				skip = true;
			}
		}
		else
		{
			skip = true;
		}

		for (int i = 0; i < memsize && !skip; i++)
		{
			if (mem[i].fid < targetfid && mem[i].ReadytoMeasure && mem[i].state == 3 && mem[i].pair->mem[0]->innode == n1 && mem[i].pair->mem[1]->innode == n2 && !mem[i].inpurif) // purifiable and have the same endpoints
			{
				j++;
			}
		}
		if (j > 1 && !skip)
		{
			purifok = true;
		}
	}
	return purifok;
}

double GetDistofNodes(Node * n1, Node * n2)
{
	if (n1 == NULL || n2 == NULL)
	{
		return -1;
	}
	bool found = false;
	double dist=0;
	Node *curnode = n1;
	while(curnode!=NULL && !found) // try right first
	{
		dist = dist + curnode->physdistright;
		if (curnode->physNoderight == n2) found = true;
		curnode = curnode->physNoderight;
	}
	if (found)
	{
		return dist;
	}
	dist = 0;
	curnode = n1;
	while (curnode != NULL && !found) // try left
	{
		dist = dist + curnode->physdistleft;
		if (curnode->physNodeleft == n2) found = true;
		curnode = curnode->physNodeleft;
	}
	if (found)
	{
		return dist;
	}
	return -1;
}

QMem** ReserveMemsforPurif(QMem * mem, int memsize)
{
	QPair* retpair = NULL;
	QMem ** toPurif = new QMem*[memsize];
	int j = 0;
	for (int i = 0; i < memsize; i++) toPurif[i] = NULL;
	//collect pairs to be purified between the same stations
	int k = 0;
	bool purifok = false;
	Node * n1 = NULL;
	Node * n2 = NULL;
	for (int k = 0; k < memsize && !purifok; k++) // check if the pairs to purify are between the same nodes or not;
	{
		bool skip = false;
		j = 0;
		for (int i = 0; i < memsize; i++) toPurif[i] = NULL;
		if (mem[k].pair != NULL)
		{
			if (mem[k].pair->mem[0] != NULL)
			{
				n1 = mem[k].pair->mem[0]->innode;
			}
			else
			{
				skip = true;
			}
			if (mem[k].pair->mem[1] != NULL)
			{
				n2 = mem[k].pair->mem[1]->innode;
			}
			else
			{
				skip = true;
			}
		}
		else
		{
			skip = true;
		}
		for (int i = 0; i < memsize && !skip; i++)
		{
			if (mem[i].fid < mem[i].innode->targetfid && mem[i].ReadytoMeasure && mem[i].state == 3 && mem[i].pair->mem[0]->innode == n1 && mem[i].pair->mem[1]->innode == n2 && !mem[i].inpurif)
			{
				toPurif[j] = &mem[i];
				j++;
			}
		}
		if (j > 1 && !skip)
		{
			purifok = true;
		}
	}
	for (int i = 0; i < memsize; i++)
	{
		if (toPurif[i] != NULL)
		{
			toPurif[i]->pair->mem[0]->inpurif = true;
			toPurif[i]->pair->mem[1]->inpurif = true;
		}
	}

	return toPurif;
}
