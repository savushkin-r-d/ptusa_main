#ifndef PLANT
#define PLANT

class plant
    {
    public:
        plant();

        //Получение нового выхода объекта на основе поданного управляющего воздействия.
        float get_new_out( float control_value );

        float get_current_out() const;

        void set_k( float new_k )
            {
            k = new_k;
            }

        float get_k() const
            {
            return k;
            }

    private:
        float k;

        float prev_obj_value;      //Предыдущее значение выхода объекта управления.
        float current_obj_value;   //Текущее значение выхода объекта управления.
        float prev_control_value;  //Предыдущее значение управляющего воздействия.
    };
#endif // PLANT