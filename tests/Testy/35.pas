Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		for a:=10; downto 50 do
		begin
			print(b);
			a:=a-10
		end
					
	end
end.
