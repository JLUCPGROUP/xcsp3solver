/*
 * GModel.h
 *
 *  Created on: 2017年9月27日
 *      Author: leezear
 */

#ifndef GMODEL_H_
#define GMODEL_H_
#include <iostream>
#include <gecode/int.hh>
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

#endif /* GMODEL_H_ */
