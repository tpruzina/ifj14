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

void *runTree(struct astNode *curr)
{
	if(!curr)
		exit(intern);

	struct symbolTableNode *left = NULL;		//toto by mali byt void pointre, ale fuck my life (resp. typecasty)
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
			runTree(curr->right);

		if(curr->left && curr->left->type == AST_CMD)	// <command list>
			runTree(curr->left);

		return NULL; //commandy boli zparsovane
		break;

	case AST_ASGN:	// <command>: <assign>
		if(curr->left && curr->right)
		{
			left = search(&top, runTree(curr->left));	// x :=
			// ak je left null tak mame neinicializovanu premennu - todo: check!!!

			right = runTree(curr->right);			//ocekavame symtabnode (tmp premenna...)

			if(right->dataType == DT_INT && left->dataType == DT_INT)
				insertDataInteger(&left, right->data.int_data);

			return right;
		}
		else
			exit(intern);
		break;


	case AST_END:
		break;

	case AST_FUNC:
		break;

	case AST_CALL:
		break;

	case AST_IF:
		break;

	case AST_WHILE:
		if(curr->left && curr->other)
		{
			while(runTree(curr->other)) //overenie <condition>
				runTree(curr->left);	// ak true, tak bezime telo
		}
		else
			exit(intern);
		break;

	case AST_REPEAT:
		break;


	case AST_EQV:
		break;

	case AST_NEQV:
		break;

	case AST_GRT:
		break;

	case AST_LSS:
		if(curr->left && curr->right)
		{
			//curr->left --> id
			tmp = search(&top,curr->left->data.str);
			//curr->right --> int

			if(tmp->data.int_data < curr->right->data.integer)
				return tmp;
			else
				return NULL;
		}
		break;

	case AST_GEQV:
		break;

	case AST_LEQV:
		break;

	case AST_ADD:
		if(curr->left && curr->right)
		{
			//curr->left -> id
			tmp = search(&top,curr->left->data.str);	//vyhledame si lokalnu verziu premennej

			//curr->right -> int, str....
			insertDataInteger(&tmp,
					(tmp->data.int_data + curr->right->data.integer)
			);

			return tmp;
		}
		else
			exit(intern);
	case AST_SUB:
	case AST_MUL:
	case AST_DIV:

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
		if(curr->data.str)
			return curr->data.str;

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
		break;

	case AST_BOOL:
		break;

	case AST_STR:
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

