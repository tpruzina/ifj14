/*****************************************************************************
****                                                                      ****
****    PROJEKT DO PREDMETU IFJ                                           ****
****                                                                      ****
****    Nazev:     Implementace interpretu imperativního jazyka IFJ14     ****
****    Datum:                                                            ****
****    Autori:    Marko Antonín    <xmarko07@stud.fit.vutbr.cz>          ****
****               Pružina Tomáš    <xpruzi01@stud.fit.vutbr.cz>          ****
****               Kubíček Martin   <xkubic34@stud.fit.vutbr.cz           ****
****               Juřík Martin     <xjurik08@stud.fit.vutbr.cz           ****
****               David Petr       <xdavid15@stud.fit.vutbr.cz>          ****
****                                                                      ****
*****************************************************************************/


#include <ctype.h> 	// tolower()
#include "scanner.h"
#include "log.h"

/* get_toc - cita zo suboru dalsi token
 * @vstup:	otvoreny file descriptor
 * @vystup:	vraci alokovany token
 */
struct toc *
getToc()
{
	int c;		// nacitany aktualny znak
	int state;	// aktualny stav
	struct toc *toc;
	struct String *str;

// makro na vratenie charu na vstup a return tokenu
#define UNGETC_AND_RETURN_TOKEN() do {	\
		unGetChar(c); 					\
		if(toc->type != T_ID &&			\
			toc->type != T_STR)			\
					freeString(str);	\
		return toc;						\
		} while(0)
		
	tocInit(&toc);
	ASSERT(toc);
	ASSERT(global.src);

	static bool bol_dot_dot = false;
	if(bol_dot_dot)
	{
		bol_dot_dot = false;
		toc->type = T_DDOT;
		return toc;
	}

	state = KA_START;

	str = NULL;
	skipWSandComments();

	while (true)
	{
		if(state == KA_STR_LIT_INSIDE || state == KA_STR_LIT)
			c = getChar();
		else
			c = tolower(getChar());

		switch(state)
		{
		case KA_START:
			if(isalpha(c) || '_' == c) // asi identifikator
			{
				str = makeNewString();
				addChar(str,c);
				state = KA_IDENT;
			}
			else if(isdigit(c))
			{
				str = makeNewString();
				addChar(str,c);
				state = KA_INTEGER;
			}
			else if(';' == c)
				state = KA_SCOL;
			else if(':' == c)
				state = KA_COL;
			else if('.' == c)
				state = KA_DOT;
			else if(',' == c)
				state = KA_COM;
			else if('(' == c)
				state = KA_LPAR;
			else if(')' == c)
				state = KA_RPAR;
			else if('[' == c)
				state = KA_LBRC;
			else if(']' == c)
				state = KA_RBRC;
			else if('\'' == c)
				state = KA_STR_LIT;
			else if('=' == c)
				state = KA_EQ;
			else if('<' == c)
				state = KA_LSS;
			else if('>' == c)
				state = KA_GRT;
			else if('+' == c)
				state = KA_ADD;
			else if('-' == c)
				state = KA_SUB;
			else if('*' == c)
				state = KA_MUL;
			else if('/' == c)
				state = KA_DIV;
			else if(EOF == c)
			{
				toc->type = T_EOF;
				return toc;
			}
			else
				state = KA_ERR;
			break;

// jednopismenkove stavy - copy & paste
		case KA_LPAR:
			toc->type = T_LPAR;		// (
			UNGETC_AND_RETURN_TOKEN();
		case KA_RPAR:
			toc->type = T_RPAR;		// )
			UNGETC_AND_RETURN_TOKEN();
		case KA_LBRC:
			toc->type = T_LBRC;		// [
			UNGETC_AND_RETURN_TOKEN();
		case KA_RBRC:
			toc->type = T_RBRC;		// ]
			UNGETC_AND_RETURN_TOKEN();
		case KA_SCOL:
			toc->type = T_SCOL;		// ;
			UNGETC_AND_RETURN_TOKEN();
		case KA_DOT:
			toc->type = T_DOT;		// .
			UNGETC_AND_RETURN_TOKEN();
		case KA_COM:
			toc->type = T_COM;		// ,
			UNGETC_AND_RETURN_TOKEN();

// POROVNANIA =, > a >=
		case KA_EQ:
			toc->type = T_EQV;
			UNGETC_AND_RETURN_TOKEN();

		case KA_GRT:
			if('=' == c)
				state = KA_GEQV;
			else
			{
				toc->type = T_GRT;
				UNGETC_AND_RETURN_TOKEN();
			}
			break;

		case KA_GEQV:
			toc->type = T_GEQV;
			UNGETC_AND_RETURN_TOKEN();

// POROVNANIA < , <> a <=
		case KA_LSS:	// mame <
			if('>' == c)	//mame <>
				state = KA_NEQV;
			else if('=' == c)
				state = KA_LEQV;
			else
			{
				toc->type = T_LSS;
				UNGETC_AND_RETURN_TOKEN();
			}
			break;
		case KA_LEQV:
			toc->type = T_LEQV;
			UNGETC_AND_RETURN_TOKEN();
		case KA_NEQV:
			toc->type = T_NEQV;
			UNGETC_AND_RETURN_TOKEN();
// : a :=
		case KA_COL:	//mame ':'
			if('=' == c)	// :=
				state = KA_ASGN;
			else
			{
				toc->type = T_COL;
				UNGETC_AND_RETURN_TOKEN();
			}
			break;

		case KA_ASGN:	// mame :=
			toc->type = T_ASGN;
			UNGETC_AND_RETURN_TOKEN();

// MATEMATICKE OPERATORY
		case KA_ADD:
			toc->type = T_ADD;
			UNGETC_AND_RETURN_TOKEN();
		case KA_SUB:
			toc->type = T_SUB;
			UNGETC_AND_RETURN_TOKEN();
		case KA_DIV:
			toc->type = T_DIV;
			UNGETC_AND_RETURN_TOKEN();
		case KA_MUL:
			toc->type = T_MUL;
			UNGETC_AND_RETURN_TOKEN();

// STRINGOVE LITERALY + ESCAPE SEQ #
// vid konecny automat z ktoreho sa tento shit da pochopit
		case KA_STR_LIT:	//mame '
			str = makeNewString();
			if('\'' == c)	//mame prazdny literal - go STR_LIT_DONE
				state = KA_STR_LIT_DONE;
			else if(ascii(c))	//mame znak v tele literalu 31-127 minus {#,'}
			{
				addChar(str,c);
				state = KA_STR_LIT_INSIDE;
			}
			else
				state = KA_ERR;
			break;

		case KA_STR_LIT_INSIDE:	//sme vnoreni v str. literale
			if('\'' == c)
				state = KA_STR_LIT_DONE; //mame ukoncenie
			else if(ascii(c))
				addChar(str,c);
			else
				state = KA_ERR;
			break;

		case KA_STR_LIT_DONE:	// mame ukoncenie
			if('\'' == c)
			{	// specialny case ked mame dve ''
				// musime pridat ' do stringu a vratit sa do inside
				addChar(str,c);
				state = KA_STR_LIT_INSIDE;
			}
			else if('#' == c)
			{
				parse_escape_seq(&c);
				addChar(str,(char)c);
				state = KA_STR_LIT_INSIDE;
			}
			else
			{
				// inak hotovo
				toc->type = T_STR;
				toc->data.str = str;
				UNGETC_AND_RETURN_TOKEN();
			}
			break;


// INTEGER + REAL LIT
		case KA_INTEGER:	//uz mame cislicu
			if(isdigit(c))
				addChar(str,c);	// 123
			else if ('.' == c)
			{
				addChar(str,c);
				state = KA_INT_DOT;	// 123.
			}
			else if('e' == c)
			{
				addChar(str,c);
				state = KA_REAL_EXP;	// 123e
			}
			else	//mame integer
			{
				toc->type = T_INT;
				toc->data.integer = atoi(str->Value);
				UNGETC_AND_RETURN_TOKEN();
			}	
			break;

		case KA_INT_DOT:
			if(isdigit(c))	//mame bodku '.', musi byt cislo
			{
				addChar(str,c);
				state = KA_REAL;
			}
			else if(c == '.')	//mame X..
			{
				bol_dot_dot = true;
				toc->type = T_INT;
				toc->data.integer = atoi(str->Value);
				freeString(str);
				return toc;
			}
			else
				state = KA_ERR;
			break;

		case KA_REAL:		// uz bola .x
			if(isdigit(c))
				addChar(str,c);
			else if ('e' == c)
			{
				addChar(str,c);
				state = KA_REAL_EXP;
			}
			else
			{
				// success, mame real cislo!!
				toc->type = T_REAL;
				toc->data.real = atof(str->Value);
				UNGETC_AND_RETURN_TOKEN();
			}
			break;
			
		case KA_REAL_EXP:	// mame exponent, moze byt +,- a cislo
			if(isdigit(c))
			{
				addChar(str,c);
				state = KA_REAL_EXP_NUM;
			}
			else if ('-' == c || '+' == c)
			{
				addChar(str,c);
				state = KA_REAL_EXP_PM;
			}
			else
				state = KA_ERR;
			break;

		case KA_REAL_EXP_PM:	// mali sme +-, dalej je povinne cislo
			if(isdigit(c))
			{
				addChar(str,c);
				state = KA_REAL_EXP_NUM;
			}
			else
				state = KA_ERR;
			break;

		case KA_REAL_EXP_NUM:	// mame real,exp,cislo, uz mozu byt len cisla	
			if(isdigit(c))
				addChar(str,c);
			else	//mame real
			{
				toc->type = T_REAL;			
				toc->data.real = atof(str->Value);
				UNGETC_AND_RETURN_TOKEN();
			}
			break;

		case KA_IDENT: // mali sme [_,a-z]
			if (isalnum (c) || (c == '_'))
				addChar(str,c);
			else
			{
				// klicove slova (zoradena dle zadani) 3.1
				// tu bude treba davat bacha na rozlisenie tokenov
				// na klucove slova vztahujuce sa na datove typy
				// T_KW_REAL vs T_REAL atp.

				if(!compareString(str,"begin"))
					toc->type = T_KW_BEGIN;
				else if(!compareString(str,"boolean"))
					toc->type = T_KW_BOOLEAN;
				else if (!compareString(str,"do"))
					toc->type = T_KW_DO;
				else if (!compareString(str,"else"))
					toc->type = T_KW_ELSE;
				else if (!compareString(str,"end"))
					toc->type = T_KW_END;
				else if (!compareString(str,"false"))
					toc->type = T_KW_FALSE;
				else if (!compareString(str,"find"))
					toc->type = T_KW_FIND;
				else if (!compareString(str,"forward"))
					toc->type = T_KW_FORW;
				else if (!compareString(str,"function"))
					toc->type = T_KW_FUNC;
				else if (!compareString(str,"if"))
					toc->type = T_KW_IF;
				else if (!compareString(str,"integer"))
					toc->type = T_KW_INT;
				else if (!compareString(str,"readln"))
					toc->type = T_KW_READLN;
				else if (!compareString(str,"real"))
					toc->type = T_KW_REAL;
				else if (!compareString(str,"sort"))
					toc->type = T_KW_SORT;
				else if (!compareString(str,"string"))
					toc->type = T_KW_STRING;
				else if (!compareString(str,"then"))
					toc->type = T_KW_THEN;
				else if (!compareString(str,"true"))
					toc->type = T_KW_TRUE;
				else if (!compareString(str,"var"))
					toc->type = T_KW_VAR;
				else if(!compareString(str, "until"))
					toc->type = T_KW_UNTIL;
				else if (!compareString(str,"uses"))
					toc->type = T_KW_USES;
				else if (!compareString(str,"while"))
					toc->type = T_KW_WHILE;
				else if (!compareString(str,"write"))
					toc->type = T_KW_WRT;
				else if(!compareString(str, "repeat"))
					toc->type = T_KW_RPT;
				else if(!compareString(str,"do"))
					toc->type = T_KW_DO;
				else if(!compareString(str,"else"))
					toc->type = T_KW_ELSE;
				else if(!compareString(str,"or"))
					toc->type = T_OR;
				else if(!compareString(str,"of"))
					toc->type = T_OF;
				else if(!compareString(str,"and"))
					toc->type = T_AND;
				else if(!compareString(str, "xor"))
					toc->type = T_XOR;
				else if(!compareString(str,"not"))
					toc->type = T_NOT;
				else if(!compareString(str,"mod"))
					toc->type = T_MOD;
				else if(!compareString(str,"do"))
					toc->type = T_KW_DO;
				else if(!compareString(str,"case"))
					toc->type = T_KW_CASE;
				else if(!compareString(str,"for"))
					toc->type = T_KW_FOR;
				else if(!compareString(str,"to"))
					toc->type = T_KW_TO;
				else if(!compareString(str,"downto"))
					toc->type = T_KW_DOWNTO;
				else if(!compareString(str,"copy"))
					toc->type = T_KW_COPY;
				else if(!compareString(str, "length"))
					toc->type = T_KW_LENGTH;
				else if(!compareString(str, "array"))
					toc->type = T_KW_ARRAY;
				else
				{
					toc->type = T_ID;
					toc->data.str = str;
				}
				UNGETC_AND_RETURN_TOKEN();
			}
			break;

		case KA_ERR:
			exit(lex);
		default:
			exit(intern);
			break;
		}
	}
	return toc;
}

int getChar()
{
	int ret = fgetc(global.src);
	if('\n' == ret)
		global.lineCounter++;
	return ret;
}

void unGetChar(char c)
{
	if('\n' == c)
		global.lineCounter--;
	ungetc(c, global.src);
}

void
tocInit(struct toc **toc)
{
	ASSERT(toc);

	struct toc *tmp = gcMalloc(sizeof(struct toc));
	if(!tmp)
		exit(intern);	// todo, error kod
	tmp->type = 0;		// todo: pridat define do headeru?
	
	*(toc) = tmp;
}

// pomocne funkcie
void skipWSandComments()
{
	ASSERT(global.src);
	int c;

	while(EOF != (c = getChar()))
	{
		if(c == '{')
		{
			while(c != '}')
			{	
				c = getChar();
				if(c == EOF)	// neukonceny komentar
					exit(lex);
			}
		}
		else if(!isspace(c))
			break;
	}
	unGetChar(c);
}

int ascii(unsigned char c)
{
	return (
			((c >= 31) && (c <= 127)) &&
			!(c == '#' || c == '\'')) ?
					1 : 0;
}

// funkcie na parsovanie escape sekvencii '#10'
void parse_escape_seq(int *c)
{
	unsigned short num = 0;
	int tmp_c = fgetc(global.src);

	if(!isdigit(tmp_c))
		exit(lex);

	while(isdigit((char)tmp_c))
	{
		num *= 10;
		num += tmp_c - '0';
		if(num > 255)
			exit(lex);
		tmp_c = fgetc(global.src);
	}

	if(num == 0 || tmp_c == EOF || '\'' != tmp_c)
		exit(lex);
	*c = (char) num;
}

// pomocna globalna premenna, funkcia returnTypeAssStr vrati
// string k token typu
struct token2str array[] = {
	{"begin", T_KW_BEGIN },
	{"boolean", T_KW_BOOLEAN },
	{"do", T_KW_DO },
	{"program", T_KW_BEGIN },
	{"else", T_KW_ELSE },
	{"end", T_KW_END },
	{"end.", T_KW_ENDDOT },
	{"false", T_KW_FALSE },
	{"find", T_KW_FIND },
	{"forward", T_KW_FORW },
	{"function", T_KW_FUNC },
	{"if", T_KW_IF },
	{"KW_integer", T_KW_INT },
	{"readln", T_KW_READLN },
	{"KW_real", T_KW_REAL },
	{"sort", T_KW_SORT },
	{"string", T_KW_STRING },
	{"then", T_KW_THEN },
	{"true", T_KW_TRUE },
	{"uses", T_KW_USES },
	{"var", T_KW_VAR },
	{"while", T_KW_WHILE },
	{"write", T_KW_WRT },
	{"of", T_OF },
	{"mod", T_MOD },
	{"and", T_AND },
	{"not", T_NOT },
	{"else", T_KW_ELSE},
	{"mod", T_MOD},
	{"and", T_AND},
	{"or", T_OR},
	{"not", T_NOT},
	{"dot", T_DOT},
	{"scol", T_SCOL},
	{"comma", T_COM},
	{"col", T_COL},
	// literals
	{"T_int", T_INT},
	{"T_real", T_REAL},
	{"T_str", T_STR},
	{"assign",T_ASGN},
	// comparison
	{"greater", T_GRT},
	{"geqv", T_GEQV},
	{"less", T_LSS},
	{"leqv",T_LEQV},
	{"eq",T_EQV},
	{"neqv",T_NEQV},
	// +,-,*,/
	{"add", T_ADD},
	{"sub", T_SUB},
	{"mul", T_MUL},
	{"div", T_DIV},
	// brackets
	{"lpar", T_LPAR},
	{"rpar", T_RPAR},
	{"lbrc", T_LBRC},
	{"rbrc", T_RBRC},
//	{"lcbr", T_LCBR},
//	{"rcbr", T_RCBR},
	//identifikator
	{"identifikator", T_ID},
	// EOF
	{"eof",T_EOF},
	{NULL, 0},
};

const char *returnTypeAsStr(int type)
{
	for(int i=0; array[i].str; i++)
		if(type == array[i].type)
			return array[i].str;
	return NULL;
}
