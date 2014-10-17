Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		a:=0;
		repeat
			begin
				print(b)
				a:=a+10
			end
		until a = 0
	end
end.
