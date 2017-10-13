#pragma once
#include <gecode/search.hh>
#include <sstream>
#include <fstream>
#include <string>
#include "XCSP3PrintCallbacks.h"
#include "BuildGModel.h"
#include "BMFileParser.h"
#include "SAC1.h"
#include "CPUSolver.h"
//#include "Timer.h"
#include <windows.h>
using namespace cudacp;
using namespace Gecode;
using namespace std;
#define LOGFILE

const string XPath = "BMPath.xml";
const int TimeLimit = 600000;

int main() {
	const string bmp_root = "E:/Projects/benchmarks/xcsp/";
	const string bmp_folder[3] = {
		"Random-D-t065/rand-2-40-40-135-065-",
		"Random-D-t080/rand-2-40-80-103-080-",
		"Random-D-t090/rand-2-40-180-84-090-"
	};
	const string bmp_ext = ".xml";

	for (size_t i = 1; i < 3; i++) {
#ifdef LOGFILE
		ofstream lofi;
		const string bm_res = bmp_root + "res/" + bmp_folder[i].substr(0, bmp_folder[0].find("/")) + "bi.txt";
		lofi.open(bm_res, ios::out | ios::app);
		cout << bm_res << endl;
		if (!lofi.is_open())
			return 0;
#endif
		double sum_but = 0;
		double sum_sact = 0;
		double sum_bat = 0;
		double sum_st = 0;
		double sum_nod = 0;
		int sol_cnt = 0;
		int num_sac = 0;
		for (size_t j = 0; j < 100; j++) {
			char num[3];
			sprintf_s(num, "%02d", j);
			const string bm_path = bmp_root + bmp_folder[i] + num + bmp_ext;
			cout << bm_path << endl;
#ifdef LOGFILE
			lofi << bm_path << endl;
#endif
			HModel *hm = new HModel();
			GetHModel(bm_path, hm);
			GModel* gm = new GModel();
			BuildGModel(hm, gm);

			Search::TimeStop ts(2000);
			Search::Options options;
			options.stop = &ts;
			GModel* dgm = static_cast<GModel*>(gm->clone());
			branch(*dgm, dgm->vars_, INT_VAR_SIZE_MIN(), INT_VALUES_MIN());
			DFS<GModel> ee(dgm, options);
			delete dgm;
			int find = -1;
			cout << "---------------gecode solving---------------" << endl;
#ifdef LOGFILE
			lofi << "---------------gecode solving---------------" << endl;
#endif
			Timer t0;
			//得到结果
			if (GModel* ss = ee.next()) {
				ss->print();
				find = 1;
				sol_cnt++;
				const int64_t gecode_solve_time = t0.elapsed();
				sum_bat += gecode_solve_time;
				cout << "nodes = " << ee.statistics().node << endl;
				cout << "Gecode solve time = " << gecode_solve_time << endl;
#ifdef LOGFILE
				lofi << "nodes = " << ee.statistics().node << endl;
				lofi << "Gecode solve time = " << gecode_solve_time << endl;
#endif
				delete ss;
			}
			else {
				//证明无解
				sol_cnt++;
				find = 0;
				const int64_t gecode_solve_time = t0.elapsed();
				sum_bat += gecode_solve_time;
				cout << "no solution!" << endl;
				cout << "nodes = " << ee.statistics().node << endl;
				cout << "Gecode solve time = " << gecode_solve_time << endl;
#ifdef LOGFILE
				lofi << "no solution!" << endl;
				lofi << "nodes = " << ee.statistics().node << endl;
				lofi << "Gecode solve time = " << gecode_solve_time << endl;
#endif
			}


			//计算超时
			if (find == -1) {
				cout << "out of time!" << endl;
#ifdef LOGFILE
				lofi << "out of time!" << endl;
#endif
			}

			////////////////////////////////////////////////////////////////////////
			cout << "------------------modeling------------------" << endl;
#ifdef LOGFILE
			lofi << "------------------modeling------------------" << endl;
#endif
			SAC1 sac1(gm);
			Timer t;
			const bool result = sac1.enforce();
			const int64_t sac_time = t.elapsed();

			if (!result) {
				cout << "UNSAC || SAC time = " << sac_time << endl;
				cout << "--------------------end---------------------" << endl;
#ifdef LOGFILE
				lofi << "UNSAC || SAC time = " << sac_time << endl;
				lofi << "--------------------end---------------------" << endl;
#endif
				delete hm;
				delete gm;
				continue;
			}

			const SearchStatistics statistics = StartSearch(hm, gm, Heuristic::VRH_DOM, Heuristic::VLH_MIN);
			const string  slv_str = (statistics.num_sol > 0) ? "SAT!!" : "UNSAT";
			delete hm;
			delete gm;


			sum_st += statistics.solve_time;
			sum_sact += sac_time;
			sum_but += statistics.build_time;
			sum_st += statistics.solve_time;
			sum_nod += statistics.nodes;

			cout << "SAC time = " << sac_time << "|| Build time = " << statistics.build_time << endl;
			cout << "------------------solving-------------------" << endl;
			cout << slv_str << "|| Solve time = " << statistics.solve_time << "|| nodes = " << statistics.nodes << endl;
			cout << "------------------sleeping------------------" << endl;
#ifdef LOGFILE
			lofi << "SAC time = " << sac_time << "|| Build time = " << statistics.build_time << endl;
			lofi << "----------------solving--------------------" << endl;
			lofi << slv_str << "|| Solve time = " << statistics.solve_time << "|| nodes = " << statistics.nodes << endl;
			lofi << "----------------sleeping--------------------" << endl;
#endif
			Sleep(1000);
		}
		cout << "---------------sum---------------" << endl;
		cout << "Gecode solve time = " << sum_bat / sol_cnt <<
			"|| time out = " << 100 - sol_cnt <<
			"|| SAC time = " << sum_sact / 100 <<
			"|| Build time = " << sum_but / 100 <<
			"|| Solve time =" << sum_st / 100 <<
			"|| nodes = " << sum_nod / 100 << endl;
#ifdef LOGFILE
		lofi << "---------------sum---------------" << endl;
		lofi << "Gecode solve time = " << sum_bat / sol_cnt <<
			"|| time out = " << 100 - sol_cnt <<
			"|| SAC time = " << sum_sact / 100 <<
			"|| Build time = " << sum_but / 100 <<
			"|| Solve time =" << sum_st / 100 <<
			"|| nodes = " << sum_nod / 100 << endl;

		lofi.close();
#endif
	}
	return 0;
}

