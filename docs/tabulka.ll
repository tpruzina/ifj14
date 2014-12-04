PROGRAM      	->	VARS FUNC begin CMD_LIST enddot
VARS		->	var VAR_DEF VAR_DEFS
VARS		->	eps
VAR_DEFS	->	VAR_DEF VAR_DEFS
VAR_DEFS	->	eps
VAR_DEF		->	id : DT_TYPE ;
DT_TYPE		->	integer
DT_TYPE		->    	real
DT_TYPE		->     	boolean
DT_TYPE		->	string
FUNC		->	HEADER
FUNC		->	eps
HEADER		->	function id ( DEF_PARAMS ) : DT_TYPE ; AFTER_HEADER
AFTER_HEADER	->	forward ;
AFTER_HEADER	->	VARS BODY ;
DEF_PAR		->	id : DT_TYPE
DEF_PARAMS	->	DEF_PAR DEF_PAR_LIST
DEF_PARAMS 	->	eps
DEF_PAR_LIST	->	, DEF_PAR
DEF_PAR_LIST	->	eps
BODYN		-> begin CMD_LIST_N end
CMD_LIST_N	-> CMD CMD_FOLLOW
CMD_LIST_N	-> eps
CMD_FOLLOW	-> ; CMD CMD_FOLLOW
CMD_FOLLOW	-> eps
BODY		-> begin CMD_LIST end
CMD_LIST	-> CMD CMD_FOLLOW
CMD		->	ASSIGN
CMD		->	BODYN
CMD		->	IF
CMD          	->	WHILE
CMD          	->	WRITE
CMD          	->	READLN
ASSIGN       	->	id := AFTER_ASSIGN
AFTER_ASSIGN	->	EXPR
AFTER_ASSIGN	->	CALL
IF           	->	if EXPR then BODYN IF_ELSE
IF_ELSE    	->	else BODYN
IF_ELSE     	->	eps
WHILE        	->	while EXPR do BODYN
CALL        	->	id ( TERM_LIST )
CALL         	->	sort ( dt_str )
CALL         	->	find ( dt_str , dt_str )
CALL         	->	length ( dt_str )
CALL         	->	copy ( dt_str , dt_str , dt_int )
READLN		->	readln ( id )
WRITE		->	write ( TERM_LIST )
TERM_LIST    	->	TERM TERM_FOLLOW
TERM_FOLLOW    	->	, TERM TERM_FOLLOW
TERM_FOLLOW	->	eps
TERM        	->	id
TERM        	->	dt_int
TERM        	->	dt_real
TERM        	->	dt_str
TERM        	->	dt_bool
EXPR        	->	vyraz

