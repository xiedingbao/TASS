#include "process_xml.h"
using namespace std;
static HMSC *msc;
unsigned char isFile=0x8;

static void parse(const char *dir);
static int parse_xml(const char *filename);
static void parse_uml(xmlDocPtr doc, xmlNodePtr cur);
static void parse_tass(xmlDocPtr doc, xmlNodePtr cur);
static void parse_collaboration(xmlDocPtr doc, xmlNodePtr cur);
static void parse_activity(xmlDocPtr doc, xmlNodePtr cur);

void process_xml (const char* dir_name){
	LIBXML_TEST_VERSION
	xmlKeepBlanksDefault(0);
	msc = new HMSC();
	/* read msc from xml */
	parse(dir_name);
	msc->initialize();
	if(VERBOSE_LEVEL>1)
		msc->print_info();
	Node *target_node = msc->get_node(target);
	if(target_node==NULL){
		fprintf(stderr,"target %s does not exist in the graph\n",target.c_str());
		delete msc;
		return;
	}
	TimeAnalyze analyzer(msc);
	analyzer.reach_check(target_node);
	delete msc;
	xmlCleanupParser();
}

static void parse(const char *dirname){
	struct dirent *dir;	
	DIR *d = opendir(dirname);
	if(d){
		while((dir =readdir(d)) != NULL){
			if(dir->d_type == isFile){
				string filepath = string(dirname)+"/"+string(dir->d_name);
				parse_xml(filepath.c_str());
			}
		}
	}else{
		fprintf(stderr,"Fatal error: failed to open directory: %s\n",dirname);
		exit(1);
	}
}


static int parse_xml(const char* filename){
	const char* ext = get_filename_ext(filename);
	if(strcmp(ext, "tass")!=0 && strcmp(ext, "uml")!=0){
		if(VERBOSE_LEVEL>1)
		  printf("Warning: ignore file %s\n", filename);
		return 1;
	}
	xmlDocPtr doc;
	xmlNodePtr root;
	doc = xmlParseFile(filename);
	if(doc == NULL){
		fprintf(stderr,"Error: Document: %s is not parsed successfully.\n",filename);
		xmlFreeDoc(doc);
		return 1;
	}
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return 1;
	}
	if(!strcmp(ext, "tass"))
		parse_tass(doc, root);
	else if(!strcmp(ext, "uml"))
		parse_uml(doc, root);
    return 0;
}

const char* get_prop(xmlNodePtr cur, const char* prop){
	return (char *) xmlGetProp(cur, (const xmlChar *) prop);
}

static void parse_collaboration(xmlDocPtr doc, xmlNodePtr cur){
	BMSC bmsc;	
	while(cur != NULL){
		if(!xmlStrcmp(xmlGetProp(cur, (const xmlChar *) "type"),(const xmlChar*)"uml:Collaboration")){
			xmlNodePtr sub_cur = cur->xmlChildrenNode;
			if(!xmlStrcmp(xmlGetProp(sub_cur, (const xmlChar *) "type"), (const xmlChar*)"uml:Interaction")){
				bmsc.setName(get_prop(sub_cur, "name"));
				sub_cur = sub_cur->xmlChildrenNode;
				assert(sub_cur != NULL);
				while(sub_cur!=NULL){
					if(!xmlStrcmp(sub_cur->name, (const xmlChar *) "ownedComment")){
						char *constraint = (char *)xmlNodeListGetString(doc, sub_cur->xmlChildrenNode->xmlChildrenNode,1);
						bmsc.setConstraints(constraint);
					}
					else if(!xmlStrcmp(sub_cur->name, (const xmlChar *) "lifeline"))
						bmsc.add_process(Process(get_prop(sub_cur, "name"), get_prop(sub_cur, "id"), get_prop(sub_cur, "coveredBy")));
					else if(!xmlStrcmp(sub_cur->name,(const xmlChar *) "message"))
						bmsc.add_message(Message(get_prop(sub_cur,"name"), get_prop(sub_cur,"id"), get_prop(sub_cur,"sendEvent"), get_prop(sub_cur,"receiveEvent")));
					else if(!xmlStrcmp(sub_cur->name,(const xmlChar *) "fragment")){
						if(!xmlStrcmp(xmlGetProp(sub_cur,(const xmlChar *)"type"), (const xmlChar *)"uml:MessageOccurrenceSpecification"))
							bmsc.add_message_spec(get_prop(sub_cur,"message"));
					}
					sub_cur = sub_cur->next;
				}
			}
		}
		else if(!xmlStrcmp(xmlGetProp(cur,(const xmlChar *)"type"),(const xmlChar*) "uml:TimeObservation")){
			bool is_send=true;
			if(!xmlStrcmp(xmlGetProp(cur,(const xmlChar *)"firstEvent"),(const xmlChar*) "false"))
			  is_send=false;
			bmsc.add_event(Event(get_prop(cur,"name"),get_prop(cur,"event"), is_send));
		}
		cur=cur->next;
	}
	bmsc.initialize();
	msc->add_bmsc(bmsc);
	if(VERBOSE_LEVEL>1)
		bmsc.print_info();
}

static void parse_activity(xmlDocPtr doc, xmlNodePtr cur){
	msc->setName(get_prop(cur, "name"));
	cur = cur->xmlChildrenNode;
	while(cur != NULL){
		if(!xmlStrcmp(cur->name, (const xmlChar*) "node")){
			string name = get_prop(cur, "name");
			const char* id = get_prop(cur, "id");
			xmlChar *type = xmlGetProp(cur, (const xmlChar *) "type");
			if(!xmlStrcmp(type, (const xmlChar*) "uml:InitialNode"))
				name = "initial";
			else if(!xmlStrcmp(type, (const xmlChar*) "uml:ActivityFinalNode"))
				name = "final";
			else if(!xmlStrcmp(type, (const xmlChar*) "uml:LoopNode")){
				xmlNodePtr sub_cur=cur->xmlChildrenNode;
				if(sub_cur != NULL){
					type = xmlGetProp(sub_cur, (const xmlChar *) "type");
					if(!xmlStrcmp(type, (const xmlChar*) "uml:CallOperationAction")){
						name = get_prop(sub_cur, "name");
						id = get_prop(sub_cur, "id");
						msc->add_succ(Succ(id,id,id));
					}
				}
			} 
			msc->add_node(Node(name,id));
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar*) "edge")){
			msc->add_succ(Succ(get_prop(cur,"id"), get_prop(cur,"source"), get_prop(cur,"target")));
		}
		cur = cur->next;
	}
}



static void parse_uml(xmlDocPtr doc, xmlNodePtr cur){
	if(xmlStrcmp(cur->name, (const xmlChar *) "Model")){
		fprintf(stderr,"document of the wrong type, root node != Model\n");
		return;
	}
	cur=cur->xmlChildrenNode;
	assert(cur!=NULL);
	cur=cur->xmlChildrenNode;
	assert(cur!=NULL);
	xmlChar *type = xmlGetProp(cur, (const xmlChar *) "type");
	assert(type != NULL);
	if(!xmlStrcmp(type, (const xmlChar *) "uml:Collaboration")){
		parse_collaboration(doc, cur);
	}
	else if(!xmlStrcmp(type, (const xmlChar *) "uml:Activity")){
		parse_activity(doc, cur);
	}
}

void parse_tass(xmlDocPtr doc, xmlNodePtr cur){
	if(xmlStrcmp(cur->name, (const xmlChar *) "TASS")){
		fprintf(stderr,"document of the wrong type, root node != TASS\n");
		xmlFreeDoc(doc);
		return;
	}
	cur = cur->xmlChildrenNode;
	while(cur!=NULL){
		if(!xmlStrcmp(cur->name, (const xmlChar *) "Constraints")){
				char *constraint = (char *)xmlNodeListGetString(doc, cur->xmlChildrenNode,1);
				msc->setConstraints(constraint);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "CommunicationTime")){
			char *constraint = (char *)xmlNodeListGetString(doc, cur->xmlChildrenNode,1);
			msc->setCommunicationTime(constraint);
		}
		cur = cur->next;
	}
}

const char* get_filename_ext(const char* filename){
	const char *dot = strrchr(filename,'.');
	if(!dot || dot == filename) 
	  return "";
	return dot+1;
}

