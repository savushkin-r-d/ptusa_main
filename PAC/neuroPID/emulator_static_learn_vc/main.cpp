// emulator_static_learn.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "sample.h"

int _tmain(int argc, _TCHAR* argv[])
    {
    const int INPUTS_COUNT = 10;

    ////1.1 Создаем персептрон NN2.     
    //mlp *emulator = new mlp( 2 * INPUTS_COUNT, 10, 1 );

    ////1.2 Загружаем данные.
    //stored_sample *s_data = new stored_sample();
    //s_data->load_from_file( "..\\emul_learning.data" );

    //s_data->create_learn_sample( 2 * INPUTS_COUNT, 35 );
    //s_data->save_to_file( "..\\aa.txt" );
 
    //emulator->init_weights();
    //emulator->static_learn( 0.00005f, s_data, 100000 );
    //emulator->save_to_file( "..\\emul_q.data" );
    
    //
    //2.1 Создаем персептрон NN1.    
    mlp *tuner = new mlp( 2 * INPUTS_COUNT, 10, 3 );

    //2.2 Загружаем данные.
    stored_sample *s_data_tuner = new stored_sample();
    s_data_tuner->load_from_file( "..\\PID_tuner_learning.data" );

    s_data_tuner->create_learn_sample( 2 * INPUTS_COUNT, 35, 1, 2, 3, true );
    s_data_tuner->save_to_file( "..\\aa1.txt" );

    tuner->init_weights();
    tuner->static_learn( 0.00000005f, s_data_tuner, 100000 );
    tuner->save_to_file( "..\\tuner_q.data" );
    return 0;  
    }

