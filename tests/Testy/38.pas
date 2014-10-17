Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		for a:=100 downto 25 do
			print(b);
			print(a);
			a:=a-10			
	end
end.
