/*
 * GModel.cpp
 *
 *  Created on: 2017年9月27日
 *      Author: leezear
 */

#include "GModel.h"

namespace cudacp {

GModel::GModel() {
	// TODO Auto-generated constructor stub

}

GModel::GModel(bool share, GModel& s) :
		Space(share, s) {
	vars_.update(*this, share, s.vars_);
}

Space* GModel::copy(bool share) {
	return new GModel(share, *this);
}

void GModel::print(void) const {
	for (size_t i = 0; i < vars_.size(); i++) {
		cout << "(" << i << " = " << vars_[i] << ") ";
	}
	cout << endl;
}

GModel::~GModel() {
	// TODO Auto-generated destructor stub
}

} /* namespace cudacp */
