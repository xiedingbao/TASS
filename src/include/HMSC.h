#ifndef _HMSC_H
#define _HMSC_H
#include <stdio.h>
#include <string>
#include <vector>
#include <assert.h>
#include "Constraint.h"
#include "BMSC.h"
extern int VERBOSE_LEVEL;
using namespace std;


class Node{
	string name;
	string id;
	BMSC* ref;
	vector<Node*> next;
	unsigned ID;//place in the node list
public:
	Node(string name_text,const char * id_text):ref(NULL){assert(id_text!=NULL);name=name_text;id=id_text;}
	void setRef(BMSC* ref){this->ref=ref;}
	void add_next(Node *n){next.push_back(n);}
	bool contain_event(string event);
	friend class HMSC;
	friend class Succ;
	friend class TimeAnalyze;
	friend class PathCheck;
};

class Succ{
	string id;
	string pre;
	string succ;
	Node* pre_node;
	Node* succ_node;
public:
	Succ(const char* id_text,const char* pre_text,const char* succ_text);
	void setPreNode(Node *n){pre_node=n;}
	void setSuccNode(Node* n){succ_node=n;}
	friend class HMSC;
};

class HMSC{
	std::string name;
	vector<Node> node_list;
	vector<Succ> succ_list;
	vector<BMSC> bmsc_list;
	vector<Constraint> constraints;
	vector<Constraint> communicationTime;
	Node* initial_node;
	Node* final_node;

public:
	HMSC():initial_node(NULL),final_node(NULL){}
	void setConstraints(const char * text);
	void setCommunicationTime(const char *text);
	void setName(string text){name=text;}
	void add_succ(Succ succ){succ_list.push_back(succ);}
	void add_node(Node node){node_list.push_back(node);}
	void add_bmsc(BMSC bmsc){bmsc_list.push_back(bmsc);}
	void initialize();
	void print_info();
	friend class TimeAnalyze;
	friend class PathCheck;
};
#endif



