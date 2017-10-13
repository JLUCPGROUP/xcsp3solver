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

bool SAC1::enforce() const {
	bool modified = false;
	SpaceStatus status = model->status();

	if (status == SS_FAILED)
		return false;

	do {
		modified = false;
		for (int i = 0; i < model->vars_.size(); i++) {
			IntVar v = model->vars_[i];
			Int::IntView view(v);
			//for (IntVarValues k(view); k();++k) {
			for (int j = view.min(); j <= view.max(); ++j) {
				if (view.in(j)) {
					GModel *s = static_cast<GModel*>(model->clone());
					Int::IntView sv(s->vars_[i]);
					sv.eq(*s, j);
					status = s->status();
					if (status == SS_FAILED) {
						view.nq(*model, j);
						status = model->status();
						if (status == SS_FAILED)
							return false;

						modified = true;
					}
					delete s;
				}
			}
		}
	} while (modified);

	return true;
}
//
//bool SAC1::enforce() const {
//	bool modified = false;
//	SpaceStatus status = model->status();
//
//	if (status == SS_FAILED)
//		return false;
//
//	do {
//		modified = false;
//		for (int i = 0; i < model->vars_.size(); i++) {
//			IntVar v = model->vars_[i];
//
//			for (int j = v.min(); j <= v.max(); ++j) {
//				if (v.in(j)) {
//					GModel *s = static_cast<GModel*>(model->clone());
//					rel(*s, s->vars_[i] == j);
//					status = s->status();
//					if (status == SS_FAILED) {
//						rel(*model, v != j);
//						status = model->status();
//						if (status == SS_FAILED) 
//							return false;
//
//						modified = true;
//					}
//					delete s;
//				}
//			}
//		}
//	} while (modified);
//
//	return true;
//}


//bool SAC1::enforce() const
//{
//	bool modified = false;
//	SpaceStatus status = model->status();
//	if (status == SS_FAILED)
//		return false;
//	do {
//		modified = false;
//		for (size_t i = 0; i < model->vars_.size(); i++) {
//			IntVar v = model->vars_[i];
//			for (IntVarValues j(v); j();) {
//				GModel *s = static_cast<GModel*>(model->clone());
//				const int a = j.val();
//				rel(*s, s->vars_[i] == a);
//				status = s->status();
//				//s->print();
//				if (status == SS_FAILED) {
//					printf("(%d, %d) not sat sac!\n", i, a);
//					rel(*model, v != a);
//					status = model->status();
//					if (status == SS_FAILED) {
//						printf("failed\n");
//						return false;
//					}
//					modified = true;
//				}
//				else {
//					++j;
//				}
//				delete s;
//			}
//		}
//
//	} while (modified);
//
//	return true;
//}

} /* namespace cudacp */
