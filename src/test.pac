{ spravny }

var
   a : integer;
   r : real;

function w(i :integer; c : real) : real;
var
	a : integer;
begin
	a := i * 50;
	w := 5.0 * a
end;

begin
	a := 20;
	r := w(a, 5.0);
	write (r)
end.


