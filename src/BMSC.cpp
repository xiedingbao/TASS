#include "BMSC.h"
#include <iostream>

Event::Event(string name_text, const char* id_text, bool isSend){
	name=name_text;
	if(id_text == NULL)
		id="";
	else
		id=id_text;
	is_send=isSend;
}

Message::Message(string name_text, const char* id_text, const char* sender_text, const char* receiver_text){
	assert(id_text!=NULL && sender_text!=NULL && receiver_text!=NULL);
	name=name_text;
	id=string(id_text);
	sender=string(sender_text);
	receiver=string(receiver_text);
	sender_name="";
	receiver_name="";
}

Process::Process(string name_text, const char* id_text, const char* msgIdStr){
	assert(id_text!=NULL);
	name=name_text;
	id=id_text;
	if(msgIdStr!=NULL){
		char *content=(char *)malloc(strlen(msgIdStr));	
		strcpy(content,msgIdStr);
		char *pch=strtok(content," ");
		while (pch!=NULL){
			message_event.push_back(pch);
			pch = strtok(NULL," ");
		}
		free(content);
	}
}

void BMSC::initialize(){
	vector<Message> tmp;
	for(unsigned i=0;i<message_list.size();i++){
		string ms=message_spec_list[2*i];
		for(unsigned j=0;j<message_list.size();j++){
			if(ms == message_list[j].id){
				tmp.push_back(message_list[j]);
				break;
			}
		}
	}
	message_list=tmp;
	Event* event=new Event[2*message_list.size()+1];
	for(unsigned i=0;i<message_list.size();i++){
		Message* m=&message_list[i];
		for(unsigned j=0;j<event_list.size();j++){
			Event* e=&event_list[j];
			if(m->id == e->id){
				e->m=m;
				if(e->is_send){
				  m->send_event=e->name;
				  event[i*2]=*e;
				}
				else{
				  m->receive_event=e->name;
				  event[i*2+1]=*e;
				}
			}
		}
		for(unsigned j=0;j<process_list.size();j++){//find the process to which every message attributes 
			for(unsigned k=0;k<process_list[j].message_event.size();k++){
				string message_event=process_list[j].message_event[k];				
				if(m->sender == message_event)
					m->sender_name=process_list[j].name;
				else if(m->receiver==message_event)
					m->receiver_name=process_list[j].name;
			}
		}
	}
	for(unsigned int i=0;i<event_list.size();i++){ //the end event
		if(event_list[i].id == "")
			event[message_list.size()*2]=event_list[i];		
	}
	event_list.clear();
	for(unsigned i=0;i<message_list.size()*2+1;i++)
		event_list.push_back(event[i]);
	delete[] event;
	generateOrderPair();
}

void BMSC::setConstraints(const char* text){
	assert(text!=NULL);
	parse_constraints(constraints,text);
}


void BMSC::generateOrderPair(){
	order_list.clear();
	string final_event=event_list.back().name;
	/* Causility */
	for(unsigned i=0; i< message_list.size(); i++){
		order_list.push_back(OrderPair(message_list[i].send_event,message_list[i].receive_event));
		order_list.push_back(OrderPair(message_list[i].receive_event,final_event));
	}
	/* Controbility */
	for(unsigned i=message_list.size()-1;i>0;i--){
		for(unsigned j=i-1; j>0; j--){
			if(message_list[i].sender_name == message_list[j].sender_name){
				order_list.push_back(OrderPair(message_list[j].send_event,message_list[i].send_event));
				break;
			}
			if(message_list[i].sender_name==message_list[j].receiver_name){
				order_list.push_back(OrderPair(message_list[j].receive_event,message_list[i].send_event));
				break;
			}
		}
	}
	/* FIFO */
	for(int i=message_list.size()-1; i>0; i--){
		for(int j=i-1; j>=0 ;j--){
			if(message_list[i].sender_name==message_list[j].sender_name && message_list[i].receiver_name==message_list[j].receiver_name){
				order_list.push_back(OrderPair(message_list[j].receive_event,message_list[i].receive_event));
				break;
			}
		}
	}
	
}

void BMSC::clear(){
	name = "";
	process_list.clear();
	message_list.clear();
	message_spec_list.clear();
	event_list.clear();
	constraints.clear();
	order_list.clear();
}

void BMSC::print_info(){
	printf("detail information of basic msc:%s\n",name.c_str());
	printf("Name of the basic MSC: %s\n",name.c_str());
	for(unsigned int i=0;i<process_list.size();i++){
		Process p=process_list[i];
		printf("process name:%s\n",p.name.c_str());
	}
	for(unsigned int i=0;i<message_list.size();i++){
		Message m=message_list[i];
		printf("message name:%s,sender:%s,receiver:%s,send_event:%s,receive_event:%s\n",m.name.c_str(),m.sender_name.c_str(),m.receiver_name.c_str(),m.send_event.c_str(),m.receive_event.c_str());
	}
	printf("event name: ");
	for(unsigned int i=0;i<event_list.size();i++){
		Event e=event_list[i];
		printf("%s ",e.name.c_str());
	}
	printf("\n");
	for(unsigned int i=0;i<constraints.size();i++)
		constraints[i].print_info();
}
	

