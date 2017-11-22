#pragma once


#include "qrep.h"
#include "elements.h"
#include <random>
#include "Simulation.h"
#include <fstream>
#include <windows.h>

//Makes even tree
void Makepow2tree(Node nodes[], int min, int max, Node * parent, int pindex, double dist);

//Makes simple linear tree
void InitLintree(Node nodes[], Node eprnodes[], Channel channels[], EPR * epr, int size, double dist, double chalength);

void Pow2Setup(Node nodes[], Node eprnodes[], Channel channels[], int eprnumber, EPR * std_epr, double dist, int memsize = 10,int epratonce=1, double chalength = 20, double targetfid = 0.98,function<int(QMem*,int,double)> PurifMethod = GreedyBU_DEJPurif);

class Pow2Sim
{
public:
	int eprnumber;
	EPR * std_epr;
	double dist;
	int memsize;
	int epratonce;
	double chalength;
	double targetfid;
	function<int(QMem*, int, double)> PurifMethod;

	// konst
	Pow2Sim();
	//dest
	~Pow2Sim();
	//other
	int SimpleSim(int targetpairs, string filename);

	int AvgSimpleSim(int targetpairs, int times, string filename);

	int AvgSim(int targetpairs, double * avgtime, double * avgmemtime);
};