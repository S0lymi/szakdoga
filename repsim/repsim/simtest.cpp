#include "qrep.h"
#include "elements.h"
#include <random>
#include "Simulation.h"
#include <fstream>
#include "Testhelp.h"
#include <chrono>

int main()
{
	pairsinmem = 0;
	memsinmem = 0;
	itemsinmem = 0;
	measuresinmem = 0;
	nodesinmem = 0;
	eprsinmem = 0;
	channelsinmem = 0;
	withpurifpp = 0;

	Pow2Sim Sim;
	Sim.eprnumber = 32;
	Sim.dist = 400;
	Sim.chalength = 20;
	Sim.memsize = 20;
	Sim.epratonce = 4;
	Sim.targetfid = 0.98;
	Sim.std_epr->fidelity = 0.7;
	Sim.std_epr->rate = 10;
	//double avgtime = 0;
	//double avgmemtime = 0;
	//measure execution time
	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
	Sim.SimpleSim(100, "simple200.m");
	//Sim.AvgFidSweep(500, 0.6, 1, 0.01, "fidsweep.m");
	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();

	int duration = chrono::duration_cast<chrono::seconds>(t2 - t1).count();

	cout << "execution time:" << duration / 3600 << " h " << (duration % 3600) / 60 << " m " << duration % 60 << " s " << endl;

	//cout << endl << "avgtime: " << avgtime << "     avgmemtime: " << avgmemtime << endl;
	cout << '\a';
	cout << endl << "pairsinmem: " << pairsinmem << "  memsinmem: " << memsinmem << "  itemsinmem: " << itemsinmem << "  measuresinmem: " << measuresinmem
		<< "  nodesinmem: " << nodesinmem << "  eprsinmem: " << eprsinmem << "  channelsinmem: " << channelsinmem << endl;
	/*
	Node* nodes= new Node[9];
	Node* eprnodes = new Node[8];
	Channel* channels = new Channel[16];
	EPR std_epr;
	std_epr.rate = 20;
	std_epr.fidelity = 0.7;

	Pow2Setup(nodes, eprnodes, channels, 8, &std_epr, 80, 10, 1);

	SimRoot Simr;
	Simr.diagnostics = 1;
	int a = 1;
	eprnodes[0].GenEPR(&Simr);
	Simr.printlisttimes();
	while (a != 0)
	{
		Simr.ExecuteNext();
		Simr.printlisttimes();
		cout << endl << "pairsinmem: " << pairsinmem << "  memsinmem: " << memsinmem << "  itemsinmem: " << itemsinmem << "  measuresinmem: " << measuresinmem
			<< "  nodesinmem: " << nodesinmem << "  eprsinmem: " << eprsinmem << "  channelsinmem: " << channelsinmem << endl;
		nodes[1].printmemstates();
		cin >> a;
	}

	//*/
	/*
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
	nodes[8].nextNode = &nodes[0];
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
	//*/
	//SimRoot Sim;
	//Sim.diagnostics = 0;
	


	//test for whole, 1 eprgen  at a time (with 8 eprs)
	/*
	//for (int i = 0; i < 8; i++) eprnodes[i].epratonce = 4;
	//for (int i = 0; i < 9; i++) nodes[i].purification = GreedyBU_DEJPurif;
	int a = 0;
	int donepairs = 0;
	for (int i = 0; i < 8; i++) eprnodes[i].GenEPR(&Sim);
	while (donepairs < 200)
	{
		
		//Sim.printlisttimes();
		Sim.ExecuteNext();
		for (int i = 0; i < nodes[8].memsize; i++)
			{
				if (nodes[8].memleft[i].state == 3 && nodes[8].memleft[i].ReadytoMeasure == 1 && nodes[8].memleft[i].fid >= nodes[8].targetfid) // pair is nice and well at endpoints
				{
					donepairs = donepairs + 1;
					//write stats
					ofstream stats;
					stats.open("stats.m", ios::out | ios::app);
					stats << Sim.curtime << " " << donepairs << " " << Sim.curtime - nodes[8].memleft[i].rcvtime << endl;
				stats.close();
				//free up the pairs
				nodes[8].memleft[i].pair->mem[!nodes[8].memleft[i].pairindex]->reset();
				delete nodes[8].memleft[i].pair;
				nodes[8].memleft[i].reset();
				cout << endl << "donepairs: " << donepairs << endl;
				}
			 }
	
	}
	//*/

}

