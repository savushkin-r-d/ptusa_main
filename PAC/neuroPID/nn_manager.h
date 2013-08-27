#ifndef NN_MANAGER
#define NN_MANAGER

#include <vector>

#include "mlp.h"
#include "plant.h"
#include "PID.h"

class nn_manager
    {
    public:
        nn_manager( int time_interval, 
            int inputs_count_per_parameter ): time_interval( time_interval ),
            p_plant( new plant() ),
            p_pid( new PID() ),
            nn2( new mlp( 21, 10, 1, 120 ) ),
            use_learning( true )
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

            nn2->init_weights();
            nn2->load_from_file( "..\\emul_q.data" );

            try
                {
                sample = new rt_sample( 50, 20, 2, 1, 1, 120 );

                sample->print();
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

            sample->shift_images();
            sample->add_new_val_to_in_image( 0, current_plant_val );
            sample->add_new_val_to_in_image( 1, new_control_val );
            sample->add_new_val_to_out_image( 0, new_plant_val );
            //sample->print();

            //Вычисляем выход эмулятора.
            float *new_emulator_output = nn2->solve_out( sample->get_last_sample_x() ); 
            
            nn2_out[ 0 ][ 0 ] = new_emulator_output[ 0 ] * nn2->get_q();

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
                    nn2->static_learn( 0.001f,  sample, 100 );
                    errors_cnt = 0;
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

        plant *p_plant; ///Установка.
        mlp   *nn2;     ///Нейронная сеть.
        PID   *p_pid;   ///ПИД.

        FILE *data_stream;

        rt_sample *sample;

        bool use_learning;
    };
#endif //NN_MANAGER