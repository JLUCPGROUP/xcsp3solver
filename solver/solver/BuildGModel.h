/*
* BuildGecodeModel.h
*
*  Created on: 2016年9月17日
*      Author: leezear
*/

#pragma once

#include "HModel.h"
#include "GModel.h"
using namespace Gecode;
enum PROP {
	PROP_BINARY,  ///< Use only binary disevsuality constraints
	PROP_MIXED,   ///< Use single distinct and binary disevsuality constraints
	PROP_DISTINCT ///< Use three distinct constraints
};
namespace cudacp {
static void BuildGModel(const HModel* h_model, GModel* gm) {
	gm->vs = IntVarArray(*gm, h_model->vars.size());
	gm->mds = h_model->max_domain_size();
	for (size_t i = 0; i < h_model->vars.size(); ++i) {
		HVar* v = h_model->vars[i];
		gm->vs[i] = IntVar(*gm, IntSet(&*(v->vals.begin()), v->vals.size()));
	}

	for (size_t i = 0; i < h_model->tabs.size(); ++i) {
		HTab* tab = h_model->tabs[i];
		TupleSet ts;
		const int size = tab->tuples.size();
		const int arity = tab->scope.size();
		for (int j = 0; j < size; ++j)
			ts.add(IntArgs(arity, &*(tab->tuples[j].begin())));
		ts.finalize();

		IntVarArgs scope;
		for (int j = 0; j < arity; ++j)
			scope << gm->vs[tab->scope[j]->id];
		extensional(*gm, scope, ts);
	}
};

static void BuildQueens(GModel* gm, const int n, const PROP p = PROP_BINARY, const IntPropLevel ipl = IPL_DEF) {
	gm->vs = IntVarArray(*gm, n, 0, n - 1);
	gm->mds = n;
	switch (p) {
	case PROP_BINARY:
		for (int i = 0; i < n; i++)
			for (int j = i + 1; j < n; j++) {
				rel(*gm, gm->vs[i] != gm->vs[j]);
				rel(*gm, gm->vs[i] + i != gm->vs[j] + j);
				rel(*gm, gm->vs[i] - i != gm->vs[j] - j);
			}
		break;
	case PROP_MIXED:
		for (int i = 0; i < n; i++)
			for (int j = i + 1; j < n; j++) {
				rel(*gm, gm->vs[i] + i != gm->vs[j] + j);
				rel(*gm, gm->vs[i] - i != gm->vs[j] - j);
			}
		distinct(*gm, gm->vs, ipl);
		break;
	case PROP_DISTINCT:
		distinct(*gm, IntArgs::create(n, 0, 1), gm->vs, ipl);
		distinct(*gm, IntArgs::create(n, 0, -1), gm->vs, ipl);
		distinct(*gm, gm->vs, ipl);
		break;
	}
};
}
