#ifndef _PATHCHECK_H
#define _PATHCHECK_H
#include "HMSC.h"
#include "z3++.h"

class PathCheck{
	HMSC *msc;
	z3::context c;
	z3::solver s;
	z3::expr switch_op(z3::expr exp,Operator op,z3::expr val);
	void syn_encode(std::vector<unsigned> path);
	void general_encode(std::vector<unsigned> path);
	z3::expr time_var(string name, int loop);
	z3::expr constraint2expr(const Constraint& con, int loop);
	void add_communication(BMSC* bmsc, int loop);
public:
	PathCheck(HMSC* msc);
	bool path_check(std::vector<unsigned> path);	
};



#endif
