/// @file node_dev.h
/// @brief Устройство управления узлом сетевых настроек.
///
/// Позволяет централизованно управлять сетевыми настройками узла и
/// отслеживать состояние команд проброса портов.

#pragma once

#include "device/device.h"
#include "bus_coupler_io.h"

//-----------------------------------------------------------------------------
/// @brief Устройство управления узлом сетевых настроек.
///
/// Предоставляет доступ к IP-адресу узла, состоянию узла и командам для
/// управления пробросом портов.
class node_dev : public device, public i_Lua_save_device
	{
	public:
		enum class PARAM
			{
			P_NODE_IDX = 1,  ///< Индекс узла I/O.

			PARAMS_COUNT
			};

		enum class PROPERTIES
			{
			ST = 1,      ///< Состояние узла (0-выключен, 1-включен).
			WEB,         ///< Состояние проброса порта 80.
			STARTUP,     ///< Состояние проброса порта 1962.
			CMD,         ///< Канал команд.
			};

		/// @param name - имя устройства.
		explicit node_dev( const char* name,
			device::DEVICE_SUB_TYPE sub_type = device::DEVICE_SUB_TYPE::DST_NODE );

		~node_dev() override = default;

		void evaluate_io() override;

		void set_string_property( const char* field, const char* value ) override;

		int save_device( char* buff ) override;

		const char* get_name_in_Lua() const override;

		/// @brief Получение IP-адреса узла.
		///
		/// @return - IP-адрес узла в виде строки.
		const char* get_ip() const;

		/// @brief Получение состояния узла.
		///
		/// @return - состояние узла (0-выключен, 1-включен, -1-ошибка).
		int get_node_state() const;

		/// @brief Установка состояния свойства.
		///
		/// @param prop - свойство для установки.
		/// @param value - новое значение.
		void set_property_value( PROPERTIES prop, int value );

		/// @brief Получение состояния свойства.
		///
		/// @param prop - свойство для получения.
		/// @return - значение свойства.
		int get_property_value( PROPERTIES prop ) const;

#ifndef PTUSA_TEST
	private:
#endif

		int st_value = 0;       ///< Состояние узла.
		int web_value = 0;      ///< Состояние проброса порта 80.
		int startup_value = 0;  ///< Состояние проброса порта 1962.
		int cmd_value = 0;      ///< Канал команд.
	};

