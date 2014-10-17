Program hello;
	var
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		a:=0;
		case a of 
			0 : begin
					write(b);
					a:=10
				end
			1 : write(a)
			2 : begin
					a:=a+20
				end
			'a': begin
					if (a>20) then
						begin
							write('Chyba')
						end
				end		
end.
