#include "qrep.h"
#include "elements.h"
#include <random>
#include "Simulation.h"
class szam
{
public:
	int sz;

	void print()
	{
		cout << endl << sz << endl;
	}
	int recprint(SimRoot * Sim,double delay)
	{
		cout << endl << sz << endl;
		SimItem* item = new SimItem;
		item->FuncToCall = bind(&szam::recprint,this,Sim,delay);
		item->extime = Sim->curtime + delay;
		Sim->Schedule(item);
		//cout <<"curtime: "<< Sim->curtime << endl;
		return 1;
	}
	int printi()
	{
		cout << endl << sz << endl;
		return 1;
	}
};
QPair* pairgen()
{
	QPair *pair = new QPair;
	return pair;
}

void plusasd(int * i1, int i2)
{
	*i1 = *i1 + i2;
}
void printint(int i)
{
	cout << endl << i << endl;
}

int EPRgen(EPR* epr, Channel * ch1, Channel*ch2, SimRoot * Sim)
{
	QPair* pair = new QPair;
	*pair = &(epr->generate());
	return 1;
}

void Makepow2tree(Node nodes[], int min, int max, Node * parent,int pindex,double dist)
{
	if ((min % 2) != 0 || (max % 2) != 0) // got to the bottom of the tree
	{
		return;
	}

	int index = (min + max) / 2; //get the middle
	nodes[index].nextNode = parent;
	if (pindex > index) 
	{ 
		parent->prevNodeleft = &nodes[index]; 
		parent->prevdistleft = dist / 2;
		parent->prevNodeleft->nextdist = parent->prevdistleft;
	}
	if (pindex < index) 
	{ 
		parent->prevNoderight = &nodes[index]; 
		parent->prevdistright = dist / 2;
		parent->prevNoderight->nextdist = parent->prevdistright;
	}
	Makepow2tree(nodes, min, (min + max) / 2, &nodes[index], index, dist / 2); //make the leaves
	Makepow2tree(nodes, (min + max) / 2, max, &nodes[index], index, dist / 2);

}

void InitLintree(Node nodes[],Node eprnodes[],Channel channels[],EPR * epr, int size, double dist, double chalength)
{
	for (int i = 0; i < size*2; i++)//set up channels between nodes
	{
		channels[i].from = &eprnodes[i / 2];
		channels[i].to = &nodes[(i + 1) / 2];
		channels[i].length = dist/2;
		channels[i].alength = chalength;
	}

	for (int i = 0; i < size; i++) // set up EPR nodes
	{
		eprnodes[i].prevNodeleft = &nodes[i];
		eprnodes[i].prevNoderight = &nodes[i + 1];
		eprnodes[i].leftch = &channels[i * 2];
		eprnodes[i].prevdistleft = channels[i * 2].length;
		eprnodes[i].rightch = &channels[i * 2 + 1];
		eprnodes[i].prevdistright = channels[i * 2 + 1].length;
		eprnodes[i].epr = epr;
		eprnodes[i].type = 1;
	}

	for (int i = 2; i < size; i++) // sets nodes 2 ... n-1
	{
		nodes[i].nextNode = &nodes[i + 1];
		nodes[i].nextdist = dist;
		nodes[i].prevNoderight = &eprnodes[i];
		nodes[i].prevdistright = nodes[i].prevNoderight->leftch->length;
		nodes[i].prevNodeleft = &nodes[i - 1];
		nodes[i].prevdistleft = nodes[i].prevNodeleft->nextdist;
	}
	nodes[0].type = 2;
	nodes[size + 1].type = 2;
	//first node after endpoint is special, because it does the first measurement in the chain
	nodes[1].nextNode = &nodes[2];
	nodes[1].nextdist = dist;
	nodes[1].prevNoderight = &eprnodes[1];
	nodes[1].prevdistright = nodes[1].prevNoderight->leftch->length;
	nodes[1].prevNodeleft = &eprnodes[0];
	nodes[1].prevdistleft = nodes[1].prevNodeleft->rightch->length;

}

void printNodeinf(Node* node)
{
	cout << endl << "parent:" << node->nextNode << endl << "self: " << node << endl << "left: " << node->prevNodeleft << "   dist: " << node->prevdistleft <<
		endl << "rigt: " << node->prevNoderight << "   dist: " << node->prevdistright << endl;
}
int printszam(int i)
{
	cout << "szam: " << i << endl;
	return i;
}

int main()
{
	/*
	double fid;
	Vector4cd T, v1;
	T << 1 / sqrt(2), 0, 0, 1/sqrt(2);
	cin >> fid;
	v1 = Cheapstatefid(fid);
	cout << "v1: " << endl << v1 << endl << "fid:" << Vec4Calcfid(v1, T) << endl;
	//*/

	/*
	Matrix2cd g1, g2, gi;
	Matrix4cd gkomb1,gkomb2;
	Vector4cd v1;
	v1 << 1, 2,3, 4;

	g2 << 1, 0, 0, -1;
	g1 << 0, 1, 1, 0;
	gi << 1, 0, 0, 1;
	cout << g1 << endl << gi << endl;
	gkomb1 = Kronecker(g1,g1);
	gkomb2 = Kronecker(g2, gi);
	gkomb1 << 1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1;
	cout << endl << gkomb1*gkomb2 << endl;
	cout << endl << v1 << endl;
	cout << endl << gkomb1*v1 << endl;
	cout << endl << gkomb2*v1 << endl;
	cout << endl << gkomb2*gkomb1*v1 << endl;
	//*/
	/*
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned int> dist(0, 20000000000);
	std::srand( dist(gen));

	double fid;
	cin >> fid;
	Matrix4cd R, RU, RU2;
	R.setRandom();
	//cout << endl << R << endl;
	Matrix4cd gi;
	gi = Matrix4cd::Identity();
	//cout << endl << R << endl;
	HouseholderQR<Matrix4cd> qr(R);
	RU = qr.householderQ();
	//cout <<"unitary:"<< endl << RU << endl;
	RU = (1-fid)*RU + fid*gi;
	//cout << "unitary+i:" << endl << RU << endl;
	HouseholderQR<Matrix4cd> qr2(RU);
	RU2 = qr2.householderQ();
	//cout << "unitary2:" << endl << RU2 << endl;
	double f = 1;
	Vector4cd v1, v2, vr;
	//vr.setRandom();
	//vr = vr / sqrt(vr.cwiseAbs2().sum());
	//cout << endl << "vr: " << vr << endl << "abs: " << vr.cwiseAbs2().sum() << endl;
	v1 << 0.707, 0, 0, 0.707;
	v2 << 0.707, 0, 0, 0.707;
	Vector4cd M;
	M << 0.707, 0, 0, 0.707;
	v2 = sqrt(fid)*v1;
	vr.setRandom();
	vr(0) = 0;
	vr(3) = 0;
	cout << endl << vr.cwiseAbs2().sum() << endl;
	vr = (vr / sqrt(vr.cwiseAbs2().sum()))*sqrt(1-fid);
	cout << endl << "vr: " << vr << endl << "vrabs: " << vr.cwiseAbs2().sum() << endl;

	v2 = v2 + vr;
	Matrix4cd fidgate;
	double fidossz=0;
	double fidmin=1;
	double fidmax=0;
	//v2 = RU2*M;
	//cout << endl << RU2*M << endl;
	cout <<"v1: "<<endl<< v1 << endl << "abs: " << v1.cwiseAbs2().sum() << endl <<"v2: "<< endl<< v2 << endl << "abs:" << v2.cwiseAbs2().sum() << endl;
	cout << endl << "v1fid: " << (v1.adjoint()*(M*M.adjoint())*v1).norm() << endl;
	cout << endl << "v2fid: " << (v2.adjoint()*(M*M.adjoint())*v2).norm() << endl;
	
	Vector4cd vtest;
	vtest = Cheapstatefid(fid);
	cout << endl << endl;
	cout << "v1: " << endl << v1 << endl << "abs: " << v1.cwiseAbs2().sum() << endl << "v2t: " << endl << vtest << endl << "abs:" << vtest.cwiseAbs2().sum() << endl;
	cout << endl << "v1fid: " << (v1.adjoint()*(M*M.adjoint())*v1).norm() << endl;
	cout << endl << "v2tfid: " << (vtest.adjoint()*(M*M.adjoint())*vtest).norm() << endl;
	//*/
	/*for (int i = 0; i < 1000; i++)
	{
		R.setRandom();
		HouseholderQR<Matrix4cd> qr(R);
		RU = qr.householderQ();
		cout << "unitary:" << endl << RU << endl;
		RU = (1 - fid)*RU + fid*gi;
		cout << "unitary+i:" << endl << RU << endl;
		HouseholderQR<Matrix4cd> qr2(RU);
		RU2 = qr2.householderQ();
		cout << "unitary2:" << endl << RU2 << endl;
		v2 = RU2*M;
		//fidgate = ((1 - fid)*RU + (fid)*gi);
		//cout << endl << "fidgate: " << endl << fidgate << endl;
		//v2 = ((1 - fid)*vr + (fid)*v1) / sqrt(((1 - fid)*vr + (fid)*v1).cwiseAbs2().sum());
		cout << v1 << endl << "abs: " << v1.cwiseAbs2().sum() << endl << v2 << endl << "abs:" << v2.cwiseAbs2().sum() << endl;
		cout << endl << "v1fid: " << (v1.adjoint()*(M*M.adjoint())*v1).norm() << endl;
		cout << endl << "v2fid: " << (v2.adjoint()*(M*M.adjoint())*v2).norm() << endl;
		fidossz = fidossz + (v2.adjoint()*(M*M.adjoint())*v2).norm();
		if ((v2.adjoint()*(M*M.adjoint())*v2).norm() > fidmax) fidmax = (v2.adjoint()*(M*M.adjoint())*v2).norm();
		if ((v2.adjoint()*(M*M.adjoint())*v2).norm() < fidmin) fidmin = (v2.adjoint()*(M*M.adjoint())*v2).norm();
		//vr.setRandom();
	}
	//*/
	//cout << endl << "atl: " << fidossz / 1000 << endl << "min: " << fidmin << endl << "max: " << fidmax << endl;
	/*
	Pair2Measure bellm;
	bellm.SetBellMeasure();
	//*/
	Node n1;
	//cout << endl << *n1.measure.auxtransmat << endl << *n1.measure.transmat << endl;
	Node nodes[9];
	Node eprnodes[8];
	Channel channels[16];
	EPR std_epr;
	std_epr.rate = 20;
	for (int i = 0; i < 16; i++)
	{
		channels[i].from = &eprnodes[i / 2];
		channels[i].to = &nodes[(i + 1) / 2];
		channels[i].length = 5;
		channels[i].alength = 20;
	}
	for (int i = 0; i < 8; i++)
	{
		eprnodes[i].prevNodeleft = &nodes[i];
		eprnodes[i].prevNoderight = &nodes[i + 1];
		eprnodes[i].leftch = &channels[i * 2];
		eprnodes[i].prevdistleft = channels[i * 2].length;
		eprnodes[i].rightch = &channels[i * 2 + 1];
		eprnodes[i].prevdistright = channels[i * 2 + 1].length;
		eprnodes[i].epr = &std_epr;
		eprnodes[i].type = 1;
	}
	Makepow2tree(nodes, 0, 8, &nodes[8], 8, 80);
	nodes[0].type = 2;
	nodes[8].type = 2;
	for (int i = 0; i < 9; i++)
	{
		if ((i % 2) != 0) // odd index nodes are the bottom of the tree, they are to be linked, with the EPR generating nodes
		{
			//setting left connection
			nodes[i].prevNodeleft = &eprnodes[i - 1];
			nodes[i].prevdistleft = nodes[i].prevNodeleft->rightch->length;
			//setting right connection
			nodes[i].prevNoderight = &eprnodes[i];
			nodes[i].prevdistright = nodes[i].prevNoderight->leftch->length;
		}

	}
	SimRoot Sim;
	/*
	SimItem * items[4];
	for (int i = 0; i < 4; i++)
	{
		items[i] = new SimItem;
		items[i]->FuncToCall = bind(printszam, i);
		items[i]->extime = i;
	}
	items[2]->extime = 0;
	Sim.Schedule(items[3]);
	Sim.Schedule(items[1]);
	Sim.Schedule(items[2]);
	Sim.Schedule(items[0]);
	Sim.printlisttimes();
	//for (int i = 0; i < 4; i++) Sim.ExecuteNext();
	//*/

	/*
	//QPair *p1 = pairgen();
	QPair *p1 = std_epr.generatep();
	QPair *p2 = std_epr.generatep();
	QPair *p3 = std_epr.generatep();
	QPair *p4 = std_epr.generatep();
	//set test
	nodes[0].memright[0].pair = p1;
	nodes[0].memright[0].pairindex = 0;
	p1->mem[0] = &nodes[0].memright[0];

	nodes[1].memleft[0].pair = p1;
	nodes[1].memleft[0].pairindex = 1;
	p1->mem[1] = &nodes[1].memleft[0];
	nodes[1].memright[0].pair = p2;
	nodes[1].memright[0].pairindex = 0;
	p2->mem[0] = &nodes[1].memright[0];

	nodes[2].memleft[0].pair = p2;
	nodes[2].memleft[0].pairindex = 1;
	p2->mem[1] = &nodes[2].memleft[0];
	nodes[2].memright[0].pair = p3;
	nodes[2].memright[0].pairindex = 0;
	p3->mem[0] = &nodes[2].memright[0];

	nodes[3].memleft[0].pair = p3;
	nodes[3].memleft[0].pairindex = 1;
	p3->mem[1] = &nodes[3].memleft[0];
	nodes[3].memright[0].pair = p4;
	nodes[3].memright[0].pairindex = 0;
	p4->mem[0] = &nodes[3].memright[0];

	nodes[4].memleft[0].pair = p4;
	nodes[4].memleft[0].pairindex = 1;
	p4->mem[1] = &nodes[4].memleft[0];

	nodes[1].memleft[0].ReadytoMeasure = true;
	nodes[1].memright[0].ReadytoMeasure = true;

	nodes[3].memleft[0].ReadytoMeasure = true;
	nodes[3].memright[0].ReadytoMeasure = true;

	nodes[3].memleft[0].state = 2;
	nodes[3].memright[0].state = 2;

	nodes[2].memright[0].state = 2;
	nodes[4].memleft[0].state = 2;

	nodes[1].Updateformeasure(&Sim);
	nodes[3].Updateformeasure(&Sim);
	//nodes[1].Bellmeasure(&Sim, &nodes[1].memleft[0], &nodes[1].memright[0]);

	//delete nodes[1].memleft[0].pair;
	//delete p1;
	//*/

	QPair * p1,* p2;
	p1 = new QPair;
	p2 = new QPair;
	Vector4cd target;
	target << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	*p1->state = Cheapstatefid(0.6);
	*p2->state = Cheapstatefid(0.6);
	cout << endl << "p1: " << *p1->state << endl << "p1fid: " << Vec4Calcfid(*p1->state, target) << endl;
	cout << endl << "p2: " << *p2->state << endl << "p2fid: " << Vec4Calcfid(*p2->state, target) << endl;
	int tova = 0;
	while (tova != 1)
	{
		QPair * auxpair = new QPair;
		*auxpair->state = Cheapstatefid(0.7);
		int result = DEJPurif(p1, auxpair);
		if (result == 0)
		{
			p1 = new QPair;
			*p1->state = Cheapstatefid(0.7);
		}
		cout << endl << "result: " << result << endl;
		if (result == 1)
		{
			cout << endl << "p1: " << *p1->state << endl << "p1fid: " << Vec4Calcfid(*p1->state, target) << endl;
		}
		cin >> tova;

	}
	cout << endl << "p1: " << *p1->state << endl << "p1fid: " << Vec4Calcfid(*p1->state, target) << endl;
	//int result = DEJPurif(p1, p2);
	//cout << endl << "result:  " << result << endl;
	//if (result!=0) cout << endl << "p1: " << *p1->state << endl << "p1fid: " << Vec4Calcfid(*p1->state, target) << endl;
	//DEJ2Purif(p1, p2);


	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned int> dist(0, 20000000000);
	std::srand(dist(gen));

	Vector4cd v1, v2;
	v1.setRandom();
	v1 = v1*(1 / sqrt(v1.cwiseAbs2().sum()));
	cout << endl <<"asd  "<< v1.cwiseAbs2().sum() << endl;
	v2 << 2, 3, 4, 5;
	Vector4cd vt1, vt2,vt3,vt4;
	vt1 << 1 / sqrt(2), 0, 0, 1 / sqrt(2);
	vt2 << 1 / sqrt(2), 0, 0, -1 / sqrt(2);
	vt3 << 0, 1 / sqrt(2), 1 / sqrt(2), 0;
	vt4 << 0, 1 / sqrt(2), -1 / sqrt(2), 0;
	cout << endl << "v1: " << endl << v1 << endl;
	cout << endl << Vec4Calcfid(v1, vt1) << endl << Vec4Calcfid(v1, vt2) << endl << Vec4Calcfid(v1, vt3) << endl << Vec4Calcfid(v1, vt4) << endl;
	Matrix4cd toBell;
	toBell << vt1.transpose(), vt2.transpose(), vt3.transpose(), vt4.transpose();
	cout << endl << toBell << endl;
	cout << endl << (toBell*v1).cwiseAbs2() << endl;




	/*
	Sim.printlisttimes();
	int a=0;

	while(a!=1)
	{
		eprnodes[0].GenEPR(&Sim);
		eprnodes[1].GenEPR(&Sim);
		while (Sim.nextItem != NULL)
		{
			Sim.printlisttimes();
			Sim.ExecuteNext();
			cout << " " << nodes[2].memleft[0].ReadytoMeasure << " " << nodes[2].memright[0].ReadytoMeasure << " " << &nodes[2].memleft[0] << " " << &nodes[2].memright[0] << endl;
		}
		cin >> a;
	}
	cout << endl;
	//*/
	//nodes[0].memright[0].pair->print();
	/*
	int a = 0;
	cin >> a;

	while (a != 1)
	{
		Sim.ExecuteNext();
		cin >> a;
	}
	//*/
	/*
	Node * ntcl, *ntcr;
	double dl, dr;
	NodestoCorrect(&nodes[4], &ntcl, &ntcr, &dl, &dr);
	for (int i = 0; i < 9; i++) cout << i << ": " << &nodes[i] << endl;
	cout << "ntcl: " << ntcl << endl;
	cout << "ntcr: " << ntcr << endl;
	cout << "dl: " << dl << endl;
	cout << "dr: " << dr << endl;
	cout << "nextdist: " << nodes[4].nextdist << endl;
	//*/

	/*
	int a = 0;
	SimRoot Sim;
	szam sz1, sz2,sz3;
	sz1.sz = 12;
	sz2.sz = 34;
	sz3.sz = 56;
	SimItem * item1 = new SimItem;
	item1->extime = 10;
	item1->FuncToCall = bind(&szam::printi, sz1);
	SimItem * item2 = new SimItem;
	item2->extime = 6;
	item2->FuncToCall = bind(&szam::printi, sz2);
	sz3.recprint(&Sim, 3);
	Sim.Schedule(item1);
	Sim.Schedule(item2);

	cin >> a;

	while (a != 1)
	{
		Sim.ExecuteNext();
		cin >> a;
	}
	*/
	


	/*
	int i = 1;
	int j = 2;
	function<void()> func2 = bind(plusasd, &j, i);
	function<void()> func = bind(printint, j);
	func();
	func2();
	func();



	EPR ep1;
	QPair p1, p2;
	p1 = &ep1.generate();
	p2 = &ep1.generate();
	*/
	/*
	p1.print();
	p2.print();
	Pair2Measure Bellm;
	Bellm.SetBellMeasure();
	Bellm.bmeasure(&p1, &p2);
	p1.print();
	p2.print();

	szam sz1;
	sz1.sz = 2;
	function<void()> fszam = bind(&szam::print, sz1);
	sz1.print();
	sz1.sz = sz1.sz + 1;
	sz1.print();
	fszam();
	*/
}

