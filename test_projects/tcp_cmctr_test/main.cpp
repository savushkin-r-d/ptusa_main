#include "tcp_cmctr_w750.h"

long int test_f( long int size, u_char *in_buff,
                 u_char *out_buff )
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
	tcp_communicator::set_instance( new tcp_communicator_w750() );
	tcp_communicator::get_instance()->reg_service( 1, test_f );
	while ( 1 )
		{
		tcp_communicator::get_instance()->evaluate();

		printf( "main cycle\n" );
		}
	}

