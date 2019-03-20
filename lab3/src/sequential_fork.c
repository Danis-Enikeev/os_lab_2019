#include <stdio.h>
int main( void ) {
	char *argv[3] = {"sequential_min_max", "1245", "100000"};

	int pid = fork();

	if ( pid == 0 ) {
	    printf ("child created\n");
		execv( "sequential_min_max", argv );
	}
	return 0;
}