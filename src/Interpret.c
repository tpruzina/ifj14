#include "Interpret.h"

int interpret()
{
	ASSERT(global.program);

	// TODO: sem pridat kontrolu na to, aby bola kazda funkcia riadne definovana
	//printAst(global.funcTable);

	struct astNode *curr = global.program;

	if(curr->type == AST_START)
		runTree(curr);
	else
		exit(intern);
	
	return 0;
}

// porovnavame dve premenne rovnakeho typu
// prerekvizita je uplne definovany typ, cize treba odpalit runTree() na operandy
bool compare(struct symbolTableNode *left,struct symbolTableNode *right,int op)
{
	ASSERT(left && right);

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
	else if(left->dataType == DT_BOOL)
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
		exit(intern);	//todo
	else if(left->dataType == DT_BOOL)
		exit(intern);	//todo
	else
		exit(intern);	// ak sme sa dostali sem tak sa nieco vazne dosralo
}



//podobne ako pri compare, tuna robime aritmeticke operacie
// rozdiel je ze vracime dynamicky alokovanu tmp premennu
struct symbolTableNode *arithmetic(struct symbolTableNode *left,struct symbolTableNode *right,int op)
{
	//pomocna premenna node
	struct symbolTableNode *tmp = makeNewSymbolTable();
	if(!tmp)
		exit(intern);
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
			(DT_REAL == right->dataType && DT_INT == left->dataType))
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
			// konkatenace
			exit(intern);
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
		else if(type == DT_REAL)
		{
			//fugly workaround okolo faktu ze nemam pekne makra na zistenie typu z unionu
			if(left->dataType == DT_INT)
				insertDataReal(&tmp, (((double)left->data.int_data) * right->data.real_data));
			else if(right->dataType == DT_INT)
				insertDataReal(&tmp, (((double)right->data.int_data) * left->data.real_data));
			else
				insertDataReal(&tmp, right->data.real_data * left->data.real_data);
		}
		else
			exit(intern);
		break;

	case AST_DIV:
		if(left->dataType == DT_INT)
			insertDataReal(&tmp, ((double)left->data.int_data / right->data.real_data));
		else if(right->dataType == DT_INT)
			insertDataReal(&tmp, (left->data.real_data / (double)right->data.int_data));
		else
			insertDataReal(&tmp, (left->data.real_data / right->data.real_data));
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
	if(AST_ID == p->type)
	{
		struct symbolTableNode *top = stackTop(global.symTable);
		id = search(&top,p->other);
		// nedefinovana premenna???
		if(!p)
			ASSERT(false);

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
	else
		ASSERT(false);
}

void readNode(struct symbolTableNode *p)
{
	struct String *tmp = makeNewString();
	int c;

	if(p->dataType == DT_STR)
	{
		while((c = fgetc(stdin)) != EOF && c != '\n')
			addChar(tmp,c);
		p->data.str_data = tmp;
	}
	else if(p->dataType == DT_REAL)
	{
		while((c = fgetc(stdin)) != EOF && c != '\n')
		{
			if(c != '\t' && c != ' ')
				addChar(tmp,c);
		}
		sscanf(tmp->Value,"%lg", &(p->data.real_data));	//todo chybove vstupy
	}
	else if(p->dataType == DT_INT)
	{
		while((c = fgetc(stdin)) != EOF && c != '\n')
		{
			if(c != '\t' && c != ' ')
				addChar(tmp,c);
		}
		sscanf(tmp->Value,"%d", &(p->data.int_data));	//todo chybove vstupy
	}
	else if(p->dataType == DT_BOOL)
		exit(4);
	else
		ASSERT(false);	// fuck you
}

/*
 * Tuna nam budu funkcie queuePop nahovno, pretoze nerobime kopiu funkcie ->
 * vsetko treba pridavat do tabulky symbolov rucne
 */
//#define offsetof(st, m) ((size_t)(&((st *)0)->m))
#define offsetof(type, member)  __builtin_offsetof (type, member)

#ifdef __GNUC__
#define member_type(type, member) __typeof__ (((type *)0)->member)
#else
#define member_type(type, member) const void
#endif

#define container_of(ptr, type, member) ((type *)( \
    (char *)(member_type(type, member) *){ ptr } - offsetof(type, member)))

struct symbolTableNode *convertAST2STN(struct astNode *ast)
{
	struct symbolTableNode *tmp = makeNewSymbolTable();

	if(ast->type == AST_ID)
		return runTree(ast);

	if(ast->type == AST_INT)
	{
		tmp->dataType = DT_INT;
		tmp->data.int_data = ast->data.integer;
	}
	else if(ast->type == AST_REAL)
	{
		tmp->dataType = DT_REAL;
		tmp->data.real_data = ast->data.real;
	}
	else if(ast->type == AST_BOOL)
	{
		tmp->dataType = DT_BOOL;
		tmp->data.bool_data = ast->data.boolean;
	}
	else if(ast->type == AST_STR)
	{
		tmp->dataType = DT_STR;
		tmp->data.str_data = ast->data.str;
//		tmp->data.str_data = makeNewString();
//		copyString(ast->data.str, tmp->data.str_data);
	}
	else
		ASSERT(false);
	return tmp;
}

struct symbolTableNode *pushVarsParsIntoTable(
		struct queue *call_params,
		struct queue *function_params,
		struct queue *function_vars)
{
	struct symbolTableNode *table = makeNewSymbolTable();

	struct queueItem *currCPItem = call_params->start;
	struct queueItem *currFPItem = function_params->start;

	struct astNode *ast_src, *ast_dest;
	struct symbolTableNode *stn_src, *stn_dest;

//	static struct symbolTableNode stn_src_static;
//	static struct symbolTableNode stn_dest_static;

	// do docasnej tabulky symbolov prekladame parametry z volania
	while(currCPItem && currFPItem)
	{
		ast_src = currCPItem->value;
		ast_dest = currFPItem->value;

		stn_src = convertAST2STN(ast_src);
		stn_dest = convertAST2STN(ast_dest);

		// ak je callee typ parametru zhodny s callerom
		if(stn_src->dataType == stn_dest->dataType)
		{
			stn_dest = insertValue(
					&table,	// do docasnej tabulky...
					ast_dest->other,	// pridaj pod menom...
					stn_src->dataType		// a typom...
			);
			//hodnotu
			if(stn_src->dataType == DT_INT)
				insertDataInteger(&stn_dest, stn_src->data.int_data);
//			else if

		}
		else
			exit(run_else);

		currCPItem = currCPItem->next;
		currFPItem = currFPItem->next;
	}

	// do docasnej tabulky symbolov narveme vars
	struct queueItem *varsQueue = function_vars->start;

	// rovnaky loop ako vyssie
	while(varsQueue)
	{
		ast_src = varsQueue->value;
		stn_src = convertAST2STN(ast_src);
		stn_dest = insertValue(
					&table,
					ast_src->other,
					stn_src->dataType
		);
		// toto je teoreticky zbytocne, pretoze vo vars neinicializujeme data???? TODO
		if(stn_src->dataType == DT_INT)
			insertDataInteger(&stn_dest,stn_src->data.int_data);

		varsQueue = varsQueue->next;
	}


	return table;
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

	// stack top je cheap operace, nech to nemusime riesit v KA, bude to v kazdej iteracii - resp. TODO
	struct symbolTableNode* top = stackTop(global.symTable);

	switch(curr->type)
	{
	case AST_START:
		ASSERT(curr->left && curr->left->type == AST_CMD);
		runTree(curr->left);

		break;

	case AST_CMD:
		if(curr->right)				//	<command>
			right = runTree(curr->right);

		if(curr->left && curr->left->type == AST_CMD)	// <command list>
			return runTree(curr->left);

		break;

	case AST_ASGN:	// <command>: <assign>
		ASSERT(	curr->left &&
				curr->left->type == AST_ID &&
				curr->right
		);

		left = runTree(curr->left);	// x :=
		// ak je left null tak mame neinicializovanu premennu - todo: check!!!
		right = runTree(curr->right);			//ocekavame symtabnode (tmp premenna...)

		if(right->dataType == DT_INT && left->dataType == DT_INT)
			insertDataInteger(&left, right->data.int_data);
		else if(right->dataType == DT_STR && left->dataType == DT_STR)
			insertDataString(&left, right->data.str_data);
		else if(right->dataType == DT_REAL && left->dataType == DT_REAL)
			insertDataReal(&left, right->data.real_data);
		else
			exit(sem_komp);

		// ak prava strana nema meno tak sme mali tmp premennu ktoru mozeme rovno smazat
		if(!right->name->Value)
			deleteTable(&right);

		return left;
		break;

// toto z pohladu interpretu potrebne nebude

	case AST_FUNC:
/*
 *		Funkce bude uložená v AST_FUNC uzlu, kde levý obsahuje tělo a
 *		pravý je typu AST_NONE a v položce OTHER má strukturu varsapars
 *		obsahující frontu pro proměnné a frontu pro parametry.
 */
//		ASSERT(curr->left);		//telo
//		ASSERT(curr->right);	// none->other (varspars)

		insertValue(&top,curr->other,curr->dataType);

		printSymbolTable(top, 0);

		runTree(curr->left);

		return  search(&top, curr->other);

	break;

	case AST_CALL:
/*	Bude řešeno přes AST_CALL, kde levy poduzel bude prázdný, protože volání neobsahuje tělo
 *  a pravý poduzel bude obsahovat AST_NONE,
 *  kde v OTHER bude uložená struktura varspars s naplněnou položkou pars parametry.
 */
		// vyhledame s v tabulke funkcii pozadovanu funkciu
		// curr->other (struct string *) jmeno_funkcie
		tmp = search(&global.funcTable,(struct String *)curr->other);
		ASSERT(tmp);	//teoreticky je toto riesene uz v parsri
		tmp_vp = curr->right->other;	//tu budu parametry
		tmp_asp = tmp->other;	//telo: todo smazat

		// vytvorime si novu lokalnu tabulky symbolov
//		stackPush(global.symTable, makeNewSymbolTable());

		// nakopirujeme parametre a vars do docasnej lokalnej tabulky symbolov
		right = pushVarsParsIntoTable(
				tmp_vp->pars,
				((struct varspars *)tmp_asp->right->other)->pars,
				((struct varspars *)tmp_asp->right->other)->vars
		);

		// pushneme
		stackPush(global.symTable,right);

		// zavolame funkciu
		tmp = runTree(tmp->other);

		// navratime povodnu tabulku symbolov
		stackPop(global.symTable);

		// vratime navratovu hodnotu funkcie
		return tmp;

		break;

// controll konstrukcie (if,while,repeat..)
	case AST_IF:
		tmp = runTree(curr->other);	// podmienka body

		if(tmp->data.bool_data == true)	//vyhodnocena true
			return runTree(curr->left);
		else if(curr->right)	// vyhodnocena false a mame v branchi else
			return runTree(curr->right);

		break;

	case AST_WHILE:
		ASSERT(curr->left && curr->other);
		while(true)
		{
			tmp = runTree(curr->other);	// evaluujeme podmienku

			if(tmp->data.bool_data)
				runTree(curr->left);	// ak true, tak bezime telo
			else
				break;
		}
		break;

	case AST_FOR:
	case AST_FOR_TO:
	case AST_FOR_DOWNTO:
	case AST_SWITCH:
	case AST_REPEAT:
		exit(intern);	//todo
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

		// pripravime tmp premennu typu bool na vysledek
		tmp = makeNewSymbolTable();
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

		tmp = makeNewSymbolTable();
		insertDataBoolean(
				&tmp,
				(curr->type == AST_AND) ? (left->data.bool_data && right->data.bool_data):
				(curr->type == AST_OR)	? (left->data.bool_data || right->data.bool_data):
				(curr->type == AST_XOR) ? (left->data.bool_data ^ right->data.bool_data): false
		);
		return tmp;

	// not je unarna operacia
	case AST_NOT:
		assert(curr->left && !curr->right);

		left = runTree(curr->left);

		tmp = makeNewSymbolTable();
		insertDataBoolean(&tmp,	!(left->data.bool_data));
		return tmp;

// unarne operacie, vracaju 'tokeny' ast (premenne, int, bool,real...)
	case AST_ID:
		// mame identifikator, hladame v tabulke symbolov
		return search(&top,curr->data.str);

	case AST_INT:
		// pridavame novu lokalnu premennu do tabulky a priradime jej hodnotu z node
		tmp = makeNewSymbolTable();
		insertDataInteger(&tmp,curr->data.integer);
		return tmp;

	case AST_REAL:
		tmp = makeNewSymbolTable();
		insertDataReal(&tmp, curr->data.real);
		return tmp;

	case AST_BOOL:
		tmp = makeNewSymbolTable();
		insertDataBoolean(&top, curr->data.boolean);
		return tmp;

	case AST_STR:
		tmp = makeNewSymbolTable();
		insertDataString(&top, curr->data.str);
		return tmp;

	case AST_ARR:
		// AST_ARR má v položce other uloženou strukturu dataTypeArray,
		// která obsahuje hlavní informace o poli - meze, datový typ a jméno pole.
		tmp = makeNewSymbolTable();
		break;

	//zatial neimplementovane builtin funkce
	case AST_WRITE:
		tmp_vp = curr->right->other;
		for(struct queueItem *p=tmp_vp->pars->start; p; )
		{
			writeNode(p->value);
			p = ((struct queueItem*) p)->next;
		}
		break;

	case AST_READLN:	// readln(a);
		ASSERT(curr);
		ASSERT(curr->right && curr->right->type == AST_NONE);


		tmp_vp = curr->right->other;			// vyrvem si varspars
		tmp_asp = tmp_vp->pars->start->value;	// vyrvem z toho ast node
		// a z vyjebaneho ast_node->other vyjebem dojebany string ktory este vyhladam v tabulke
		tmp = search(&top, tmp_asp->other);
		// a po tomto celom bullshite este zavolam funkciu ktora zapise do premennej
		readNode(tmp);

		break;

	case AST_COPY:
	case AST_LENGTH:
	case AST_FIND:
	case AST_SORT:
		exit(intern);
		break;

	// tieto veci by sa do interpretu nemali dostat
	case AST_NUM:
	case AST_END:
	case AST_NONE:
	default:
		ASSERT(false);
	}
	return NULL;
}

