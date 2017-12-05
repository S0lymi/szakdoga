#include "Testhelp.h"

void Makepow2tree(Node nodes[], int min, int max, Node * parent, int pindex, double dist)
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

//Makes simple linear tree 

void InitLintree(Node nodes[], Node eprnodes[], Channel channels[], EPR * epr, int size, double dist, double chalength)
{

	for (int i = 1; i < size; i++) // sets nodes 2 ... n-1
	{
		nodes[i].nextNode = &nodes[i + 1];
		nodes[i].nextdist = dist/size;
		nodes[i].prevNoderight = &eprnodes[i];
		nodes[i].prevdistright = nodes[i].prevNoderight->leftch->length;
		nodes[i].prevNodeleft = &nodes[i - 1];
		nodes[i].prevdistleft = nodes[i].prevNodeleft->nextdist;
	}
	nodes[0].type = 2;
	nodes[size].type = 2;

	//first node after endpoint is special, because it does the first measurement in the chain
	nodes[1].nextNode = &nodes[2];
	nodes[1].nextdist = dist;
	nodes[1].prevNoderight = &eprnodes[1];
	nodes[1].prevdistright = nodes[1].prevNoderight->leftch->length;
	nodes[1].prevNodeleft = &eprnodes[0];
	nodes[1].prevdistleft = nodes[1].prevNodeleft->rightch->length;

}

void Pow2Setup(Node nodes[], Node eprnodes[], Channel channels[], int eprnumber, EPR * std_epr, double dist, int memsize, int epratonce, double chalength, double targetfid, function<int(SimRoot*, QMem**, int, double)> PurifMethod)
{
	if (eprnumber == 1)
	{
		//set channels
		channels[0].from = &eprnodes[0];
		channels[0].to = &nodes[0];
		channels[1].from = &eprnodes[0];
		channels[1].to = &nodes[1];
		channels[0].length = dist / (eprnumber * 2);
		channels[0].alength = chalength;
		channels[1].length = dist / (eprnumber * 2);
		channels[1].alength = chalength;
		//set epr
		eprnodes[0].prevNodeleft = &nodes[0];
		eprnodes[0].prevNoderight = &nodes[1];
		eprnodes[0].leftch = &channels[0];
		eprnodes[0].prevdistleft = channels[0].length;
		eprnodes[0].rightch = &channels[1];
		eprnodes[0].prevdistright = channels[1].length;
		eprnodes[0].epr = std_epr;
		eprnodes[0].type = 1;
		eprnodes[0].epratonce = epratonce;
		//set nodes
		nodes[0].type = 2;
		nodes[1].type = 2;
		nodes[1].nextNode = &nodes[0];
		nodes[1].nextdist = eprnodes[0].leftch->length + eprnodes[0].rightch->length;
		nodes[0].ChangeMemsize(memsize);
		nodes[0].targetfid = targetfid;
		nodes[0].purification = PurifMethod;
		nodes[1].ChangeMemsize(memsize);
		nodes[1].targetfid = targetfid;
		nodes[1].purification = PurifMethod;
		nodes[1].prevNodeleft = &eprnodes[0];
		nodes[1].prevdistleft = nodes[1].prevNodeleft->rightch->length;
		nodes[0].prevNoderight = &nodes[1];
		nodes[0].prevdistright = nodes[0].prevNoderight->nextdist;

		//set up physical positions
		nodes[0].physNoderight = &eprnodes[0];
		nodes[0].physdistright = eprnodes[0].leftch->length;
		nodes[eprnumber].physNodeleft = &eprnodes[eprnumber - 1];
		nodes[eprnumber].physdistleft = eprnodes[eprnumber - 1].rightch->length;
		eprnodes[0].physNodeleft = &nodes[0];
		eprnodes[0].physNoderight = &nodes[1];
		eprnodes[0].physdistleft = eprnodes[0].leftch->length;
		eprnodes[0].physdistleft = eprnodes[0].rightch->length;

		return;


	}
	// set up the channels
	for (int i = 0; i < eprnumber*2; i++)
	{
		channels[i].from = &eprnodes[i / 2];
		channels[i].to = &nodes[(i + 1) / 2];
		channels[i].length = dist/(eprnumber*2);
		channels[i].alength = chalength;
	}
	//set up eprs
	for (int i = 0; i < eprnumber; i++)
	{
		eprnodes[i].prevNodeleft = &nodes[i];
		eprnodes[i].prevNoderight = &nodes[i + 1];
		eprnodes[i].leftch = &channels[i * 2];
		eprnodes[i].prevdistleft = channels[i * 2].length;
		eprnodes[i].rightch = &channels[i * 2 + 1];
		eprnodes[i].prevdistright = channels[i * 2 + 1].length;
		eprnodes[i].epr = std_epr;
		eprnodes[i].type = 1;
		eprnodes[i].epratonce = epratonce;
	}
	//set up nodes
	Makepow2tree(nodes, 0, eprnumber, &nodes[eprnumber], eprnumber, dist);
	nodes[0].type = 2;
	nodes[eprnumber].type = 2;
	nodes[eprnumber].nextNode = &nodes[0];

	// set additional parameters
	for (int i = 0; i < eprnumber + 1; i++)
	{
		nodes[i].ChangeMemsize(memsize);
		nodes[i].targetfid = targetfid;
		nodes[i].purification = PurifMethod;
	}
	//set up epr connections
	for (int i = 0; i < eprnumber+1; i++)
	{
		if ((i % 2) != 0) // odd index nodes are the bottom of the tree, they are to be linked, with the EPR generating nodes
		{
			//setting left connection
			nodes[i].prevNodeleft = &eprnodes[i - 1];
			nodes[i].prevdistleft = nodes[i].prevNodeleft->rightch->length;
			//setting right connection
			if (eprnumber % 2 != 1)
			{
				nodes[i].prevNoderight = &eprnodes[i];
				nodes[i].prevdistright = nodes[i].prevNoderight->leftch->length;
			}
		}

	}
	//set up physical positions
	nodes[0].physNoderight = &eprnodes[0];
	nodes[0].physdistright = eprnodes[0].leftch->length;
	nodes[eprnumber].physNodeleft = &eprnodes[eprnumber - 1];
	nodes[eprnumber].physdistleft = eprnodes[eprnumber - 1].rightch->length;
	for (int i = 1; i < eprnumber; i++)
	{
		nodes[i].physNodeleft = &eprnodes[i-1];
		nodes[i].physdistleft = eprnodes[i-1].rightch->length;
		nodes[i].physNoderight = &eprnodes[i];
		nodes[i].physdistright = eprnodes[i].leftch->length;
	}
	for (int i = 0; i < eprnumber; i++)
	{
		eprnodes[i].physNodeleft = &nodes[i];
		eprnodes[i].physdistleft = eprnodes[i].leftch->length;
		eprnodes[i].physNoderight = &nodes[i+1];
		eprnodes[i].physdistright = eprnodes[i].rightch->length;
	}

	//spec
	//nodes[0].targetfid = 0.98;
	//nodes[eprnumber].targetfid = 0.98;
}

void LinSetup(Node nodes[], Node eprnodes[], Channel channels[], int eprnumber, EPR * std_epr, double dist, int memsize, int epratonce, double chalength, double targetfid, function<int(SimRoot*, QMem**, int, double)> PurifMethod)
{
	// set up the channels
	for (int i = 0; i < eprnumber * 2; i++)
	{
		channels[i].from = &eprnodes[i / 2];
		channels[i].to = &nodes[(i + 1) / 2];
		channels[i].length = dist / (eprnumber * 2);
		channels[i].alength = chalength;
	}
	//set up eprs
	for (int i = 0; i < eprnumber; i++)
	{
		eprnodes[i].prevNodeleft = &nodes[i];
		eprnodes[i].prevNoderight = &nodes[i + 1];
		eprnodes[i].leftch = &channels[i * 2];
		eprnodes[i].prevdistleft = channels[i * 2].length;
		eprnodes[i].rightch = &channels[i * 2 + 1];
		eprnodes[i].prevdistright = channels[i * 2 + 1].length;
		eprnodes[i].epr = std_epr;
		eprnodes[i].type = 1;
		eprnodes[i].epratonce = epratonce;
	}
	//set up structure
	for (int i = 1; i < eprnumber; i++)
	{
		nodes[i].nextNode = &nodes[i + 1];
		nodes[i].nextdist = dist / eprnumber;
		nodes[i].prevNoderight = &eprnodes[i];
		nodes[i].prevdistright = dist / (eprnumber * 2);
		nodes[i].prevNodeleft = &nodes[i - 1];
		nodes[i].prevdistleft = dist / eprnumber;
	}
	//spec second node
	nodes[1].prevNodeleft = &eprnodes[0];
	nodes[1].prevdistleft = dist / (eprnumber * 2);
	//set nodes
	nodes[0].type = 2;
	nodes[eprnumber].type = 2;
	nodes[eprnumber].nextNode = &nodes[0];
	nodes[eprnumber].nextdist = dist;
	nodes[eprnumber].prevNodeleft = &nodes[eprnumber - 1];
	nodes[eprnumber].prevdistleft = dist / eprnumber;


	//InitLintree(nodes, eprnodes, channels, std_epr, eprnumber, dist, chalength);
	// set additional parameters
	for (int i = 0; i < eprnumber + 1; i++)
	{
		nodes[i].ChangeMemsize(memsize);
		nodes[i].targetfid = targetfid;
		nodes[i].purification = PurifMethod;
	}
	//set eprs
	eprnodes[0].epr = std_epr;
	eprnodes[0].type = 1;
	eprnodes[0].epratonce = epratonce;


	//set up physical positions
	nodes[0].physNoderight = &eprnodes[0];
	nodes[0].physdistright = eprnodes[0].leftch->length;
	nodes[eprnumber].physNodeleft = &eprnodes[eprnumber - 1];
	nodes[eprnumber].physdistleft = eprnodes[eprnumber - 1].rightch->length;
	for (int i = 1; i < eprnumber; i++)
	{
		nodes[i].physNodeleft = &eprnodes[i - 1];
		nodes[i].physdistleft = eprnodes[i - 1].rightch->length;
		nodes[i].physNoderight = &eprnodes[i];
		nodes[i].physdistright = eprnodes[i].leftch->length;
	}
	for (int i = 0; i < eprnumber; i++)
	{
		eprnodes[i].physNodeleft = &nodes[i];
		eprnodes[i].physdistleft = eprnodes[i].leftch->length;
		eprnodes[i].physNoderight = &nodes[i + 1];
		eprnodes[i].physdistright = eprnodes[i].rightch->length;
	}
}

Pow2Sim::Pow2Sim()
{
	eprnumber = 8;
	std_epr = new EPR;
	dist = 80;
	memsize = 10;
	epratonce = 4;
	chalength = 20;
	targetfid = 0.98;
	PurifMethod = GreedyBU_DEJPurif2;
}

Pow2Sim::~Pow2Sim()
{
	delete std_epr;
}

int Pow2Sim::SimpleSim(int targetpairs, string filename)
{
	Node * nodes = new Node[eprnumber + 1];
	Node * eprnodes = new Node[eprnumber];
	Channel * channels = new Channel[2 * eprnumber];
	SimRoot * Sim = new SimRoot;
	Sim->diagnostics = 0;
	
	//Setup tree
	Pow2Setup(nodes, eprnodes, channels, eprnumber, std_epr, dist, memsize, epratonce, chalength, targetfid, PurifMethod);
	//LinSetup(nodes, eprnodes, channels, eprnumber, std_epr, dist, memsize, epratonce, chalength, targetfid, PurifMethod);
	//Sim->diagnostics = 1;
	int donepairs = 0;
	ofstream stats;
	stats.open(filename, ios::out | ios::trunc);
	stats.close();
	 // start simulation
	for (int i = 0; i < eprnumber; i++) eprnodes[i].GenEPR(Sim); // firts stimuli
	while (donepairs < targetpairs)
	{
		//Sim->printlisttimes();
		Sim->ExecuteNext();
		/*for (int asd = 0; asd < eprnumber+1; asd++)
		{
			cout << endl << "left" << asd<< endl;
			meminfo(nodes[asd].memleft, nodes[asd].memsize);
			cout << endl << "right" << asd << endl;
			meminfo(nodes[asd].memright, nodes[asd].memsize);
		}*/
		for (int i = 0; i < nodes[eprnumber].memsize; i++)
		{
			if (nodes[eprnumber].memleft[i].state == 3 && nodes[eprnumber].memleft[i].ReadytoMeasure == 1 && nodes[eprnumber].memleft[i].fid >= nodes[eprnumber].targetfid) // pair is nice and well at endpoints
			{
				donepairs = donepairs + 1;
				//write stats
				stats.open(filename, ios::out | ios::app);
				stats << donepairs << " " << Sim->curtime << " " << Sim->curtime - nodes[eprnumber].memleft[i].rcvtime << endl;
				stats.close();
				//free up the pairs
				nodes[eprnumber].memleft[i].pair->mem[!nodes[eprnumber].memleft[i].pairindex]->reset();
				delete nodes[eprnumber].memleft[i].pair;
				nodes[eprnumber].memleft[i].reset();
				cout << endl << "donepairs: " << donepairs << endl;
			}
		}
	}

	//clean up after
	delete Sim;
	delete[] nodes;
	delete[] eprnodes;
	delete[] channels;

	return 0;
}

int Pow2Sim::AvgSimpleSim(int targetpairs, int times, string filename)
{
	double* results= new double[targetpairs * 3];
	for (int i = 0; i < targetpairs * 3; i++)
	{
		results[i] = 0;
	}


	int donepairs = 0;
	// start simulation
	for (int st = 0; st < times; st++)
	{

		Node * nodes = new Node[eprnumber + 1];
		Node * eprnodes = new Node[eprnumber];
		Channel * channels = new Channel[2 * eprnumber];
		// Setup tree
		Pow2Setup(nodes, eprnodes, channels, eprnumber, std_epr, dist, memsize, epratonce, chalength, targetfid, PurifMethod);

		//cout << "1 ";
		SimRoot * Sim = new SimRoot;
		//cout << "2 ";
		Sim->diagnostics = 0;
		donepairs = 0;
		for (int i = 0; i < eprnumber; i++) eprnodes[i].GenEPR(Sim); // firts stimuli
		while (donepairs < targetpairs)
		{
			int a = 0;
			int executes = 0;
			if (a != 0) Sim->printlisttimes();
			Sim->ExecuteNext();
			executes++;
			if (executes > 10000)
			{
				if (a == 1) 
				{
					for (int j = 0; j < eprnumber + 1; j++)
					{
						cout << endl << "node" << j;
						nodes[j].printmemstates();
					}
					
				}
				cin >> a;
			}
			if (a == 2) Sim->diagnostics = 1;
			if (a == 10) Sim->diagnostics = 0;
			//cout << "4 ";
			for (int i = 0; i < nodes[eprnumber].memsize; i++)
			{
				if (nodes[eprnumber].memleft[i].state == 3 && nodes[eprnumber].memleft[i].ReadytoMeasure == 1 && nodes[eprnumber].memleft[i].fid >= nodes[eprnumber].targetfid) // pair is nice and well at endpoints
				{
					executes = 0;
					donepairs = donepairs + 1;
					//write stats to variable
					results[(donepairs-1) * 3] = results[(donepairs - 1) * 3] + donepairs;
					results[((donepairs-1) * 3) + 1] = results[((donepairs - 1) * 3) + 1] + Sim->curtime;
					results[((donepairs - 1) * 3) + 2] = results[((donepairs - 1) * 3) + 2] + Sim->curtime - nodes[eprnumber].memleft[i].rcvtime;
					//free up the pairs
					nodes[eprnumber].memleft[i].pair->mem[!nodes[eprnumber].memleft[i].pairindex]->reset();
					//cout << "5 ";
					delete nodes[eprnumber].memleft[i].pair;
					//cout << "6 ";
					nodes[eprnumber].memleft[i].reset();
					cout << endl << "donepairs"<<st<<": " << donepairs << endl;
				}
			}
		}
		delete Sim;
		delete[] nodes;
		delete[] eprnodes;
		delete[] channels;
		//reset memories for everyone
		//cout << "5 ";
	}
	//calculate average from sum
	for (int i = 0; i < targetpairs; i++) 
	{
		results[i * 3] = results[i * 3] / times;
		results[(i * 3) + 1] = results[(i * 3) + 1] / times;
		results[(i * 3) + 2] = results[(i * 3) + 2] / times;
	}
	//write to file
	ofstream stats;
	stats.open(filename, ios::out | ios::trunc);
	for (int i = 0; i < targetpairs; i++)
	{
		stats << results[i * 3] << " " << results[(i * 3) + 1] << " " << results[(i * 3) + 2] << endl;
	}
	stats.close();

	//clean up after
	delete[] results;
	//delete[] nodes;
	//delete[] eprnodes;
	//delete[] channels;

	return 0;
}

int Pow2Sim::AvgSim(int targetpairs, double * avgtime, double * avgmemtime)
{
	Node * nodes = new Node[eprnumber + 1];
	Node * eprnodes = new Node[eprnumber];
	Channel * channels = new Channel[2 * eprnumber];
	SimRoot * Sim = new SimRoot;
	Sim->diagnostics = 0;
	*avgmemtime = 0;
	

	//Setup tree
	Pow2Setup(nodes, eprnodes, channels, eprnumber, std_epr, dist, memsize, epratonce, chalength, targetfid, PurifMethod);

	int donepairs = 0;
	// start simulation
	for (int i = 0; i < eprnumber; i++) eprnodes[i].GenEPR(Sim); // firts stimuli
	while (donepairs < targetpairs && Sim->curtime < timelimit*targetpairs)
	{
		Sim->ExecuteNext();
		//Sim->printlisttimes();
		//cout << endl;
		//nodes[0].printmemstates();
		//nodes[1].printmemstates();
		for (int i = 0; i < nodes[eprnumber].memsize; i++)
		{
			//cout << nodes[eprnumber].memleft[i].state <<"  "<< nodes[eprnumber].memleft[i].ReadytoMeasure << endl;
			if (nodes[eprnumber].memleft[i].state == 3 && nodes[eprnumber].memleft[i].ReadytoMeasure == 1 && nodes[eprnumber].memleft[i].fid >= nodes[eprnumber].targetfid) // pair is nice and well at endpoints
			{
				donepairs = donepairs + 1;
				//write stats
				*avgmemtime = *avgmemtime + (Sim->curtime - nodes[eprnumber].memleft[i].rcvtime);
				//free up the pairs
				nodes[eprnumber].memleft[i].pair->mem[!nodes[eprnumber].memleft[i].pairindex]->reset();
				delete nodes[eprnumber].memleft[i].pair;
				nodes[eprnumber].memleft[i].reset();
				cout << endl << "donepairs: " << donepairs << endl << "pairsinmem: " << pairsinmem << "  memsinmem: " << memsinmem << "  itemsinmem: " << itemsinmem << "  measuresinmem: " << measuresinmem
					<< "  nodesinmem: " << nodesinmem << "  eprsinmem: " << eprsinmem << "  channelsinmem: " << channelsinmem << endl;
				
				//change title
				ostringstream scim;
				scim <<title<<" AvgSim: "<< (int) (donepairs*100)/targetpairs << "%";
				string str;
				str = scim.str();
				wstring wstr;
				wstr.assign(str.begin(), str.end());
				SetConsoleTitle(wstr.c_str());
			}
		}
	}
	if (donepairs != 0)
	{
		*avgtime = Sim->curtime / (double)donepairs;
		*avgmemtime = *avgmemtime / (double)donepairs;
	}
	else
	{
		*avgtime = 0;
		*avgmemtime = 0;
	}

	//clean up after
	delete Sim;
	delete[] nodes;
	delete[] eprnodes;
	delete[] channels;

	return 0;

	return 0;
}

int Pow2Sim::AvgFidSweep(int targetpairs, double from, double to, double step, string filename)
{
	//reset file
	ofstream stats;
	stats.open(filename, ios::out | ios::trunc);
	stats.close();
	//do the simulations
	for (double curfid = from; curfid <= to; curfid = curfid + step)
	{
		//set title
		int percent = (int)(((curfid - from) * 100) / (to - from));
		ostringstream stit;
		stit << "repsim: " << percent << "% ";
		title = stit.str();
		//do simulation for given fidelity
		double avgtime = 0;
		double avgmemtime = 0;
		std_epr->fidelity = curfid;
		this->AvgSim(targetpairs, &avgtime, &avgmemtime);
		// write results
		stats.open(filename, ios::out | ios::app);
		stats << curfid << " " << avgtime << " " << avgmemtime<<endl;
		stats.close();
	}
	ostringstream scim;
	scim << "repsim: Done!";
	string str = scim.str();
	wstring wstr;
	wstr.assign(str.begin(), str.end());
	SetConsoleTitle(wstr.c_str());
	return 0;
}

int Pow2Sim::AvgLengthSweep(int targetpairs, double from, double to, double step, string filename)
{
	//reset file
	ofstream stats;
	stats.open(filename, ios::out | ios::trunc);
	stats.close();
	//do the simulations
	for (double curfid = from; curfid <= to; curfid = curfid + step)
	{
		//set title
		int percent = (int)(((curfid - from) * 100) / (to - from));
		ostringstream stit;
		stit << "repsim: " << percent << "% ";
		title = stit.str();
		//do simulation for given fidelity
		double avgtime = 0;
		double avgmemtime = 0;
		dist = curfid;
		this->AvgSim(targetpairs, &avgtime, &avgmemtime);
		// write results
		stats.open(filename, ios::out | ios::app);
		stats << curfid << " " << avgtime << " " << avgmemtime << endl;
		stats.close();
	}
	ostringstream scim;
	scim << "repsim: Done!";
	string str = scim.str();
	wstring wstr;
	wstr.assign(str.begin(), str.end());
	SetConsoleTitle(wstr.c_str());
	return 0;
}

int meminfo(QMem * mem, int memsize)
{
	for (int i = 0; i < memsize; i++)
	{
		cout << "fid" << i << ": " << mem[i].fid << "  st: " << mem[i].state << "  rd: " << mem[i].ReadytoMeasure << "  pur: " << mem[i].inpurif << "    ";
		if (i % 2 == 1) cout << endl;
	}

	return 0;
}

int Pow2Sim::MultiAvgFidSweep(int targetpairs, double from, double to, double step, string filename, int simthreads)
{
	//reset file
	ofstream stats;
	stats.open(filename, ios::out | ios::trunc);
	stats.close();
	//set up threads
//	function<int(int, double*,double*,double,int*)> toSim=Pow2Sim::AvgSimMFid ;

	TaskThread<int(int, double*,double*,double, int*), int>  *sthreads;
	sthreads = new TaskThread<int(int, double*, double*, double, int*), int>[simthreads];
	//console
	int *threadprogress = new int[simthreads];
	for (int i = 0; i < simthreads; i++) threadprogress[i] = 0;
	int progress = 0;
	bool console_on = true;
	thread console_thread(&ManageConsoleTitle, &console_on, &progress, threadprogress, simthreads);
	//set up
	for (int i = 0; i < simthreads; i++)
	{
		sthreads[i].task = packaged_task<int (int, double*, double*, double, int*)> (std::bind(&Pow2Sim::AvgSimMFid, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
		sthreads[i].future = sthreads[i].task.get_future();
	}
	//do the simulations
	int iter = 0;
	double *avgtime = new double[targetpairs];
	double *avgmemtime = new double[targetpairs];

	double curfid = from;
	//start threads
	for (int i = 0; i < simthreads && curfid <= to; i++)
	{
		cout << "aa";
		sthreads[i].thread = thread(move(sthreads[i].task), targetpairs, &avgtime[iter], &avgmemtime[iter], curfid, &threadprogress[i]);
		iter++;
		curfid = curfid + step;
		cout << i;
		cout << "bb";
	}
	cout << "cc";
	//check for done threads
	while (curfid <= to)
	{
		for (int i = 0; i < simthreads; i++)
		{
			cout << "it  " << iter << endl;
			if (sthreads[i].future.wait_for(chrono::milliseconds(50)) == future_status::ready && curfid <= to)
			{
				sthreads[i].thread.join();
				sthreads[i].task = packaged_task<int(int, double*, double*, double, int*)>(std::bind(&Pow2Sim::AvgSimMFid, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				sthreads[i].future = sthreads[i].task.get_future();
				sthreads[i].thread = thread(move(sthreads[i].task), targetpairs, &avgtime[iter], &avgmemtime[iter], curfid, &threadprogress[i]);
				iter++;
				cout << "fut" << endl;
				curfid = curfid + step;
				{
					lock_guard<mutex> lock(consoletitle);
					progress = (int)(((curfid - from) * 100) / (to - from));
				}
			}
		}
	}

	//cleanup
	cout << endl << "cleanup" << endl;
	for (int i = 0; i < simthreads; i++)
	{
		if (sthreads[i].thread.joinable()) sthreads[i].thread.join();
		cout << endl << i << endl;
	}
	console_on = false;
	cout << "consolej";
	console_thread.join();
	cout << endl << "consolej done" << endl;

	// write results
	stats.open(filename, ios::out | ios::app);
	int kiindex = 0;
	for (double wfid = from; wfid <= to; wfid = wfid + step)
	{
		stats << wfid << " " << avgtime[kiindex] << " " << avgmemtime[kiindex] << endl;
		kiindex++;
	}
	stats.close();

	ostringstream scim;
	scim << "repsim: Done!";
	string str = scim.str();
	wstring wstr;
	wstr.assign(str.begin(), str.end());
	SetConsoleTitle(wstr.c_str());
	//clean up memory
	delete[] sthreads;
	delete[] avgtime;
	delete[] avgmemtime;

	return 0;
}

int Pow2Sim::AvgSimMFid(int targetpairs, double * avgtime, double * avgmemtime, double actfid, int * progress)
{
	Node * nodes = new Node[eprnumber + 1];
	Node * eprnodes = new Node[eprnumber];
	Channel * channels = new Channel[2 * eprnumber];
	SimRoot * Sim = new SimRoot;
	Sim->diagnostics = 0;
	*avgmemtime = 0;
	EPR cur_epr;
	cur_epr.state = new Vector4cd;
	cur_epr.fidelity = actfid;
	cur_epr.rate = std_epr->rate;


	//Setup tree
	Pow2Setup(nodes, eprnodes, channels, eprnumber, &cur_epr, dist, memsize, epratonce, chalength, targetfid, PurifMethod);
	//LinSetup(nodes, eprnodes, channels, eprnumber, &cur_epr, dist, memsize, epratonce, chalength, targetfid, PurifMethod);
	int donepairs = 0;
	// start simulation
	for (int i = 0; i < eprnumber; i++) eprnodes[i].GenEPR(Sim); // firts stimuli
	while (donepairs < targetpairs && Sim->curtime < timelimit*targetpairs)
	{
		Sim->ExecuteNext();
		//Sim->printlisttimes();
		//cout << endl;
		//nodes[0].printmemstates();
		//nodes[1].printmemstates();
		for (int i = 0; i < nodes[eprnumber].memsize; i++)
		{
			//cout << nodes[eprnumber].memleft[i].state <<"  "<< nodes[eprnumber].memleft[i].ReadytoMeasure << endl;
			if (nodes[eprnumber].memleft[i].state == 3 && nodes[eprnumber].memleft[i].ReadytoMeasure == 1 && nodes[eprnumber].memleft[i].fid >= nodes[eprnumber].targetfid) // pair is nice and well at endpoints
			{
				donepairs = donepairs + 1;
				//write stats
				*avgmemtime = *avgmemtime + (Sim->curtime - nodes[eprnumber].memleft[i].rcvtime);
				//free up the pairs
				nodes[eprnumber].memleft[i].pair->mem[!nodes[eprnumber].memleft[i].pairindex]->reset();
				delete nodes[eprnumber].memleft[i].pair;
				nodes[eprnumber].memleft[i].reset();
				/*cout << endl << "donepairs: " << donepairs << endl << "pairsinmem: " << pairsinmem << "  memsinmem: " << memsinmem << "  itemsinmem: " << itemsinmem << "  measuresinmem: " << measuresinmem
					<< "  nodesinmem: " << nodesinmem << "  eprsinmem: " << eprsinmem << "  channelsinmem: " << channelsinmem << endl;*/

				//change title
				{
					lock_guard<mutex> lock(consoletitle);
					*progress = (int)(donepairs * 100) / targetpairs;
				}
			}
		}
	}
	if (donepairs != 0)
	{
		*avgtime = Sim->curtime / (double)donepairs;
		*avgmemtime = *avgmemtime / (double)donepairs;
	}
	else
	{
		*avgtime = 0;
		*avgmemtime = 0;
	}

	//clean up after
	delete Sim;
	delete[] nodes;
	delete[] eprnodes;
	delete[] channels;

	return 0;

	return 0;
}

void ManageConsoleTitle(bool * work, int * allprcnt, int * threadprcnt, int threads)
{
	while (*work)
	{
		{
			lock_guard<mutex> lock(consoletitle);
			std::ostringstream scim;
			scim << "test: " << *allprcnt << "% ";
			//scim << " " << threads;
			for (int i = 0; i < threads; i++)
				scim << "t" << i << ": " << threadprcnt[i] << "% ";
			std::string str = scim.str();
			std::wstring wstr;
			wstr.assign(str.begin(), str.end());
			SetConsoleTitle(wstr.c_str());
		}
		std::this_thread::sleep_for(chrono::milliseconds(500));
	}
}

int Pow2Sim::AvgSimMEPR(int targetpairs, double * avgtime, double * avgmemtime, double rate, int * progress)
{
	Node * nodes = new Node[eprnumber + 1];
	Node * eprnodes = new Node[eprnumber];
	Channel * channels = new Channel[2 * eprnumber];
	SimRoot * Sim = new SimRoot;
	Sim->diagnostics = 0;
	*avgmemtime = 0;
	EPR cur_epr;
	cur_epr.state = new Vector4cd;
	cur_epr.fidelity = std_epr->fidelity;
	cur_epr.rate = rate;


	//Setup tree
	Pow2Setup(nodes, eprnodes, channels, eprnumber, &cur_epr, dist, memsize, epratonce, chalength, targetfid, PurifMethod);

	int donepairs = 0;
	// start simulation
	for (int i = 0; i < eprnumber; i++) eprnodes[i].GenEPR(Sim); // firts stimuli
	while (donepairs < targetpairs && Sim->curtime < timelimit*targetpairs)
	{
		Sim->ExecuteNext();
		//Sim->printlisttimes();
		//cout << endl;
		//nodes[0].printmemstates();
		//nodes[1].printmemstates();
		for (int i = 0; i < nodes[eprnumber].memsize; i++)
		{
			//cout << nodes[eprnumber].memleft[i].state <<"  "<< nodes[eprnumber].memleft[i].ReadytoMeasure << endl;
			if (nodes[eprnumber].memleft[i].state == 3 && nodes[eprnumber].memleft[i].ReadytoMeasure == 1 && nodes[eprnumber].memleft[i].fid >= nodes[eprnumber].targetfid) // pair is nice and well at endpoints
			{
				donepairs = donepairs + 1;
				//write stats
				*avgmemtime = *avgmemtime + (Sim->curtime - nodes[eprnumber].memleft[i].rcvtime);
				//free up the pairs
				nodes[eprnumber].memleft[i].pair->mem[!nodes[eprnumber].memleft[i].pairindex]->reset();
				delete nodes[eprnumber].memleft[i].pair;
				nodes[eprnumber].memleft[i].reset();
				//cout << endl << "donepairs: " << donepairs << endl << "pairsinmem: " << pairsinmem << "  memsinmem: " << memsinmem << "  itemsinmem: " << itemsinmem << "  measuresinmem: " << measuresinmem
				//	<< "  nodesinmem: " << nodesinmem << "  eprsinmem: " << eprsinmem << "  channelsinmem: " << channelsinmem << endl;

				//change title
				{
					lock_guard<mutex> lock(consoletitle);
					*progress = (int)(donepairs * 100) / targetpairs;
				}
			}
		}
	}
	if (donepairs != 0)
	{
		*avgtime = Sim->curtime / (double)donepairs;
		*avgmemtime = *avgmemtime / (double)donepairs;
	}
	else
	{
		*avgtime = 0;
		*avgmemtime = 0;
	}

	//clean up after
	delete Sim;
	delete[] nodes;
	delete[] eprnodes;
	delete[] channels;

	return 0;

	return 0;
}

int Pow2Sim::MultiAvgEPRSweep(int targetpairs, double from, double to, double step, string filename, int simthreads)
{
	//reset file
	ofstream stats;
	stats.open(filename, ios::out | ios::trunc);
	stats.close();
	//set up threads
	//	function<int(int, double*,double*,double,int*)> toSim=Pow2Sim::AvgSimMFid ;

	TaskThread<int(int, double*, double*, double, int*), int>  *sthreads;
	sthreads = new TaskThread<int(int, double*, double*, double, int*), int>[simthreads];
	//console
	int *threadprogress = new int[simthreads];
	for (int i = 0; i < simthreads; i++) threadprogress[i] = 0;
	int progress = 0;
	bool console_on = true;
	thread console_thread(&ManageConsoleTitle, &console_on, &progress, threadprogress, simthreads);
	//set up
	for (int i = 0; i < simthreads; i++)
	{
		sthreads[i].task = packaged_task<int(int, double*, double*, double, int*)>(std::bind(&Pow2Sim::AvgSimMEPR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
		sthreads[i].future = sthreads[i].task.get_future();
	}
	//do the simulations
	int iter = 0;
	double *avgtime = new double[targetpairs];
	double *avgmemtime = new double[targetpairs];

	double curfid = from;
	//start threads
	for (int i = 0; i < simthreads && curfid <= to; i++)
	{
		cout << "aa";
		sthreads[i].thread = thread(move(sthreads[i].task), targetpairs, &avgtime[iter], &avgmemtime[iter], curfid, &threadprogress[i]);
		iter++;
		curfid = curfid * step;
		cout << i;
		cout << "bb";
	}
	cout << "cc";
	//check for done threads
	while (curfid <= to)
	{
		for (int i = 0; i < simthreads; i++)
		{
			cout << "it  " << iter << endl;
			if (sthreads[i].future.wait_for(chrono::milliseconds(50)) == future_status::ready && curfid <= to)
			{
				sthreads[i].thread.join();
				sthreads[i].task = packaged_task<int(int, double*, double*, double, int*)>(std::bind(&Pow2Sim::AvgSimMEPR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				sthreads[i].future = sthreads[i].task.get_future();
				sthreads[i].thread = thread(move(sthreads[i].task), targetpairs, &avgtime[iter], &avgmemtime[iter], curfid, &threadprogress[i]);
				iter++;
				cout << "fut" << endl;
				curfid = curfid * step;
				{
					lock_guard<mutex> lock(consoletitle);
					progress = (int)(((curfid - from) * 100) / (to - from));
				}
			}
		}
	}

	//cleanup
	cout << endl << "cleanup" << endl;
	for (int i = 0; i < simthreads; i++)
	{
		if (sthreads[i].thread.joinable()) sthreads[i].thread.join();
		cout << endl << i << endl;
	}
	console_on = false;
	cout << "consolej";
	console_thread.join();
	cout << endl << "consolej done" << endl;

	// write results
	stats.open(filename, ios::out | ios::app);
	int kiindex = 0;
	for (double wfid = from; wfid <= to; wfid = wfid * step)
	{
		stats << wfid << " " << avgtime[kiindex] << " " << avgmemtime[kiindex] << endl;
		kiindex++;
	}
	stats.close();

	ostringstream scim;
	scim << "repsim: Done!";
	string str = scim.str();
	wstring wstr;
	wstr.assign(str.begin(), str.end());
	SetConsoleTitle(wstr.c_str());
	//clean up memory
	delete[] sthreads;
	delete[] avgtime;
	delete[] avgmemtime;

	return 0;
}