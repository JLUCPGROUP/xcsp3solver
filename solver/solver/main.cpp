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
////#define LOGFILE
//
//const string XPath = "BMPath.xml";
//
//int main() {
//	DWORD t1, t2;
//	const string bmp_root = "E:/Projects/benchmarks/xcsp/";
//	const string bmp_folder[4] = {
//		"Random-D-t010/rand-2-40-8-753-010-",
//		"Random-D-t020/rand-2-40-11-414-020-",
//		"Random-D-t035/rand-2-40-16-250-035-",
//		"Random-D-t050/rand-2-40-25-180-050-"
//	};
//	const string bmp_ext = ".xml";
//
//	for (size_t i = 2; i < 3; i++) {
//#ifdef LOGFILE
//		ofstream lofi;
//		const string bm_res = bmp_root + "res/" + bmp_folder[i].substr(0, bmp_folder[0].find("/")) + "bi.txt";
//		lofi.open(bm_res, ios::out | ios::app);
//		cout << bm_res << endl;
//		if (!lofi.is_open())
//			return 0;
//#endif
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
//			//GModel* dgm = (GModel*)gm->clone();
//			//branch(*dgm, dgm->vars_, INT_VAR_SIZE_MIN(), INT_VALUES_MIN());
//			//DFS<GModel> ee(dgm);
//			//delete dgm;
//
//			//t1 = GetTickCount();
//			//if (GModel* ss = ee.next()) {
//			//	ss->print();
//			//	cout << "nodes = " << ee.statistics().node << endl;
//			//	delete ss;
//			//}
//			//t2 = GetTickCount();
//			//DWORD gecode_solve_time = t2 - t1;
//
//			SAC1 sac1(gm);
//			bool result;
//			t1 = GetTickCount();
//			result = sac1.enforce();
//			t2 = GetTickCount();
//			DWORD sac_time = t2 - t1;
//			if (!result) {
//				cout << "UNSAC || SAC time = " << sac_time << endl;
//				cout << "--------------------end---------------------" << endl;
//				return 0;
//			}
//			gm->print();
//
//			t1 = GetTickCount();
//			CPUSolver s(hm, gm);
//			t2 = GetTickCount();
//			DWORD build_time = t2 - t1;
//			SearchStatistics statistics;
//			//s.n_.bm_.Show();
//
//			t1 = GetTickCount();
//			statistics = s.MAC();
//			t2 = GetTickCount();
//			DWORD solve_time = t2 - t1;
//			const string  slv_str = (statistics.num_sol > 0) ? "SAT!!" : "UNSAT";
//
//			delete hm;
//			delete gm;
//			cout << "---------------gecode solving---------------" << endl;
//			cout << "Gecode solve time = " << gecode_solve_time << endl;
//			cout << "------------------modeling------------------" << endl;
//			cout << "SAC time = " << sac_time << "|| Build time = " << build_time << endl;
//			cout << "------------------solving-------------------" << endl;
//			cout << slv_str << "|| Solve time = " << solve_time << "|| nodes = " << statistics.nodes << endl;
//			cout << "------------------sleeping------------------" << endl;
//#ifdef LOGFILE
//			lofi << "---------------gecode solving---------------" << endl;
//			lofi << "Gecode solve time = " << gecode_solve_time << endl;
//			lofi << "------------------modeling------------------" << endl;
//			lofi << "SAC time = " << sac_time << "|| Build time = " << build_time << endl;
//			lofi << "----------------solving--------------------" << endl;
//			lofi << slv_str << "|| Solve time = " << solve_time << "|| nodes = " << statistics.nodes << endl;
//			lofi << "----------------sleeping--------------------" << endl;
//#endif
//			Sleep(1000);
//		}
//#ifdef LOGFILE
//		lofi.close();
//#endif
//	}
//	return 0;
//}
//
