Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		for a downto 250 do
		begin
			print(b);
			print(a);
			a:=a-10
		end				
	end
end.
