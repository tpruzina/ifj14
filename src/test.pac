var
	x : integer;
	y : integer;

begin
	for x := 0 to 10 do begin
		for y := 10 downto x do begin
			write('>')
		end;
		write('/');
		for y := 10 - x to 10 do begin
			write('<')
		end;
		write(''#10'')
	end
end.
