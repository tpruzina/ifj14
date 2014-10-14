#include "Interpret.h"

int interpret()
{
	ASSERT(global);
	ASSERT(global.program);

	printf("##################INTERPRET############################\n");

	struct astNode *curr = global.program;

	if(curr->type == AST_START)
		runTree(curr);
	else
		exit(intern);

	printf("##################INTERPRET############################\n");
	return 0;
}

#define NODE_LEFT_TYPE(node,type) ((node)->left) && ((node)->left->type == (type)))



#define GET_DATATYPE(node) ((node)->dataType)

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



void *runTree(struct astNode *curr)
{
	if(!curr)
		exit(intern);

	struct symbolTableNode *left = NULL;
	struct symbolTableNode *right = NULL;

	struct symbolTableNode *tmp = NULL;

	// stack top je cheap operace, nech to nemusime riesit v KA, bude to v kazdej iteracii - resp. TODO
	struct symbolTableNode* top = stackTop(global.symTable);

	switch(curr->type)
	{
	case AST_START:
		if(curr->left && curr->left->type == AST_CMD)
			runTree(curr->left);
		break;

	case AST_CMD:
		if(curr->right)				//	<command>
			right = runTree(curr->right);

		if(curr->left && curr->left->type == AST_CMD)	// <command list>
			return runTree(curr->left);

		break;

	case AST_ASGN:	// <command>: <assign>
		if(curr->left && curr->left->type == AST_ID &&
			curr->right)
		{
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
		}
		else
			exit(intern);
		break;

	case AST_FUNC:
		break;

	case AST_CALL:
		break;

// controll konstrukcie (if,while,repeat..)
	case AST_IF:
		tmp = runTree(curr->other);	// podmienka body

		if(tmp->data.bool_data == true)	//vyhodnocena true
			return runTree(curr->left);
		else if(curr->right)	// vyhodnocena false a mame v branchi else
				return runTree(curr->right);
		// jinak
		return NULL;
		break;

	case AST_WHILE:
		if(curr->left && curr->other)
		{
			while(true)
			{
				tmp = runTree(curr->other);
				if(tmp->data.bool_data)
					runTree(curr->left);	// ak true, tak bezime telo
				else
					break;
			}
			return NULL;
		}
		else
			exit(intern);
		break;

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
		if(curr->left && curr->right)
		{
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
		}
		else
			exit(intern);
		break;

// aritmeticke operace +,-,/,*
	case AST_ADD:
	case AST_SUB:
	case AST_MUL:
	case AST_DIV:
		if(curr->left && curr->right)
		{
			left = runTree(curr->left);		// *a* (op) b
			right = runTree(curr->right);	// a (op) *b*

			// spravime aritmeticku operaciu
			// vraci tmp premennu v symtab node
			return arithmetic(left,right,curr->type);
		}
		else
			exit(intern);
		break;

// logicke operace
	// tieto su binarne
	case AST_AND:
	case AST_OR:
	case AST_XOR:
		if(curr->left && curr->right)
		{
			right = runTree(curr->right);
			left = runTree(curr->left);

			tmp = makeNewSymbolTable();
			insertDataBoolean(
					&tmp,
					(curr->type == AST_AND) ? (left->data.bool_data && right->data.bool_data):
					(curr->type == AST_OR)	? (left->data.bool_data || right->data.bool_data):
					(curr->type == AST_XOR) ? (left->data.bool_data ^ right->data.bool_data):
					false
			);
			return tmp;
		}
		else
			exit(intern);
		break;

	// not je unarna operacia
	case AST_NOT:
		if(curr->left)
		{
			left = runTree(curr->left);

			tmp = makeNewSymbolTable();
			insertDataBoolean(&tmp,	!(left->data.bool_data));
			return tmp;
		}
		else
			exit(intern);
		break;

// ????
	case AST_NUM:
		break;


// unarne operacie, vracaju 'tokeny' ast (premenne, int, bool,real...)
	case AST_ID:
		// mame identifikator, hladame v tabulke symbolov
		return search(&top,curr->data.str);
		break;

	case AST_INT:
		// pridavame novu lokalnu premennu do tabulky a priradime jej hodnotu z node
		tmp = makeNewSymbolTable();
		insertDataInteger(&tmp,curr->data.integer);
		return tmp;
		break;

	case AST_REAL:
		tmp = makeNewSymbolTable();
		insertDataReal(&tmp, curr->data.real);
		return tmp;
		break;

	case AST_BOOL:
		tmp = makeNewSymbolTable();
		insertDataBoolean(&top, curr->data.boolean);
		return tmp;
		break;

	case AST_STR:
		tmp = makeNewSymbolTable();
		insertDataString(&top, curr->data.str);
		return tmp;
		break;

	case AST_ARR:
		exit(intern);	//todo
		break;

// ????
	case AST_END:
		return NULL;
		break;

	case AST_NONE:
	default:
//		exit(intern);
		return NULL;
	}
	return NULL;
}

