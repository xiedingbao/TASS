#ifndef _BASIC_MSC_H
#define _BASIC_MSC_H
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include "Constraint.h"
using namespace std;

class Message;
class Event{
	string name;
	string id;
	bool is_send;
	Message* m;
public:
	Event(){}
	Event(string name_text,const char* id_text,bool isSend);
	friend class BMSC;
	friend class OrderPair;
	friend class PathCheck;
	friend class Node;
};

class Message{
	string id;
	string name;
	string sender;
	string receiver;
	string sender_name;
	string receiver_name;
	string send_event;
	string receive_event;
public:
	Message(string name_text,const char* id_text,const char* sender_text,const char* receiver_text);
	void setEvent(string send,string receive){send_event=send;receive_event=receive;}
	friend class BMSC;
	friend class PathCheck;
};


class Process{
	string name;
	string id;
	vector<string> message_event;
public:
	Process(string name_text,const char* id_text,const char* msgIdStr);
	friend class BMSC;


};
class OrderPair{
	string predecessor;
	string successor;
public:
	OrderPair(string pre,string succ){predecessor=pre;successor=succ;}
	friend class PathCheck;
};
class BMSC{
private:
	vector<Process> process_list;
	vector<Message> message_list;
	vector<string> message_spec_list;
	vector<Event> event_list;
	vector<Constraint> constraints;	
	vector<OrderPair> order_list;
	string name;
	void generateOrderPair();
public:
	BMSC(){}
	BMSC(string name_text){name=name_text;}
	string getName(){return name;}
	void setName(string name_text){name=name_text;}
	void setConstraints(const char* text);
	void add_message_spec(const char *text){assert(text!=NULL);message_spec_list.push_back(string(text));}
	void add_event(Event e){event_list.push_back(e);}
	void add_process(Process p){process_list.push_back(p);}
	void add_message(Message m){message_list.push_back(m);}
	void print_info();
	void clear(); /* reset to the initial state */
	void initialize();
	friend class PathCheck;
	friend class Node;

};



#endif
