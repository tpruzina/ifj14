Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		for b downto 25 do
		begin
			print(b);
			print(a);
			a:=a-10
		end			
	end
end.
