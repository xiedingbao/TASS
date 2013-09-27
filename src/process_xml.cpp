#include "process_xml.h"
using namespace std;

#define BUFFER_SIZE 512
unsigned char isFile=0x8;
static HMSC *msc;
static BMSC bmsc;

/* track the current level in the xml tree */
static int depth = 0;
/* the char buffer, it may overflow */
static char last_content[1024];
static bool overflow;
static size_t offs;
static string diagram_type;
static bool isLoop=false;

static inline void reset_last_content(){
	offs=0;
	overflow=false;
}
static inline void process_last_content(){
	if(offs>0){
		last_content[offs]='\0';
	}
	if(overflow){
		fprintf(stderr,"Fatal error: the content buffer is overflow\n");
		exit(1);
	}
}
static void tass_start_element(void *data, const char *element, const char **attribute){

	depth++;
}

static void tass_end_element(void *data, const char *el){
	depth--;
	process_last_content();
	if(depth==1){
		if(strcmp(el, "Constraints")== 0)
			msc->setConstraints(last_content);
		if(strcmp(el, "CommunicationTime")== 0)
			msc->setCommunicationTime(last_content);
		//printf("element: %s,content: %s\n",el,last_content);
	}
	reset_last_content();
}

/* first when start element is encountered */
static void start_element(void *data, const char *element, const char **attribute){
	depth++;
	const char *source=NULL,*target=NULL,*id=NULL;
	string name;
	if(strcmp(element,"packagedElement") ==0){
		bool isSend=true;
		for(int i=0;attribute[i];i+=2){
			if(strcmp(attribute[i],"name")==0)
				name=attribute[i+1];
			else if(strcmp(attribute[i],"xmi:type")==0)
				diagram_type=string(attribute[i+1]);
			else if(strcmp(attribute[i],"event")==0)
				id=attribute[i+1];
			else if(strcmp(attribute[i],"firstEvent")==0 && strcmp(attribute[i+1],"false")==0)
				isSend=false;
		}
		if(diagram_type=="uml:Activity")
	 		msc->setName(name);
		else if(diagram_type=="uml:TimeObservation"){
			bmsc.add_event(Event(name,id,isSend));
			diagram_type="uml:Interaction";
		}
	}

	else if(strcmp(element,"ownedBehavior")==0){
		for(int i=0;attribute[i];i+=2){
			if(strcmp(attribute[i],"name")==0)
				name=attribute[i+1];
			else if(strcmp(attribute[i],"xmi:type")==0)
				diagram_type=string(attribute[i+1]);
			if(diagram_type=="uml:Interaction"){
				bmsc.setName(name);
			}
		}
	}
	else if(strcmp(element,"node")==0){
		for(int i=0;attribute[i];i+=2){
			if(strcmp(attribute[i],"name")==0)
				name=attribute[i+1];
			else if(strcmp(attribute[i],"xmi:type")==0)
				diagram_type=string(attribute[i+1]);
			else if(strcmp(attribute[i],"xmi:id")==0)
				id=attribute[i+1];
		}
		if(diagram_type=="uml:InitialNode")
			  name="initial";
		else if(diagram_type=="uml:ActivityFinalNode")
			  name="final";
		else if(diagram_type=="uml:LoopNode"){
			isLoop=true;
		}
		else if(diagram_type=="uml:CallOperationAction"&&isLoop){
				msc->add_succ(Succ(id,id,id));
				isLoop=false;
		}
		msc->add_node(Node(name,id));
	}
	else if(strcmp(element,"edge")==0){
		for(int i=0;attribute[i];i+=2){		
			if(strcmp(attribute[i],"xmi:id")==0)
				id=attribute[i+1];
			else if(strcmp(attribute[i],"source")==0)
				source=attribute[i+1];
			else if(strcmp(attribute[i],"target")==0)
				target=attribute[i+1];
		}
		msc->add_succ(Succ(id,source,target));
	}
	else if(strcmp(element,"lifeline")==0){
		const char *msgIdStr=NULL;
		for(int i=0;attribute[i];i+=2){		
			if(strcmp(attribute[i],"xmi:id")==0)
				id=attribute[i+1];
			else if(strcmp(attribute[i],"name")==0)
				name=attribute[i+1];
			else if(strcmp(attribute[i],"coveredBy")==0)
				msgIdStr=attribute[i+1];
		}
		bmsc.add_process(Process(name,id,msgIdStr));
	}
	else if(strcmp(element,"message")==0){
		const char *sender=NULL,*receiver=NULL;
		for(int i=0;attribute[i];i+=2){		
			if(strcmp(attribute[i],"xmi:id")==0)
				id=attribute[i+1];
			if(strcmp(attribute[i],"name")==0)
				name=attribute[i+1];
			else if(strcmp(attribute[i],"sendEvent")==0)
				sender=attribute[i+1];
			else if(strcmp(attribute[i],"receiveEvent")==0)
				receiver=attribute[i+1];
		}
		bmsc.add_message(Message(name,id,sender,receiver));
	}
	else if(strcmp(element,"fragment")==0){
		const char *message=NULL;
		for(int i=0;attribute[i];i+=2){		
			if(strcmp(attribute[i],"xmi:type")==0)
				diagram_type=string(attribute[i+1]);
			else if(strcmp(attribute[i],"message")==0)
				message=attribute[i+1];
		}
		if(diagram_type=="uml:MessageOccurrenceSpecification")
			bmsc.add_message_spec(message);
	}



}

/* decrement the current level of the tree */
static void end_element(void *data, const char *el){
	depth--;
	process_last_content();
	if(strcmp(el,"body")==0&&diagram_type=="uml:Interaction"){
	  bmsc.setConstraints(last_content);
	}
	if(depth==0 && diagram_type=="uml:Interaction"){
	  bmsc.initialize();
	  msc->add_bmsc(bmsc);
	  if(VERBOSE_LEVEL>1)
		bmsc.print_info();
	  bmsc.clear();
	}
	reset_last_content();
}

void handle_data(void *data, const char *content, int length){
	if(!overflow){
		if (length+offs>=sizeof(last_content))
		  overflow=true;
		else{
			memcpy(last_content+offs,content,length);
			offs +=length;
		}
	}
}
int parse_xml(const char* file_path,char *buff){
    FILE *fp = fopen(file_path, "r");
    assert(fp != NULL);
    XML_Parser parser = XML_ParserCreate(NULL);
    int done;
	const char* ext=get_filename_ext(file_path);
	if(strcmp(ext, "tass")==0)
		XML_SetElementHandler(parser, tass_start_element, tass_end_element);
	else if(strcmp(ext, "uml")==0)
		XML_SetElementHandler(parser, start_element, end_element);
	else{
		if(VERBOSE_LEVEL>1)
		  printf("Warning: ignore file %s\n",file_path);
		return 1;
	}
	XML_SetCharacterDataHandler(parser, handle_data);
	do{
		int len = (int)fread(buff, sizeof(char), sizeof(buff), fp);
		done= len < (int)sizeof(buff);
		/* parse the xml */
		if(XML_Parse(parser, buff, len, done) == XML_STATUS_ERROR ){
		      fprintf(stderr,"%s at line %lu\n", XML_ErrorString(XML_GetErrorCode(parser)),										               XML_GetCurrentLineNumber(parser));
		exit(1);
		}
	} while (!done);
    fclose(fp);
    XML_ParserFree(parser);
    return 0;
}

static void parse_xml(const char *dir_name){
	char buffer[BUFFER_SIZE];
	struct dirent *dir;	
	DIR *d = opendir(dir_name);
	if(d){
		while ((dir =readdir(d)) !=NULL){
			if(dir->d_type==isFile){
				string file_path=string(dir_name)+"/"+string(dir->d_name);
				parse_xml(file_path.c_str(),buffer);
			}
		}
	}else{
		fprintf(stderr,"Fatal error: failed to open the directory: %s\n",dir_name);
		exit(1);
	}
}
const char* get_filename_ext(const char* filename){
	const char *dot = strrchr(filename,'.');
	if(!dot || dot == filename) 
	  return "";
	return dot+1;
}
void process_xml (const char* dir_name){
	msc = new HMSC();
	/* read msc from xml */
	parse_xml(dir_name);
	msc->initialize();
	if(VERBOSE_LEVEL>1)
		msc->print_info();
	TimeAnalyze analyzer(msc);
	analyzer.reach_check();
	delete msc;
}

