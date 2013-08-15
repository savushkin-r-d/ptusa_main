//ОБЩЕЕ ОПИСАНИЕ.
//  Содержит описания классов, которые используются реализации нейронной сети.
#ifndef MLP_H
#define MLP_H

#ifdef WIN32
#define printf_ printf
//---------------------------------------------------------------------------------------
//ОПИСАНИЕ
//  Класс, который представляет интерфейс обучающей выборки.
class i_learn_samples
    {
    public:
        //Возвращает количество элементов в обучающей выборке.    
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   Количество элементов в обучающей выборке.        
        virtual int get_samples_cnt() const;

        //Возвращает выборку с заданным номером.
        //ПАРАМЕТРЫ.
        //   index   - номер обучающей выборки;
        //   samle_x - эталонные значения входов.
        //   samle_y - эталонные значения выходов.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        // > 0 - OK;
        // <=0 - ошибка.        
        virtual float* get_sample_x( int index ) = 0;
        virtual float* get_sample_y( int index ) = 0;

        //Получение параметров выборки.
        virtual int get_inputs_cnt()  const;
        virtual int get_outputs_cnt() const;

        //Считывание коэффициентов из файла в текстовом виде.
        //ПАРАМЕТРЫ.
        //   file_name - имя файла.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        virtual int load_from_file( char *file_name ) = 0;

        //Сохранение коэффициентов в файл в текстовом виде.
        //ПАРАМЕТРЫ.
        //   file_name - имя файла.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        virtual int save_to_file( char *file_name ) = 0;

    protected:
        int window_size;
        int samples_cnt;
    };
//---------------------------------------------------------------------------------------
//ОПИСАНИЕ
//  Класс, который представляет интерфейс обучающей выборки для некоторой функции.
class function_sample: public i_learn_samples
    {
    public:
        //Конструктор.
        //ПАРАМЕТРЫ.
        //   x_start        - начальное значение х;
        //   x_end          - конечное значение х;
        //   dx             - значение шага dx;
        //   window_size    - количество элементов в окне.
        function_sample( float x_start, float x_end, float dx, int window_size );

        function_sample()
            {
            }

        //Возвращает выборку с заданным номером.
        //ПАРАМЕТРЫ.
        //   index   - номер обучающей выборки;
        //   samle_x - эталонные значения входов.
        //   samle_y - эталонные значения выходов.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        // > 0 - указатель на данные выборки;
        //   0 - ошибка.        
        float* get_sample_x( int index );
        float* get_sample_y( int index );

        void print();

        int load_from_file( char *file_name );
        int save_to_file( char *file_name );

    protected:
        float **x; //Эталонные входы.
        float **y; //Эталонные выходы.
    };
//---------------------------------------------------------------------------------------
//ОПИСАНИЕ
//  Класс, который представляет интерфейс обучающей выборки для некоторой функции.
class function_sample_with_time: public function_sample
    {
    public:
        //Конструктор.
        //ПАРАМЕТРЫ.
        //   x_start        - начальное значение х;
        //   x_end          - конечное значение х;
        //   dx             - значение шага dx;
        //   window_size    - количество элементов в окне.
        function_sample_with_time( float x_start, float x_end, float dx, int window_size );
    };
//---------------------------------------------------------------------------------------
//ОПИСАНИЕ
//  Класс, который представляет обучающую выборку, которая загружается из файла. Файл имеет
//  следующую структуру:
//    число переменных (столбцов данных);
//    число данных (строк данных);
//    макс. значение, на него будут делиться числа, чтобы получить значения < 1;
//    далее идут данные в столбцах.
//  Пример, sample.txt:
//  3
//  5
//  200
//  122 134 234 
//  102 134  34 
//  122 134   9 
//  102 234  34 
class stored_sample: public i_learn_samples
    {
    public:

        //Возвращает коэффициент масштабирования значений выборки.
        int get_factor_k() const;

        //Возвращает размер окна для отдельного столбца.
        int get_column_window_size() const;

        //Конструктор.
        stored_sample();

        //Возвращает обучающую выборку. Далее на ее основе можно прогнозировать
        //значения.
        float** get_sample_x();

        //Возвращает выборку с заданным номером.
        //ПАРАМЕТРЫ.
        //   index   - номер обучающей выборки;
        //   samle_x - эталонные значения входов.
        //   samle_y - эталонные значения выходов.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        // > 0 - указатель на данные выборки;
        //   0 - ошибка.        
        float* get_sample_x( int index );
        float* get_sample_y( int index );

        //Создает обучающую выборку.
        //ПАРАМЕТРЫ.
        //   window_size - размер окна;
        //   exclude_every_i  - использовать каждую i-ю строку для формирования
        //      обучающих данных. Если равен 1, использовать все строки.
        int create_learn_sample( int window_size, int learn_samples_count = -1, 
            int exclude_every_i = 1, int etalon_column_number = 0 );

        void print();

        int load_from_file( char *file_name );
        int load_from_file( char *file_name, char is_ignore_first_column );
        int save_to_file( char *file_name );

    private:        
        int column_cnt;
        int row_cnt;
        int factor_k;

        float **data;   //Считанные значения
        float **x;      //Эталонноые входы.
        float **y;      //Эталонноые выходы.  
    };
//---------------------------------------------------------------------------------------
//ОПИСАНИЕ
//  Класс, который представляет интерфейс обучающей выборки для тестовой функции.
class test_sample: public i_learn_samples
    {
    public:
        //Конструктор.
        //ПАРАМЕТРЫ.
        //   x_start        - начальное значение х;
        //   x_end          - конечное значение х;
        //   dx             - значение шага dx;
        //   window_size    - количество элементов в окне.
        test_sample( float x_start, float x_end, float dx, int window_size );

        //Возвращает выборку с заданным номером.
        //ПАРАМЕТРЫ.
        //   index   - номер обучающей выборки;
        //   samle_x - эталонные значения входов.
        //   samle_y - эталонные значения выходов.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        // > 0 - указатель на данные выборки;
        //   0 - ошибка.        
        float* get_sample_x( int index )
            {            
            if ( index >= samples_cnt )
                {
                return 0;
                }

            return x[ index ];
            }

        float* get_sample_y( int index )
            {
            if ( index >= samples_cnt )
                {
                return 0;
                }

            return y[ index ] ;
            }

        void print() {}

        int load_from_file( char *file_name )
            {
            return 0;
            }

        int save_to_file( char *file_name )
            {
            return 0;
            }

        int get_factor_k() const
            {
            return 1;
            }
        
        int get_column_window_size() const
            {
            return 1;
            }

        float** get_sample_x()
            {
            return x;
            }

    private:
        float **x; //Эталонные входы.
        float **y; //Эталонные выходы.
    };
//---------------------------------------------------------------------------------------
//ОПИСАНИЕ
//  Однослойный персептрон (без скрытого слоя).
class slp  
    {
    public: 
        //Конструктор. 
        //ПАРАМЕТРЫ.
        //   inputs_cnt  - число входов;
        //   outputs_cnt - число выодов.
        slp( int inputs_cnt, int outputs_cnt );

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
        int learn( float e, i_learn_samples *sample, int max_iteration_cnt );

        //Вывод на консоль коэффициентов персептрона.
        void print();

        //Прогнозирует значение функции.
        //ПАРАМЕТРЫ.
        //   cnt   - сколько значений спрогнозировать;
        //   x_in  - стартовые значения;
        //   y_out - массив спрогнозированных значений.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка.        
        int prognose( int cnt, float *x_in, float *y_out );

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

    private:
        int inputs_cnt; //Количество входов.
        int outputs_cnt;//Количество выходов.

        float **w;      //Весовые коэффициенты.
        float *T;       //Пороговые значения.

        float *x;       //Входные значения.
        float *y;       //Выходные значения.  

        //Получить значения выходов НС для заданного входного образа.
        float* solve_out( float *x_in );

        static const int MAX_BUFF_SIZE;

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
    };
#endif // WIN32

#ifdef PAC
#define printf_ Print
#endif // PAC
//---------------------------------------------------------------------------------------
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
#ifdef PAC 
        //Конструктор создания объекта на основе файла.
        mlp( char *filename );

        //Установить веса по умолчанию на основе описания из файла.
        int set_default_weight( );
#endif // PAC

#ifdef WIN32
        //Конструктор. 
        //ПАРАМЕТРЫ.
        //   inputs_cnt  - число входов;
        //   in_cnt      - число нейронов в скрытом слое;
        //   outputs_cnt - число выходов.
        mlp( int inputs_cnt, int in_cnt, int outputs_cnt );

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
        int static_learn( float e, i_learn_samples *sample, int max_iteration_cnt );

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
#endif // WIN32
                     
        //Вывод на консоль коэффициентов персептрона.
        void print();

        //Получить значения выходов НС для заданного входного образа.
        //ПАРАМЕТРЫ.
        //   x_in  - выборка для прогнозирования. 
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   прогнозируемый выход.
        float* solve_out( float *x_in );

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

    private:
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

        //Получить значения выходов слоя НС для заданного входного образа.
        float* solve_layer_out( float *x_in, int layer_type, int activation_f_type );

        enum VAL_RANGE
            {
            VR_MIN = -20,
            VR_MAX = 20,
            };
         
        static const int MAX_BUFF_SIZE;

#ifdef PAC
        char mlp_filename[ 13 ];    //Файл с описанием структуры.
#endif // PAC

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
//---------------------------------------------------------------------------------------
#ifdef PAC
//ОПИСАНИЕ
//Класс, предназначенный для сохранения данных о работе о создания обучающих образов.
//Образ выглядит следующим образом, допустим для прогнозирования используется температура
//t1 и t2, управляющее воздействие u1. Тогда образ выглядит следующим образом (размер окна
//равен 3): [ t1 - 2, t1 - 1, t1,   t2 - 2, t2 - 1, t2,   u1 - 2, u1 - 1, u1 ]. Где 
//t1     - текущая температура, 
//t1 - 1 - температура в предудущий момент времени - один такт назад,
//t1 - 2 - температура два такта назад.
//Аналогично рассматриваются t2 и u1.
class MLP_sample
    {
    public:
        //Конструктор.
        //ПАРАМЕТРЫ.
        //   window_size     - размер окна (общий для всех переменных);
        //   variables_count - количество переменных;
        //   max_var_value   - максимальное значение переменных,
        //                     для масштабирования значения в интервале [ 0; 1 ].
        MLP_sample( int window_size, int variables_count, int max_var_value );

        //Возвращает данные, на основании которых получаем прогноз.
        float* get_sample_for_prognose();

        //Возвращает предыдущие данные, на основании которых получаем прогноз.
        float* get_prev_sample_for_prognose();

        //Добавляем новые данные в выборку для прогнозирования, при этом
        //сдвигаем данные в выборке.
        //ПАРАМЕТРЫ.
        //   variable_number - номер переменной, для которой добавляются данные;
        //   value           - добавляемое значение.
        //ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ.
        //   0 - OK;
        // < 0 - ошибка. 
        int add_new_data( int variable_number, float value );

        //Возвращает максимальное значение переменных.
        int get_max_var_value() const;

#ifdef DEBUG
        void print();
#endif // DEBUG

    private:
        int     variables_count;    //Количество переменных, которые образуют выборку.
        float   *data;              //Данные, образующие выборку. 

        int     window_size;        //Размер окна (общий для всех переменных).
        int     max_var_value;      //Максимальное значение переменных,
                                    //для масштабирования значения в интервале [ 0; 1 ].
    };
//---------------------------------------------------------------------------------------
////ОПИСАНИЕ
////  Нейроконтроллер.
//class n_controller
//    {
//    public:
//        TParams *par;        
//        enum PARAM 
//            {
//            PAR_Z,            //0 Требуемое значение.
//            PAR_dt,           //1 Интервал расчёта
//            PAR_dmax,         //2 Мax значение входной величины.
//            PAR_dmin,         //3 Мin значение входной величины.
//            PAR_AccelTime,    //4 Время выхода на режим регулирования.
//            PAR_IsManualMode, //5 Ручной режим.
//            PAR_UManual,      //6 Заданное ручное значение выходного сигнала.
//            PAR_Uk,           //7 Выход ПИД.
//            };         
//        enum WORK_PARAMS 
//            {
//            WPAR_Uk,          //0 Выход ПИД-регулятора.
//            WPAR_Z,           //1 Требуемое значение ПИД.
//            };
//
//        //Конструктор. Создает структуру персептронов на основе описания из файла.
//        //ПАРАМЕТРЫ.
//        //   par        - указатель на параметры;
//        //   file_name  - строка с именем файла, содержащего структуру персептронов;
//        n_controller( TParams* par, char file_name = "n_contr.txt",
//            int startParamIndex = 0, int startWorkParamsIndex = 8 );
//        //~n_controller(); 
//
//        void  On( char isDownToInAccelMode = 0 );
//        void  Off();
//
//        float Eval( float currentValue, int deltaSign = 1 );
//        void  Reset( float new_uk_1 = 0 );
//
//        void  SetZ ( float newZ );        //Установить новое задание ПИД.
//
//    private:
//        mlp *n_control;         //Нейроконтроллер.
//        mlp *n_emulator;        //Нейроэмулятор.
//
//        enum STATES 
//            {
//            ST_OFF,
//            ST_ON,  
//            };
//        int state;
//
//        int startParamIndex;  
//        int startWorkParamsIndex;
//
//        unsigned long startTime;
//        unsigned long lastTime;
//
//        char prevManualMode;
//
//        char isDownToInAccelMode;  //Надо ли при старте регулятора уменьшать, а не увеличивать выходную величину.
//    };
#endif // PAC

#endif // MLP_H