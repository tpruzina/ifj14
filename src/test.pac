var
   x: string;
   len: integer;
begin
	x := '';
	repeat
		x := x + 'x';
		len := length(x)
	until true;
	write(x)
end.
