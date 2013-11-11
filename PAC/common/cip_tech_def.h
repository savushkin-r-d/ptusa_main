#ifndef CIP_TECH_DEF
#define CIP_TECH_DEF

#include "tech_def.h"
#include "g_device.h"
#include "PID.h"


enum workParameters 
	{
	P_CONC_RATE = 1,	//расход концентрата при наведении
	P_ZAD_PODOGR,	//текущее задание подогрева
	P_ZAD_FLOW,		//текущее задание расхода
	P_VRAB,			//заданный объем операции
	P_MAX_OPER_TM,	//максимальное время текущей операции
	P_OP_TIME_LEFT,	//прошло времени текущей операции
	P_CONC,			//текущая концентрация
	P_SUM_OP,		//литров с начала операции
	P_ZAD_CONC,		//заданная концентрация
	P_LOADED_RECIPE,	//загруженный рецепт

	P_PROGRAM,		//программа мойки

	P_CUR_REC,		//текущий выбранный рецепт
	P_RET_STATE,	//состояние возвратного насоса
	P_SELECT_REC,	//загрузка рецепта
	P_SELECT_PRG,	//загрузка программы

	PV1,			//объем V1 (от модуля до объекта)
	PV2,			//объем V2 (от объекта до модуля)
	P_OBJ_TYPE,		//тип объекта (танк, линия и пр.)
	P_FLOW,			//расход
	P_PODP_CIRC,	//подпитывать ли бачок во время циркуляции  
	P_DELTA_TR,		//разница температур на подаче и возврате (если меньше заданной, то начинается отсчет времени циркуляции)

	P_T_WP,	//температура предварительного ополаскивания
	P_T_WSP,	//температура промежуточной промывки после щелочи
	P_T_WKP,	//температура промежуточной промывки после кислоты
	P_T_WOP,	//температура окончательного ополаскивания
	P_T_S,	//температура мойки щелочью
	P_T_K,	//температура мойки кислотой
	P_T_D,	//температура дезинфекции на воде

	P_T_SANITIZER,	//температура дезинфекции дезинфицирующим средством
	P_DOP_V_PR_OP,	//дополнительный объем предварительного ополаскивания
	P_DOP_V_AFTER_S,	//дополнительный объем ополаскивания после щелочи
	P_DOP_V_AFTER_K,	//дополнительный объем ополаскивания после кислоты
	P_DOP_V_OK_OP,		//дополнительный объем окончательного ополаскивания

	P_RET_STOP,	//объем до конца V1 для остановки возвратного насоса
	P_V_RAB_ML,   //количество литров выше среднего уровня при котором заканчивается заполнение бачка циркуляции
	PV_RET_DEL,	//через сколько литров после начала подачи V2 включать возвратный насос
	PTM_OP,	//время циркуляции при ополаскивании (операции не используются)
	PTM_S,	//время циркуляции на щелочи
	PTM_K,	//время циркуляции на кислоте
	PTM_S_SK,	//время циркуляции на щелочи при мойке кислота+щелочь
	PTM_K_SK,	//время циркуляции на кислоте при мойке кислота+щелочь
	PTM_D,	//время дезинфекции на воде
	PTM_SANITIZER, //время дезинфекции дезинфицирующим средством
	PTM_SANITIZER_INJECT, //время внесения дезинфицирующего средства при дезинфекции

	P_N_RET,		//номер возвратного насоса
	P_N_UPR,		//номер обратной связи
	P_OS,			//обратная связь
	P_OBJ_EMPTY,	//сигнал "объект опорожнен"
	P_PROGRAM_MASK, //маска режимов мойки
	P_T_CLEAN_RINSING, //Температура ополаскивания чистой водой
	P_V_CLEAN_RINSING, //объем ополаскивания чистой водой
	P_RESERV_START,


	STP_QAVS,		//средняя концентрация щелочи
	STP_QAVK,		//средняя концентрация кислоты
	STP_WC,			//чистая вода
	STP_WS,			//вторичная вода
	STP_LV,			//промежуточный параметр
	STP_WC_INST_WS,	//чистая вода, использованная по причине нехватки вторичной
	STP_WASH_START, //время начала мойки (используется сервером для хранения)
	STP_STEPS_OVER,	//количество пропущенных операций
	STP_RESETSTEP,	//шаг, на котором сбросили
	STP_ERRCOUNT,	//количество ошибок
	};

	enum storedParameters
		{
		P_TM_RET_IS_EMPTY = 1, //время отсутствия протока на возврате для окончания опорожнения объекта
		P_V_LL_BOT,	//количество литров от пропадания нижнего уровня в бачке до завершения операции "опорожнение бачка"
		P_R_NO_FLOW,	//минимальный расход при котором считать, что его нет
		P_TM_R_NO_FLOW,	//время возникновения ошибки "нет расхода на подаче"
		P_TM_NO_FLOW_R,	//время появления ошибки "нет расхода на возврате"	
		P_TM_NO_CONC,	//время появления ошибки "нет концентрации в возвратной трубе"
		};


class cipline_tech_object: public tech_object
{
protected:
	int ncmd;

	int ret_overrride; //флаг принудительного включения/выключения возвратного насооса
	int concentration_ok; //есть концентрация на возврате
	int return_ok; //есть расход на возврате
	int tank_is_empty; //используется для того, чтобы определить, нужно ли отключать возвратный насос
	unsigned long sort_delay; //таймер задержки при сортировке растворов (чтобы не переключалось хаотически)
	unsigned long steam_valve_delay; //таймер задержки включения отсечного клапана пара (чтобы не включался/выключался на граничных значениях температуры)
	unsigned long bachok_lvl_err_delay; //задержка для появления ошибки уровня бачка


	unsigned long enddelayTimer;
	int valvesAreInConflict;
	virtual void resetProgramName();
	virtual void resetRecipeName();
	virtual void resetProgramList(unsigned long programmask = 0xB00);
	virtual void formProgramList(unsigned long programmask);
	virtual void loadProgramFromList(int selectedPrg);
	virtual void closeLineValves();
	int isTank();
	int isLine();
	int getValvesConflict();

	//Управление подающим насосом
	int pump_control;
	//TPumpController* NPC;
    public:
    cipline_tech_object( const char* name, u_int number, u_int type, const char* name_Lua,
            u_int states_count,
            u_int timers_count,
            u_int par_float_count, u_int runtime_par_float_count,
            u_int par_uint_count, u_int runtime_par_uint_count );
    i_DO_device* V1;

	static saved_params<float, true>* parpar;

	PID* PIDF;
	PID* PIDP;
	void initline();

	static int nextpidnumber();
	static int pidnumber;

	//overriden members
	int save_device( char *buff );
	int evaluate();
};

#endif //CIP_TECH_DEF
