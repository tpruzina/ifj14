{ test na vzajemne volani funkci }
var
	ret: integer;

function g(x:integer): integer; forward;
function f(x:integer): integer; forward;


function f(x:integer): integer;
begin
	f := g(x)
end;
function g(x:integer): integer;
begin
	g := x + 1
end;

begin
	ret := f(5);
	write(ret)
end.
