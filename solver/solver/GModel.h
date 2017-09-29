/*
 * GModel.h
 *
 *  Created on: 2017年9月27日
 *      Author: leezear
 */

#pragma once
#include <iostream>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>
#include "HModel.h"

namespace cudacp {

using namespace Gecode;
using namespace std;
class GModel: public Space {
private:
public:
	IntVarArray vars_;
	GModel();
	GModel(bool share, GModel& s);
	virtual Space* copy(bool share);
	void print(void) const;
	virtual ~GModel();
};

} /* namespace cudacp */

