#ifndef _PROCESS_XML_H
#define _PROCESS_XML_H
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <string>
#include "HMSC.h"
#include "BMSC.h"
#include "TimeAnalyze.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

void process_xml(const char* dirname);
const char* get_filename_ext(const char* filename);

extern int VERBOSE_LEVEL;
#endif

