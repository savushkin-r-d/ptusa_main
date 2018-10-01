#include "w_console.h"

HANDLE _hStdOut;

void SetColor( WORD color )
    {
    if ( _hStdOut == NULL ) _hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( _hStdOut, color );
    }

int kb_hit()
    {
    return kbhit();
    }

int get_char()
    {
    //fflush( stdout );
    //getch();

    return 0; // В режиме эмуляции просто выводим на консоль сообщение.
    }