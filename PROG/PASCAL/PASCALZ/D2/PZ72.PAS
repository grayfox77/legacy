{Program 11.3
procedure parameters}

Program parameters;		{ * ( output ) is implicit in Pascal/Z * }

var	a, b : integer;

procedure h( x : integer; var y : integer );
begin
	x := x + 1; y := y + 1;
	writeln( x, y )
end;

begin
	a := 0; b := 0;
	h( a, b );
	writeln( a, b )
end.
                                                                                      