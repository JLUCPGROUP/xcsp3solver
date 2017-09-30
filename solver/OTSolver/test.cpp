#pragma once
#include <sstream>
#include <fstream>
#include <string>
#include "XCSP3PrintCallbacks.h"
#include "BMFileParser.h"
#include "Timer.h"
#include "ortools/constraint_solver/constraint_solveri.h"
#include "ortools\base\logging.h"
#include "ortools\util\tuple_set.h"
#include <windows.h>
using namespace cudacp;
using namespace std;
using namespace operations_research;
//#define LOGFILE

const string XPath = "BMPath.xml";

int main() {
	DWORD t1, t2;
	string bm_path;
	if (FindBMPath(XPath))
		bm_path = _bm_path;
	cout << bm_path << endl;
	HModel *hm = new HModel();
	GetHModel(bm_path, hm);
	Solver s("CPSimple");
	vector<IntVar*> vars;
	for (auto v : hm->vars)
		vars.push_back(s.MakeIntVar(v->vals, v->name));

	for (auto c : hm->tabs) {
		vector<IntVar*> vs;
		for (auto v : c->scope)
			vs.push_back(vars[v->id]);
		IntTupleSet ts(c->scope.size());
		ts.InsertAll(c->tuples);
		s.AddConstraint(s.MakeAllowedAssignments(vs, ts));
	}

	delete hm;

	DecisionBuilder* const db = s.MakePhase(vars, Solver::CHOOSE_MIN_SIZE, Solver::ASSIGN_MIN_VALUE);
	s.Solve(db);

	if (s.NextSolution())
		cout << "find" << endl;
	//	for (auto v : vars)
	//		cout << v->name() << " = " << v->Value()<<" ";
	//cout << endl;
	return 0;
}

