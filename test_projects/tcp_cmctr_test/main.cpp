#include "tcp_cmctr.h"

long int test_f( DESTDATA type, long int size, unsigned char *in_buff,
                 unsigned char *out_buff )
	{
	out_buff[ 0 ] = 't';
	out_buff[ 1 ] = 'e';
	out_buff[ 2 ] = 's';
	out_buff[ 3 ] = 't';
	out_buff[ 4 ] = '\n';
	out_buff[ 5 ] =  0;
	return 6;
	}

int main()
	{
	tcp_communicator test_cmctr;
	test_cmctr.regService( 1, test_f );
	while ( 1 )
		{
		test_cmctr.Evaluate();

		printf( "main cycle\n" );
		}
	}

