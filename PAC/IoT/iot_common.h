#ifndef iot_common_h__
#define iot_common_h__

/*!
 * \brief Идентификаторы для классов, использующих сокеты
 */
#define SOCKID_ALTIVAR 1000

/*!
 * \class i_iot_node
 *
 * \brief Класс-интерфейс для связывания устройств в программе с реальными устройствами
 *
 */
class i_iot_node
{
public:
	virtual int set_state(int value, int index = 0);
	virtual int get_state(int index = 0);
	virtual int set_value(float value, int index = 0);
	virtual float get_value(int index = 0);
	virtual unsigned long set_counter(unsigned long value, int index = 0);
	virtual int get_counter(int index = 0);

	i_iot_node();
	virtual ~i_iot_node();
};

/*!
 * \brief Общая функция инициализации
 */
void IOT_INIT();

/*!
 * \brief Общая функция выполняемая в главном цикле программы
 */
void IOT_EVALUATE();

/*!
 * \brief Выполняется при завершении программы
 */
void IOT_FINAL();

#endif // iot_common_h__