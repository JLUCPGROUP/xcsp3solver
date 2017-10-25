#pragma once
#include <deque>
#include <bitset>
#include "Timer.h"
#include "GModel.h"
using namespace std;
namespace cudacp {
typedef unsigned int u32;
const u32 M1[32] = {
	0x80000000, 0x40000000, 0x20000000, 0x10000000,
	0x08000000, 0x04000000, 0x02000000, 0x01000000,
	0x00800000, 0x00400000, 0x00200000, 0x00100000,
	0x00080000, 0x00040000, 0x00020000, 0x00010000,
	0x00008000, 0x00004000, 0x00002000, 0x00001000,
	0x00000800, 0x00000400, 0x00000200, 0x00000100,
	0x00000080, 0x00000040, 0x00000020, 0x00000010,
	0x00000008, 0x00000004, 0x00000002, 0x00000001
};

const u32 M0[32] = {
	0x7FFFFFFF, 0xBFFFFFFF, 0xDFFFFFFF, 0xEFFFFFFF,
	0xF7FFFFFF, 0xFBFFFFFF, 0xFDFFFFFF, 0xFEFFFFFF,
	0xFF7FFFFF, 0xFFBFFFFF, 0xFFDFFFFF, 0xFFEFFFFF,
	0xFFF7FFFF, 0xFFFBFFFF, 0xFFFDFFFF, 0xFFFEFFFF,
	0xFFFF7FFF, 0xFFFFBFFF, 0xFFFFDFFF, 0xFFFFEFFF,
	0xFFFFF7FF, 0xFFFFFBFF, 0xFFFFFDFF, 0xFFFFFEFF,
	0xFFFFFF7F, 0xFFFFFFBF, 0xFFFFFFDF, 0xFFFFFFEF,
	0xFFFFFFF7, 0xFFFFFFFB, 0xFFFFFFFD, 0xFFFFFFFE
};

//最大论域的int表示大小
enum ByteSize {
	u1to32,
	u33to64,
	u65to96,
	u97to128,
	u129to160,
	u161to192,
	u193to224,
	u225to256,
	gt257
};

enum SolutionNum {
	SN_ONE,
	SN_ALL
};

enum SearchMethod {
	SM_MAC,
	SM_DFS,
	SM_BAB,
	SM_BIDFS
};

enum SearchState {
	S_BRANCH
	, S_FAILED
	, S_SOLVED
	//,	S_BACKTRACK
};

namespace Heuristic {
enum Var {
	VRH_LEX,
	VRH_MIN_DOM,
	VRH_VWDEG
};

enum Val {
	VLH_MIN,
	VLH_MIN_DOM,
	VLH_MIN_INC,
	VLH_MAX_INC,
	VLH_VWDEG
};
};

namespace Limits {

}

struct SearchStatistics {
	int num_sol = 0;
	int nodes = 0;
	int64_t build_time = 0;
	int64_t solve_time = 0;
	bool time_out = false;
	//int n_deep = 0;
};

inline u32 _32f1(u32 v) {
	static const char msb_256_table[256] =
	{
		0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4,4, 4, 4, 4,4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	};
	int result = 0;

	if (v > 0xFFFF) {
		v >>= 16;
		result += 16;
	}
	if (v > 0xFF) {
		v >>= 8;
		result += 8;
	}

	return 31 - (result + msb_256_table[v]);

};

template<class T>
class BitSetModel {
public:
	BitSetModel() {};
	void initial(GModel* gm) {
		mds = gm->mds;
		vs_size = gm->vs.size();

		bsd.resize(vs_size, vector<vector<T>>(mds, vector<T>(vs_size)));
		bd.resize(vs_size, 0);
		for (size_t i = 0; i < gm->vs.size(); i++) {
			const IntVar v = gm->vs[i];
			for (IntVarValues j(v); j(); ++j) {
				//bd[i] |= M1[j.val()];
				bd[i][j.val()] = 1;
				GModel *s = static_cast<GModel*>(gm->clone());
				rel(*s, s->vs[i] == j.val());
				s->status();
				for (size_t k = 0; k < s->vs.size(); k++) {
					const IntVar vv = s->vs[k];
					for (IntVarValues l(vv); l(); ++l)
						bsd[i][j.val()][k][l.val()] = 1;
				}
				delete s;
			}
		}
	}
	virtual ~BitSetModel() {};
	vector<vector<vector<T>>> bsd;
	vector<T> bd;
	void Show() {
		cout << "----------------------bitDom----------------------" << endl;
		for (size_t i = 0; i < bd.size(); i++)
			cout << "bd[" << i << "]: " << bd[i] << endl;

		cout << "----------------------bitSubDom----------------------" << endl;
		for (size_t i = 0; i < bsd.size(); i++) {
			for (size_t j = 0; j < bsd[i].size(); j++) {
				printf("[%2d][%2d]: ", i, j);
				for (size_t k = 0; k < bsd[i][j].size(); k++) {
					cout << bsd[i][j][k] << " ";
				}
				cout << endl;
			}
		}
	}
	int mds;
	int vs_size;
protected:
	GModel *gm_;
};

class IntVal {
public:
	int v;
	int a;
	bool aop = true;
	IntVal() {};
	IntVal(const int v, const int a, const bool aop = true) :v(v), a(a), aop(aop) {};

	const IntVal& operator=(const IntVal& rhs);
	void flip();
	IntVal next() const;
	inline bool operator==(const IntVal& rhs);
	inline bool operator!=(const IntVal& rhs);
	friend std::ostream& operator<< (std::ostream &os, IntVal &v_val);
	~IntVal() {};
};

namespace SearchNode {
const IntVal RootNode = IntVal(-1, 0);
const IntVal Deadend = IntVal(-2, -1);
const IntVal NullNode = IntVal(-2, -2);
const IntVal OutLastBroNode = IntVal(-2, -3);
const IntVal OutLastNode = IntVal(-2, -4);
};

class AssignedStack {
public:
	AssignedStack() {};

	void initial(GModel* m);
	~AssignedStack() {};
	void push(IntVal& v_a);
	IntVal pop();
	IntVal top() const;
	int size() const;
	int capacity() const;
	bool full() const;
	bool empty() const;
	IntVal operator[](const int i) const;
	IntVal at(const int i) const;
	void clear();
	bool assiged(const int v) const;
	friend std::ostream& operator<< (std::ostream &os, AssignedStack &I);
	friend std::ostream& operator<< (std::ostream &os, AssignedStack* I);

protected:
	vector<IntVal> vals_;
	vector<bool> asnd_;
	int top_ = 0;
	int max_size_;
};

template< class T>
class NetworkStack {
public:
	NetworkStack() {};
	void initial(GModel* gm, AssignedStack* I) {
		gm_ = gm;
		I_ = I;
		bm_.initial(gm);
		vs_size_ = gm->vs.size();
		mds_ = gm->mds;
		r_ = bm_.bd;
		s_.resize(vs_size_ + 1, vector<T>(vs_size_, 0));
		s_[0].assign(r_.begin(), r_.end());
		++top_;
	}

	SearchState push_back(const IntVal& val) {
		return val.aop ? reduce_dom(val) : remove_value(val);
	}

	SearchState reduce_dom(const IntVal& val) {
		const int pre = I_->size();
		top_ = I_->size() + 1;
		for (size_t i = 0; i < vs_size_; i++) {
			s_[pre][i] = s_[pre - 1][i] & bm_.bsd[val.v][val.a][i];
			if (!s_[pre][i].any())
				return S_FAILED;
		}
		return S_BRANCH;
	}

	SearchState remove_value(const IntVal& val) {
		const int pre = I_->size() + 1;
		top_ = I_->size() + 1;
		r_.assign(vs_size_, 0);
		s_[pre - 1][val.v][val.a] = 0;
		if (!s_[pre - 1][val.v].any())
			return S_FAILED;

		for (size_t i = gm_->vs[val.v].min(); i <= gm_->vs[val.v].max(); ++i)
			if (s_[pre - 1][val.v].test(i))
				for (size_t j = 0; j < vs_size_; ++j)
					r_[j] |= bm_.bsd[val.v][i][j];

		for (size_t i = 0; i < vs_size_; ++i) {
			s_[pre][i] = r_[i] & s_[pre - 1][i];
			if (!s_[pre][i].any())
				return S_FAILED;
		}
		return S_BRANCH;
	}

	vector<T>& nt_back() {
		return s_[top_ - 1];
	}

	void pop_back() {
		--top_;
	}
	//bool empty();
	int size() const {
		return s_.size();
	}

	IntVal selectIntVal(const Heuristic::Var vrh, const Heuristic::Val vlh) {
		const int var = select_var(vrh);
		const int val = select_val(var, vlh);
		return IntVal(var, val);
	}

	int select_var(const Heuristic::Var vrh) {
		if (vrh == Heuristic::VRH_MIN_DOM) {
			int smt = INT_MAX;
			int idx = 0;
			for (size_t i = 0; i < vs_size_; ++i) {
				if (!I_->assiged(i)) {
					const int cnt = s_[top_ - 1][i].count();
					if (cnt < smt) {
						smt = cnt;
						idx = i;
					}
				}
			}
			return idx;
		}
		else if (vrh == Heuristic::VRH_LEX) {
			return I_->size();
		}
	}

	int select_val(const int var, const Heuristic::Val vlh) {
		switch (vlh) {
		case Heuristic::VLH_MIN:
			for (size_t i = 0; i < mds_; ++i)
				if (s_[top_ - 1][var].test(i))
					return i;
			break;
		case Heuristic::VLH_MIN_INC:
		{
			int min_delt = INT_MAX;
			int selected = 0;
			for (size_t i = 0; i < mds_; ++i) {
				int delete_nodes = 0;
				if (s_[top_ - 1][var].test(i)) {
					//统计应用该值(var, i)后网络会删多少个值
					for (size_t j = 0; j < vs_size_; ++j) {
						tmp_ = s_[top_ - 1][j] & bm_.bsd[var][i][j];
						delete_nodes += s_[top_ - 1][j].count() - tmp_.count();
					}
					if (min_delt > delete_nodes) {
						min_delt = delete_nodes;
						selected = i;
					}

				}

			}
			return selected;
		}
		case Heuristic::VLH_MAX_INC:
		{
			int max_delt = INT_MIN;
			int selected = 0;
			for (size_t i = 0; i < mds_; ++i) {
				int delete_nodes = 0;
				if (s_[top_ - 1][var].test(i)) {
					//统计应用该值(var, i)后网络会删多少个值
					for (size_t j = 0; j < vs_size_; ++j) {
						tmp_ = s_[top_ - 1][j] & bm_.bsd[var][i][j];
						delete_nodes += s_[top_ - 1][j].count() - tmp_.count();
					}
					if (max_delt < delete_nodes) {
						max_delt = delete_nodes;
						selected = i;
					}

				}

			}
			return selected;
		}
		break;
		case Heuristic::VLH_MIN_DOM:
		{
			int min_delt = INT_MAX;
			int selected = 0;
			for (size_t i = 0; i < mds_; ++i) {
				if (s_[top_ - 1][var].test(i)) {
					//统计应用该值(var, i)后网络会删多少个值
					for (size_t j = 0; j < vs_size_; ++j) {
						tmp_ = s_[top_ - 1][j] & bm_.bsd[var][i][j];

						if (tmp_.count() == 0)
							return i;

						if (min_delt > tmp_.count()) {
							min_delt = tmp_.count();
							selected = i;
						}
					}

				}

			}
			return selected;
		}
		break;
		default:
			break;
		}
	}

	void restore(const int p) {
		top_ = p;
	}

	~NetworkStack() {};
	BitSetModel<T> bm_;
private:
	GModel* gm_;
	//bit model
	//计算删值后网络bitDom的中间变量
	vector<T> r_;
	//赋值栈
	AssignedStack *I_;
	//网络栈
	vector<vector<T>> s_;
	T tmp_;
	int vs_size_;
	int mds_;
	int top_ = 0;
};

template<class T>
class CPUSolver {
public:
	AssignedStack I;
	CPUSolver(GModel* gm) :gm_(gm) {
		I.initial(gm_);
		n.initial(gm_, &I);
	}
	~CPUSolver() {};

	SearchStatistics MAC(const Heuristic::Var varh, const Heuristic::Val valh, const int64_t time_limit) {
		Timer t;
		bool finished = false;
		SearchStatistics statistics;

		while (!finished) {
			IntVal val = n.selectIntVal(varh, valh);

			if (t.elapsed() > time_limit) {
				cout << t.elapsed() << endl;
				statistics.time_out = true;
				return statistics;
			}

			I.push(val);
			++statistics.nodes;
			SearchState state = n.push_back(val);

			if ((state == S_BRANCH) && I.full()) {
				cout << I << endl;
				++statistics.num_sol;
				statistics.solve_time = t.elapsed();
				//state = S_FAILED;
				return statistics;
			}

			while (!(state == S_BRANCH) && !I.empty()) {
				val = I.pop();
				val.flip();
				//++statistics.nodes;
				state = n.push_back(val);
			}

			if (!(state == S_BRANCH))
				finished = true;
		}
		statistics.solve_time = t.elapsed();
		return statistics;
	}
	NetworkStack<T> n;

private:
	GModel* gm_;
};

static ByteSize GetBitSetSize(const int mds) {
	switch ((mds + 1) / 32) {
	case 0:
		return u1to32;
		break;
	case 1:
		return u33to64;
		break;
	case 2:
		return u65to96;
		break;
	case 3:
		return u97to128;
		break;
	case 4:
		return u129to160;
		break;
	case 5:
		return u161to192;
		break;
	case 6:
		return u193to224;
		break;
	case 7:
		return u225to256;
		break;
	default:
		return gt257;
		break;
	}
};

template<class T>
SearchStatistics binary_search(GModel *gm, Heuristic::Var varh, Heuristic::Val valh, const int64_t time_limit) {
	Timer t2;
	CPUSolver<T> s(gm);
	const int64_t build_time = t2.elapsed();
	SearchStatistics statistics = s.MAC(varh, valh, time_limit);
	statistics.build_time = build_time;
	return statistics;
}

static SearchStatistics StartSearch(GModel *gm, const Heuristic::Var varh, const Heuristic::Val valh, const int64_t time_limit) {
	switch (GetBitSetSize(gm->mds)) {
	case u1to32:
		return binary_search<bitset<32>>(gm, varh, valh, time_limit);
	case u33to64:
		return binary_search<bitset<64>>(gm, varh, valh, time_limit);
	case u65to96:
		return binary_search<bitset<96>>(gm, varh, valh, time_limit);
	case u97to128:
		return binary_search<bitset<128>>(gm, varh, valh, time_limit);
	case u129to160:
		return binary_search<bitset<160>>(gm, varh, valh, time_limit);
	case u161to192:
		return binary_search<bitset<192>>(gm, varh, valh, time_limit);
	case u193to224:
		return binary_search<bitset<224>>(gm, varh, valh, time_limit);
	case u225to256:
		return binary_search<bitset<256>>(gm, varh, valh, time_limit);
	default:
		cout << "out of limit!!" << endl;
		SearchStatistics s;
		s.build_time = -1;
		return s;
	}
}

}