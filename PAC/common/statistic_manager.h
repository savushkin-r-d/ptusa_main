﻿/// @file statistic_manager.h
/// @brief Классы, которые реализуют функции сбора статистики устройств.
/// 
/// К собираемой статистике относятся следующие параметры:
/// - Количества изменений состояния устройства, например, количество 
/// срабатывания цилиндра; 
/// - Время активной работы (state > 0), например, время работы мотора;
/// - Ресурс устройства (задается в описании устройства в поле stat);
/// - Износ устройства, отношение количества срабатываний к ресурсу устройства;
/// 
/// @author Озимок Ярослав Викторович.
#ifndef STATISTIC_MANAGER_H
#define STATISTIC_MANAGER_H
#include "param_ex.h"
#include "PAC_dev.h"
#include <fmt/core.h>
#include <memory>

//-----------------------------------------------------------------------------
/// @brief Интерфейс объекта со сбором статистики.
/// 
/// Объект, реализующая данный интерфейс, собирает собственную статистику,
/// которая затем передается на сервер в виде тегов.
class i_statistic_collecting : public i_cmd_device
	{
	public:
		/// @brief Запись в буфер текущей статистики объекта.
		/// @param buff [ out ] - адрес буфера, куда будут записываться данные.
		/// @return >= 0 - количество записанных байт.
		virtual int save_common_stat( char *buff ) = 0;

		/// @brief Итерация сбора собственной статистики.
		virtual void evaluate_collecting() = 0;

		/// @brief Получение имени объекта.
		/// @return Имя объекта.
		virtual const char *get_name() = 0;

		virtual ~i_statistic_collecting() = default;
	};
//-----------------------------------------------------------------------------
/// @brief Класс реализует устройство (мотор, клапан) со сбором статистики.
/// 
/// В каждом цикле управляющей программы проверяется изменение состояния
/// устройства. Реализованы методы для получения текущей статистики.
class device_with_statistic : public i_statistic_collecting
	{
	public:
		device_with_statistic( device* dev, int device_resource );

		/// @brief Получение текущей статистики изменения состояния устройства.
		/// @return Количество изменений состояния устройства.
		int get_cur_device_stat() const;

		/// @brief Получение общего времени работы устройства.
		/// @return Общее время работы устройства в секундах.
		int get_device_working_time_sec() const;

		/// @brief Получение общего времени работы устройства.
		/// @return Общее время работы устройства в часах.
		int get_device_working_time_h() const;

		/// @brief Получение текущего износа устройства.
		/// @return Текущий износ устройства (%).
		float get_cur_device_wear() const;

		/// @brief Проверка изменения состояния устройства.
		void evaluate_collecting() override;

		/// @brief Запись в буфер текущей статистики устройва.
		/// @param buff [ out ] - адрес буфера, куда будут записываться данные.
		/// @return >= 0 - количество записанных байт.
		int save_common_stat( char* buff ) override;

		/// @brief Получение имени устройства, у которого собирается статистика.
		/// @return Имя устройства.
		const char *get_name() override;

		/// @brief Выполнение числовой команды.
		/// @param prop [ in ] - имя свойства.
		/// @param idx [ in ]  - индекс для свойства.
		/// @param val [ in ]  - значение.
		/// @return 0 - ок.
		/// @return 1 - ошибка.
		int set_cmd( const char *prop, u_int idx, double val ) override;

		/// @brief Выполнение строковой команды.
		/// @param prop [ in ] - имя свойства.
		/// @param idx [ in ]  - индекс для свойства.
		/// @param val [ in ]  - значение.
		/// @return 0 - ок.
		/// @return 1 - Ошибка.
		int set_cmd( const char *prop, u_int idx, char *val ) override;

	private:
		device *dev;                  ///< Отслеживаемое устройство.
		int prev_device_state;        ///< Предыдущее состояние устройства.
		unsigned long start_time = 0; ///< Время начала работы (state > 0).
		int state_change_count;       ///< Текущее количество изменений состояния.
		unsigned long working_time;   ///< Общее время работы устройства.
		int device_resource;          ///< Ресурс устройства.
		saved_params_float par = saved_params_float( STAT_INDEXES_COUNT );
								      ///< Сохраняемые параметры статистики.
		enum STAT_INDEXES             ///< Индексы полей статистики в параметрах.
			{
			STATE_CHANGE_INDEX = 1,   ///< Индекс текущего количества изменений.
			WORKING_TIME_INDEX = 2,   ///< Индекс общего времени работы.
			STAT_INDEXES_COUNT = 2,   ///< Количество парметров статистики.
			};
	};
//-----------------------------------------------------------------------------
/// @brief Класс-менеджер устройств со сбором статистики.
/// 
/// Реализует методы для работы с устройствами.
class statistic_manager : public i_Lua_save_device
	{
	public:
		statistic_manager();

		/// @brief Получение единственного экземпляра класса.
		/// @return Единственный экземпляр менеджера устройств статистики.
		static statistic_manager *get_instance();

		/// @brief Добавление нового устройства со сбором статистики 
		/// (вызывается из Lua).
		/// @param *dev - указатель на устройство, статистику которого следует
		/// собирать.
		/// @param device_resource - ресурс (запас прочности) устройства.
		/// @return Указатель на добавленное устройство со статистикой.
		device_with_statistic* add_new_dev_with_stat( device *dev, int device_resource );

		/// @brief Итерация сбора статистики. 
		void evaluate() const;

		/// @brief Сохранение статистики устройств в буфер.
		/// @param buff [ out ] - адрес буфера, куда будут записываться данные.
		/// @return >= 0 - количество записанных байт.
		int save_device( char *buff ) override;

		/// @brief Отладочная печать объекта в консоль.
		const char *get_name_in_Lua() const override;

	private:
		/// @brief Единственный экземпляр класса.
		static auto_smart_ptr < statistic_manager > instance;
		/// @brief Вектор устройств со сбором статистики
		std::vector< std::unique_ptr< i_statistic_collecting > > objs_with_stat;
	};
//-----------------------------------------------------------------------------
/// @brief Глобальная точка доступа к менеджеру устройств со статистикой.
/// @return Единственный экземпляр менеджера устройств статистики.
statistic_manager* G_STATISTIC_MANAGER();
#endif //STATISTIC_MANAGER_H