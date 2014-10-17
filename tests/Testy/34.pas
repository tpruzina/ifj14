Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		for a:=250; downto 0 do
		begin
			print(b);
			a:=a-10
		end
					
	end
end.
