#include "PathCheck.h"
using namespace z3;


PathCheck::PathCheck(HMSC* msc):msc(msc),s(c){
	
}

bool PathCheck::path_check(vector<unsigned> path){

	syn_encode(path);
//	cout<<s<<endl;
	if(s.check() == sat)
	  return true;
	return false;

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
		  s.add(current_end<=time_var(next->event_list[j].name,i+1));
	}

}


void PathCheck::general_encode(vector<unsigned> path){
	//timing constraint of every basic msc in the path   
	for(unsigned i=1;i<path.size()-1;i++){
		BMSC *bmsc=msc->node_list[path[i]].ref;
		if(i==1){  //occurence time of the first event equals to 0
			s.add(time_var(bmsc->event_list[0].name,i)==0);
		}
		//timeing specification of basic msc
		for(unsigned j=0;j<bmsc->constraints.size();j++)
		  s.add(constraint2expr(bmsc->constraints[j], i));
		//communication time
		add_communication(bmsc, i);
		//add order pair
		for(unsigned j=0;j<bmsc->order_list.size();j++){
			OrderPair order=bmsc->order_list[j];
			s.add(time_var(order.predecessor,i)<=time_var(order.successor,i));
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
						s.add(switch_op(exp, con.op, c.real_val(con.value.c_str())));
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
		Constraint con=msc->communicationTime[i];
		assert(con.pvlist.size() == 2);
		for(unsigned j=0;j<bmsc->message_list.size();j++){
			string sender=bmsc->message_list[j].sender_name;
			string receiver=bmsc->message_list[j].receiver_name;
			if(sender==con.pvlist[1].getName() && receiver==con.pvlist[0].getName()){
				exp=time_var(bmsc->message_list[i].receive_event, loop)-time_var(bmsc->message_list[i].send_event, loop);
				s.add(switch_op(exp, con.op, c.real_val(con.value.c_str())));
			}
		}
	}
}

expr PathCheck::time_var(string name,int loop){
	char tname[256];
	sprintf(tname,"%s_%i",name.c_str(),loop);
	return c.real_const(tname);
}
