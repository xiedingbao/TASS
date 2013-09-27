#include "process_xml.h"
#include <string.h>
#define BUFFER_SIZE 512
/* track the current level in the xml tree */
static int depth = 0;
/* first when start element is encountered */
void start_element(void *data, const char *element, const char **attribute)
{
int i;

for(i = 0; i < depth; i++)
{
    printf(" ");
}

printf("%s", element);

for(i = 0; attribute[i]; i += 2)
{
    printf(" %s= '%s'", attribute[i], attribute[i + 1]);
}

printf("\n");
depth++;
}

/* decrement the current level of the tree */
void end_element(void *data, const char *el)
{
depth--;
}
int parse_xml(char *buff, size_t buff_size)
{
    FILE *fp;
    fp = fopen("start_indication.xml", "r");
    if(fp == NULL)
    {
    printf("Failed to open file\n");
    return 1;
    }

    XML_Parser parser = XML_ParserCreate(NULL);
    int done;
    XML_SetElementHandler(parser, start_element, end_element);

    memset(buff, 0, buff_size);
    printf("strlen(buff) before parsing: %d\n", strlen(buff));

    size_t file_size = 0;
    file_size = fread(buff, sizeof(char), buff_size, fp);

    /* parse the xml */
    if(XML_Parse(parser, buff, strlen(buff), XML_TRUE) == XML_STATUS_ERROR)
    {
        printf("Error: %s\n", XML_ErrorString(XML_GetErrorCode(parser)));
    }

    fclose(fp);
    XML_ParserFree(parser);

    return 0;
}
int
main(int argc, char **argv)
{
    int             result;
    char            buffer[BUFFER_SIZE];
    result = parse_xml(buffer, BUFFER_SIZE);
    printf("Result is %i\n", result);
    return 0;
}

