#include <gecode/search.hh>
#include "XCSP3PrintCallbacks.h"
#include "BuildGModel.h"
#include "BMFileParser.h"
//#include "Timer.h"
using namespace cudacp;
using namespace Gecode;

const string XPath = "BMPath.xml";;

int main(int argc, char **argv) {
	HModel *hm = new HModel();
	string bm_path;
	if (FindBMPath(XPath))
		bm_path = _bm_path;
	cout << "bm_path: " << bm_path << endl;
	GetHModel(bm_path, hm);
	hm->Show();
	GModel* gm = new GModel();
	BuildGModel(hm, gm);
	delete hm;
	////	branch(*gm, gm->vars_, INT_VAR_SIZE_MIN(), INT_VALUES_MIN());
	branch(*gm, gm->vars_, INT_VAR_AFC_SIZE_MAX(0), INT_VAL_MIN());
	DFS<GModel> ee(gm);
	////	gm->status();
	////	gm->print();
	delete gm;
	//Timer t;
	if (GModel* ss = ee.next()) {
		ss->print();
		cout << "nodes = " << ee.statistics().node << endl;
		delete ss;
	}
	//int64_t search_time = t.elapsed();
	//cout << "search time = " << search_time << endl;
	cout << "--------------------end---------------------" << endl;
	return 0;
}

