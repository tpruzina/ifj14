var
    a : array[0..10] of integer;
    b : array[1..2] of string;
    message : string;
    i : integer;
    
begin
    a[0] := 10 - 5;
    a[7] := 0 - 10;
    b[1] := 'Ahoj';
    b[2] := ' svete!';
    message := b[1] + b[2];
    i := a[0] + a[7];
    write(message, ''#10'', i, ''#10'')
end.
