#ifndef PLANT
#define PLANT

class plant
    {
    public:
        plant();

        //ѕолучение нового выхода объекта на основе поданного управл€ющего воздействи€.
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

        void set_T( float new_T )
            {
            T = new_T;
            }

        float get_T() const
            {
            return T;
            }

    private:
        float k;
        float T;

        float h;

        float prev_obj_v1;          //ѕредыдущее значение выхода объекта управлени€.

        float current_obj_value;    //“екущее значение выхода объекта управлени€.
        
        float prev_control_v1;      //t-1  значение управл€ющего воздействи€.
        float prev_control_v2;      //t-2 значение управл€ющего воздействи€.
        float prev_control_v3;      //t-3 значение управл€ющего воздействи€.
        float prev_control_v4;      //t-4 значение управл€ющего воздействи€.
        float prev_control_v5;      //t-5 значение управл€ющего воздействи€.
    };
#endif // PLANT