/// @file statistic_manager.h
/// @brief Классы, которые реализуют функции сбора статистики устройств

#ifndef STATISTIC_MANAGER_H
#define STATISTIC_MANAGER_H
#include "param_ex.h"
#include "PAC_dev.h"


class device_with_statistic
	{
	public:
		device_with_statistic();
		device_with_statistic( device* dev, int device_resource );

		int get_cur_device_stat();
		int get_device_working_time();
		float get_cur_device_wear();
		void check_state_changes();
	private:
		device *dev;
		int prev_device_state = 0;
		int cur_stat;
		int working_time;
		int device_resource;
		saved_params_float *par;
	};

class statistic_manager
	{
	public:
		statistic_manager();
		~statistic_manager();
		/// @brief Получение единственного экземпляра класса.
		static statistic_manager *get_instance();
		void add_dev( device *dev, int device_resource );
		void evaluate();
		statistic_manager *get_instance();

	private:
		/// @brief Единственный экземпляр класса.
		static auto_smart_ptr < statistic_manager > instance;
		std::vector<device_with_statistic *> devs_with_stat;
	};

statistic_manager* G_STATISTIC_MANAGER();
#endif //STATISTIC_MANAGER_H
