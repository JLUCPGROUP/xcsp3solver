//#pragma once
//#include <gecode/search.hh>
//#include <sstream>
//#include <fstream>
//#include <string>
//#include "XCSP3PrintCallbacks.h"
//#include "BuildGModel.h"
//#include "BMFileParser.h"
//#include "SAC1.h"
//#include "CPUSolver.h"
////#include "Timer.h"
//#include <windows.h>
//using namespace cudacp;
//using namespace Gecode;
//using namespace std;
//#define LOGFILE
//
//const string XPath = "BMPath.xml";
//
//int main() {
//	DWORD t1, t2;
//	const string bmp_root = "E:/Projects/benchmarks/xcsp/";
//	const string bmp_folder[3] = {
//		"Random-D-t065/rand-2-40-40-135-065-",
//		"Random-D-t080/rand-2-40-80-103-080-",
//		"Random-D-t090/rand-2-40-180-84-090-"
//	};
//	const string bmp_ext = ".xml";
//
//	for (size_t i = 1; i < 3; i++) {
//#ifdef LOGFILE
//		ofstream lofi;
//		const string bm_res = bmp_root + "res/" + bmp_folder[i].substr(0, bmp_folder[0].find("/")) + "bi.txt";
//		lofi.open(bm_res, ios::out | ios::app);
//		cout << bm_res << endl;
//		if (!lofi.is_open())
//			return 0;
//#endif
//		double sum_but = 0;
//		double sum_sact = 0;
//		double sum_bat = 0;
//		double sum_st = 0;
//		double sum_nod = 0;
//		for (size_t j = 0; j < 100; j++) {
//			char num[3];
//			sprintf_s(num, "%02d", j);
//			const string bm_path = bmp_root + bmp_folder[i] + num + bmp_ext;
//			//string bm_path;
//			//if (FindBMPath(XPath))
//			//	bm_path = _bm_path;
//			//cout << bm_path << endl;
//			//const string num = tostr<int>(bmp_idx[i] + j);
//			//const string bm_path = bmp_root + bmp_folder[i] + num + bmp_ext;
//			cout << bm_path << endl;
//#ifdef LOGFILE
//			lofi << bm_path << endl;
//#endif
//			HModel *hm = new HModel();
//			GetHModel(bm_path, hm);
//			GModel* gm = new GModel();
//			BuildGModel(hm, gm);
//
//			GModel* dgm = (GModel*)gm->clone();
//			branch(*dgm, dgm->vars_, INT_VAR_SIZE_MIN(), INT_VALUES_MIN());
//			DFS<GModel> ee(dgm);
//			delete dgm;
//
//			cout << "---------------gecode solving---------------" << endl;
//			Timer t0;
//			if (GModel* ss = ee.next()) {
//				ss->print();
//				cout << "nodes = " << ee.statistics().node << endl;
//				delete ss;
//			}
//			int64_t gecode_solve_time = t0.elapsed();
//
////			SAC1 sac1(gm);
////			bool result;
////			Timer t;
////			result = sac1.enforce();
////			int64_t sac_time = t.elapsed();
////
////			if (!result) {
////				cout << "UNSAC || SAC time = " << sac_time << endl;
////				cout << "--------------------end---------------------" << endl;
////#ifdef LOGFILE
////				lofi << "Gecode solve time = " << gecode_solve_time << endl;
////#endif
////				delete hm;
////				delete gm;
////				break;
////			}
////
////			SearchStatistics statistics;
////			statistics = StartSearch(hm, gm, Heuristic::VRH_DOM, Heuristic::VLH_MIN);
////			const string  slv_str = (statistics.num_sol > 0) ? "SAT!!" : "UNSAT";
////			delete hm;
////			delete gm;
//
//			sum_bat += gecode_solve_time;
//			//sum_sact += sac_time;
//			//sum_but += statistics.build_time;
//			//sum_st += statistics.solve_time;
//			//sum_nod += statistics.nodes;
//
//			cout << "Gecode solve time = " << gecode_solve_time << endl;
//			cout << "------------------modeling------------------" << endl;
//			//cout << "SAC time = " << sac_time << "|| Build time = " << statistics.build_time << endl;
//			//cout << "------------------solving-------------------" << endl;
//			//cout << slv_str << "|| Solve time = " << statistics.solve_time << "|| nodes = " << statistics.nodes << endl;
//			//cout << "------------------sleeping------------------" << endl;
//#ifdef LOGFILE
//			lofi << "---------------gecode solving---------------" << endl;
//			lofi << "Gecode solve time = " << gecode_solve_time << endl;
//			//lofi << "------------------modeling------------------" << endl;
//			//lofi << "SAC time = " << sac_time << "|| Build time = " << statistics.build_time << endl;
//			//lofi << "----------------solving--------------------" << endl;
//			//lofi << slv_str << "|| Solve time = " << statistics.solve_time << "|| nodes = " << statistics.nodes << endl;
//			//lofi << "----------------sleeping--------------------" << endl;
//#endif
//			Sleep(1000);
//		}
//		cout << "---------------avg---------------" << endl;
//		cout << "Gecode solve time = " << sum_bat / 100 << endl;
//			//"|| SAC time = " << sum_sact / 100 <<
//			//"|| Build time = " << sum_but / 100 <<
//			//"|| Solve time =" << sum_st / 100 <<
//			//"|| nodes = " << sum_nod / 100 << endl;
//#ifdef LOGFILE
//		lofi << "---------------avg---------------" << endl;
//		lofi << "Gecode solve time = " << sum_bat / 100 << endl;
//			//"|| SAC time = " << sum_sact / 100 <<
//			//"|| Build time = " << sum_but / 100 <<
//			//"|| Solve time =" << sum_st / 100 <<
//			//"|| nodes = " << sum_nod / 100 << endl;
//
//		lofi.close();
//#endif
//	}
//	return 0;
//}
//
