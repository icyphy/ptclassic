// Helper app for Windows32 to spawn a command and subsequently remove
// all files passed as arguments. 
// 
// Usage: ptspawn "command" "list of filenames" (must be enclosed in quotes)
//
// The program assumes that the first argument passed is the command, the
// second a list of files to pass the commands as arguments and to delete
// afterwards. To allow quotes within the command (e.g. pxgraph -t "Title")
// these quotes must be replaced by § signs.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv) {
	if( argc != 2 ) {
		// Need two args
		fprintf(stderr, "ptspawn needs two arguments.\n");
		return -1;
	}

	// Construct command
	char *cmd = new char [ strlen( argv[0] ) + strlen( argv[1] ) + 2 ];
	strcpy( cmd, argv[0] );	
	strcat( cmd, " " );
	strcat( cmd, argv[1] );

	// Subsitute § with "
	while( strchr( cmd, '§' ) )
		*strchr( cmd, '§' ) = '\"';

	// Issue the system command
	system( cmd );

	// Now kill argument files
	char *args = argv[1];
	while( args && *args ) {
		if( args && *args ) {
		  // Use system command to avoid hassle with wildcards
			char rmCmd[512];
			strcpy( rmCmd, "del " );

			if( strchr( args, ' ' ) )
				strncat( rmCmd, args, strchr( args, ' ' )-args );
			else
				strcat( rmCmd, args );

			system( rmCmd );
		}

		args = strchr( args, ' ' );
		while( args && *args && *args==' ' )
			++args;
	}		

	// Done
	delete cmd;
	return 0;
}