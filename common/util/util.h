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

//-----------------------------------------------------------------------------
#if defined LINUX || defined UCLINUX
#define debug_break asm ( "int" "3" ) ///< Установка точки прерывания.
#endif // defined LINUX || defined UCLINUX
//-----------------------------------------------------------------------------
/// @brief Защищенная работа с указателем.
///
/// Реализована проверка на работу с null указателем, автоматическое удаление
/// объекта.
template < class type > class smart_ptr
    {
private:
    type* pointer;

public:
    smart_ptr() : pointer( NULL )
        {
        }

    smart_ptr( type* p ) : pointer( p )
        {
        }

    smart_ptr( const smart_ptr< type > &ptr ) : pointer( ptr.pointer )
        {
        }

    smart_ptr< type >& operator = ( type* new_t )
        {
        if ( pointer ) delete pointer;
        pointer = new_t;
        
        return *this;
        }

    ~smart_ptr()
        {
        if ( pointer ) delete pointer;
        pointer = NULL;
        }

    operator type*()
        {
        if ( pointer == NULL )
            {
#ifdef DEBUG

#ifdef DEBUG_TRACE
            debug_break;
#endif // DEBUG_TRACE

#if defined LINUX || defined UCLINUX
            printf( "smart_ptr - pointer is NULL!\n" );
#endif // defined LINUX || defined UCLINUX

#endif // DEBUG
            pointer = new type;
            }

        return pointer;
        }

    type* operator->()
        {
        if ( pointer == NULL )
            {
#ifdef DEBUG
#if defined LINUX || defined UCLINUX
            printf( "smart_ptr - dereferencing NULL!\n" );
#endif // defined LINUX || defined UCLINUX

#ifdef DEBUG_TRACE
            debug_break;
#endif // DEBUG_TRACE

#endif // DEBUG

            pointer = new type;
            }
        return pointer;
        }

private:
    type* operator[] ( unsigned int index )
        {
        return pointer;
        }

    smart_ptr< type >& operator = ( const smart_ptr< type >& new_t )
        {
        return *this;
        }

    };
//-----------------------------------------------------------------------------
