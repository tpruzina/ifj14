Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		a:=0;
		repeat
			begin
				a:=a+10;
			end
		until b = 0
	end
end.
