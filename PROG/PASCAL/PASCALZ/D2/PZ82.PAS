{Program 11.9
recursive formulation of god}

Program recursivegod;		{ * ( output ) is implicit in Pascal/Z * }

var	x, y, n : integer;

function god( m, n : integer ) : integer;
begin
	if n = 0 then god := m
	else god := god( n, m mod n )
end;		{god}

procedure try( a, b : integer );
begin
	writeln( a, b, god( a, b ) )
end;

begin
	try( 18, 27 );
	try( 312, 2142 );
	try( 61, 53 );
	try( 98, 868 )
end.
                                                                                  