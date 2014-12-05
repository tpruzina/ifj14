{ test na seznam parametru funkce }
var
	ret: integer;

{druhy parametru}
function f1(a:integer; b:real; c:boolean; d:string):integer;
begin
	f1 := a
end;

{zadny parametr}
function f2():integer;
begin
	f2 := 0
end;

begin
	ret := f1(1, 1.2, TRUE, 'Ahoj');
	write(ret);

	ret := f1();
	write(ret)
end.
