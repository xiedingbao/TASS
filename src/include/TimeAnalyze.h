#ifndef _TIMEANALYZE_H
#define _TIMEANALYZE_H
#include "HMSC.h"
#include "z3++.h"
#include "PathCheck.h"
extern int inputbound;
extern bool asynchronous;

class TimeAnalyze{
	HMSC *msc;
	z3::context c;
	z3::solver s;
	int bound;
	void encode_graph(Node *target_node);
	z3::expr node_var(Node *node, int loop);
	std::vector<unsigned> decode_path();
	void decode_name(string name,int& ID,int&loop);
	std::string path_str(vector<unsigned> path);
	void block_path(std::vector<unsigned> path, int start, int end);

public:
	TimeAnalyze(HMSC *msc);
	bool reach_check(Node *target_node);
};


#endif
