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
	for (int i = 0; i < size * 2; i++)//set up channels between nodes
	{
		channels[i].from = &eprnodes[i / 2];
		channels[i].to = &nodes[(i + 1) / 2];
		channels[i].length = dist / 2;
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

void Pow2Setup(Node nodes[], Node eprnodes[], Channel channels[], int eprnumber, EPR * std_epr, double dist, int memsize, int epratonce, double chalength, double targetfid, function<int(QMem*, int, double)> PurifMethod)
{
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
			nodes[i].prevNoderight = &eprnodes[i];
			nodes[i].prevdistright = nodes[i].prevNoderight->leftch->length;
		}

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
	PurifMethod = GreedyBU_DEJPurif;
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

	int donepairs = 0;
	ofstream stats;
	stats.open(filename, ios::out | ios::trunc);
	stats.close();
	 // start simulation
	for (int i = 0; i < eprnumber; i++) eprnodes[i].GenEPR(Sim); // firts stimuli
	while (donepairs < targetpairs)
	{
		Sim->ExecuteNext();
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
	delete[] nodes;
	delete[] eprnodes;
	delete[] channels;
	delete Sim;

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
	while (donepairs < targetpairs)
	{
		Sim->ExecuteNext();
		for (int i = 0; i < nodes[eprnumber].memsize; i++)
		{
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
	*avgtime = Sim->curtime / (double) targetpairs;
	*avgmemtime = *avgmemtime / (double)targetpairs;
	//clean up after
	delete[] nodes;
	delete[] eprnodes;
	delete[] channels;
	delete Sim;

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

