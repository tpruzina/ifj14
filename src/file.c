#include <stdio.h>

#include "defines.h"
#include "file.h"

int mmap_file(char *path)
{
	FILE *fd;

	fd = fopen(path, "r");
	if (!fd)
		return -1;

}
