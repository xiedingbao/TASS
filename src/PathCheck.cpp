#include "PathCheck.h"
using namespace z3;


PathCheck::PathCheck(HMSC* msc):msc(msc),s(c),path_start(0),path_end(0){
	params p(c);
	p.set(":unsat-core", true);
	p.set(":auto-config",false);
	s.set(p);
}

/* check reachability of one path */
bool PathCheck::path_check(vector<unsigned> path){
	s.reset();
	syn_encode(path);
	cout<<s<<endl;
	if(s.check() == sat)
	  return true;
	expr_vector core = s.unsat_core();
	analyze_unsat_core(core);
	return false;
}
/* analyze the unsat core to extract the infeasible path segment */
void PathCheck::analyze_unsat_core(expr_vector& core){
	assert(core.size()!=0);
	int from = INT_MAX,to=0; 	
	for (unsigned i=0; i< core.size(); i++){
		int small,max;
		func_decl v=core[i].decl();
		get_constraint_index(v.name().str(), small , max);
		if(from > small)
			from = small;
		if(to < max)
			to = max;
	}
	path_start = from;
	path_end = to;
}
void PathCheck::get_IIS_path(int& start,int& end){
	start = path_start;
	end = path_end;
}

/* synchronous concatenation  */
void PathCheck::syn_encode(vector<unsigned> path){
	general_encode(path);
	//synchronous condition
	for(unsigned i=1;i<path.size()-2;i++){
		BMSC* current=msc->node_list[path[i]].ref;
		BMSC* next=msc->node_list[path[i+1]].ref;
		assert(current!=NULL && next!=NULL);
		expr current_end=time_var(current->event_list.back().name,i);
		for(unsigned j=0;j<next->event_list.size();j++)
		  s.add(current_end<=time_var(next->event_list[j].name,i+1),con_name(i,i+1).c_str());
	}

}


void PathCheck::general_encode(vector<unsigned> path){
	//timing constraint of every basic msc in the path   
	for(unsigned i=1;i<path.size()-1;i++){
		BMSC *bmsc=msc->node_list[path[i]].ref;
		if(i == 1){  //occurence time of the first event equals to 0
			s.add(time_var(bmsc->event_list[0].name,i)==0, con_name(1,1).c_str());
		}
		//timeing specification of basic msc
		for(unsigned j=0;j<bmsc->constraints.size();j++)
		  s.add(constraint2expr(bmsc->constraints[j], i),con_name(i,i).c_str());
		//communication time
		add_communication(bmsc, i);
		//add order pair
		for(unsigned j=0;j<bmsc->order_list.size();j++){
			OrderPair order=bmsc->order_list[j];
			s.add(time_var(order.predecessor,i)<=time_var(order.successor,i), con_name(i,i).c_str());
		}
	}
	//add global timing specification
	for(unsigned i=0;i<msc->constraints.size();i++){
		Constraint con=msc->constraints[i];
		string later=con.pvlist[0].getName();	
		string former=con.pvlist[1].getName();	
		int lastindex=0;
		for(unsigned j=1;j<path.size()-1;j++){
			if(msc->node_list[path[j]].contain_event(later)){
				for(unsigned k=lastindex+1;k<j;k++){
					if(msc->node_list[path[k]].contain_event(former)){
						expr exp=time_var(later,j)-time_var(former,k);
						s.add(switch_op(exp, con.op, c.real_val(con.value.c_str())), con_name(k,j).c_str());
						break;
					}
				}
				lastindex=j;
			}
		}
	}	
}

expr PathCheck::switch_op(expr exp,Operator op,expr val){
	switch(op){
		case LT:exp=exp<val;break;
		case LE:exp=exp<=val;break;
		case GT:exp=exp>val;break;
		case GE:exp=exp>=val;break;
		case EQ:exp=exp==val;break;
	}
	return exp;
}

expr PathCheck::constraint2expr(const Constraint& con,int loop){
	expr exp(c);
	for(unsigned i=0;i<con.pvlist.size();i++){
		Variable var=con.pvlist[i];
		expr tmp=c.real_val(var.getCoefficient().c_str())*time_var(var.getName(), loop);
		if(i==0)
		  exp=tmp;
		else
		  exp=exp+tmp;
	}
	expr val=c.real_val(con.value.c_str());
	return switch_op(exp, con.op, val);
}

/* add timing constraints of communication time */
void PathCheck::add_communication(BMSC* bmsc,int loop){
	expr exp(c);
	for(unsigned i=0;i<msc->communicationTime.size();i++){
		Constraint con = msc->communicationTime[i];
		assert(con.pvlist.size() == 2);
		for(unsigned j=0;j<bmsc->message_list.size();j++){
			string sender = bmsc->message_list[j].sender_name;
			string receiver = bmsc->message_list[j].receiver_name;
			if(sender==con.pvlist[1].getName() && receiver==con.pvlist[0].getName()){
				exp=time_var(bmsc->message_list[j].receive_event, loop)-time_var(bmsc->message_list[j].send_event, loop);
				s.add(switch_op(exp, con.op, c.real_val(con.value.c_str())),con_name(loop,loop).c_str());
			}
		}
	}
}

expr PathCheck::time_var(string name, int loop){
	char tname[256];
	sprintf(tname,"%s_%i",name.c_str(),loop);
	return c.real_const(tname);
}
string PathCheck::con_name(unsigned start, unsigned end){
	char ch[256];
	sprintf(ch,"%d_%d",start,end);
	return string(ch);
}
void PathCheck::get_constraint_index(string name,int& small,int& max){
	char *ptr=(char *)malloc(strlen(name.c_str()));
	strcpy(ptr,name.c_str());
	char *pch = strtok(ptr,"_");
	small = atoi(pch);
	max = atoi(strtok(NULL,"_"));
	free(ptr);
	assert(small!=0 && max!=0);
}

