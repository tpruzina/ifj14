#include "Interpret.h"

int interpret()
{
	ASSERT(global);
	ASSERT(global.program);

	printf("##################INTERPRET############################\n");

	// result of previous operation
	void *result;

	// pointer to current node
	struct astNode *curr;
	curr = global.program;

	if(curr->type == AST_START)
		run_tree(curr);
	else
		exit(intern);

	printf("##################INTERPRET############################\n");
}

#define NODE_LEFT_TYPE(node,type) ((node)->left) && ((node)->left->type == (type)))

void *run_tree(struct astNode *curr)
{
	if(!curr)
		exit(intern);

	struct symbolTableNode *left = NULL;		//toto by mali byt void pointre, ale fuck my life (resp. typecasty)
	struct symbolTableNode *right = NULL;
	void *tmp = NULL;

	struct symbolTableNode* top = NULL;	//kvoli blbej derefencii, C nepodporuje vytvorenie tmp premennej v switchi

	switch(curr->type)
	{
	case AST_START:
		if(curr->left && curr->left->type == AST_CMD)
			run_tree(curr->left);
		break;

	case AST_CMD:
		if(curr->left && curr->left->type == AST_CMD)
			run_tree(curr->left);
		else if(curr->right && curr->right->type == AST_ASGN)
			run_tree(curr->right);
		break;

	case AST_ASGN:
		if(curr->left && curr->right)
		{
			top = stackTop(global.symTable);
			left = search(&top, run_tree(curr->left));			//ocekavame string* ("x") -> vyhledej

			right = run_tree(curr->right);			//ocekavame symtabnode (tmp premenna...)

			if(
					right->dataType == DT_INT &&
					left->dataType == DT_INT
			)
			{
				exit(1);
			}
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
		break;

	case AST_GEQV:
		break;

	case AST_LEQV:
		break;

	case AST_ADD:
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
		top = stackTop(global.symTable);
		return insertValue(
				&top,
				generateUniqueID(),
				DT_INT
		);
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
		exit(intern);
	}
	return NULL;
}

