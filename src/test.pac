var
   x: array[0..1] of integer;
   y: array[0..1] of integer;
   
begin
	x[0] := 0;
	x[1] := 1;
	
	y := x;
	write(y[0], y[1])
end.
