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
FUNC	  	->	FUNC_HEADER forward ;
FUNC		->	FUNC_HEADER VARS BODY;
FUNC_HEADER	->	function id ( DEF_PARAMS ) : DT_TYPE ;
DEF_PARAMS 	->	eps
DEF_PARAMS	->	DEF_PAR_LIST
DEF_PAR_LIST	->	DEF_PAR
DEF_PAR_LIST	->	DEF_PAR_LIST , DEF_PAR
DEF_PAR		->	id : DT_TYPE

BODY		->	begin CMD_LIST end
BODYN		->	begin end
BODYN		->	BODY

CMD_LIST	->	CMD
CMD_LIST	->	CMD_LIST ; CMD

CMD		->	ASSIGN
CMD		->	BODYN
CMD		->	IF
CMD          	->	WHILE
CMD          	->	WRITE
CMD          	->	READLN

ASSIGN       	->	id := EXPR
ASSIGN       	->	id := CALL

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

