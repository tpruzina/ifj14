{Boolean test - hlavne precedencni analyza...}

var 
  a : boolean;
  b : boolean;
  c : boolean;

begin
  a := true;
  b := false;

  write('a - true: ', a, ''#10'');
  write('b - false: ', b, ''#10'');

  c := a < b;
  write('a < b:  ', c, ''#10'');
  c := a > b;
  write('a > b:  ', c, ''#10'');
  c := a <= b;
  write('a <= b: ', c, ''#10'');
  c := a >= b;
  write('a >= b: ', c, ''#10'');
  c := a = b;
  write('a = b:  ', c, ''#10'');
  c := a <> b;
  write('a <> b: ', c, ''#10'');
  c := a > b;
  write('a > b:  ', c, ''#10'');

  if true then begin
    write('Bool v ifu...'#10'')
  end else begin
  end
end.
