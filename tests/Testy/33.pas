Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:="Hello word!";
		for a:=10 to 250 do
			print(b);
			print(a);
			a:=a+10			
	end
end.