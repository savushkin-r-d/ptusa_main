require "sys_wago" --Системные функции.

editor_version = 12
file_version   = 13

--Узлы WAGO
nodes =
    {
        {
        ntype   = 0,
        address = 1,
        modules =
            {
            { 466, '', '', '', '', '' },
            { 466, '', '', '', '', '' },
            { 402, '', '', '', '', '' },
            { 402, '', '', '', '', '' },
            { 402, '', '', '', '', '' },
            { 402, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 504, '', '', '', '', '' },
            { 638, '', '', '', '', '' },
            { 461, '', '', '', '', '' },
            { 461, '', '', '', '', '' },
            { 461, '', '', '', '', '' },
            { 612, '', '', '', '', '' },
            { 512, '', '', '', '', '' },
            { 512, '', '', '', '', '' },
            { 602, '', '', '', '', '' },
            { 402, '', '', '', '', '' },
            { 600, '', '', '', '', '' },
            }
        },
    }

--Устройства
devices =
    {
        {
        descr   = 'Счетчик №1',
        dtype   = 7,
        subtype = 0,
        number  = 1,
        AI  =
            {
                {
                node   = 0,
                offset = 4
                }
            },
        },

        {
        descr   = 'Счетчик №2',
        dtype   = 7,
        subtype = 0,
        number  = 2,
        AI  =
            {
                {
                node   = 0,
                offset = 5
                }
            },
        },

        {
        descr   = 'Текущий уровень 1',
        dtype   = 9,
        subtype = 0,
        number  = 1,
        AI  =
            {
                {
                node   = 0,
                offset = 0
                }
            },
        },

        {
        descr   = 'Текущий уровень 2',
        dtype   = 9,
        subtype = 0,
        number  = 2,
        AI  =
            {
                {
                node   = 0,
                offset = 1
                }
            },
        },

        {
        descr   = 'Текущий уровень 3',
        dtype   = 9,
        subtype = 0,
        number  = 3,
        AI  =
            {
                {
                node   = 0,
                offset = 2
                }
            },
        },

        {
        descr   = 'Текущий уровень 4',
        dtype   = 9,
        subtype = 0,
        number  = 4,
        AI  =
            {
                {
                node   = 0,
                offset = 3
                }
            },
        },

        {
        descr   = '1LS1-Н.У. Танк № 1',
        dtype   = 3,
        subtype = 0,
        number  = 101,
        DI  =
            {
                {
                node   = 0,
                offset = 1
                }
            },
        },

        {
        descr   = '1LS2-В.У. Танк № 1',
        dtype   = 3,
        subtype = 0,
        number  = 102,
        DI  =
            {
                {
                node   = 0,
                offset = 0
                }
            },
        },

        {
        descr   = '2LS1-Н.У. Танк № 2',
        dtype   = 3,
        subtype = 0,
        number  = 201,
        DI  =
            {
                {
                node   = 0,
                offset = 3
                }
            },
        },

        {
        descr   = '2LS2-В.У. Танк № 2',
        dtype   = 3,
        subtype = 0,
        number  = 202,
        DI  =
            {
                {
                node   = 0,
                offset = 2
                }
            },
        },

        {
        descr   = '3LS1-Н.У. Танк № 3',
        dtype   = 3,
        subtype = 0,
        number  = 301,
        DI  =
            {
                {
                node   = 0,
                offset = 5
                }
            },
        },

        {
        descr   = '3LS2-В.У. Танк № 3',
        dtype   = 3,
        subtype = 0,
        number  = 302,
        DI  =
            {
                {
                node   = 0,
                offset = 4
                }
            },
        },

        {
        descr   = '4LS1-Н.У. Танк № 4',
        dtype   = 3,
        subtype = 0,
        number  = 401,
        DI  =
            {
                {
                node   = 0,
                offset = 7
                }
            },
        },

        {
        descr   = '4LS2-2.У. Танк № 4',
        dtype   = 3,
        subtype = 0,
        number  = 402,
        DI  =
            {
                {
                node   = 0,
                offset = 6
                }
            },
        },

        {
        descr   = 'N1-Насос выдачи №1',
        dtype   = 1,
        subtype = 0,
        number  = 1,
        DO  =
            {
                {
                node   = 0,
                offset = 36
                }
            },
        DI  =
            {
                {
                node   = 0,
                offset = 8
                }
            },
        },

        {
        descr   = 'N-Насос выдачи №2',
        dtype   = 1,
        subtype = 0,
        number  = 2,
        DO  =
            {
                {
                node   = 0,
                offset = 37
                }
            },
        DI  =
            {
                {
                node   = 0,
                offset = 10
                }
            },
        },

        {
        descr   = '22N2 -Возврат мойки',
        dtype   = 1,
        subtype = 0,
        number  = 2202,
        DO  =
            {
                {
                node   = 0,
                offset = 38
                }
            },
        DI  =
            {
                {
                node   = 0,
                offset = 9
                }
            },
        },

        {
        descr   = 'SB1 старт выдача 1',
        dtype   = 10,
        subtype = 0,
        number  = 1,
        DI  =
            {
                {
                node   = 0,
                offset = 12
                }
            },
        },

        {
        descr   = 'SB2 стоп выдача 11',
        dtype   = 10,
        subtype = 0,
        number  = 2,
        DI  =
            {
                {
                node   = 0,
                offset = 14
                }
            },
        },

        {
        descr   = 'SB3 старт выдачи 2',
        dtype   = 10,
        subtype = 0,
        number  = 3,
        DI  =
            {
                {
                node   = 0,
                offset = 13
                }
            },
        },

        {
        descr   = 'SB4 стоп выдачи 2',
        dtype   = 10,
        subtype = 0,
        number  = 4,
        DI  =
            {
                {
                node   = 0,
                offset = 15
                }
            },
        },

        {
        descr   = 'Есть поток выдача 1',
        dtype   = 10,
        subtype = 0,
        number  = 5,
        DI  =
            {
                {
                node   = 0,
                offset = 16
                }
            },
        },

        {
        descr   = 'Есть поток выдача 2',
        dtype   = 10,
        subtype = 0,
        number  = 6,
        DI  =
            {
                {
                node   = 0,
                offset = 18
                }
            },
        },

        {
        descr   = 'Температура Т. №1',
        dtype   = 4,
        subtype = 0,
        number  = 1,
        AI  =
            {
                {
                node   = 0,
                offset = 6
                }
            },
        },

        {
        descr   = 'Температура Т. №2',
        dtype   = 4,
        subtype = 0,
        number  = 2,
        AI  =
            {
                {
                node   = 0,
                offset = 7
                }
            },
        },

        {
        descr   = 'Температура Т. №3',
        dtype   = 4,
        subtype = 0,
        number  = 3,
        AI  =
            {
                {
                node   = 0,
                offset = 8
                }
            },
        },

        {
        descr   = 'Температура Т. №4',
        dtype   = 4,
        subtype = 0,
        number  = 4,
        AI  =
            {
                {
                node   = 0,
                offset = 9
                }
            },
        },

        {
        descr   = 'темп. выдачи 1даче 1',
        dtype   = 4,
        subtype = 0,
        number  = 5,
        AI  =
            {
                {
                node   = 0,
                offset = 10
                }
            },
        },

        {
        descr   = 'темп. выдачи 2даче 1',
        dtype   = 4,
        subtype = 0,
        number  = 6,
        AI  =
            {
                {
                node   = 0,
                offset = 11
                }
            },
        },

        {
        descr   = 'HL1 готов к выдаче 1',
        dtype   = 11,
        subtype = 0,
        number  = 1,
        DO  =
            {
                {
                node   = 0,
                offset = 32
                }
            },
        },

        {
        descr   = 'HL2 выдача идет 1е 2',
        dtype   = 11,
        subtype = 0,
        number  = 2,
        DO  =
            {
                {
                node   = 0,
                offset = 34
                }
            },
        },

        {
        descr   = 'HL3 готов к выдаче 2',
        dtype   = 11,
        subtype = 0,
        number  = 3,
        DO  =
            {
                {
                node   = 0,
                offset = 33
                }
            },
        },

        {
        descr   = 'HL4 идет выдача 2',
        dtype   = 11,
        subtype = 0,
        number  = 4,
        DO  =
            {
                {
                node   = 0,
                offset = 35
                }
            },
        },

        {
        descr   = 'V1- Запорный пар',
        dtype   = 0,
        subtype = 1,
        number  = 1,
        DO  =
            {
                {
                node   = 0,
                offset = 28
                }
            },
        par = { 0 }
        },

        {
        descr   = 'V2- Запорный пар',
        dtype   = 0,
        subtype = 1,
        number  = 2,
        DO  =
            {
                {
                node   = 0,
                offset = 30
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V1-Донный Т. №1',
        dtype   = 0,
        subtype = 1,
        number  = 101,
        DO  =
            {
                {
                node   = 0,
                offset = 0
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V2-Наполнение Т.№1',
        dtype   = 0,
        subtype = 1,
        number  = 102,
        DO  =
            {
                {
                node   = 0,
                offset = 2
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V3-Выдача  Т.№1',
        dtype   = 0,
        subtype = 1,
        number  = 103,
        DO  =
            {
                {
                node   = 0,
                offset = 1
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V4- Выдача  Т.№1',
        dtype   = 0,
        subtype = 1,
        number  = 104,
        DO  =
            {
                {
                node   = 0,
                offset = 3
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V5-Мойка  Т.№1',
        dtype   = 0,
        subtype = 1,
        number  = 105,
        DO  =
            {
                {
                node   = 0,
                offset = 4
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V6-Запорный Л.сыво',
        dtype   = 0,
        subtype = 1,
        number  = 106,
        DO  =
            {
                {
                node   = 0,
                offset = 6
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V7-Зпорный Л.Выдачи',
        dtype   = 0,
        subtype = 1,
        number  = 107,
        DO  =
            {
                {
                node   = 0,
                offset = 5
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V8-Вид. разр. Л.сыв',
        dtype   = 0,
        subtype = 1,
        number  = 108,
        DO  =
            {
                {
                node   = 0,
                offset = 7
                }
            },
        par = { 0 }
        },

        {
        descr   = '1V9-Запорный Л.мойки',
        dtype   = 0,
        subtype = 1,
        number  = 109,
        DO  =
            {
                {
                node   = 0,
                offset = 29
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V1-Донный Т.№2',
        dtype   = 0,
        subtype = 1,
        number  = 201,
        DO  =
            {
                {
                node   = 0,
                offset = 8
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V2-Наполнение  Т.№2',
        dtype   = 0,
        subtype = 1,
        number  = 202,
        DO  =
            {
                {
                node   = 0,
                offset = 10
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V3-  Выдача  Т.№2',
        dtype   = 0,
        subtype = 1,
        number  = 203,
        DO  =
            {
                {
                node   = 0,
                offset = 9
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V4- Выдача  Т.№2',
        dtype   = 0,
        subtype = 1,
        number  = 204,
        DO  =
            {
                {
                node   = 0,
                offset = 11
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V5-Мойка  Т.№2',
        dtype   = 0,
        subtype = 1,
        number  = 205,
        DO  =
            {
                {
                node   = 0,
                offset = 12
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V6-Выход мойки танк',
        dtype   = 0,
        subtype = 1,
        number  = 206,
        DO  =
            {
                {
                node   = 0,
                offset = 14
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V7-Зпорный Л.Выдачи',
        dtype   = 0,
        subtype = 1,
        number  = 207,
        DO  =
            {
                {
                node   = 0,
                offset = 13
                }
            },
        par = { 0 }
        },

        {
        descr   = '2V8- Вид.разрыв мойк',
        dtype   = 0,
        subtype = 1,
        number  = 208,
        DO  =
            {
                {
                node   = 0,
                offset = 15
                }
            },
        par = { 0 }
        },

        {
        descr   = '3V1-Донный Т.№3',
        dtype   = 0,
        subtype = 1,
        number  = 301,
        DO  =
            {
                {
                node   = 0,
                offset = 16
                }
            },
        par = { 0 }
        },

        {
        descr   = '3V2-Наполнение  Т.№3',
        dtype   = 0,
        subtype = 1,
        number  = 302,
        DO  =
            {
                {
                node   = 0,
                offset = 18
                }
            },
        par = { 0 }
        },

        {
        descr   = '3V3-  Выдача  Т.№3',
        dtype   = 0,
        subtype = 1,
        number  = 303,
        DO  =
            {
                {
                node   = 0,
                offset = 17
                }
            },
        par = { 0 }
        },

        {
        descr   = '3V4- Выдача  Т.№3',
        dtype   = 0,
        subtype = 1,
        number  = 304,
        DO  =
            {
                {
                node   = 0,
                offset = 19
                }
            },
        par = { 0 }
        },

        {
        descr   = '3V5-Мойка  Т.№3',
        dtype   = 0,
        subtype = 1,
        number  = 305,
        DO  =
            {
                {
                node   = 0,
                offset = 20
                }
            },
        par = { 0 }
        },

        {
        descr   = '3V7-Зпорный Л.Выдачи',
        dtype   = 0,
        subtype = 1,
        number  = 307,
        DO  =
            {
                {
                node   = 0,
                offset = 22
                }
            },
        par = { 0 }
        },

        {
        descr   = '4V1-Донный Т.№4',
        dtype   = 0,
        subtype = 1,
        number  = 401,
        DO  =
            {
                {
                node   = 0,
                offset = 21
                }
            },
        par = { 0 }
        },

        {
        descr   = '4V2-Наполнение  Т.№7',
        dtype   = 0,
        subtype = 1,
        number  = 402,
        DO  =
            {
                {
                node   = 0,
                offset = 23
                }
            },
        par = { 0 }
        },

        {
        descr   = '4V3-  Выдача  Т.№4',
        dtype   = 0,
        subtype = 1,
        number  = 403,
        DO  =
            {
                {
                node   = 0,
                offset = 24
                }
            },
        par = { 0 }
        },

        {
        descr   = '4V4- Выдача  Т.№4',
        dtype   = 0,
        subtype = 1,
        number  = 404,
        DO  =
            {
                {
                node   = 0,
                offset = 26
                }
            },
        par = { 0 }
        },

        {
        descr   = '4V5-Мойка  Т.№4',
        dtype   = 0,
        subtype = 1,
        number  = 405,
        DO  =
            {
                {
                node   = 0,
                offset = 25
                }
            },
        par = { 0 }
        },

        {
        descr   = '5V7-Зпорный Л.Выдачи',
        dtype   = 0,
        subtype = 1,
        number  = 507,
        DO  =
            {
                {
                node   = 0,
                offset = 27
                }
            },
        par = { 0 }
        },

    }
