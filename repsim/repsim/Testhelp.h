#pragma once


#include "qrep.h"
#include "elements.h"
#include <random>
#include "Simulation.h"
#include <fstream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <future>

static mutex consoletitle;

//Makes even tree
void Makepow2tree(Node nodes[], int min, int max, Node * parent, int pindex, double dist);

//Makes simple linear tree
void InitLintree(Node nodes[], Node eprnodes[], Channel channels[], EPR * epr, int size, double dist, double chalength);

void Pow2Setup(Node nodes[], Node eprnodes[], Channel channels[], int eprnumber, EPR * std_epr, double dist, int memsize = 10, int epratonce = 1, double chalength = 20, double targetfid = 0.98, function<int(SimRoot*, QMem**, int, double)> PurifMethod = GreedyBU_DEJPurif2);

void LinSetup(Node nodes[], Node eprnodes[], Channel channels[], int eprnumber, EPR * std_epr, double dist, int memsize = 10, int epratonce = 1, double chalength = 20, double targetfid = 0.98, function<int(SimRoot*, QMem**, int, double)> PurifMethod = GreedyBU_DEJPurif2);
//for threads
void ManageConsoleTitle(bool * work, int * allprcnt, int * threadprcnt, int threads);

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
	function<int(SimRoot*, QMem**, int, double)> PurifMethod;
	string title;

	// konst
	Pow2Sim();
	//dest
	~Pow2Sim();
	//other
	int SimpleSim(int targetpairs, string filename);

	int AvgSimpleSim(int targetpairs, int times, string filename);

	int AvgSim(int targetpairs, double * avgtime, double * avgmemtime);
	
	int AvgFidSweep(int targetpairs, double from, double to, double step, string filename);

	int AvgLengthSweep(int targetpairs, double from, double to, double step, string filename);

	int MultiAvgFidSweep(int targetpairs, double from, double to, double step, string filename, int simthreads);

	int AvgSimMFid(int targetpairs, double * avgtime, double * avgmemtime, double actfid, int* progress);

	int AvgSimMEPR(int targetpairs, double * avgtime, double * avgmemtime, double rate, int * progress);

	int MultiAvgEPRSweep(int targetpairs, double from, double to, double step, string filename, int simthreads);


	
};

template <class T1, class T2> class TaskThread
{
public:
	thread thread;
	packaged_task<T1> task;
	future<T2> future;
	//konst
	TaskThread();
	//dest
	~TaskThread();

};


int meminfo(QMem * mem, int memsize);

template<class T1, class T2>
inline TaskThread<T1, T2>::TaskThread()
{
}

template<class T1, class T2>
inline TaskThread<T1, T2>::~TaskThread()
{
}
