//ОБЩЕЕ ОПИСАНИЕ.
//  Содержит описания классов, которые используются реализации нейронной сети.
#ifndef MLP_H
#define MLP_H

#include "sample.h"

#define printf_ printf
//------------------------------------------------------------------------------
//ОПИСАНИЕ
//  Многослойный персептрон (1 скрытый слой).
class mlp
    {
    public: 
        enum T
            {
            T_SAMPLE_Y = 1,
            T_ERROR,
            };

        enum LAYERS
            {
            L_INPUT = 0,
            L_HIDDEN,
            L_OUTPUT,
            };

        enum F_ACTIVATION
            {
            F_LINEAR = 0,
            F_SYGMOID,
            };

        //Конструктор. 
        //ПАРАМЕТРЫ.
        //   inputs_cnt  - число входов;
        //   in_cnt      - число нейронов в скрытом слое;
        //   outputs_cnt - число выходов.
        //   q           - коэффициент масштабирования (входы делятся на него,
        //                 выход умножается).
        mlp( int inputs_cnt, int in_cnt, int outputs_cnt, int q = 1 );

        //Инициализация коэффициентов случайным образом.
        void init_weights();

        //Обновляет состояние всех объектов для данного контроллера.
        //ПАРАМЕТРЫ.
        //   е - желаемая ошибка обучения;
        //   sample - обучающая выборка;
        //   max_iteration_cnt - максимальное число итераций. Обучение останавливается
        //      при его достижении.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int static_learn( float e, i_learn_samples *sample, int max_iteration_cnt,
            bool is_err = false );

        //Прогнозирует значение функции.
        //ПАРАМЕТРЫ.
        //   cnt   - сколько значений спрогнозировать;
        //   x_in  - стартовые значения;
        //   y_out - массив прогнозированных значений.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int prognose( int cnt, float *x_in, float *y_out );

        int prognose( int cnt, float k, float data[] );

        int save_prognose_to_file( char *file_name );

        //Прогнозирует значение функции.
        //ПАРАМЕТРЫ.
        //   cnt   - сколько значений спрогнозировать;
        //   x_in  - стартовые значения;
        //   out_col_n - номер столбца, куда записывать выходные значения.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int prognose( int cnt, float **x_in, int out_col_n );

        //Сохранение коэффициентов в файл в текстовом виде.
        //ПАРАМЕТРЫ.
        //   file_name - имя файла.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int save_to_file( char *file_name );

        //Считывание коэффициентов из файла в текстовом виде.
        //ПАРАМЕТРЫ.
        //   file_name - имя файла.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int load_from_file( char *file_name );

        //Сохранение прогноза в файл в текстовом виде.
        //ПАРАМЕТРЫ.
        //   file_name - имя файла;
        //   y_out     - массив со значениями прогноза.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int save_prognose_to_file( char *file_name, float *y_out, int cnt );

        //Сохранение прогноза в файл в текстовом виде.
        //ПАРАМЕТРЫ.
        //   file_name - имя файла;
        //   y_out     - массив, 3-е значение - прогноз.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int save_prognose_to_file( char *file_name, float **y_out, 
            int cnt, int column_cnt, int column_window_size,
            int factor_k );
                     
        //Вывод на консоль коэффициентов персептрона.
        void print();

        //Получить значения выходов НС для заданного входного образа.
        //ПАРАМЕТРЫ.
        //   x_in  - выборка для прогнозирования. 
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   прогнозируемый выход.
        float* solve_out( float *x_in );
        float* solve_out( float **x_in );

        int get_q() const
            {
            return q;
            }

    private:
        //Выполнение одной итерации обучения.
        //ПАРАМЕТРЫ.
        //   sample_x           - выборка для обучения; 
        //   sample_y_or_error  - эталонный выход (ошибка прогнозирования); 
        //   type               - тип обучения ( на основе эталонного значения или ошибки); 
        //   E_k                - возвращается среднеквадратичная ошибка; 
        //   err                - массивы для хранения ошибок нейронов сети; 
        //   is_solve_net_out   - пересчитывать ли выход сети.
        void learn_iteration( float *sample_x, float *sample_y_or_error, 
            char type, float &E_k, float **err, char is_solve_net_out );

    
        int inputs_cnt;     //Количество входов.
        int hidden_cnt;     //Количество нейронов в скрытом слое.
        int outputs_cnt;    //Количество выходов.

        float ***w;     //Весовые коэффициенты.
        float **T;      //Пороговые значения.

        float *x;       //Входные значения.
        float *y_hidden;//Выходные значения для скрытого слоя.  
        float *y;       //Выходные значения.  

        float *y_prognose; //Прогнозные значения.
        int    y_prognose_cnt;

        int q;  //Коэффициент масштабирования.

        //Получить значения выходов слоя НС для заданного входного образа.
        float* solve_layer_out( float *x_in, int layer_type, int activation_f_type );
        float* solve_layer_out( float **x_in, int layer_type, int activation_f_type );

        enum VAL_RANGE
            {
            VR_MIN = -20,
            VR_MAX = 20,
            };
         
        static const int MAX_BUFF_SIZE;


#ifdef WIN32
        //Сохранение коэффициентов в поток в текстовом виде.
        //ПАРАМЕТРЫ.
        //   stream - указатель на массив типа char.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int save_to_stream( char *stream );

        //Считывание коэффициентов из потока в текстовом виде.
        //ПАРАМЕТРЫ.
        //   stream - указатель на массив типа char.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int load_from_stream( char *stream );
#endif // WIN32
    };

#endif // MLP_H