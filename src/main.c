#include <stdio.h>

#include "defines.h"
#include "lex.h"
#include "parser.h"
#include "interpret.h"

struct global params = {0};

int parse_args(int argc, char **argv)
{
	int ret_code = 0;
	
	// parsuj cmdline argumenty

	return ret_code;
}

int main(int argc, char **argv)
{
	int ret = 0;
	//parse args
	if((ret = parse_args(argc, argv)) != RET_OK);

	//open file
	if((ret = open_src("/dev/null")) != RET_OK);

	return ret;
}
