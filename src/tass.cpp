#include "tass.h"

int main(int argc, char *argv[]){
	if (argc==1){
		help_message();
		exit(1);
	}
	process_flags(argc,argv);
	return 0;
}

static void welcome_message(){
	printf("TASS, last modified September, 2013\n");
}

static void help_message(){
	printf("USage: tass [OPTIONS] <file directory> <target> <bound>\n");
	printf("-h more information:\n");
	printf("For further information, please visit: http://seg.nju.edu.cn/TASS\n");
}
/* show the command options */
static void info_message(){
	printf("-h more information:\n");
	printf("-v detailed outpout (0:silent, 1:some, 2:all), e.g. -v0:\n");
	printf("-a asynchronous concatenation (default: synchronous concatenation):\n");
	printf("-s show the statistical information:\n");
}

/* process the command options */
static void process_flag(char* flagstr){
	switch (flagstr[0]){
		case 'h':
			welcome_message();
			info_message();
			exit(1);
			break;
		case 'v':
			if((strlen(flagstr) < 2)){
				printf("warning: Illegal flag: -v<integer> required\n");
			}
			else{
				VERBOSE_LEVEL = atoi(flagstr+1);
				printf("Setting output level to %d\n",VERBOSE_LEVEL);
			}
			break;
		case 'a':
			asynchronous = true;
			break;
		case 's':
			st = true;
			break;
		default:
			printf("Warning: illegal flag (%s) ignored\n",flagstr);
	}
}

static void process_flags(int argc, char *argv[]){
	char * dir_name=NULL;
	int count=0;
	for(int i=1;i<argc;i++){
		if(argv[i][0] == '-'){
			//then process a flag
			process_flag(argv[i]+1);
		}
		else{
			if(count == 0)
				dir_name = argv[i];
			else if(count == 1)
				target = argv[i];
			else if(count == 2)
				inputbound=atoi(argv[i]);  
			count++;
		}
	}
	if(count == 3)
		process_xml(dir_name);
	else{
		help_message();
		exit(1);
	}
	if(st){
		double mem_used_peak = memUsedPeak();
		double cpu_time = cpuTime();
		if(mem_used_peak !=0)
			printf("Memory used: %2.f MB\n", mem_used_peak);
		printf("CPU time: %g s\n", cpu_time);
	}
}
