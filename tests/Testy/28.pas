Program hello;
	var:
		a: integer;
		b: string;

	begin
		b:='Hello word!';
		a:=0;
		case a:=0 of 
			0 : begin
					print(b)
					a:=10;
				end
			1 : print(a)
			2 : begin
					a:=a+20
				end
			'a': begin
					if (a>20) then
						begin
							print('Chyba')
						end
					else
						begin
						end
		end;			
	end
end.
