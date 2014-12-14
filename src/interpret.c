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

#include "interpret.h"

void interpret()
{
	ASSERT(global.program);

	struct astNode *curr = global.program;

	// tabulka symbolov z parsru pouzita ako globalna tabulka - zalohovat pointer
	global.globalTable = stackTop(global.symTable);

	if(!global.globalTable)
		stackPush(global.symTable, makeNewSymbolTable());

	// v roote by mal byt AST_START
	if(curr->type == AST_START)
		runTree(curr);
	else
		exit(intern);
}

void for_to_downto(struct astNode *curr)
{
	struct astNode *body = curr->left;
	struct astNode *cond = curr->right;

	struct astNode *asgn = cond->left;
	struct symbolTableNode *id = runTree(asgn->left);

	struct symbolTableNode *value = runTree(asgn->right);
	struct symbolTableNode *literal = runTree(cond->right);

	if((cond->type == AST_FOR_DOWNTO && value->data.int_data < literal->data.int_data) ||
			(cond->type == AST_FOR_TO && value->data.int_data > literal->data.int_data))
	{
		insertDataInteger(&id, 0);
		return;
	}
	else
		insertDataInteger(&id,value->data.int_data);

	int boundary = literal->data.int_data;
	int *iterator = &(id->data.int_data);

	if(cond->type == AST_FOR_TO)
	{
		while(*iterator <= boundary)
		{
			runTree(body);
			(*iterator)++;
		}
	}
	else if(cond->type == AST_FOR_DOWNTO)
	{
		while(*iterator >= boundary)
		{
			runTree(body);
			(*iterator)--;
		}
	}
	else
		exit(intern);
	*iterator = boundary;
}

void initArray(struct symbolTableNode *arr)
{
	// DT_INT, DT_REAL, DT_BOOL, DT_STR, DT_ARR
	struct dataTypeArray *tmp_arr = arr->other;

	size_t size = (tmp_arr->high - tmp_arr->low + 1);

	struct symbolTableNode template;
	memset(&template,0,sizeof(struct symbolTableNode));
	template.dataType = tmp_arr->type;
	template.init = true;

	tmp_arr->data = gcMalloc(size * sizeof(struct symbolTableNode *));
	if(!tmp_arr->data)
		exit(intern);

	for(unsigned i=0; i < size; i++)
	{
		tmp_arr->data[i] = makeNewSymbolTable();
		memcpy(tmp_arr->data[i], &template, sizeof(template));
	}

	arr->init = true;
}

struct symbolTableNode *getArrayIndex(struct symbolTableNode *tmp, int index)
{
	if(!tmp)
		exit(intern);
	struct dataTypeArray *dta = tmp->other;

	if(index < dta->low || index > dta->high)
		exit(intern);

	int real_index = abs(dta->low - index);

	return dta->data[real_index];
}

void *runTree(struct astNode *curr)
{
	if(!curr)
		exit(intern);

	struct symbolTableNode *left = NULL;
	struct symbolTableNode *right = NULL;
	struct symbolTableNode *tmp = NULL;

	struct varspars *tmp_vp = NULL;
	struct astNode	*tmp_asp = NULL;

//	struct symbolTableNode* top = stackTop(global.symTable);
	struct symbolTableNode* top = global.symTable->Top->Value;
//#define TOP (global.symTable->Top->Value

	switch(curr->type)
	{
	case AST_START:
		ASSERT(curr->left && curr->left->type == AST_CMD);
		runTree(curr->left);

		break;

	case AST_CMD:
		if(curr->right)				//	<command>
			runTree(curr->right);

		if(curr->left && curr->left->type == AST_CMD)	// <command list>
			runTree(curr->left);

		break;

	case AST_ASGN:	// <command>: <assign>
		ASSERT(curr->left && curr->right);

		left = runTree(curr->left);	// x :=
		right = runTree(curr->right);			//ocekavame symtabnode (tmp premenna...)

		if(!right->init)
			exit(run_ninit);

		if(right->dataType != left->dataType)
			exit(sem_komp);

		if(right->dataType == DT_INT)
			insertDataInteger(&left, right->data.int_data);
		else if(right->dataType == DT_STR)
			insertDataString(&left, right->data.str_data);
		else if(right->dataType == DT_REAL)
			insertDataReal(&left, right->data.real_data);
		else if(right->dataType == DT_BOOL)
			insertDataBoolean(&left, right->data.bool_data);
		else if(right->dataType == DT_ARR)
			exit(intern);	//TODO kopirovanie arraya
		else
			exit(sem_komp);

		// ak prava strana nema meno tak sme mali tmp premennu ktoru mozeme rovno smazat
		if(!right->name || (right->name && right->name->Length == 0))
			deleteTable(&right);

		left->init = true;

		return left;
		break;

	case AST_FUNC:
		ASSERT(curr->other && curr->left);
		/*
		 *		Funkce bude uložená v AST_FUNC uzlu, kde levý obsahuje tělo a
		 *		pravý je typu AST_NONE a v položce OTHER má strukturu varsapars
		 *		obsahující frontu pro proměnné a frontu pro parametry.
		 */
		// do tabulky symbolov pridame return premennu
		insertValue(&top,curr->other,curr->dataType);

		// pustime telo funkcie
		runTree(curr->left);
#ifdef _DEBUG	//vypiseme lokalnu tabulku symbolov
		printSymbolTable(top, 0);
#endif
		// vratime return hodnotu ( AST_CALL robi cleanup )
		return searchST(&top, curr->other);
	break;

	case AST_CALL:
		ASSERT(curr->other);
/*	Bude řešeno přes AST_CALL, kde levy poduzel bude prázdný, protože volání neobsahuje tělo
 *  a pravý poduzel bude obsahovat AST_NONE,
 *  kde v OTHER bude uložená struktura varspars s naplněnou položkou pars parametry.
 */
		// vyhledame s v tabulke funkcii pozadovanu funkciu
		// curr->other (struct string *) jmeno_funkcie
		tmp = searchST(&global.funcTable,(struct String *)curr->other);
		ASSERT(tmp);	//teoreticky je toto riesene uz v parsri
		tmp_vp = curr->right->other;	//tu budu parametry
		tmp_asp = tmp->other;

		// nakopirujeme parametre a vars do docasnej lokalnej tabulky symbolov
		right = pushVarsParsIntoTable(
				tmp_vp->pars,
				((struct varspars *)tmp_asp->right->other)->pars,
				((struct varspars *)tmp_asp->right->other)->vars
		);

		// pushneme
		stackPush(global.symTable,right);

		// zavolame funkciu (body)
		tmp = runTree(tmp->other);

		// navratime povodnu tabulku symbolov
		stackPop(global.symTable);

		// vratime navratovu hodnotu funkcie
		return tmp;

		break;

// controll konstrukcie (if,while,repeat..)
	case AST_IF:
		ASSERT(curr->other);

		tmp = runTree(curr->other);	// podmienka body

		if(tmp->data.bool_data == true)	//vyhodnocena true
			runTree(curr->left);
		else if(curr->right)	// vyhodnocena false a mame v branchi else
			runTree(curr->right);


		if(tmp->name && tmp->name->Length == 0)
			deleteTable(&tmp);

		break;

	case AST_WHILE:
		ASSERT(curr->left && curr->other);
		while(true)
		{
			tmp = runTree(curr->other);	// evaluujeme podmienku
			if(!tmp->data.bool_data)
				break;
			else
				deleteTable(&tmp);
			runTree(curr->left);	// ak true, tak bezime telo
		}
		break;

	case AST_REPEAT:
		ASSERT(curr->left && curr->other);
		do
		{
			if(tmp)
				deleteTable(&tmp);
			runTree(curr->left);	// telo
			tmp = runTree(curr->other);	//podmienka
		} while(!tmp->data.bool_data);
		break;

	case AST_FOR:
		ASSERT(curr->left && curr->right);
		for_to_downto(curr);

		return NULL;

	case AST_FOR_DOWNTO:
	case AST_SWITCH:
		exit(intern);	// todo
		break;

// porovnavanie =,!=,<,<=,>,>=
	case AST_EQV:
	case AST_NEQV:
	case AST_GRT:
	case AST_LSS:
	case AST_GEQV:
	case AST_LEQV:
		ASSERT(curr->left && curr->right);

		// ziskame levu & pravu stranu porovnania
		left = runTree(curr->left);
		right = runTree(curr->right);

		if(!left->init || !right->init)
			exit(run_ninit);

		// pripravime tmp premennu typu bool na vysledek
		tmp = makeNewSymbolTable();
		ASSERT(tmp);
		insertDataBoolean(
				&tmp,
				compare(left,right,curr->type)	//porovnavame a (op) b
		);
		return tmp;

// aritmeticke operace +,-,/,*
	case AST_ADD:
	case AST_SUB:
	case AST_MUL:
	case AST_DIV:
		ASSERT(curr->left && curr->right);

		left = runTree(curr->left);		// *a* (op) b
		right = runTree(curr->right);	// a (op) *b*

		if(!left->init || !right->init)
			exit(run_ninit);

		// spravime aritmeticku operaciu
		// vraci tmp premennu v symtab node
		return arithmetic(left,right,curr->type);

// logicke operace
	// tieto su binarne
	case AST_AND:
	case AST_OR:
	case AST_XOR:
		ASSERT(curr->left && curr->right);

		right = runTree(curr->right);
		left = runTree(curr->left);

		if(!left->init || !right->init)
			exit(run_ninit);

		tmp = makeNewSymbolTable();
		ASSERT(tmp);
		insertDataBoolean(
				&tmp,
				(curr->type == AST_AND) ? (left->data.bool_data && right->data.bool_data):
				(curr->type == AST_OR)	? (left->data.bool_data || right->data.bool_data):
				(curr->type == AST_XOR) ? (left->data.bool_data ^ right->data.bool_data): false
		);
		return tmp;

	// not je unarna operacia
	case AST_NOT:
		assert(curr && curr->left && !curr->right);

		left = runTree(curr->left);

		if(!left->init)
			exit(run_ninit);

		tmp = makeNewSymbolTable();
		ASSERT(tmp);
		insertDataBoolean(&tmp,	!(left->data.bool_data));
		return tmp;

// unarne operacie, vracaju 'tokeny' ast (premenne, int, bool,real...)
	case AST_ID:
		ASSERT(curr->data.str && curr->data.str->Length > 0);
		// mame identifikator, hladame v tabulke symbolov
		if(curr->data.str)
			return searchST(&top,curr->data.str);
		else
			exit(intern);

	case AST_INT:
		// pridavame novu lokalnu premennu do tabulky a priradime jej hodnotu z node
		tmp = makeNewSymbolTable();
		ASSERT(tmp);
		insertDataInteger(&tmp,curr->data.integer);
		return tmp;

	case AST_REAL:
		tmp = makeNewSymbolTable();
		ASSERT(tmp);
		insertDataReal(&tmp, curr->data.real);
		return tmp;

	case AST_BOOL:
		tmp = makeNewSymbolTable();
		ASSERT(tmp);
		insertDataBoolean(&tmp, curr->data.boolean);
		return tmp;

	case AST_STR:
		tmp = makeNewSymbolTable();
		ASSERT(tmp);
		insertDataString(&tmp, curr->data.str);
		return tmp;

	case AST_ARR:
/*
 * Pokazde je pouzit uzel AST_ARR, u definice jsou synovske uzly prazdne (NULL) a data.str
 * obsahuje jmeno promenne. V polozce other se nachazi dataTypeArray struktura,
 * ve ktere jsou informace o rozsahu pole, atd.
 *
 * Tato struktura se kopiruje odkazem i do indexovaciho uzlu.
 * Ten ma v levem synovskem uzlu AST_ID a v pravem AST_INT (nebo AST_ID ale s datovym typem INT),
 * protoze indexovat se muze pouze celociselnym literalem.
 *
 * Datovy typ dataType je v tabulce symbolu DT_ARR, ale v astNode je urcen datovym typem obsahu,
 * takze pokud je obsah typu int, AST_ARR ma datovy typ DT_INT.
 */
		tmp = searchST(&top,curr->left->data.str);

		if(!tmp->init)
			initArray(tmp);

		//index [ID,INT]
		right = runTree(curr->right);
		if(right->dataType != DT_INT)
			exit(intern);

		return getArrayIndex(tmp, right->data.int_data);

		break;

	case AST_WRITE:
		tmp_vp = curr->right->other;
		for(struct queueItem *p=tmp_vp->pars->start; p; )
		{
			writeNode(p->value);
			p = ((struct queueItem*) p)->next;
		}
		break;

	case AST_READLN:	// readln(a);
		ASSERT(curr->right && curr->right->type == AST_NONE);

		// ignorovat hate nizsie, vznikol po 3 hodinach debugovania @4AM
		tmp_vp = curr->right->other;			// vyrvem si varspars
		tmp_asp = tmp_vp->pars->start->value;	// vyrvem z toho ast node
		// a z vyjebaneho ast_node->other vyjebem dojebany string ktory este vyhladam v tabulke
		tmp = searchST(&top, tmp_asp->data.str);
		// a po tomto celom bullshite este zavolam funkciu ktora zapise do premennej
		readNode(tmp);
		break;

	case AST_COPY:
		ASSERT(curr->right && curr->right->type == AST_NONE);

		tmp_vp = curr->right->other;			// vyrvem si varspars
		return btnCopy(tmp_vp->pars);

	case AST_FIND:
		ASSERT(curr->right && curr->right->type == AST_NONE);

		tmp_vp = curr->right->other;			// vyrvem si varspars
		return btnFind(tmp_vp->pars);

	case AST_LENGTH:
		ASSERT(curr->right && curr->right->type == AST_NONE);

		tmp_vp = curr->right->other;			// vyrvem si varspars
		return btnLength(tmp_vp->pars);

	case AST_SORT:
		ASSERT(curr->right && curr->right->type == AST_NONE);

		tmp_vp = curr->right->other;			// vyrvem si varspars
		return btnSort(tmp_vp->pars);

	default:
		ASSERT(false);
		exit(intern);
	}
	return NULL;
}

// hladanie v tabulke symbolov
// najprv prehladame lokalnu (funkcia), nasledne tu globalnu
// navrati NULL ak sme nic nenasli, inak node
struct symbolTableNode* searchST(struct symbolTableNode** table, struct String* name)
{
	ASSERT(global.globalTable);

	struct symbolTableNode* tmp = search(table, name);
	if(!tmp) // ak sme nenasli v lokalnej tabulke, tak sa pozrieme do globalu
		tmp = search(&(global.globalTable), name);
	return tmp;
}

// porovnavame dve premenne rovnakeho typu
// prerekvizita je uplne definovany typ, cize treba odpalit runTree() na operandy
bool compare(struct symbolTableNode *left,struct symbolTableNode *right,int op)
{
	ASSERT(left && right);

	if(!left->init || !right->init)
		exit(run_ninit);

	// ak nie je rovnaky typ tak zarveme chybu (teoreticky to failne uz v parsri)
	if(left->dataType != right->dataType)
		exit(4);

	if(left->dataType == DT_INT)
	{
		switch(op)
		{
		case AST_GRT:
			return (left->data.int_data > right->data.int_data) ? true : false;
		case AST_GEQV:
			return (left->data.int_data >= right->data.int_data) ? true : false;
		case AST_LSS:
			return (left->data.int_data < right->data.int_data) ? true : false;
		case AST_LEQV:
			return (left->data.int_data <= right->data.int_data) ? true : false;
		case AST_EQV:
			return (left->data.int_data == right->data.int_data) ? true : false;
		case AST_NEQV:
			return (left->data.int_data != right->data.int_data) ? true : false;
		default:
			exit(intern);
		}
	}
	else if(left->dataType == DT_REAL)
	{
		switch(op)
		{
		case AST_GRT:
			return (left->data.real_data > right->data.real_data) ? true : false;
		case AST_GEQV:
			return (left->data.real_data >= right->data.real_data) ? true : false;
		case AST_LSS:
			return (left->data.real_data < right->data.real_data) ? true : false;
		case AST_LEQV:
			return (left->data.real_data <= right->data.real_data) ? true : false;
		case AST_EQV:
			return (left->data.real_data == right->data.real_data) ? true : false;
		case AST_NEQV:
			return (left->data.real_data != right->data.real_data) ? true : false;
		default:
			exit(intern);
		}
	}
	else if(left->dataType == DT_STR)
	{
		ASSERT(op == AST_EQV || op == AST_NEQV);
		if(op == AST_EQV)
			return (compareStrings(left->data.str_data, right->data.str_data)) ?
						false : true;
		else	// (op == AST_NEQV)
			return (compareStrings(left->data.str_data, right->data.str_data)) ?
						true: false;
	}
	else if(left->dataType == DT_BOOL)
	{
		ASSERT(op == AST_EQV || op == AST_NEQV);
		if(op == AST_EQV)
			return (left->data.bool_data == right->data.bool_data) ? true : false;
		else // if(op == AST_NEQV)
			return (left->data.bool_data != right->data.bool_data) ? true : false;
	}
	else
		exit(intern);	// ak sme sa dostali sem tak sa nieco vazne dosralo

	return false;	// umlcanie warningu, program sa sem nikdy nedostane (exit(intern))
}



//podobne ako pri compare, tuna robime aritmeticke operacie
// rozdiel je ze vracime dynamicky alokovanu tmp premennu
struct symbolTableNode *arithmetic(struct symbolTableNode *left,struct symbolTableNode *right,int op)
{
	ASSERT(left && right);
	//pomocna premenna node
	struct symbolTableNode *tmp = makeNewSymbolTable();
	if(!tmp)
		exit(intern);

	if(!left->init || !right->init)
		exit(run_ninit);

	// pomocna premenna type, budeme musiet rozlisovat typy
	int type;

	if(op == AST_DIV)	//implicitne vysledok divisionu je REAL
		type = DT_REAL;
	else if(DT_INT == left->dataType && DT_INT == right->dataType)
		type = DT_INT;
	else if(DT_STR == left->dataType && DT_STR == right->dataType)
		type = DT_STR;
	else if(	//ak je jeden z typov real a druhy int, vysledok bude real
			(DT_REAL == left->dataType && DT_INT == right->dataType) ||
			(DT_REAL == right->dataType && DT_INT == left->dataType) ||
			(DT_REAL == right->dataType && DT_REAL == left->dataType) )
		type = DT_REAL;
	else
		exit(4);	//jinak mame chybu v typoch, toto by malo byt osetrena v parsri ?

	// ulozime si vysledny typ
	tmp->dataType = type;

	switch(op)
	{
	case AST_ADD:
		if(type == DT_INT)
			insertDataInteger(&tmp, left->data.int_data + right->data.int_data);
		else if(type == DT_REAL)
		{
			//fugly workaround okolo faktu ze nemam pekne makra na zistenie typu z unionu
			if(left->dataType == DT_INT)
				insertDataReal(&tmp, ((double)left->data.int_data + right->data.real_data));
			else if(right->dataType == DT_INT)
				insertDataReal(&tmp, (left->data.real_data + (double)right->data.int_data));
			else
				insertDataReal(&tmp, (left->data.real_data + right->data.real_data));
		}
		else if(type == DT_STR)
		{
			struct String *tmp_str = makeNewString();
			copyString(left->data.str_data,&tmp_str);
			for(unsigned i=0; i < (strlen(right->data.str_data->Value)); i++)
				addChar(tmp_str, right->data.str_data->Value[i]);
			insertDataString(&tmp,tmp_str);
		}
		else
			exit(intern);
		break;

	case AST_SUB:
		if(type == DT_INT)
			insertDataInteger(&tmp, left->data.int_data - right->data.int_data);
		else if(type == DT_REAL)
		{
			//fugly workaround okolo faktu ze nemam pekne makra na zistenie typu z unionu
			if(left->dataType == DT_INT)
				insertDataReal(&tmp, ((double)left->data.int_data - right->data.real_data));
			else if(right->dataType == DT_INT)
				insertDataReal(&tmp, (left->data.real_data - (double)right->data.int_data));
			else
				insertDataReal(&tmp, (left->data.real_data - right->data.real_data));
		}
		else
			exit(intern);
		break;

	case AST_MUL:
		if(type == DT_INT)
			insertDataInteger(&tmp, left->data.int_data * right->data.int_data);
		else //if(type == DT_REAL)
		{
			//fugly workaround okolo faktu ze nemam pekne makra na zistenie typu z unionu
			if(left->dataType == DT_INT)
				insertDataReal(&tmp, (((double)left->data.int_data) * right->data.real_data));
			else if(right->dataType == DT_INT)
				insertDataReal(&tmp, (((double)right->data.int_data) * left->data.real_data));
			else
				insertDataReal(&tmp, right->data.real_data * left->data.real_data);
		}
		break;

	case AST_DIV:
		if(right->dataType == DT_INT)	// (../int)
		{
			if(right->data.int_data == 0)
				exit(run_div);

			if(left->dataType == DT_INT) // (int / int)
				insertDataReal(&tmp, ((double)left->data.int_data / (double) right->data.real_data));
			else	// (real/int)
				insertDataReal(&tmp, (left->data.real_data / (double)right->data.int_data));
		}
		else	// (../real)
		{
			//typecast do floatu, pretoze vo floate je nula
			//reprezentovana jedinou hodnotou
			if((float)right->data.real_data == (float)0)
				exit(run_div);
			
			if(left->dataType == DT_INT)	// (int/real)
				insertDataReal(&tmp, ((double)left->data.int_data / right->data.real_data));
			else	//(real/real)
				insertDataReal(&tmp, (left->data.real_data / right->data.real_data));
		}
		break;
	}

	// vratime tmp premennu s vysledkom operace
	return tmp;
}

/*
 * Pomocna funkcia AST_WRITE, dostane symtab node a vypise ho
 */

void writeNode(struct astNode *p)
{
	ASSERT(p);

	struct symbolTableNode *id = NULL;

	// ak dostaneme ID tak odpalime search a vratime lokalnu verziu premennej
	// kedze nedostavame symtab ale ast tak robime cely tento shit .... fuck
	if(AST_ID == p->type || p->type == AST_ARR)
	{
//		struct symbolTableNode *top = stackTop(global.symTable);
//		id = searchST(&top,p->data.str);

		id = runTree(p);

		// nedefinovana premenna???
		if(!id || !id->init)
			exit(run_ninit);

		p = makeNewAST();

		// konverzia symtab typy na AST typy
		if(id->dataType == DT_STR)
		{
			p->type = AST_STR;
			p->data.str = id->data.str_data;
		}
		else if(id->dataType == DT_INT)
		{
			p->type = AST_INT;
			p->data.integer = id->data.int_data;
		}
		else if(id->dataType == DT_REAL)
		{
			p->type = AST_REAL;
			p->data.real = id->data.real_data;
		}
		else if(id->dataType == DT_BOOL)
		{
			p->type = AST_BOOL;
			p->data.boolean = id->data.bool_data;
		}
		else
			exit(intern);	// shit is going down
	}

	if(AST_STR == p->type)
	{
		ASSERT(p->data.str->Value);
		printf("%s",p->data.str->Value);
	}
	else if (AST_REAL == p->type)
		printf("%g",p->data.real);
	else if(AST_INT == p->type)
		printf("%d",p->data.integer);
	else if(AST_BOOL)
	{
		if(p->data.boolean)
			printf("true");
		else
			printf("false");
	}
	else
		ASSERT(false);
}

// citame zo vstupu (cosi) a potom to ulozime do
// TODO: kontroly korektnosti vstupu
void readNode(struct symbolTableNode *p)
{
	struct String *tmp = makeNewString();
	int c;

	int ret = 0;

	c = fgetc(stdin);
	if(c == EOF)
		exit(6);
	else
		ungetc(c,stdin);

	p->init = true;

	if(p->dataType == DT_STR)
	{
		while(((c = fgetc(stdin)) != EOF) && c != '\n')
			addChar(tmp,c);

		p->data.str_data = tmp;
	}
	else if(p->dataType == DT_REAL)
	{
		// skip whitespace
		c = fgetc(stdin);
		while(isspace(c))
			c = fgetc(stdin);
		// check EOF

		if(c == '-')
		{
			addChar(tmp, c);
			c = fgetc(stdin);
		}

		if(c  == EOF || !isdigit(c))
			exit(6);
		do
		{
			addChar(tmp, c);
			c = fgetc(stdin);
		}	// while podmienka je fugly ale teoreticky spravne
		while(isdigit(c) || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E');

		ret = sscanf(tmp->Value,"%lg", &(p->data.real_data));	//todo chybove vstupy
		if(!ret)
			exit(6);
	}
	else if(p->dataType == DT_INT)
	{
		// skip whitespace
		c = fgetc(stdin);
		while(isspace(c))
			c = fgetc(stdin);
		// check EOF

		if(c == '-')
		{
			addChar(tmp, c);
			c = fgetc(stdin);
		}

		if(c  == EOF || !isdigit(c))
			exit(6);
		do
		{
			addChar(tmp, c);
			c = fgetc(stdin);
		}
		while(isdigit(c));


		ret = sscanf(tmp->Value,"%d", &(p->data.int_data));	//todo chybove vstupy
		if(!ret)
			exit(6);
	}
	else if(p->dataType == DT_BOOL)
		exit(4);
	else
		ASSERT(false);
}

// konvertuje astNode na sTN (typicke pouzitie v stringovych literaloch a AST_ID)
struct symbolTableNode convertAST2STN(struct astNode *ast)
{
	struct symbolTableNode tmp = {0};

	tmp.init = true;

	if(ast->type == AST_ID || ast->type == AST_ARR)
	{
		struct symbolTableNode *p = runTree(ast);

		if(!p)
			exit(intern);

		if(!p->init)
			exit(run_ninit);

		tmp = *p;
		return tmp;
	}

	if(ast->type == AST_INT)
	{
		tmp.dataType = DT_INT;
		tmp.data.int_data = ast->data.integer;
	}
	else if(ast->type == AST_REAL)
	{
		tmp.dataType = DT_REAL;
		tmp.data.real_data = ast->data.real;
	}
	else if(ast->type == AST_BOOL)
	{
		tmp.dataType = DT_BOOL;
		tmp.data.bool_data = ast->data.boolean;
	}
	else if(ast->type == AST_STR)
	{
		tmp.dataType = DT_STR;
		tmp.data.str_data = ast->data.str;
	}
	else
		ASSERT(false);
	return tmp;
}

// pomocna funkcia pre  volanie funkcii, vytvori novu tabulku symbolov do ktorej nakopiruje
// jednotlive parametre / vytvori premenne
struct symbolTableNode *pushVarsParsIntoTable(
		struct queue *call_params,
		struct queue *function_params,
		struct queue *function_vars)
{
	ASSERT(call_params && function_params);

	struct symbolTableNode *table = makeNewSymbolTable();
	struct symbolTableNode *new = NULL;
	struct astNode *ast_src, *ast_dest;

	struct queueItem *currCPItem = call_params->start;
	ASSERT(function_params);
	struct queueItem *currFPItem = function_params->start;

	static struct symbolTableNode stn_src;
	static struct symbolTableNode stn_dest;

	// do docasnej tabulky symbolov prekladame parametry z volania
	while(currCPItem && currFPItem)
	{
		ast_src = currCPItem->value;
		ast_dest = currFPItem->value;

		stn_src = convertAST2STN(ast_src);
		//		stn_dest = convertAST2STN(ast_dest);

		if(ast_dest->type == AST_ID)
		{
			stn_dest.name = ast_dest->data.str;
			stn_dest.dataType = ast_dest->dataType;
		}
		else
			stn_dest = convertAST2STN(ast_dest);

		if(stn_src.dataType == stn_dest.dataType)
		{
			new = insertValue(&table, ast_dest->data.str, stn_src.dataType);
			if(stn_src.dataType == DT_INT)
				insertDataInteger(&new, stn_src.data.int_data);
			else if(stn_src.dataType == DT_BOOL)
				insertDataBoolean(&new, stn_src.data.bool_data);
			else if(stn_src.dataType == DT_REAL)
				insertDataReal(&new, stn_src.data.real_data);
			else if(stn_src.dataType == DT_STR)
				insertDataString(&new, stn_src.data.str_data);
		}
		else
			exit(run_else);

		currCPItem = currCPItem->next;
		currFPItem = currFPItem->next;
	}

	// do docasnej tabulky symbolov narveme vars
	if(function_vars)
	{
		struct queueItem *varsQueue = function_vars->start;
		while(varsQueue)
		{
			ast_src = varsQueue->value;

			ASSERT(ast_src->type == AST_ID);

			new = insertValue(&table, ast_src->data.str, ast_src->dataType);

			ASSERT(varsQueue != varsQueue->next);

			varsQueue = varsQueue->next;
		}
	}
	return table;
}

struct symbolTableNode *btnCopy(struct queue *pars)
{
	struct symbolTableNode *res = makeNewSymbolTable();

	struct astNode *p0 = pars->start->value;
	struct astNode *p1 = pars->start->next->value;
	struct astNode *p2 = pars->start->next->next->value;

	struct symbolTableNode p0node = convertAST2STN(p0);
	struct symbolTableNode p1node = convertAST2STN(p1);
	struct symbolTableNode p2node = convertAST2STN(p2);


	res->data.str_data = copy(
			p0node.data.str_data,
			p1node.data.int_data,
			p2node.data.int_data
	);
	res->init = true;
	res->dataType = DT_STR;
	return res;
}

struct symbolTableNode *btnSort(struct queue *pars)
{
	struct symbolTableNode *res = makeNewSymbolTable();

	struct astNode *p0 = pars->start->value;
	struct symbolTableNode p0node = convertAST2STN(p0);

	insertDataString(&res,sort(p0node.data.str_data));
	return res;
}

struct symbolTableNode *btnFind(struct queue *pars)
{
	struct astNode *p0 = pars->start->value;
	struct astNode *p1 = pars->start->next->value;

	struct symbolTableNode p0node = convertAST2STN(p0);
	struct symbolTableNode p1node = convertAST2STN(p1);

	struct symbolTableNode *res = makeNewSymbolTable();

	insertDataInteger(&res,find(p0node.data.str_data,p1node.data.str_data));
	return res;
}

struct symbolTableNode *btnLength(struct queue *pars)
{
	struct symbolTableNode *res = makeNewSymbolTable();

	struct astNode *p0 = pars->start->value;
	struct symbolTableNode p0node = convertAST2STN(p0);

	insertDataInteger(&res,length(p0node.data.str_data));
	return res;
}
