/// @file rm_tech_def.h
/// @brief Реализована функциональность технологических объектов для управления
/// удаленно (через мастер-контроллер).
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.
///
/// @par Текущая версия:
/// @$Rev: 678 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-05-29 16:44:39#$.

#ifndef RM_TECH_DEFINITION_H
#define RM_TECH_DEFINITION_H

#include <vector>

#include "s_types.h"
#include "PAC_err.h"

//-----------------------------------------------------------------------------
/// @brief Работа с технологическим объектом.
///
/// Базовый класс для технологического объекта (танка, гребенки). Содержит
/// основные методы работы - работа с режимами и т.д.
/// Для информировании сервера (события, аварии, ...) используется следующий 
/// подход: имеется вектор из сообщений и методы работы с ним.
///
class rm_tech_object
    {
    public:
        /// @brief Включение/выключение режима.
        ///
        /// @param mode      - режим.
        /// @param new_state - новое состояние режима.
        ///
        /// @return  0 - режим включен (отключен).
        /// @return  1 - режим ранее был включен (отключен).
        /// @return  3 - нет такого режима.
        int set_mode( u_int mode, int new_state, char* err_info );

        /// @brief Проверка возможности включения режима.
        ///
        /// Если данный метод возвращает 1, то тогда режим не включается.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим нельзя включить.
        /// @return 0 - режим можно включить.
        int check_on_mode( u_int mode, char* err_info );    

        private:
            /// @brief Имя объекта.
            std::string name;
    };
//-----------------------------------------------------------------------------
class i_rm_cmmctr
    {
    public:
        i_rm_cmmctr( char* name, char* IP_address, int remote_PAC_id ):
            name( name ), IP_address( IP_address ), remote_PAC_id( remote_PAC_id )
            {
            }

        /// @brief Отсылка заданной команды PAC.
        virtual int send_2_PAC( int service_n, const char *cmd_str,
            int length  ) = 0;

        /// @brief Обмен данными от PAC.
        virtual int evaluate() = 0;

        void print() const
            {
            Print( "\"%s\" - \"%s\" (%d)\n",
                name.c_str(), IP_address.c_str(), remote_PAC_id );
            }

    protected:
        char* get_out_data( unsigned int &cnt ) 
            {    
            cnt = answer_size;
            return Lua_str;
            }

        enum PARAMS
            {
            P_MAX_BUFFER_SIZE = 20*1024,
            };

        enum SERVICE_IDS
            {
            PAC_CMMCTR_SERVICE_ID = 1,
            };

        /// @brief Буфер для обмена данными с контроллером.
        char in_buff[ P_MAX_BUFFER_SIZE ];

        /// @brief Указатель в буфере на начало данных.
        char *Lua_str;

        std::string name;
        std::string IP_address;

        // @brief Номер удаленного PAC. 
        int remote_PAC_id;

        // @brief Размер ответа удаленного PAC.
        u_int answer_size;
    };
#endif // RM_TECH_DEFINITION_H
