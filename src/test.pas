{ test na vyraz }
var
	ret : integer;

function mul(x:integer; y:integer):integer;
begin
	mul := x * y
end;


begin
	ret := mul(2,3);
	write(ret)
end.
