#include "CPUSolver.h"
#include "CPUSolver.h"
using namespace std;

namespace cudacp {
////////////////////////////////////////////////////////////////////////////
void BitModel::initial(HModel *hm, GModel* gm) {
	mds = hm->max_domain_size();
	vs_size = hm->vars.size();
	bsd.resize(vs_size, vector<vector<u32>>(mds, vector<u32>(vs_size, 0)));
	bd.resize(vs_size, 0);

	for (size_t i = 0; i < gm->vars_.size(); i++) {
		IntVar v = gm->vars_[i];
		for (IntVarValues j(v); j(); ++j) {
			bd[i] |= M1[j.val()];
			GModel *s = (GModel*)gm->clone();
			rel(*s, s->vars_[i] == j.val());
			s->status();
			for (size_t k = 0; k < s->vars_.size(); k++) {
				IntVar vv = s->vars_[k];
				for (IntVarValues l(vv); l(); ++l) {
					bsd[i][j.val()][k] |= M1[l.val()];
				}
			}
			delete s;
		}
	}
}
void BitModel::Show() {
	for (size_t i = 0; i < bsd.size(); i++) {
		for (size_t j = 0; j < bsd[i].size(); j++) {
			printf("[%2d][%2d]: ", i, j);
			for (size_t k = 0; k < bsd[i][j].size(); k++) {
				printf("%8x ", bsd[i][j][k]);
			}
			printf("\n");
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void AssignedStack::initial(HModel *m) {
	m_ = m;
	max_size_ = m->vars.size();
	vals_.resize(m->vars.size());
	asnd_.resize(m->vars.size(), false);
};

void AssignedStack::push(IntVal& v_a) {
	vals_[top_] = v_a;
	asnd_[v_a.v] = v_a.aop ? true : false;
	++top_;
};

const IntVal AssignedStack::pop() {
	--top_;
	asnd_[vals_[top_].v] = false;
	return vals_[top_];
}

IntVal AssignedStack::top() const { return vals_[top_]; };
int AssignedStack::size() const { return top_; }
int AssignedStack::capacity() const { return max_size_; }
bool AssignedStack::full() const { return top_ == max_size_; }
bool AssignedStack::empty() const { return top_ == 0; }
IntVal AssignedStack::operator[](const int i) const { return vals_[i]; };
IntVal AssignedStack::at(const int i) const { return vals_[i]; };
void AssignedStack::clear() { top_ = 0; };
bool AssignedStack::assiged(const int v) const { return asnd_[v]; };

ostream & operator<<(ostream & os, AssignedStack & I) {
	for (int i = 0; i < I.size(); ++i)
		os << I[i] << " ";
	return os;
}

ostream & operator<<(ostream & os, AssignedStack * I) {
	for (int i = 0; i < I->size(); ++i)
		os << I->at(i) << " ";
	return os;
}
//////////////////////////////////////////////////////////////////////////
const IntVal & IntVal::operator=(const IntVal & rhs) {
	v = rhs.v;
	a = rhs.a;
	aop = rhs.aop;
	return *this;
}

void IntVal::flop() {
	aop = !aop;
}

inline bool IntVal::operator==(const IntVal & rhs) {
	return (this == &rhs) || (v == rhs.v && a == rhs.a && aop == rhs.aop);
}

inline bool IntVal::operator!=(const IntVal & rhs) {
	return !((this == &rhs) || (v == rhs.v && a == rhs.a && aop == rhs.aop));
}

ostream & operator<<(ostream & os, IntVal & v_val) {
	const string s = (v_val.aop) ? " = " : " != ";
	os << "(" << v_val.v << s << v_val.a << ")";
	return os;
}
////////////////////////////////////////////////////////////////////////////

void NetworkStack::initial(HModel *hm, AssignedStack* I, GModel* gm) {
	hm_ = hm;
	I_ = I;
	//bm_ = bm;
	bm_.initial(hm, gm);
	vs_size_ = hm_->vars.size();
	mds_ = hm_->max_domain_size();
	//初始化中间变量
	r_ = bm_.bd;
	s_.resize(vs_size_ + 1, vector<u32>(vs_size_, 0));
	//将根节点与初始网络加入队列
	s_[0].assign(r_.begin(), r_.end());
	++top_;
}

SearchState NetworkStack::push_back(const IntVal & val) {
	return val.aop ? reduce_dom(val) : remove_value(val);
}

SearchState NetworkStack::reduce_dom(const IntVal & val) {
	int pre = I_->size();
	top_ = I_->size() + 1;
	////若当前网络已经删除将要赋值的val
	//所有变量都要做一次按位运算
	for (size_t i = 0; i < vs_size_; i++) {
		s_[pre][i] = s_[pre - 1][i] & bm_.bsd[val.v][val.a][i];
		//有变量域变空
		if (s_[pre][i] == 0)
			return S_FAILED;
	}
	return S_BRANCH;
}

SearchState NetworkStack::remove_value(const IntVal & val) {
	int pre = I_->size() + 1;
	top_ = I_->size() + 1;
	//--------------------制作r_---------------------------
	r_.assign(vs_size_, 0);
	//先删该点val
	s_[pre - 1][val.v] &= M0[val.a];
	if (s_[pre - 1][val.v] == 0)
		return S_FAILED;

	//遍历该变量中所的有值
	for (size_t i = 0; i < hm_->vars[val.v]->vals.size(); ++i)
		//若变量值已不存在，跳过
		if (s_[pre - 1][val.v] & M1[i])
			//生成subDom(x != a)
			for (size_t j = 0; j < vs_size_; ++j)
				r_[j] |= bm_.bsd[val.v][i][j];
	//--------------------制作r_---------------------------
	//所有变量都要做一次按位运算
	for (size_t i = 0; i < vs_size_; i++) {
		s_[pre][i] = r_[i] & s_[pre - 1][i];
		//有变量域变空
		if (r_[i] == 0)
			return S_FAILED;
	}
	//--------------------制作r_---------------------------

	//将AC的r_加入队列中
	//AC成功
	return S_BRANCH;
}

vector<u32>& NetworkStack::nt_back() {
	return s_[top_ - 1];
}

void NetworkStack::pop_back() {
	--top_;
}

int NetworkStack::size()const {
	return s_.size();
}

IntVal NetworkStack::selectIntVal(const VarHeuristic vrh, const ValHeuristic vlh /*= VLH_MIN*/) {
	int var = select_var(vrh);
	int val = select_val(var, vlh);
	return IntVal(var, val);
}

int NetworkStack::select_var(const VarHeuristic vrh) {
	if (vrh == VRH_DOM) {
		int smt = INT_MAX;
		int idx = 0;
		int cnt;
		for (size_t i = 0; i < vs_size_; ++i) {
			if (!I_->assiged(i)) {
				cnt = _mm_popcnt_u32(s_[top_ - 1][i]);
				if (cnt < smt) {
					smt = cnt;
					idx = i;
				}
			}
		}
		return idx;
	}
	else if (vrh == VRH_LEX) {
		return I_->size();
	}
}

int NetworkStack::select_val(const int var, const ValHeuristic vlh) {
	switch (vlh) {
	case VLH_MIN:
		return _32f1(s_[top_ - 1][var]);
	default:
		break;
	}
}

void NetworkStack::restore(const int p) {
	top_ = p;
}


////////////////////////////////////////////////////////////////////////
CPUSolver::CPUSolver(HModel *hm, GModel* gm) :hm_(hm), gm_(gm) {
	I.initial(hm_);
	n_.initial(hm_, &I, gm_);
}

SearchStatistics CPUSolver::MAC() {
	bool finished = false;
	IntVal val;
	SearchState state;
	SearchStatistics statistics;

	while (!finished) {
		val = n_.selectIntVal(VRH_DOM, VLH_MIN);
		++statistics.nodes;
		I.push(val);
		state = n_.push_back(val);

		if ((state == S_BRANCH) && I.full()) {
			std::cout << I << std::endl;
			statistics.n_deep = n_.size();
			++statistics.num_sol;
			return statistics;
		}

		while (!(state == S_BRANCH) && !I.empty()) {
			val = I.pop();
			val.flop();
			state = n_.push_back(val);
		}

		if (!(state == S_BRANCH))
			finished = true;
	}
	return statistics;
}
//////////////////////////////////////////////////////////////////////////

};

