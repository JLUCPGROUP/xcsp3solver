#include "CPUSolver.h"
using namespace std;

namespace cudacp {
////////////////////////////////////////////////////////////////////////////
//void AssignedStack::initial(HModel *m) {
//	m_ = m;
//	max_size_ = m->vars.size();
//	vals_.resize(m->vars.size());
//	asnd_.resize(m->vars.size(), false);
//};

void AssignedStack::initial(GModel* m) {
	max_size_ = m->vs.size();
	vals_.resize(m->vs.size());
	asnd_.resize(m->vs.size(), false);
};

void AssignedStack::push(IntVal& v_a) {
	vals_[top_] = v_a;
	asnd_[v_a.v] = v_a.aop ? true : false;
	++top_;
};

IntVal AssignedStack::pop() {
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

void IntVal::flip() {
	aop = !aop;
}

IntVal IntVal::next() const {
	return IntVal(v, a + 1, true);
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
}