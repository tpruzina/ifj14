var
	i : integer;
BEGIN
	i := 0;
	repeat
		write(i, #10);
	
		i := i + 1
	until i = 10;
	write(i)
END.
