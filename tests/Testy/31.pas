Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:="Hello word!";
		for a=20 to 250 do
		begin
			print(b);
			print(a);
			a:=a+10
		end
					
	end
end.