#include "HMSC.h"


Succ::Succ(const char* id_text,const char* pre_text,const char* succ_text):pre_node(NULL),succ_node(NULL){
	assert(id_text!=NULL);
	assert(pre_text!=NULL);
	assert(succ_text!=NULL);
	id=id_text;
	pre=pre_text;
	succ=succ_text;
}

bool Node::contain_event(string event){
	for(unsigned i=0;i<ref->event_list.size();i++){
		if(ref->event_list[i].name == event)
		  return true;
	}
	return false;
}

void HMSC::setConstraints(const char * text){
	assert(text!=NULL);
	parse_constraints(constraints,text);
}

void HMSC::setCommunicationTime(const char * text){
	assert(text!=NULL);
	parse_constraints(communicationTime,text);
}

void HMSC::initialize(){
	for(unsigned i=0;i<node_list.size();i++){
		Node* node=&node_list[i];
		node->ID = i;
		if(node->name == "initial"){
			node->setRef(NULL);
			initial_node=node;
		}
		else if(node->name == "final"){
			node->setRef(NULL);
			final_node=node;
		}else{
			for(unsigned i=0;i<bmsc_list.size();i++){
				BMSC* b = &bmsc_list[i];
				if(b->getName()==node->name || b->getName()==node->name+"Interaction")
				  node->setRef(b);
			}
		}
	}
	for(unsigned i=0;i<succ_list.size();i++){
		Succ* s = &succ_list[i];
		for(unsigned j=0;j<node_list.size();j++){
			Node* node=&node_list[j];
			if(s->pre==node->id)
			  s->setPreNode(node);
			if(s->succ==node->id)
			  s->setSuccNode(node);
		}
	}
	for(unsigned i=0;i<node_list.size();i++){
		Node *n = &node_list[i];
		for(unsigned j=0;j<succ_list.size();j++){
			if(succ_list[j].pre_node == n)
			  n->add_next(succ_list[j].succ_node);
		}
	}
}

Node *HMSC::get_node(string name){
	for(unsigned i=0;i<node_list.size();i++){
	  if(node_list[i].name==name)
		return &node_list[i];
	}
	return NULL;
}
/* show information of the high-level message sequence chart */
void HMSC::print_info(){
	printf("detail information of HMSC: %s\n",name.c_str());
	for (unsigned int i=0;i<node_list.size();i++){
		printf("Node name: %s, ID:%d",node_list[i].name.c_str(),node_list[i].ID);
		if(node_list[i].ref!=NULL)
		  printf("\nreference bmsc: %s",node_list[i].ref->getName().c_str());
		printf("\n");
	}
	for (unsigned int i=0;i<succ_list.size();i++){
		Succ succ=succ_list[i];
		if(succ.pre_node!=NULL)
		  printf(" predecessor: %s",succ.pre_node->name.c_str());
		if(succ.succ_node!=NULL)
		  printf(" successor: %s",succ.succ_node->name.c_str());
		printf("\n");
	}
	printf("timing specification:\n");
	for (unsigned int i=0;i<constraints.size();i++){
		constraints[i].print_info();
	}
	printf("communication time:\n");
	for (unsigned int i=0;i<communicationTime.size();i++){
		communicationTime[i].print_info();
	}

}

