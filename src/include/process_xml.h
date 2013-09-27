#ifndef _PROCESS_XML_H
#define _PROCESS_XML_H
#include "expat.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <string>
#include "HMSC.h"
#include "BMSC.h"
#include "TimeAnalyze.h"

void process_xml(const char* dir_name);
const char* get_filename_ext(const char* filename);
//static inline void reset_last_content();
//static inline void process_last_content();

extern int VERBOSE_LEVEL;
#endif

