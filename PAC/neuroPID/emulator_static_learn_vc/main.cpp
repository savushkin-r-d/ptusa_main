// emulator_static_learn.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
    {
    ////1. Создаем персептрон. 
    //const int INPUTS_COUNT = 10;
    //mlp *emulator = new mlp( 2 * INPUTS_COUNT, 10, 1 );

    ////2. Загружаем данные.
    //stored_sample *s_data = new stored_sample();
    //s_data->load_from_file( "..\\emul_learning.data" );

    //s_data->create_learn_sample( 2 * INPUTS_COUNT, 35, 1, 1 );
    //s_data->save_to_file( "..\\aa.txt" );
 
    //emulator->init_weights();
    //emulator->static_learn( 0.00005f, s_data, 100000 );
    //emulator->save_to_file( "..\\emul_q.data" );
    //return 0;

    throw std::exception ( "Test!" );
    return 0;
    }

