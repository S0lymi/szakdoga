#include "qrep.h"
#include "elements.h"
#include <random>
#include "Simulation.h"
#include <fstream>
#include "Testhelp.h"
#include <chrono>

int main()
{
	
	timelimit = 5000;
	pairsinmem = 0;
	memsinmem = 0;
	itemsinmem = 0;
	measuresinmem = 0;
	nodesinmem = 0;
	eprsinmem = 0;
	channelsinmem = 0;
	withpurifpp = 1;
		Pow2Sim Sim;
		Sim.eprnumber = 4;
		Sim.dist = 50;
		Sim.chalength = 20;
		Sim.memsize = 20;
		Sim.epratonce = 5;
		Sim.targetfid = 0.98;
		Sim.std_epr->fidelity = 0.7;
		Sim.std_epr->rate = 10;

	//double avgtime = 0;
	//double avgmemtime = 0;
	//measure execution time
	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
	int threads = 8;
	Sim.dist = 150;
	Sim.eprnumber = 8;
	Sim.AvgSimpleSim(100, 50, "results/start/start_8");
	Sim.eprnumber = 4;
	Sim.AvgSimpleSim(100, 50, "results/start/start_4");



	//Sim.PurifMethod = GreedyTD_DEJPurif;
	//Sim.dist = 150;
	//Sim.eprnumber = 4;
	//Sim.MultiAvgFidSweep(200, 0.6, 1, 0.01, "results/purifprot/TD_4.m",threads);
	//Sim.eprnumber = 8;
	//Sim.MultiAvgFidSweep(200, 0.6, 1, 0.01, "results/purifprot/TD_8.m", threads);
	//Sim.eprnumber = 16;
	//Sim.MultiAvgFidSweep(200, 0.6, 1, 0.01, "results/purifprot/TD_16.m", threads);


	/*Sim.dist = 150;
	Sim.PurifMethod = GreedyBU_DEJPurif2;
	Sim.eprnumber = 4;
	Sim.MultiAvgFidSweep(200, 0.6, 1, 0.01, "results/purifprot/lower_4.m", threads);
	Sim.eprnumber = 8;
	Sim.MultiAvgFidSweep(200, 0.6, 1, 0.01, "results/purifprot/lower_8.m", threads);
	Sim.eprnumber = 16;
	Sim.MultiAvgFidSweep(200, 0.6, 1, 0.01, "results/purifprot/lower_16.m", threads);*/
	//Sim.dist = 400;
	//ofstream stats;
	//stats.open("results/sorrend/Lin_250.m", ios::out | ios::trunc);
	//stats.close();
	////Sim.SimpleSim(200, "sorrend/test.m");
	//for (int i = 4; i < 17; i=i*2)
	//{
	//	int asd = 0;
	//	double time, memtime;
	//	Sim.eprnumber = i;
	//	Sim.AvgSimMFid(200, &time, &memtime, Sim.std_epr->fidelity, &asd);
	//	// write results
	//	stats.open("results/sorrend/Pow2_250.m", ios::out | ios::app);
	//	stats << i << " " << time << " " << memtime << endl;
	//	stats.close();
	//}
	//Sim.MultiAvgFidSweep(200, 0.6, 1, 0.05, "sorrend/Lin_250.m", threads);
	//ch
	//Sim.std_epr->rate = 10.0/ 8.0;
	//Sim.memsize = 40;
	//Sim.eprnumber = 1;
	//Sim.AvgLengthSweep(200, 5, 500, 5, "results/repvsch/chfid07handicapped_5_500.m");
	//rep2
	/*

	Sim.eprnumber = 4;
	Sim.dist = 150;
	//2
	Sim.std_epr->rate = 2;
	Sim.memsize = 80;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep2_150.m", threads);
	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_150.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_150.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_150.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_150.m", threads);
	
	//200
	Sim.dist = 200;
	 
	//2
	Sim.std_epr->rate = 2;
	Sim.memsize = 80;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep2_200.m", threads);
	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_200.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_200.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_200.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_200.m", threads);

	//250
	Sim.dist = 250;

	//2
	Sim.std_epr->rate = 2;
	Sim.memsize = 80;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep2_250.m", threads);
	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_250.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_250.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_250.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_250.m", threads);

	//300
	Sim.dist = 300;

	//2
	Sim.std_epr->rate = 2;
	Sim.memsize = 80;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep2_300.m", threads);
	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_300.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_300.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_300.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_300.m", threads);

	//350
	Sim.dist = 350;

	//2
	Sim.std_epr->rate = 2;
	Sim.memsize = 80;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep2_350.m", threads);
	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_350.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_350.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_350.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_350.m", threads);

	//400
	Sim.dist = 400;

	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_400.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_400.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_400.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_400.m", threads);

	//450
	Sim.dist = 450;

	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_450.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_450.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_450.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_450.m", threads);

	//500
	Sim.dist = 500;

	//4
	Sim.std_epr->rate = 4;
	Sim.memsize = 40;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep4_500.m", threads);
	//8
	Sim.std_epr->rate = 8;
	Sim.memsize = 20;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_500.m", threads);
	//16
	Sim.std_epr->rate = 16;
	Sim.memsize = 10;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep16_500.m", threads);
	//32
	Sim.std_epr->rate = 32;
	Sim.memsize = 5;
	Sim.MultiAvgFidSweep(500, 0.6, 1, 0.05, "results/Resource/rep8_500.m", threads);
	//*/
	/*
	//fid 0.7
	Sim.std_epr->fidelity = 0.7;
	Sim.eprnumber = 4;
	Sim.AvgLengthSweep(500, 40, 1000, 20,"lengthfid07_40_1000_20_4");
	Sim.eprnumber = 8;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid07_40_1000_20_8");
	Sim.eprnumber = 16;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid07_40_1000_20_16");
	Sim.eprnumber = 32;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid07_40_1000_20_32");
	Sim.eprnumber = 64;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid07_40_1000_20_64");
	//fid 0.8
	Sim.std_epr->fidelity = 0.8;
	Sim.eprnumber = 4;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid08_40_1000_20_4");
	Sim.eprnumber = 8;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid08_40_1000_20_8");
	Sim.eprnumber = 16;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid08_40_1000_20_16");
	Sim.eprnumber = 32;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid08_40_1000_20_32");
	Sim.eprnumber = 64;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid08_40_1000_20_64");
	//fid 0.9
	Sim.std_epr->fidelity = 0.9;
	Sim.eprnumber = 4;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid09_40_1000_20_4");
	Sim.eprnumber = 8;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid09_40_1000_20_8");
	Sim.eprnumber = 16;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid09_40_1000_20_16");
	Sim.eprnumber = 32;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid09_40_1000_20_32");
	Sim.eprnumber = 64;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid09_40_1000_20_64");
	//fid 1.0
	Sim.std_epr->fidelity = 1;
	Sim.eprnumber = 4;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid10_40_1000_20_4");
	Sim.eprnumber = 8;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid10_40_1000_20_8");
	Sim.eprnumber = 16;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid10_40_1000_20_16");
	Sim.eprnumber = 32;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid10_40_1000_20_32");
	Sim.eprnumber = 64;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid10_40_1000_20_64");
	//fid 0.6
	Sim.std_epr->fidelity = 0.6;
	Sim.eprnumber = 4;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid06_40_1000_20_4");
	Sim.eprnumber = 8;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid06_40_1000_20_8");
	Sim.eprnumber = 16;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid06_40_1000_20_16");
	Sim.eprnumber = 32;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid06_40_1000_20_32");
	Sim.eprnumber = 64;
	Sim.AvgLengthSweep(500, 40, 1000, 20, "lengthfid06_40_1000_20_64");
	//*/


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

