#ifndef NN_MANAGER
#define NN_MANAGER

#include <vector>

#include "mlp.h"
#include "plant.h"
#include "PID.h"

class nn_manager
    {
    public:
        ~nn_manager()
            {
            fclose( data_stream );
            }

        nn_manager( int time_interval, 
            int inputs_count_per_parameter ): time_interval( time_interval ),
            p_plant( new plant() ),
            p_pid( new PID() ),
            nn2_emulator( new mlp( 20, 10, 1, 120 ) ),
            use_learning( true ),
            nn1_tuner( new mlp( 20, 10, 3, 120 ) )
            {
            //Создание необходимых массивов и выделение памяти [1]. Запись
            //данных инициализации [2]. Запись указателей для выборки обучения [3].
            p_plant_data = new std::vector<float>();                        //1
            PID_data = new std::vector<float>();

            nn2_out = new std::vector<float>;
            nn2_in = new std::vector<float*>;

            p_plant_data->reserve( time_interval );
            PID_data->reserve( time_interval );
            nn2_out->reserve( time_interval );  

            int samples_count = time_interval - inputs_count_per_parameter + 1;
            nn2_in->reserve( 2 * inputs_count_per_parameter * samples_count );

            for ( int i = 0; i < time_interval; i++ )                       //2
                {
                p_plant_data->push_back( 0 );
                PID_data->push_back( 0 );
                nn2_out->push_back( 0 );                
                }

            for ( int i = 0; i < 2 * inputs_count_per_parameter * samples_count;
                i++ )
                {               
                nn2_in->push_back( 0 );
                }
            
             for ( int i = 0; i < samples_count; i++ )                      //3
                {
                for ( int j = 0; j < inputs_count_per_parameter; j++ )
                    {                    
                    nn2_in[ 0 ][ i * inputs_count_per_parameter + j ] =
                        &p_plant_data[ 0 ][ i + j ]; 
                    }

                for ( int j = 0; j < inputs_count_per_parameter; j++ )
                    {                    
                    nn2_in[ 0 ][ i * inputs_count_per_parameter + inputs_count_per_parameter + j ] = 
                        &PID_data[ 0 ][ i + j ]; 
                    }
                }            
            
            p_pid->par[ PID::PAR_k ] = 1;             //1 Параметр k.
            p_pid->par[ PID::PAR_Ti ] = 2;            //2 Параметр Ti.
            p_pid->par[ PID::PAR_Td ] = 0.01f;        //3 Параметр Td.
            p_pid->par[ PID::PAR_dt ] = 1;            //4 Интервал расчёта
            p_pid->par[ PID::PAR_dmax ] = 200;        //5 Мax значение входной величины.
            p_pid->par[ PID::PAR_dmin ] = 0;          //6 Мin значение входной величины.
            p_pid->par[ PID::PAR_AccelTime ] = 0;     //7 Время выхода на режим регулирования.
            p_pid->par[ PID::PAR_IsManualMode ] = 0;  //8 Ручной режим.
            p_pid->par[ PID::PAR_UManual ] = 0;       //9 Заданное ручное значение выходного сигнала.

            p_pid->Reset( 0 );
            p_pid->SetZ( 90 );
            p_pid->On();

            fopen_s( &data_stream, "..\\simul_system_data_.prn", "w" );
            fprintf( data_stream, "new_control_val\tnew_plant_val\tnew_emulator_output_value\n" );

            nn2_emulator->init_weights();
            nn2_emulator->load_from_file( "..\\emul_q.data" );

            nn1_tuner->init_weights();
            nn1_tuner->load_from_file( "..\\tuner_q.data" );
            try
                {
                emul_sample = new rt_sample( 50, 20, 2, 1, 1, 120 );
                tuner_sample = new rt_sample( 50, 20, 2, 3, 3, 120 );

                tuner_future_sample = new rt_sample( 1, 20, 2, 3, 3, 120 );

                //emul_sample->print();
                //tuner_sample->print();
                }    
            catch ( char *ex )
                {
                printf( "%s\n", ex );
                }  
            }

        plant* get_plant() const
            {
            return p_plant;
            }

        PID* get_PID() const
            {
            return p_pid;
            }

        void set_learning( bool use_learning )
            {
            this->use_learning = use_learning;
            } 

        void eval()
            {
            //Получаем текущее значение объекта управления [1]. Получаем новое 
            //значение управляющего воздействия [2]. Применяем его и фиктивно 
            //ожидаем [3]. Получаем новое значение объекта управления [4].
            //Сдвигаем сохраненные ранее точки [5]. Добавляем новые точки [6].
            float current_plant_val = p_plant->get_current_out();           //1            
            float new_control_val = p_pid->Eval( current_plant_val );       //2
                                                                            //3
            float new_plant_val = p_plant->get_new_out( new_control_val );  //4 
               
            for ( int i = time_interval - 1; i > 0; i-- )                   //5
                {                                                             
                p_plant_data[ 0 ][ i ] = p_plant_data[ 0 ][ i - 1 ];
                PID_data[ 0 ][ i ] = PID_data[ 0 ][ i - 1 ]; 
                nn2_out[ 0 ][ i ] = nn2_out[ 0 ][ i - 1 ];                 
                }          
                                                              
            p_plant_data[ 0 ][ 0 ] = new_plant_val;                         //6   
            PID_data[ 0 ][ 0 ] = new_control_val;

            emul_sample->shift_images();
            emul_sample->add_new_val_to_in_image( 0, current_plant_val );
            emul_sample->add_new_val_to_in_image( 1, new_control_val );
            emul_sample->add_new_val_to_out_image( 0, new_plant_val );
            //emul_sample->print();

            //Вычисляем выход эмулятора.
            float *new_emulator_output = 
                nn2_emulator->solve_out( emul_sample->get_last_sample_x() );  
            float new_emulator_output_value = new_emulator_output[ 0 ] * nn2_emulator->get_q();
            nn2_out[ 0 ][ 0 ] = new_emulator_output_value;
           
            fprintf( data_stream, "%f\t%f\t%f\n", 
                new_control_val, new_plant_val, new_emulator_output_value );

            //Обучение эмулятора.
            static unsigned int time = 0;
            time++;
            
            static unsigned int errors_cnt = 0;

            if ( use_learning && time > 20 )
            	{
                if ( abs( new_plant_val - nn2_out[ 0 ][ 0 ] ) > 2 )
                	{
                    errors_cnt++;                    
                	}            
                else
                    {
                    errors_cnt = 0;
                    }

                if ( errors_cnt > 10 )
                	{
                    nn2_emulator->static_learn( 0.001f,  emul_sample, 100 );
                    errors_cnt = 0;
                	}
            	}  

            tuner_sample->shift_images();
            tuner_sample->add_new_val_to_in_image( 0, current_plant_val );
            tuner_sample->add_new_val_to_in_image( 1, new_control_val );   
            float tuner_err = ( p_pid->get_z() - new_plant_val ) / nn1_tuner->get_q();
            tuner_sample->add_new_val_to_out_image( 0, tuner_err );
            tuner_sample->add_new_val_to_out_image( 1, tuner_err );
            tuner_sample->add_new_val_to_out_image( 2, tuner_err );
            //tuner_sample->print();

            tuner_future_sample->add_new_val_to_in_image( 0, current_plant_val );
            tuner_future_sample->add_new_val_to_in_image( 1, new_control_val );   
            
            static unsigned int errors_tuner_cnt = 0;
            if ( time > 40 )
                {
                if ( abs( tuner_err ) > 2.f / nn1_tuner->get_q() )
                    {
                    errors_tuner_cnt++;
                    }

                if ( errors_tuner_cnt > 10 )
                    {
                    //float tuner_future_err = ( p_pid->get_z() - new_plant_val ) / nn1_tuner->get_q();
                    //tuner_future_sample->add_new_val_to_out_image( 0, tuner_future_err );
                    //tuner_future_sample->add_new_val_to_out_image( 1, tuner_future_err );
                    //tuner_future_sample->add_new_val_to_out_image( 2, tuner_future_err );
                    
                    //Вычисляем выход тюнера ПИД.            
                    float *new_PID_q = nn1_tuner->solve_out( tuner_sample->get_last_sample_x() ); 

                    float new_p = new_PID_q[ 0 ] * nn1_tuner->get_q();
                    float new_i = new_PID_q[ 1 ] * nn1_tuner->get_q();
                    float new_d = new_PID_q[ 2 ] * nn1_tuner->get_q();

                    nn1_tuner->static_learn( 0.0001f,  tuner_sample, 10, true, false );
                    printf( "Learn iteration -> " );
                    printf( "%f %f %f\n", new_p, new_i, new_d );

                    p_pid->set_p( new_p );
                    p_pid->set_i( new_i );
                    p_pid->set_d( new_d );

                    errors_tuner_cnt = 0;
                    } 
                }            
            }

        std::vector<float> *get_plant_data()
            {
            return p_plant_data;
            }

        std::vector<float> *get_PID_data()
            {
            return PID_data;
            }

        std::vector<float> *get_nn2_emul_data()
            {
            return nn2_out;
            }

    private:
        int time_interval;

        std::vector<float>  *p_plant_data;
        std::vector<float>  *PID_data;
        std::vector<float>  *nn2_out;
        std::vector<float*> *nn2_in;

        plant *p_plant;         ///Установка.
        mlp   *nn2_emulator;    ///Нейронная сеть.
        PID   *p_pid;           ///ПИД.
        mlp   *nn1_tuner;       ///Нейронная сеть.

        FILE *data_stream;

        rt_sample *emul_sample;
        rt_sample *tuner_sample;

        rt_sample *tuner_future_sample;

        bool use_learning;
    };
#endif //NN_MANAGER