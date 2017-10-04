//#pragma once
//#include <gecode/search.hh>
//#include <sstream>
//#include <fstream>
//#include <string>
//#include "XCSP3PrintCallbacks.h"
//#include "BuildGModel.h"
//#include "BMFileParser.h"
//#include "Timer.h"
//#include "SAC1.h"
//#include "CPUSolver.h"
//using namespace cudacp;
//using namespace Gecode;
//using namespace std;
////#define LOGFILE
//
//const string XPath = "BMPath.xml";
//
//int main() {
//	string bm_path;
//	if (FindBMPath(XPath))
//		bm_path = _bm_path;
//	cout << bm_path << endl;
//	HModel *hm = new HModel();
//	GetHModel(bm_path, hm);
//	GModel* gm = new GModel();
//	BuildGModel(hm, gm);
//
//	GModel* dgm = (GModel*)gm->clone();
//	branch(*dgm, dgm->vars_, INT_VAR_SIZE_MIN(), INT_VALUES_MIN());
//	DFS<GModel> ee(dgm);
//	delete dgm;
//
//	Timer t0;
//	if (GModel* ss = ee.next()) {
//		ss->print();
//		cout << "nodes = " << ee.statistics().node << endl;
//		delete ss;
//	}
//	int64_t gecode_solve_time = t0.elapsed();
//
//	SAC1 sac1(gm);
//	bool result;
//	Timer t;
//	result = sac1.enforce();
//	int64_t sac_time = t.elapsed();
//
//	if (!result) {
//		cout << "UNSAC || SAC time = " << sac_time << endl;
//		cout << "--------------------end---------------------" << endl;
//		delete hm;
//		delete gm;
//		return 0;
//	}
//
//	SearchStatistics statistics;
//	statistics = StartSearch(hm, gm, Heuristic::VRH_DOM, Heuristic::VLH_MIN);
//	const string  slv_str = (statistics.num_sol > 0) ? "SAT!!" : "UNSAT";
//	delete hm;
//	delete gm;
//	cout << "---------------gecode solving---------------" << endl;
//	cout << "Gecode solve time = " << gecode_solve_time << endl;
//	cout << "------------------modeling------------------" << endl;
//	cout << "SAC time = " << sac_time << "|| Build time = " << statistics.build_time << endl;
//	cout << "------------------solving-------------------" << endl;
//	cout << slv_str << "|| Solve time = " << statistics.solve_time << "|| nodes = " << statistics.nodes << endl;
//	cout << "------------------sleeping------------------" << endl;
//
//	return 0;
//}
//
