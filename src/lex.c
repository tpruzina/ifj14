#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "Structs.h"

int is_aplha(char ch){
	return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}
int is_numeric(char ch){
	return (ch >= '0' && ch <= '9');
}


struct token* identificator(struct main_all** ma){
	struct String* str = makeNewString(ma);
	char cur = fgetc(*(ma->source));
	
	while(is_alpha(cur) || is_numeric(cur) || cur == '_'){
		addChar(str, cur);
		
		cur = fgetc((*ma)->source);
	}


}

struct token* getNextToken(struct main_all** ma){
	

}

int load_lex(struct main_all** ma){


}
