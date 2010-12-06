require "sys_wago" --Системные функции.
-- ----------------------------------------------------------------------------
editor_version = 12
file_version   = 74

--Узлы WAGO
nodes = 
    {
        {
        ntype   = 1,
        address = 1,
        modules = 
            {
            { 504, '', '', '', '', '' }, 
            { 530, '', '', '', '', '' }, 
            { 402, '', '', '', '', '' }, 
            { 430, '', '', '', '', '' }, 
            { 530, '', '', '', '', '' }, 
            { 504, '', '', '', '', '' }, 
            { 554, '', '', '', '', '' }, 
            { 554, '', '', '', '', '' }, 
            { 638, '', '', '', '', '' }, 
            { 466, '', '', '', '', '' }, 
            { 461, '', '', '', '', '' }, 
            { 461, '', '', '', '', '' }, 
            { 600, '', '', '', '', '' }, 
            }
        },
    }

--Устройства
devices = 
    {
        {
        descr   = 'VC100 рег. 1-ый контур подогрева',
        dtype   = 8,
        subtype = 0,
        number  = 100,
        AO  = 
            {
                {
                node   = 0,
                offset = 0
                }
            },
        },

        {
        descr   = 'VC101 рег. 2-ой контур подогрева',
        dtype   = 8,
        subtype = 0,
        number  = 101,
        AO  = 
            {
                {
                node   = 0,
                offset = 1
                }
            },
        },

        {
        descr   = 'Частота насоса N101',
        dtype   = 8,
        subtype = 0,
        number  = 1001,
        AO  = 
            {
                {
                node   = 0,
                offset = 2
                }
            },
        },

        {
        descr   = 'Счетчик продукта',
        dtype   = 7,
        subtype = 0,
        number  = 101,
        AI  = 
            {
                {
                node   = 0,
                offset = 0
                }
            },
        },

        {
        descr   = 'Расход продукта',
        dtype   = 5,
        subtype = 0,
        number  = 101,
        AI  = 
            {
                {
                node   = 0,
                offset = 2
                }
            },
        par = { 0, 30 } 
        },

        {
        descr   = 'Н.У. бачка с продуктом',
        dtype   = 3,
        subtype = 0,
        number  = 1,
        DI  = 
            {
                {
                node   = 0,
                offset = 0
                }
            },
        },

        {
        descr   = 'Ср.У. бачка с продуктом',
        dtype   = 3,
        subtype = 0,
        number  = 2,
        DI  = 
            {
                {
                node   = 0,
                offset = 2
                }
            },
        },

        {
        descr   = 'Ср.У. бачка с продуктом',
        dtype   = 3,
        subtype = 0,
        number  = 3,
        DI  = 
            {
                {
                node   = 0,
                offset = 1
                }
            },
        },

        {
        descr   = 'М1 - гомогенизатор',
        dtype   = 2,
        subtype = 0,
        number  = 1,
        DO  = 
            {
                {
                node   = 0,
                offset = 18
                }
            },
        DI  = 
            {
                {
                node   = 0,
                offset = 7
                }
            },
        },

        {
        descr   = 'N100 - насос первого контура подогрева',
        dtype   = 1,
        subtype = 0,
        number  = 100,
        DO  = 
            {
                {
                node   = 0,
                offset = 2
                }
            },
        DI  = 
            {
                {
                node   = 0,
                offset = 4
                }
            },
        },

        {
        descr   = 'N101 - насос продукта',
        dtype   = 1,
        subtype = 0,
        number  = 101,
        DO  = 
            {
                {
                node   = 0,
                offset = 1
                }
            },
        DI  = 
            {
                {
                node   = 0,
                offset = 6
                }
            },
        },

        {
        descr   = 'т.1-12 готовы к выдаче',
        dtype   = 10,
        subtype = 0,
        number  = 21,
        DI  = 
            {
                {
                node   = 0,
                offset = 8
                }
            },
        },

        {
        descr   = 'т.32-38 готовы к приемке',
        dtype   = 10,
        subtype = 0,
        number  = 22,
        DI  = 
            {
                {
                node   = 0,
                offset = 10
                }
            },
        },

        {
        descr   = 'из т.1-12 продукт идет',
        dtype   = 10,
        subtype = 0,
        number  = 23,
        DI  = 
            {
                {
                node   = 0,
                offset = 5
                }
            },
        },

        {
        descr   = 'Запрос мойки R3 из т.32-38',
        dtype   = 10,
        subtype = 0,
        number  = 24,
        DI  = 
            {
                {
                node   = 0,
                offset = 3
                }
            },
        },

        {
        descr   = 'Тумблер проталкивания до бачка',
        dtype   = 10,
        subtype = 0,
        number  = 25,
        DI  = 
            {
                {
                node   = 0,
                offset = 9
                }
            },
        },

        {
        descr   = 'Открыть маршрут мойки л. уст. №7',
        dtype   = 10,
        subtype = 0,
        number  = 26,
        DI  = 
            {
                {
                node   = 0,
                offset = 11
                }
            },
        },

        {
        descr   = 'выход продукта с 1-ого контура подогрева',
        dtype   = 4,
        subtype = 0,
        number  = 100,
        AI  = 
            {
                {
                node   = 0,
                offset = 4
                }
            },
        },

        {
        descr   = 'выход продукта со 2-ого контура подогрева',
        dtype   = 4,
        subtype = 0,
        number  = 101,
        AI  = 
            {
                {
                node   = 0,
                offset = 5
                }
            },
        },

        {
        descr   = 'теплоноситель 1-ого контура подогрева',
        dtype   = 4,
        subtype = 0,
        number  = 102,
        AI  = 
            {
                {
                node   = 0,
                offset = 6
                }
            },
        },

        {
        descr   = 'Сирена',
        dtype   = 11,
        subtype = 0,
        number  = 1,
        DO  = 
            {
                {
                node   = 0,
                offset = 11
                }
            },
        },

        {
        descr   = 'Уст. №2 готова к приемке',
        dtype   = 11,
        subtype = 0,
        number  = 21,
        DO  = 
            {
                {
                node   = 0,
                offset = 4
                }
            },
        },

        {
        descr   = 'Работа от установки N2',
        dtype   = 11,
        subtype = 0,
        number  = 22,
        DO  = 
            {
                {
                node   = 0,
                offset = 6
                }
            },
        },

        {
        descr   = 'З.П. в т. 1-12',
        dtype   = 11,
        subtype = 0,
        number  = 23,
        DO  = 
            {
                {
                node   = 0,
                offset = 8
                }
            },
        },

        {
        descr   = 'Мойка идет для т.32-38',
        dtype   = 11,
        subtype = 0,
        number  = 24,
        DO  = 
            {
                {
                node   = 0,
                offset = 3
                }
            },
        },

        {
        descr   = 'Лампочка тумблера проталкивания до бачка',
        dtype   = 11,
        subtype = 0,
        number  = 25,
        DO  = 
            {
                {
                node   = 0,
                offset = 14
                }
            },
        },

        {
        descr   = 'Маршрут мойки л. уст. №7 открыт',
        dtype   = 11,
        subtype = 0,
        number  = 26,
        DO  = 
            {
                {
                node   = 0,
                offset = 12
                }
            },
        },

        {
        descr   = 'Вода в т.32-38',
        dtype   = 11,
        subtype = 0,
        number  = 100,
        DO  = 
            {
                {
                node   = 0,
                offset = 13
                }
            },
        },

        {
        descr   = 'Белая фаза в т.32-38',
        dtype   = 11,
        subtype = 0,
        number  = 101,
        DO  = 
            {
                {
                node   = 0,
                offset = 15
                }
            },
        },

        {
        descr   = 'Продукт в т.32-38',
        dtype   = 11,
        subtype = 0,
        number  = 102,
        DO  = 
            {
                {
                node   = 0,
                offset = 17
                }
            },
        },

        {
        descr   = 'Циркуляция в т.32-38',
        dtype   = 11,
        subtype = 0,
        number  = 103,
        DO  = 
            {
                {
                node   = 0,
                offset = 19
                }
            },
        },

        {
        descr   = 'V22 - клапан набора давления 1-ого этапа',
        dtype   = 0,
        subtype = 1,
        number  = 22,
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
        descr   = 'V23 - клапан набора давления 2-ого этапа',
        dtype   = 0,
        subtype = 1,
        number  = 23,
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
        descr   = 'V94 - дренаж (NO)',
        dtype   = 0,
        subtype = 1,
        number  = 94,
        DO  = 
            {
                {
                node   = 0,
                offset = 0
                }
            },
        par = { 1 } 
        },

        {
        descr   = 'V95 - в бачок',
        dtype   = 0,
        subtype = 1,
        number  = 95,
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
        descr   = 'V96+V97 - вода',
        dtype   = 0,
        subtype = 1,
        number  = 96,
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
        descr   = 'V97 - вода',
        dtype   = 0,
        subtype = 1,
        number  = 97,
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
        descr   = 'V98(99) отсечной в л. под. молока на уст.№2 (NO)',
        dtype   = 0,
        subtype = 1,
        number  = 98,
        DO  = 
            {
                {
                node   = 0,
                offset = 10
                }
            },
        par = { 1 } 
        },

        {
        descr   = 'под. молока в бачок уст.№2',
        dtype   = 0,
        subtype = 1,
        number  = 99,
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
        descr   = 'R3V7(8) R3- в уст. №2 (NC)',
        dtype   = 0,
        subtype = 1,
        number  = 54307,
        DO  = 
            {
                {
                node   = 0,
                offset = 7
                }
            },
        par = { 1 } 
        },

        {
        descr   = 'R3V8 R3- в уст. №2(N0)',
        dtype   = 0,
        subtype = 1,
        number  = 54308,
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
        descr   = 'R3V9(10) R3+ из уст. №2 (NC)',
        dtype   = 0,
        subtype = 1,
        number  = 54309,
        DO  = 
            {
                {
                node   = 0,
                offset = 9
                }
            },
        par = { 1 } 
        },

        {
        descr   = 'R3V10 R3+ из уст. №2(N0)',
        dtype   = 0,
        subtype = 1,
        number  = 54310,
        DO  = 
            {
                {
                node   = 0,
                offset = 9
                }
            },
        par = { 0 } 
        },

    }

init_tech_objects_modes = function()
	return
	{
		{
		n = 1,
		name = 'Grebenka',
		modes = 
			{
			[ 1 ] = 
				{
				name = 'Мойка л. уст. №7',
				opened_devices = 
					{
					V = { 54309, 54310 }
					},
				pair_dev = 
					{
					pair = { { 26, 26 } }
					},
				},
			},
		},
		{
		n = 2,
		name = 'Установка',
		modes = 
			{
			[ 1 ] = 
				{
				name = 'Работа установки',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Работа установки',
						},
					},
				},
			[ 2 ] = 
				{
				name = 'Мойка установки',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Мойка установки',
						},
					},
				},
			[ 3 ] = 
				{
				name = 'Выход на режим',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Выход на режим',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310, 95 }
							},
						},
					[ 2 ] = 
						{
						name = 'Выход на режим рецепта',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310, 95 }
							},
						},
					[ 3 ] = 
						{
						name = 'Готов к производству',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310, 95 }
							},
						},
					},
				},
			[ 4 ] = 
				{
				name = 'Заполнение продуктом',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Заполнение продуктом',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 98, 99 }
							},
						},
					},
				},
			[ 5 ] = 
				{
				name = 'Проталкивание продукта в установку',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Проталкивание продукта в установку',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310, 95 }
							},
						},
					[ 2 ] = 
						{
						name = 'Заполнение  продукта до гомогенизатора',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 95, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310 }
							},
						},
					[ 3 ] = 
						{
						name = 'Заполнение установки',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 95, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310 }
							},
						},
					[ 4 ] = 
						{
						name = 'Проталкивание белой воды',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 95, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310 }
							},
						},
					},
				},
			[ 6 ] = 
				{
				name = 'Пастеризация',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Пастеризация',
						opened_devices = 
							{
							V = { 54307, 54308, 94, 95, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54309, 54310 }
							},
						},
					},
				},
			[ 7 ] = 
				{
				name = 'Циркуляция',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Циркуляция',
						opened_devices = 
							{
							V = { 94, 95, 98, 99 }
							},
						closed_devices = 
							{
							V = { 54307, 54308, 54309, 54310 }
							},
						},
					},
				},
			[ 8 ] = 
				{
				name = 'Вытеснение продукта',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Вытеснение продукта',
						opened_devices = 
							{
							V = { 54307, 54308 }
							},
						},
					[ 2 ] = 
						{
						name = 'Вытеснение установки',
						opened_devices = 
							{
							V = { 54307, 54308 }
							},
						},
					[ 3 ] = 
						{
						name = 'Вытеснение белой воды',
						opened_devices = 
							{
							V = { 54307, 54308 }
							},
						},
					[ 4 ] = 
						{
						name = 'Заполнение водой линии  до приемника',
						opened_devices = 
							{
							V = { 54307, 54308 }
							},
						},
					},
				},
			[ 9 ] = 
				{
				name = 'Дезинфекция',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Дезинфекция',
						closed_devices = 
							{
							V = { 54307, 54308, 54309, 54310 }
							},
						},
					},
				},
			[ 10 ] = 
				{
				name = 'Кислота',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Кислота',
						},
					},
				},
			[ 11 ] = 
				{
				name = 'Щелочь',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Щелочь',
						},
					},
				},
			[ 12 ] = 
				{
				name = 'Ополаскивание',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Ополаскивание',
						opened_devices = 
							{
							V = { 54307, 54308 }
							},
						},
					[ 2 ] = 
						{
						name = 'Ополаскивание',
						opened_devices = 
							{
							V = { 54307, 54308 }
							},
						},
					},
				},
			[ 13 ] = 
				{
				name = 'Проталкивание от установки N7',

				steps = 
					{
					[ 1 ] = 
						{
						name = 'Проталкивание от установки N7',
						opened_devices = 
							{
							V = { 54307, 54308, 54309, 54310 }
							},
						},
					},
				},
			}
		}
    }
end
