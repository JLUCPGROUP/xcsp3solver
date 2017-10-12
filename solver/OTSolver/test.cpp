#pragma once
#include <sstream>
#include <string>

#include "ortools/base/join.h"
#include "XCSP3PrintCallbacks.h"
#include "BMFileParser.h"
#include "Timer.h"
#include "ortools/constraint_solver/constraint_solver.h"
#include "ortools/base/logging.h"
#include "ortools/util/tuple_set.h"
#include <glog/logging.h>

using namespace cudacp;
using namespace std;
using namespace operations_research;

//#define LOGFILE

const string XPath = "BMPath.xml";

////typedef ResultCallback1<int64, int64> IndexEvaluator1;
//
//class MiddleVariableIndexSelector : public Solver::IndexEvaluator1 {
//public:
//	MiddleVariableIndexSelector(const int64 n) : n_(n), middle_var_index_((n - 1) / 2) {}
//	~MiddleVariableIndexSelector() {}
//	int64 Run(const int64 index) {
//		return abs(middle_var_index_ - index);
//	}
//
//private:
//	const int64 n_;
//	const int64 middle_var_index_;
//};

enum FindSolution {
	Finding = 0,
	FindOne = 1,
	Non = -1
};

namespace {

class NQueensDecisionBuilder : public DecisionBuilder {
public:
	NQueensDecisionBuilder(const int size, const vector<IntVar*>& vars) :
		size_(size), vars_(vars), middle_var_index_((size - 1) / 2) {
		CHECK_EQ(vars_.size(), size_);
	}
	~NQueensDecisionBuilder() {}

	// Select a variable with the smallest domain starting from the center
	IntVar* SelectVar(Solver* const s) const {
		IntVar* selected_var = nullptr;
		int64 id = -1;
		int64 min_domain_size = kint64max;

		// go left on the chessboard
		for (int64 i = middle_var_index_; i >= 0; --i) {
			IntVar* const var = vars_[i];
			if (!var->Bound() && var->Size() < min_domain_size) {
				selected_var = var;
				id = i;
				min_domain_size = var->Size();
			}
		}

		// go right on the chessboard
		for (int64 i = middle_var_index_ + 1; i < size_; ++i) {
			IntVar* const var = vars_[i];
			if (!var->Bound() && var->Size() < min_domain_size) {
				selected_var = var;
				id = i;
				min_domain_size = var->Size();
			}
		}

		if (id == -1) {
			return nullptr;
		}
		else {
			return selected_var;
		}
	}

	int64 count_number_of_row_incompatibilities(const int64 row) const {
		int64 count = 0;
		for (int64 i = 0; i < size_; ++i) {
			if (!vars_[i]->Contains(row)) {
				++count;
			}
		}
		return count;
	}

	//  For a given variable, take the row with the least compatible columns, starting from the center
	int64 SelectValue(const IntVar* const v) const {
		CHECK_GE(v->Size(), 2);

		int64 max_incompatible_columns = -1;
		int64 selected_value = -1;

		const int64 vmin = v->Min();
		const int64 vmax = v->Max();
		const int64 v_middle = (vmin + vmax) / 2;

		for (int64 i = v_middle; i >= vmin; --i) {
			if (v->Contains(i)) {
				const int64 nbr_of_incompatibilities = count_number_of_row_incompatibilities(i);
				if (nbr_of_incompatibilities > max_incompatible_columns) {
					max_incompatible_columns = nbr_of_incompatibilities;
					selected_value = i;

				}
			}
		}

		for (int64 i = v_middle; i <= vmin; ++i) {
			if (v->Contains(i)) {
				const int64 nbr_of_incompatibilities = count_number_of_row_incompatibilities(i);
				if (nbr_of_incompatibilities > max_incompatible_columns) {
					max_incompatible_columns = nbr_of_incompatibilities;
					selected_value = i;

				}
			}
		}

		CHECK_NE(selected_value, -1);

		return selected_value;
	}

	Decision* Next(Solver* const s) override {
		IntVar* const var = SelectVar(s);
		if (nullptr != var) {
			const int64 value = SelectValue(var);
			return s->MakeAssignVariableValue(var, value);
		}

		return nullptr;
	}

private:
	const int size_;
	const vector<IntVar*> vars_;
	const int middle_var_index_;
}; //  class NQueensDecisionBuilder



}  // namespace

DecisionBuilder* MakeNQueensDecisionBuilder(Solver* const s, const int size, const vector<IntVar*>& vars) {
	return s->RevAlloc(new NQueensDecisionBuilder(size, vars));
}


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
	//SolutionCollector* const collector = s.MakeFirstSolutionCollector();
	//collector->Add(vars);
	//monitors.push_back(collector);
	//vector<SearchMonitor*> monitors;
	//DecisionBuilder* const db = MakeNQueensDecisionBuilder(&s, vars.size(), vars);
	//SolutionCollector* const solution_counter = s.MakeAllSolutionCollector(NULL);
	//monitors.push_back(solution_counter);
	//MiddleVariableIndexSelector * index_evaluator = new MiddleVariableIndexSelector(vars.size());
	//DecisionBuilder* const db = s.MakePhase(vars, Solver::CHOOSE_MIN_SIZE, Solver::ASSIGN_MIN_VALUE);
	//DecisionBuilder* const db = s.MakePhase(vars, index_evaluator, Solver::ASSIGN_CENTER_VALUE); //  ASSIGN_CENTER_VALUE, ASSIGN_MIN_VALUE
	DecisionBuilder* const db = s.MakePhase(vars, Solver::CHOOSE_MIN_SIZE, Solver::ASSIGN_MIN_VALUE);
	SearchLimit* limit = s.MakeTimeLimit(2000);
	s.NewSearch(db, limit);
	int64 solve_time = 0;
	FindSolution find = Finding;
	if (s.NextSolution()) {
		find = FindOne;
		solve_time = s.wall_time();
		for (auto v : vars)
			cout << v->name() << " = " << v->Value() << " ";
		cout << endl;
	}
	else {
		find = Non;
	}
	s.EndSearch();

	cout << "solve time = " << s.wall_time() << endl;
	delete hm;
	cout << "--------------------------end--------------------------" << endl;
	return 0;
};