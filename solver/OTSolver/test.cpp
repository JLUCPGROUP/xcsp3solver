#pragma once
#include <sstream>
#include <fstream>
#include <string>
#include "XCSP3PrintCallbacks.h"
#include "BMFileParser.h"
#include "Timer.h"
#include "ortools/constraint_solver/constraint_solveri.h"
#include "ortools/base/logging.h"
#include "ortools/util/tuple_set.h"
#include <complex>
//#include <windows.h>
using namespace cudacp;
using namespace std;
using namespace operations_research;
//#define LOGFILE

const string XPath = "BMPath.xml";

class MiddleVariableIndexSelector :public Solver::IndexEvaluator1 {
public:
	MiddleVariableIndexSelector(const int64 n) :n_(n), mid_((n - 1) / 2) {}
	~MiddleVariableIndexSelector() {}
	int64 Run(int64 index) const
	{
		return abs(mid_ - n_);
	}
private:
	const int64 n_;
	const int64 mid_;
};

int main() {
	string bm_path;
	if (FindBMPath(XPath))
		bm_path = _bm_path;
	cout << bm_path << endl;
	HModel *hm = new HModel();
	GetHModel(bm_path, hm);
	//hm->Show();
	//////////////////////////////////////////////////////////////////////////
	Solver s("CPSimple");
	vector<IntVar*> vars(hm->vars.size());

	for (auto v : hm->vars)
		vars[v->id] = s.MakeIntVar(v->vals, v->name);

	for (auto t : hm->tabs) {
		vector<IntVar*> vs;
		for (auto v : t->scope)
			vs.push_back(vars[v->id]);
		IntTupleSet ts(t->scope.size());
		ts.InsertAll(t->tuples);
		s.AddConstraint(s.MakeAllowedAssignments(vs, ts));
		ts.Clear();
	}
	cout << "-------------------------solve-------------------------" << endl;
	//vector<SearchMonitor*> monitors;
	//MiddleVariableIndexSelector *misel = new MiddleVariableIndexSelector(s);
	DecisionBuilder* const db = s.MakePhase(vars, Solver::CHOOSE_MIN_SIZE, Solver::ASSIGN_MIN_VALUE);
	s.NewSearch(db);
	Timer t;
	//s.AddPropagationMonitor()
	if (s.NextSolution())
		for (auto v : vars) {
			cout << v->name() << " = " << v->Value() << " ";
		}

	s.EndSearch();
	cout << endl;
	const int64_t solve_time = t.elapsed();
	delete hm;
	cout << "solve time = " << solve_time << endl;
	cout << "--------------------------end--------------------------" << endl;
	return 0;
}