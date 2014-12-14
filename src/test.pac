var
	x: array[0..10] of string;
	id: integer;
	b: boolean;

begin
	b := true;

	for id := 0 to 10 do
	begin
		if b then
		begin
			x[id] := '^'
		end
		else
		begin
			x[id] := 'v'
		end;
		{ schvalne jestli tohle projde }
		b := NOT b		
	end;

	id := 0;
	while id <= 10 do
	begin
		write(x[id]);
		id := id + 1		
	end
end.
