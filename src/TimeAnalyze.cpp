#include "TimeAnalyze.h"
using namespace z3;

/* constructor */
TimeAnalyze::TimeAnalyze(HMSC *msc):msc(msc),s(c){
	this->bound=inputbound;
}

/* reachability analysis of the HMSC */
bool TimeAnalyze::reach_check(){
	assert(msc->initial_node!=NULL && msc->final_node!=NULL);
	encode_graph();
	unsigned num_of_path=0;
	PathCheck verify(msc);	
	while(s.check() == sat){
		num_of_path++;
		vector<unsigned> path=decode_path();		
		if(VERBOSE_LEVEL>1)
			printf("Checking %s\n",(path_str(path)).c_str());
		if(verify.path_check(path)){
			printf("The target is reachable\nPath: %s\n",path_str(path).c_str());
			return true;
		}
		block_path(path);
	}
	printf("The target is not reachable\nNumber of path checked: %d\n",num_of_path);
	return false;
}


/* encode the graph structure of HMSC in terms of SAT formulas */
void TimeAnalyze::encode_graph(){
	//initial condition
	for(unsigned i=0;i<msc->node_list.size();i++){
		if(msc->initial_node == &msc->node_list[i])
		  s.add(node_var(msc->initial_node,0));
		else
		  s.add(!node_var(&msc->node_list[i],0));
	}
	//not exactly in one location, exclude condition
	for(int i=0;i<=bound;i++){
		for(unsigned j=0;j<msc->node_list.size()-1;j++){
			for(unsigned k=j+1;k<msc->node_list.size();k++){
				s.add(!node_var(&msc->node_list[j],i)||!node_var(&msc->node_list[k],i));
			}
		}
	}
	//transition relation, if has no outgoing edge or is the target, add stutter jump
	for(unsigned i=0;i<msc->node_list.size();i++){
		for(int j=0;j<bound;j++){
			expr current=node_var(&msc->node_list[i],j);
			expr next(c);
			if(msc->node_list[i].next.size()==0 || &msc->node_list[i]==msc->final_node){
				next=node_var(&msc->node_list[i],j+1);
			}
			else{
				for(unsigned k=0;k<msc->node_list[i].next.size();k++){
					if(k==0)
						next=node_var(msc->node_list[i].next[k],j+1);
					else
						next=next||node_var(msc->node_list[i].next[k],j+1);
				}
			}
			s.add(implies(current,next));
		}
	}
	//target condition
	expr target=node_var(msc->final_node,0);
	for(int i=1;i<=bound;i++){
		target=target || node_var(msc->final_node,i);
	}
	s.add(target);

}

/* block a path in the graph */
void TimeAnalyze::block_path(vector<unsigned> path){
	for(unsigned i=0;i<bound-path.size()+2;i++){
		expr exp(c);
		for(unsigned j=0;j<path.size();j++){
			expr node=node_var(&msc->node_list[path[j]],i+j);
			if(j==0)
			  exp=!node;
			else
			  exp=exp||!node;
		}
		s.add(exp);
	}
}

expr TimeAnalyze::node_var(Node *node,int loop){
	char vname[256];
	sprintf(vname,"%i_%d",loop,node->ID);
	return c.bool_const(vname);
}

/* get path from a truth assignment */
vector<unsigned> TimeAnalyze::decode_path(){
	unsigned* path=new unsigned[bound+1];
	vector<unsigned> compress_path;
	model m=s.get_model();
	for(unsigned i=0;i<m.size();i++){
		func_decl v=m[i];
		expr value=m.get_const_interp(v);
		if(eq(value, c.bool_val(true))){
			int ID,loop;
			decode_name(v.name().str(),ID,loop);
			path[loop]=ID;
		}
	}
	for(int i=0;i<bound+1;i++){
		compress_path.push_back(path[i]);
		if(path[i] == msc->final_node->ID)
		  break;
	}
	delete[] path;
	return compress_path;
}

void TimeAnalyze::decode_name(string name,int& ID,int& loop){
	char *content=(char *)malloc(name.length());
	strcpy(content,name.c_str());
	char *pch=strtok(content,"_");
	assert(pch!=NULL);
	loop=atoi(pch);
	pch=strtok(NULL,"_");
	assert(pch!=NULL);
	ID=atoi(pch);
	free(content);
}

string TimeAnalyze::path_str(vector<unsigned> path){
	string str="";
	for(unsigned i=0;i<path.size();i++){
		str+=msc->node_list[path[i]].name;
		if(i!=path.size()-1)
		  str+="^";
	}
	return str;
}

