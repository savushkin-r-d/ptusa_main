#ifndef w_log_h__
#define w_log_h__

#include "log.h"

#include <stdio.h>
//-----------------------------------------------------------------------------
/// @brief Работа с журналом.
///
/// Базовый интерфейсный класс для журнала.
/// Содержит основные методы работы - запиь в лог, т.д.
///
class w_log: public i_log
	{
	public:

		virtual ~w_log()
			{
			}

		void virtual write_log(PRIORITIES priority)
			{
			printf("\n\r%s", msg);
			}

		w_log()
			{
			}
	};

#endif // w_log_h__
