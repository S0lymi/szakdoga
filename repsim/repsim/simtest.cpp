#include "qrep.h"
#include "elements.h"
#include <random>
#include "Simulation.h"
#include <fstream>
#include "Testhelp.h"
#include <chrono>

int main()
{
	int threads = 8;
		Pow2Sim Sim;
		Sim.eprnumber = 4;
		Sim.dist = 50;
		Sim.chalength = 20;
		Sim.memsize = 20;
		Sim.epratonce = 5;
		Sim.targetfid = 0.98;
		Sim.std_epr->fidelity = 0.7;
		Sim.std_epr->rate = 10;

	//measure execution time
	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
	//Simulate first 100 pair,get average from 10 simulations
	Sim.dist = 150;
	Sim.eprnumber = 8;
	Sim.AvgSimpleSim(100, 10, "Simple8.txt");
	Sim.eprnumber = 4;
	Sim.AvgSimpleSim(100, 10, "Simple4.txt");

	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();

	int duration = chrono::duration_cast<chrono::seconds>(t2 - t1).count();

	cout << "execution time:" << duration / 3600 << " h " << (duration % 3600) / 60 << " m " << duration % 60 << " s " << endl;
	cout << '\a';


}

