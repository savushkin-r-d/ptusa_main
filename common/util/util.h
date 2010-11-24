/// @file util.h
/// @brief Реализованы различные полезные функции.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 96 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-07-09 15:57:50#$.

#include <stddef.h>
#include <stdio.h>
//-----------------------------------------------------------------------------
#if defined LINUX_OS && !defined PAC_WAGO_750_860
#define debug_break asm ( "int" "3" ) ///< Установка точки прерывания.
#endif // defined LINUX_OS

#ifdef WIN_OS
#define debug_break _asm { int 3 }
#endif
//-----------------------------------------------------------------------------
size_t strlcpy( char *dst, const char *src, size_t siz );
//-----------------------------------------------------------------------------
/// @brief Защищенная работа с указателем.
///
/// Реализована проверка на работу с null указателем.
template < class type > class smart_ptr
    {
    protected:
        type* pointer;

    public:
        /// @brief Конструктор по умолчанию.
        smart_ptr() : pointer( NULL )
            {
            }

        /// @brief Конструктор на основе указателя.
        smart_ptr( type* p ) : pointer( p )
            {
            }

        /// @brief Конструктор копирования.
        smart_ptr( const smart_ptr< type > &ptr ) : pointer( ptr.pointer )
            {
            }

        /// @brief Оператор присваивания указателя.
        smart_ptr< type >& operator = ( type* new_t )
            {
            pointer = new_t;

            return *this;
            }

        /// @brief Оператор приведения типа к указателю.
        operator type*()
            {
            if ( pointer == NULL )
                {
    #ifdef DEBUG

    #ifdef PAC_PC
                debug_break;
    #endif // PAC_PC

                printf( "smart_ptr - pointer is NULL!\n" );
    #endif // DEBUG
                }

            return pointer;
            }

        /// @brief Оператор обращения к членам класса.
        type* operator->()
            {
            if ( pointer == NULL )
                {
    #ifdef DEBUG
                printf( "smart_ptr - dereferencing NULL!\n" );

    #ifdef PAC_PC
                debug_break;
    #endif // PAC_PC

    #endif // DEBUG
                }
            return pointer;
            }

    private:
        /// @brief Закрытый оператор индексирования - запрет использования.
        type* operator[] ( unsigned int index )
            {
            return pointer;
            }

        /// @brief Закрытый оператор присваивания - запрет использования.
        smart_ptr< type >& operator = ( const smart_ptr< type > &new_t )
            {
            return *this;
            }
    };
//-----------------------------------------------------------------------------
/// @brief Защищенная работа с указателем.
///
/// Автоматическое освобождение памяти при уничтожении, переприсваивании объекта.
template < class type > class auto_smart_ptr : public smart_ptr< type >
    {
    public:
        /// @brief Конструктор по умолчанию.
        auto_smart_ptr() : smart_ptr< type >( NULL )
            {
            }

        /// @brief Конструктор на основе указателя.
        auto_smart_ptr( type* p ) : smart_ptr< type >( p )
            {
            }

        void free()
            {
            if ( smart_ptr< type >::pointer )
                {
                delete smart_ptr< type >::pointer;
                smart_ptr< type >::pointer = NULL;
                }
            }

        virtual ~auto_smart_ptr()
            {
            free();
            }

        /// @brief Оператор присваивания указателя.
        auto_smart_ptr< type >& operator = ( type* new_t )
            {
            if ( smart_ptr< type >::pointer )
                {
                delete smart_ptr< type >::pointer;
                }
            smart_ptr< type >::pointer = new_t;
            return *this;
            }
    };
//-----------------------------------------------------------------------------
