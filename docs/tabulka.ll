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

FUNC		->	eps
FUNC		->	HEADER

HEADER		->	function id ( DEF_PARAMS ) : DT_TYPE ; AFTER_HEADER

AFTER_HEADER	->	forward ;
AFTER_HEADER	->	VARS BODY ;

DEF_PARAMS 	->	eps
DEF_PARAMS	->	DEF_PAR DEF_PAR_LIST

DEF_PAR_LIST	->	eps
DEF_PAR_LIST	->	, DEF_PAR

DEF_PAR		->	id : DT_TYPE

BODYN		-> begin CMD_LIST_N end
CMD_LIST_N	-> eps
CMD_LIST_N	-> CMD CMD_FOLLOW
CMD_FOLLOW	-> eps
CMD_FOLLOW	-> ; CMD CMD_FOLLOW

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

IF           	->	if EXPR then BODY IF_ELSE
IF_ELSE     	->	eps
IF_ELSE    	->	else BODY
WHILE        	->	while EXPR do BODY
CALL        	->	id ( TERM_LIST )
CALL         	->	sort ( dt_str )
CALL         	->	find ( dt_str , dt_str )
CALL         	->	length ( dt_str )
CALL         	->	copy ( dt_str , dt_str , dt_int )

READLN		->	readln ( id )

WRITE		->	write ( TERM_LIST )

TERM_LIST    	->	TERM
TERM_LIST    	->	TERM_LIST , TERM

TERM        	->	id
TERM        	->	dt_int
TERM        	->	dt_real
TERM        	->	dt_bool
TERM        	->	dt_str

EXPR        	->	vyraz

