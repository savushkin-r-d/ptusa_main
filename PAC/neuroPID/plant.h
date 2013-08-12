class plant
    {
    public:
        plant();

        //Получение нового выхода объекта на основе поданного управляющего воздействия.
        double get_new_out( double control_value );

        double get_current_out() const;

        void set_k( double new_k )
            {
            k = new_k;
            }

        double get_k() const
            {
            return k;
            }

    private:
        double k;

        double prev_obj_value;      //Предыдущее значение выхода объекта управления.
        double current_obj_value;   //Текущее значение выхода объекта управления.
        double prev_control_value;  //Предыдущее значение управляющего воздействия.
    };