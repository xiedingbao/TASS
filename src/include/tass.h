#ifndef _TASS_H
#define _TASS_H
#include <stdio.h>
#include <stdlib.h>
#include "process_xml.h"
#include "System.h"

int VERBOSE_LEVEL=0;     //0:silent 1:some 2:all
bool asynchronous=false; //asynchronous or synchronous concatenation (default: synchronous) 
int inputbound;

static void help_message();
static void welcome_message();
static void info_message();
static void process_flag(char* flagstr);
static void process_flags(int argc, char *argv[]);
#endif
