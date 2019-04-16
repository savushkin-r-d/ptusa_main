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