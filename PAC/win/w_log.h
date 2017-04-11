#ifndef w_log_h__
#define w_log_h__

#include "log.h"

#include <iostream>
#include <iomanip>
#include "ctime"
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

#pragma warning( push )
#pragma warning( disable: 4996 ) //warning C4996: 'localtime': This function or variable may be unsafe.
		void virtual write_log(PRIORITIES priority)
			{
            std::time_t _tm = std::time( nullptr );
            std::tm tm = *std::localtime( &_tm );

            std::cout << std::put_time( &tm, "log -> \"%Y-%m-%d %H.%M.%S\" " );
            
            switch ( priority )
                {
                case i_log::P_ALERT:
                    std::cout << "Alert  (1) -> ";
                    break;

                case i_log::P_CRIT:
                    std::cout << "Critic (2) -> ";
                    break;

                case i_log::P_ERR:
                    std::cout << "Error  (3) -> ";
                    break;

                case i_log::P_INFO:
                    std::cout << "Info   (6) -> ";
                    break;

                default:                    
                    break;
                }

            std::cout << msg << std::endl;	

            fflush( stdout );
			}
#pragma warning( pop )

		w_log()
			{
			}
	};

#endif // w_log_h__
