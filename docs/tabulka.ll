Program        	=>		<VARS> <FUNC> begin <CMD_LIST> enddot

<VARS>        	=>		var <VAR_DEF> <VAR_DEFS>
<VARS>        	=>		empty
<VAR_DEFS>     	=>		<VAR_DEF> <VAR_DEFS>
<VAR_DEFS>    	=>		empty
<VAR_DEF>      	=>		ID : <DT_TYPE> ;
<DT_TYPE>      	=>		integer
                		real
                		boolean
                		string

<FUNC>        	=>		empty
<FUNC>        	=>		<FUNC_HEADER> forward ;
<FUNC>        	=>		<FUNC_HEADER> <VARS> <BODY>;
<FUNC_HEADER>	=>		function ID ( <DEF_PARAMS> ) : <DT_TYPE> ;
<DEF_PARAMS> 	=>		empty
<DEF_PARAMS>	=>		<DEF_PAR_LIST>
<DEF_PAR_LIST>	=>		<DEF_PAR>
<DEF_PAR_LIST>	=>		<DEF_PAR_LIST>, <DEF_PAR>
<DEF_PAR>		=>		ID : <DT_TYPE>

<BODY>			=>		begin <CMD_LIST> end
<BODYN>			=>		begin end
<BODYN>			=>		<BODY>

<CMD_LIST>		=>		<CMD>
<CMD_LIST>		=>		<CMD_LIST>; <CMD>

<CMD>			=>		<ASSIGN>
    			=>		<BODYN>
    			=>		<IF>
            	=>		<WHILE>
            	=>		<WRITE>
            	=>		<READLN>

<ASSIGN>       	=>		ID := <EXPR>
            	=>		ID := <CALL>
<IF>           	=>		if <EXPR> then <BODY> <IF_ELSE>
<IF_ELSE>     	=>		empty
<IF_ELSE>    	=>		else <BODY>
<WHILE>        	=>		while <EXPR> do <BODY>
<CALL>        	=>		ID ( <TERM_LIST> )
            	=>		sort(DT_STR)
            	=>		find(DT_STR, DT_STR)
            	=>		length(DT_STR)
            	=>		copy(DT_STR, DT_INT, DT_INT)

<READLN>        =>		readln ( ID )

<WRITE>         =>		write ( <TERM_LIST> )
<TERM_LIST>    	=>		<TERM>
<TERM_LIST>    	=>		<TERM_LIST>, <TERM>
<TERM>        	=>		ID
<TERM>        	=>		DT_INT
<TERM>        	=>		DT_REAL
<TERM>        	=>		DT_BOOL
<TERM>        	=>		DT_STR

<EXPR>        	=>		výraz

