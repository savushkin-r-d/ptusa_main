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
class node_dev : public device
	{
	public:

		/// @param name - имя устройства.
		explicit node_dev( const char* name );

        void set_io_node( io_manager::io_node *io_node );

		~node_dev() override = default;

		void evaluate_io() override;

        int save_device( char* buff ) const override;

		const char* get_name_in_Lua() const override;

		/// @brief Получение IP-адреса узла.
		///
		/// @return - IP-адрес узла в виде строки.
		const char* get_ip() const;

#ifndef PTUSA_TEST
	private:
#endif
        int web_value{};      ///< Состояние проброса порта 80.
        int startup_value{};  ///< Состояние проброса порта 1962.

        io_manager::io_node* node{ nullptr };
	};

