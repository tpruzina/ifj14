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
				exit(intern);

			return left;
		}
		else
			exit(intern);
		break;


	case AST_END:
		return NULL;
		break;

	case AST_FUNC:
		break;

	case AST_CALL:
		break;

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
		break;


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
			tmp = insertValue(
					&top,
					generateUniqueID(),
					DT_BOOL);

			insertDataBoolean(
					&tmp,
					compare(left,right,curr->type)
			);
			return tmp;
		}
		else
			exit(intern);
		break;

	case AST_ADD:
		if(curr->left && curr->right)
		{	//curr->left -> id
			tmp = search(&top,curr->left->data.str);	//vyhledame si lokalnu verziu premennej
			//curr->right -> int, str....
			if(DT_INT == tmp->dataType)
				insertDataInteger(&tmp,(tmp->data.int_data + curr->right->data.integer));
			else if(DT_STR == tmp->dataType)
				exit(intern);
			else
				exit(intern);

			return tmp;
		}
		else
			exit(intern);

	case AST_SUB:
		if(curr->left && curr->right)
		{
			tmp = search(&top,curr->left->data.str);	//vyhledame si lokalnu verziu premennej
			if(DT_INT == tmp->dataType)
			{
				if(tmp->data.int_data - curr->right->data.integer < 0)
					exit(run_num);
				insertDataInteger(&tmp,(tmp->data.int_data - curr->right->data.integer));
			}
			else
				exit(intern);
			return tmp;
		}
		else
			exit(intern);

	case AST_MUL:
		if(curr->left && curr->right)
		{
			left = runTree(curr->left);		// _a_ / b
			right = runTree(curr->right);	// a / _b_

			tmp = insertValue(
					&top,
					generateUniqueID(),
					DT_INT);

			insertDataInteger(&tmp,
					(left->data.int_data * right->data.int_data)
			);
			return tmp;
		}
		else
			exit(intern);

	case AST_DIV:
		if(curr->left && curr->right)
		{
			tmp = search(&top,curr->left->data.str);	//vyhledame si lokalnu verziu premennej
			if(DT_INT == tmp->dataType)
			{
				if(curr->right->data.integer == 0)
					exit(run_div);

				insertDataInteger(&tmp,(tmp->data.int_data / curr->right->data.integer));
			}
			else if(DT_REAL == tmp->dataType)
			{
				if(curr->right->data.real == (double)0)	//todo: takto sa realy neporovnavaju, medze!!!
					exit(run_div);

				insertDataReal(&tmp,(tmp->data.int_data / curr->right->data.integer));
			}
			else
				exit(intern);
			return tmp;
		}
		else
			exit(intern);

		break;

	case AST_AND:
		break;

	case AST_OR:
		break;

	case AST_XOR:
		break;

	case AST_NOT:
		break;

	case AST_NUM:
		break;

	case AST_ID:
		// mame identifikator, navrat string o level vyssie
		return search(&top,curr->data.str);
		break;

	case AST_INT:
		// pridavame novu lokalnu premennu do tabulky a priradime jej hodnotu z node
		tmp = insertValue(
				&top,
				generateUniqueID(),
				DT_INT
		);
		insertDataInteger(&tmp,curr->data.integer);

		return tmp;
		break;

	case AST_REAL:
		tmp = insertValue(
				&top,
				generateUniqueID(),
				DT_REAL
		);
		insertDataInteger(&tmp, curr->data.real);
		return tmp;
		break;

	case AST_BOOL:
		tmp = insertValue(
				&top,
				generateUniqueID(),
				DT_BOOL
		);
		insertDataBoolean(&top, curr->data.boolean);
		return tmp;
		break;

	case AST_STR:
		tmp = insertValue(
				&top,
				generateUniqueID(),
				DT_STR
		);
		insertDataString(&top, curr->data.str);
		return tmp;
		break;

	case AST_ARR:
		break;


	default:
	case AST_NONE:
//		exit(intern);
		return NULL;
	}
	return NULL;
}

