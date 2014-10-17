Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:="Hello word!";
		a:=0;
		while a = 50 do
		begin
			print(b);
			a:=b+5
		end
	end
end.