#ifndef _LEX_H
#define _LEX_H

struct toc {
	int type;

	union data {
		char *id;
	} d;
};

struct toc get_toc();

#endif
