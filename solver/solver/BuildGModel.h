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

namespace cudacp {
static void BuildGModel(const HModel* h_model, GModel* g_model) {
	g_model->vars_ = IntVarArray(*g_model, h_model->vars.size());

	for (size_t i = 0; i < h_model->vars.size(); ++i) {
		HVar* v = h_model->vars[i];
		g_model->vars_[i] = IntVar(*g_model, IntSet(&*(v->vals.begin()), v->vals.size()));
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
			scope << g_model->vars_[tab->scope[j]->id];
		extensional(*g_model, scope, ts);
	}
}
}
