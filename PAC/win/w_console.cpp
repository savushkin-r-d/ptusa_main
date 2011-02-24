#include "w_console.h"

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