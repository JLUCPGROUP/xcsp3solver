/*
 * SAC1.cpp
 *
 *  Created on: 2016年11月17日
 *      Author: leezear
 */

#include "SAC1.h"
using namespace Gecode;
namespace cudacp {
SAC1::SAC1(GModel* model) :
	model(model) {
	// TODO Auto-generated constructor stub

}

SAC1::~SAC1() {
	// TODO Auto-generated destructor stub
}

bool SAC1::enforce() {
	bool modified = false;
	SpaceStatus status;
	status = model->status();
	if (status == SS_FAILED)
		return false;
	do {
		modified = false;
		for (size_t i = 0; i < model->vars_.size(); i++) {
			IntVar v = model->vars_[i];

			for (IntVarValues j(v); j(); ++j) {
				GModel *s = (GModel*)model->clone();
				rel(*s, s->vars_[i] == j.val());
				status = s->status();
				//s->print();
				delete s;
				if (status == SS_FAILED) {
					//printf("(%d, %d) not sat sac!\n", i, j.val());
					rel(*model, v != j.val());
					status = model->status();
					if (status == SS_FAILED) {
						printf("failed\n");
						return false;
					}

					modified = true;
				}
				else {
					//printf("(%d, %d) is sac!\n", i, j.val());
				}
			}
		}

	} while (modified);

	return true;
}

} /* namespace cudacp */
