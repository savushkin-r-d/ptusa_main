//ОБЩЕЕ ОПИСАНИЕ.
//ОПИСАНИЕ DEFINE.
//  I7188_E - определяет компиляцию для контроллера I7188E.
//  I7186_E - определяет компиляцию для контроллера I7186E.
//  NO_TANKS_MODE - не использовать 32 режима танка, а только 1. Сделано
//      для оптимизации памяти.
#ifndef PLC_H
#define	PLC_H

#include <stdio.h>
#include "params.h"  
#include "PAC_err.h"
#include "utils.h"

#include "g_device.h"

#ifdef I7188_E
#include "tcpip32.h"
#include "7188E.h"
#endif

#ifdef I7186_E
#include "tcpip32.h"
#include "7186E.h"
#endif

#ifdef uPAC5000
#include "Tcpip32.h"
#include "uPAC5000.h"
#endif

#if !defined I7188_E && !defined I7186_E && !defined uPAC5000 
#include "I7188.h"
#endif

//for levels
#if defined LEVEL_REVERSE
#define LIS     1
#define LNO     0
#else
#define LIS     0
#define LNO     1
#endif
#define OFF     0
#define ON      1

enum devType
    {  
    dtV,
    dtV2DO,
    dtV1DO1DI,
    dtV1DO2DI,
    dtV2DO2DI,
    dtN1DO1DI,
    dtM1DO1DI,
    dtLS,     
    dtTE,     
    dtFE,     
    dtFS,     
    dtCTR,    
    dtAO,     
    dtLE1,    
    dtLE2,    
    dtLE3,    
    dtLE4,    
    dtMix,    
    dtOS,     
    dtUpr,    
    dtQE,     
    dtMF,     

    dtV1DO3DI, 
    dtV1DO2DI_S, //2 FB на одно из состояний.
    dtASMix,     //Клапан микспруф с AS интерфейсом.
    dtAI,

    dtLS_ex,     //Настраиваемый предельный уровень.
    }; 

enum VDEVICE_TYPES
    {  
    VD_TYPES_COUNT = 16,

    VD_V = 0, 
    VD_N,
    VD_M,
    VD_LS,     
    VD_TE,     
    VD_FE,     
    VD_FS,     
    VD_CTR,    
    VD_AO,     
    VD_LE,
    VD_FB,     
    VD_UPR,    
    VD_QE,    
    VD_AI,
    VD_CTR_STATE,
	VD_TE_FILTER,
    };

enum NODE_TYPE
    {
    N_315 = 0,
    N_815,   //Programmable.
    N_341,   //Ethernet 750-341
    N_841,   //Ethernet 750-841 Programmable
    N_PET,   //Ethernet ICP CON PET-7051 (16 DI)
    };  

class TTimer;

void ModbusCRC(unsigned char *Buf,int bc);

#define TS_INIT     1
#define TS_ENABLED  2
#define TS_DISABLED 3

typedef struct {
    FILE_DATA *fd;
    char *data;
    unsigned long pos;
    } FILE7188;

struct TRange {
    float rl;
    float rh;
    };

struct TTblRec {
    int inid;
    int inoffs;
    int outid;
    int outoffs;
    };

struct TTblRecs {
    int RecCnt;
    struct TTblRec *R;
    };

struct dev_elem
	{
	devType type;
	int no;

	//	Для задания импульсных сигналов
	//ulong par_impuls; //	"0" - постоянный сигнал; ">0" - периодичность, мс
	};

struct TPathRec 
    {
	//	Для включения режима
	//		Необходимые для включения сигналы
	int FB_need_Cnt;
	dev_elem *FB_need;
	
	//		Включающие режим сигналы
	int FB_on_mode_Cnt;
	dev_elem *FB_on_mode;
 	
	//-----------------------------------------		  
	
	//	Для работы режима
	//		Включать
	int OVCnt;
	dev_elem *OV;
		
	//		Выключать
	int CVCnt;
	dev_elem *CV;

	//		Верхний флип
	int USCnt;
	dev_elem *US;

	//		Нижний флип
	int LSCnt;
	dev_elem *LS;

	//		Отправляемые сигналы (можно отнести к Включаемым усройствам)
	int UPRCnt;
	dev_elem *UPR;

	//		Управляющие устройствами сигналы
	int FB_control_Cnt;
	dev_elem *FB_control;
	dev_elem *FB_control_dev;
	
	//		Зависящие_от_устройств_сигналы
	int UPR_control_Cnt;
	dev_elem *UPR_control_dev;
	dev_elem *UPR_control;

	//		Использовать ключ
	int Key_signal_Cnt;
	dev_elem *Key_signal;

	//-----------------------------------------
	
	//	Для выключения режима
	//		Выключающие режим устройства (сигналы)
	int Dev_off_Cnt;
	dev_elem *Dev_off;

	//		Включаемые по завершению устройства
	int FVCnt;
	dev_elem *FV;

	//-----------------------------------------

	//	Дополнительные параметры режима
	//		Время работы режима (шага)
	uint work_time_par;

	//		Номер следующего режима (шага)
	uint next_mode;

	//		Блокирующие режимы объектов
	int	Block_cm_cnt;
	uint *Block_cm;	//	Блокирующие режимы гребенок ((no * 1000) + mode)

	int	Block_tm_cnt;
	uint *Block_tm;	//	Блокирующие режимы танков ((no * 1000) + mode)

	//		Блокирующие устройства
	int Block_dev_Cnt;
	dev_elem *Block_dev;	

	//		Включать режимы танка
	int On_mode_Cnt;
	uint *On_mode;

	//		Выключать режимы танка
	int Off_mode_Cnt;
	uint *Off_mode;

	//-----------------------------------------

	//	Переменная времени для выключения UPR при отсутствии обратной связи с насосом.
	ulong		*time_for_off_UPR;
	//	Переменная времени для включения режима при включении заданного устройства (сигнала).
	ulong		*time_for_on_mode;
	//	Переменная времени для выключения режима при включении заданного устройства (сигнала).
	ulong		*time_for_off_mode;
	//	Переменная периодичности мигания лампочки в ключе
	ulong		lamp_blink_start_time;
	//	Начало отсчета переходного процесса
	ulong		DT_time;

/*
    int OVCnt;
    int CVCnt;
    int FVCnt;
    int USCnt;
    int LSCnt;
	int FBCnt;

    int *OV;
    int *CV;
    int *FV;
    int *US;
    int *LS;
    int *FB;

	int washFB;
	int washNCnt;
	int *washN;
	int washVCnt;
    int *washV;

    int washUPRCnt;
    int *washUPR;

    int in_FB_cnt;
    int *in_FB_val;
    int out_UPR_cnt;
    int *out_UPR_val;
*/
    //-Параметры маршрута гребенки.
	int in_x;
	int in_y;
	int out_x;
	int out_y;        
    //-Параметры маршрута гребенки.-!>
    };

struct TPLCTable {
    int state;
    unsigned long DrvID;
    unsigned long ID;
    //Digital outputs (DO)
    int DOcnt; //amount of DO
    unsigned char *DO;  //current values
    unsigned char *_DO; //to write
    //Analog outputs (AO)
    int AOcnt; //amount of AO
    float *AO;  //current values
    float *_AO; //to write
    struct TRange *AOR;
    //Digital inputs (DI)
    int DIcnt; //amount of DI
    unsigned char *DI;  //current values
    //Analog inputs (AI)
    int AIcnt; //amount of AI
    float *AI;  //current values
    struct TRange *AIR;
    };

#define MAX_TABLES_CNT      128
struct TPLCTables {
    int TablesCnt;
    struct TPLCTable *Table[MAX_TABLES_CNT];
    };

void InitTables (void);
int RegisterTable(TPLCTable *tptr);
int UnregisterTable(TPLCTable *tptr);
TPLCTable *GetTableByID(int Id);

//table access functions
int SetDOTable(TPLCTable *tptr, int N, char val);
int GetDOTable(TPLCTable *tptr, int N);
int SetAOTable(TPLCTable *tptr, int N, float val);
float GetAOTable(TPLCTable *tptr, int N);
int SetAITable( TPLCTable *tptr, int N, float val ); 
float GetAITable(TPLCTable *tptr, int N);
int GetDITable(TPLCTable *tptr, int N);
int SetDITable(TPLCTable *tptr, int N, char val);

int SetAOTableN(int nom, int offs, float val);
int SetAITableN(int nom, int offs, float val);
int SetDOTableN(int nom, int offs, char val);
int SetDITableN(int nom, int offs, char val);

#define TYPE_DO   0
#define TYPE_DI   1
#define TYPE_AO   3
#define TYPE_AI   4
#define TYPE_STUB 5
#define TYPE_UNKN -1
#define TYPE_AIAO 6

#define MAX_UNSUCCSESSFUL_NODE_CONNECT_TRIES 3
#define DISCONNECT_TIME_FOR_ERROR 4000L
#define WAGO_RECONNECT_TIMEOUT 3000L

struct TNode {
    struct TPLCTable *Table;
    NODE_TYPE nodeType; 
    unsigned char NodeIp[ 16 ];
    //unsigned char* NodeIp;
    int connectedstate;
    unsigned long lastconnecttry;
	unsigned long lastsuccessfullexchange;
    int socknumber;
    int clientID;
    int trcnt;
    int *mAI;
    int mAIcnt;
    int wAIcnt;
    int *mAO;
    int mAOcnt;
    int wAOcnt;
    };

class TVDEV: public i_simple_device  
    {
    public:
        unsigned int nom;

		virtual int Set(unsigned char Val)=0;
		virtual int On(void)=0;
		virtual int Off(void)=0;
		virtual float Value(void)=0;
#ifdef DEBUG
		virtual int State(void)=0;
#endif
		//int Set(void) {return 0;}
		//int On(void) {return 0;}
		//int Off(void) {return 0;}
		//int State(void) {return 0;}
		//float Value(void) {return 0;}

#ifdef USE_SIMPLE_DEV_ERRORS
    public:
        enum DEV_TYPE
            {
            DT_VALVE = 1,
            DT_PUMP,
            DT_MIXER,       // Мешалка.
            DT_COUNTER,

            DT_T,           // Температура.
            };

        enum DEV_ERROR_PARAMS  ///< Параметры ошибки, определяют номера битов.
            {
            DE_IS_ENABLE = 1,  ///< Блокировка тревоги на этапе проектирования.    
            DE_IS_INHIBIT = 2, ///< Блокировка тревоги во время работы.    
            DE_IS_SUPPRESS = 4,///< Подавление тревоги клиентами.
            };

        TVDEV()
            {
            par = new TParams( 1 );
            }

        void set_type( int dev_type )
            {
            this->dev_type = dev_type;
            }

        int get_type() const
            {
            return dev_type;
            }

        TParams *par;           ///< Параметры устройства.

    private:
        char    dev_type;        

    public:
        enum CONSTANTS
            {
            C_MAX_NAME_LENGTH = 45,
            };
        char name[ C_MAX_NAME_LENGTH ];
        char short_name[ C_MAX_NAME_LENGTH ];        
        
        void print() const
            {  
#ifdef DEBUG
            Print( "%5u : %s\n", nom, name );
#endif // DEBUG            
            }

        void set_name( char *new_name );

        virtual int State()
            {
            return 1;
            }

        const char * get_name() const
            {
            return name;
            }

        const char * get_short_name() const
            {
            return short_name;
            }


#else // USE_SIMPLE_DEV_ERRORS
        void print() const
            {  
#ifdef DEBUG
            Print( "%5u\n", nom );
#endif // DEBUG          
            }

#endif // USE_SIMPLE_DEV_ERRORS

        //Возвращает номер устройства.
        unsigned long get_n() const
            {
            return nom;
            }

        //Сохранение состояния устройства в буфер.
        int save_state( char *buff  )
            {     
            buff++;
            return 0;
            }

		//Сохранение измененного состояния устройства в буфер.
        int save_changed_state( char *buff )
            {     
            buff++;
            return 0;
            }

        //Считывание состояния устройства из буфера.
        int load_state( char *buff  )
            {
            buff++;
            return 0;
            }

		//Считывание измененного состояния устройства из буфера.
        int load_changed_state( char *buff )
            {     
            buff++;
            return 0;
            }

        //Сохранение самого устройства в буфер.
        int save_device( char *buff  )
            {
            ( ( unsigned long* ) buff )[ 0 ] = nom;
            return 4;
            }

        //Считывание самого устройства из буфера.
        int load_device( char *buff  )
            {
            buff++;
            return 0;
            }

        //Выполнение команды, хранящейся в буфере.
        int parse_cmd( char *buff  ) = 0; 

        u_int_4 get_idx()
            {
            return 0;
            }
        
        void set_idx( u_int_4 new_idx )
            { 
            new_idx++;
            }   
    };

class TVDO: public TVDEV 
    {
	char	prev_state;

    public:
        int HI;
        virtual int Set(unsigned char Val)=0;
        virtual int On(void)=0;
        virtual int Off(void)=0;
        virtual int State(void)=0;
        virtual int UState(void)=0;

		//	<--Формальная реализация виртуальных функций
		float Value(void) {return 0;}
		//	--> Формальная реализация виртуальных функций

		//Сохранение измененного состояния устройства в буфер.
        int save_changed_state( char *buff )
            {
			if ( prev_state != State() )
				{
				buff[ 0 ] = State();
				prev_state = State();
				return 1;
				}
                
            return 0;
            }

        //Сохранение состояния устройства в буфер.
        int save_state( char *buff  )
            {
			prev_state = State();
            buff[ 0 ] = State();
            return 1;
            }

        //Выполнение команды, хранящейся в буфере.
        int parse_cmd( char *buff )
            {
            Set( ( ( u_int_4* ) buff )[ 0 ] );
            return 4;
            }          
    };

class TVDI: public TVDEV  
    {
char	prev_state;

    public:
        virtual int State() = 0;
        virtual int StateNow() = 0;
        virtual int InitTmr( int dt = 2000 ) = 0;
        virtual int SetState ( int st = 0 ) = 0;    

		//	<--Формальная реализация виртуальных функций
		int Set(unsigned char Val) {return Val;}
		int On(void) {return 0;}
		int Off(void) {return 0;}
		//int State(void) {return 0;}
		float Value(void) {return 0;}
		//	--> Формальная реализация виртуальных функций

		//Сохранение измененного состояния устройства в буфер.
        int save_changed_state( char *buff )
            {
			if ( prev_state != State() )
				{
				buff[ 0 ] = State();
				prev_state = State();
				return 1;
				}
                
            return 0;
            }

        //Сохранение состояния устройства в буфер.
        int save_state( char *buff  )
            {
			prev_state = State();
            buff[ 0 ] = State();
            return 1;
            }

        //Выполнение команды, хранящейся в буфере.
        int parse_cmd( char *buff  ) = 0;
//            {
//#ifdef DEBUG
//            Print( "TVDI::parse_cmd()\n" );
//#endif // DEBUG
//            SetState( ( ( u_int_4* ) buff )[ 0 ] );
//            return 4;
//            }

            //Возвращает номер устройства.
        virtual unsigned long get_n() const
            {
            return nom;
            }    

        int active_state;

        int is_active()
            {
            if ( active_state == State() )
                {
                return 1;
                }
            return 0;
            }
    };

class TVAO: public TVDEV  
    {
	float	prev_val;

    public:
        virtual int Set(float Val)=0;
        virtual float Value(void)=0;

		//	<--Формальная реализация виртуальных функций
		int Set(unsigned char Val) {return Val;}
		int On(void) {return 0;}
		int Off(void) {return 0;}
		int State(void) {return 0;}
		//float Value(void) {return 0;}
		//	--> Формальная реализация виртуальных функций

		//Сохранение измененного состояния устройства в буфер.
        int save_changed_state( char *buff )
            {
			if ( prev_val != Value() )
				{
				( ( float* ) buff )[ 0 ] = Value();				
				prev_val = Value();
				return 4;
				}
                
            return 0;
            }

        //Сохранение состояния устройства в буфер.
        int save_state( char *buff )
            {
            ( ( float* ) buff )[ 0 ] = Value();
			prev_val = Value();
            return 4;
            }

        //Выполнение команды, хранящейся в буфере.
        int parse_cmd( char *buff  )
            {
            Set( ( ( float* ) buff )[ 0 ] );
            return 4;
            }
    };

class TVAI: public TVDEV  
    {
	float	prev_val;

    public:
        //virtual float Value(void)=0; 

		//	<--Формальная реализация виртуальных функций
		int Set(unsigned char Val) {return Val;}
		int On(void) {return 0;}
		int Off(void) {return 0;}
		//int State(void) {return 0;}
		//float Value(void) {return 0;}
		//	--> Формальная реализация виртуальных функций

		//Сохранение измененного состояния устройства в буфер.
        int save_changed_state( char *buff )
            {
			if ( prev_val != Value() )
				{
				( ( float* ) buff )[ 0 ] = Value();				
				prev_val = Value();
				return 4;
				}                      
            return 0;
            }

        //Сохранение состояния устройства в буфер.
        int save_state( char *buff  )
            {
            ( ( float* ) buff )[ 0 ] = Value();
			prev_val = Value();
            return 4;
            }     
    };


class TCounter;

typedef void TChangeProc(int no, char val);
typedef TChangeProc *PChangeProc;

struct TChangerDev {
    int no;
    char last_v;
    PChangeProc on_change;
    };

class TChanger {
private:

public:
    int cnt;
    TChangerDev *data;
    TChanger(int count); 
    ~TChanger(void); 
    Evaluate(void);
    };



struct TDevices {
    int VCnt;
    TVDO **VDev;
    int NCnt;
    TVDO **NDev;
    int MCnt;
    TVDO **MDev;
    int LSCnt;
    TVDI **LSDev;
    int FSCnt;
    TVDI **FSDev;
    int AICnt;
    TVAI **AIDev;
    int AOCnt;
    TVAO **AODev;
    int CTRCnt;
    TCounter **CTRDev;
    int TECnt;
    TVAI **TEDev;
    int FECnt;
    TVAI **FEDev;
    int LECnt;
    TVAI **LEDev;
    int FBCnt;
    TVDI **FBDev;
    int UPRCnt;
    TVDO **UPRDev;
    int QECnt;
    TVAI **QEDev;
    };

TVDO *V(unsigned int nom
#ifdef DEBUG
        , char is_print_error = 0
#endif
        );
TVDO *N(unsigned int nom);
TVDO *M(unsigned int nom);
TVDI *LS(unsigned int nom);  //Возвращает верхний\нижний уровень, устанавливает задержку на срабатывание в 2000 мс.
TVDI *FS(unsigned int nom);
TVAI *AI(unsigned int nom);	 //	для аналогового входа ( vvv )
TVAO *AO(unsigned int nom);
TCounter *CTR(unsigned int nom);
TVAI *TE(unsigned int nom);
TVAI *FE(unsigned int nom);
TVAI *LE(unsigned int nom);
TVDI *FB(unsigned int nom);
TVDO *UPR(unsigned int nom);
TVAI *QE(unsigned int nom);

class TDriver {
public:
    int NodeCnt;
    struct TNode *Nodes;
    //   struct TPLCTable *Tables;
    virtual unsigned long GetDrvID(void)=0;
    virtual int LoadDescrB(char *fn)=0;
    virtual int Run(void)=0;
    virtual int ReadInputs(void)=0;
    virtual int WriteOutputs(void)=0;
    };

//---file procedures for i7188------

FILE7188 *fopen7188(char *filename);
void fclose7188(FILE7188 *f);
int fgetc7188(FILE7188 *f);
int BlockRead(FILE7188 *f, char *data,int col);

//----------------------------------


//Wago definition
class  TWagoDrv: public TDriver {
private:
    TPLCTable *tbls;
    void ReadRanges(FILE7188 *df);
    void _FreeNodes(void);
    struct TTblRecs TRS;
public:
    int retrCnt;
    int port;
    long int speed;
    char dataBits;
    char parity;
    char stopBits;
    int ThreadID;
    int IsRTS;
    unsigned char Buf[262];
    int GetChCnt(int module);
    int GetModuleType(int module);
    int GetABC(int module,NODE_TYPE nt = N_315);
    float GetDefVal(int module);
    float GetReal(int *pidx, int module, int ch, TRange r, NODE_TYPE nt = N_315);
    int SetWord(int *pidx, int module, float val, TRange r, NODE_TYPE nt = N_315);
    int GetAnswer(int i, int bc);
    int GetChar(void);
    TWagoDrv(void);
    ~TWagoDrv(void);
    unsigned long GetDrvID(void);

#ifndef USE_NO_TANK_COMB_DEVICE
    int LoadDescrB(char *fn);
#endif // USE_NO_TANK_COMB_DEVICE
	
//	void Read_mode_proportis( FILE7188 *df, int i, TTank *obj );
	void Read_Value( FILE7188 *df, uint &value );
	void Read_list_mode( FILE7188 *df, int mode, uint **Mode_list, int &mode_cnt );
	//dev_elem* Read_list_dev( FILE7188 *df, int mode, dev_elem *Dev_list, int &Dev_cnt );
	void Read_list_dev( FILE7188 *df, int mode, dev_elem **Dev_list, int &Dev_cnt );
	void Read_dual_list( FILE7188 *df, int mode, dev_elem **Dev_list, dev_elem **Dev_list_2, int &Dev_cnt );
		
    //int LoadTables(char *fn);
    int ReadInputs(void);
    int WriteOutputs(void);
    int Run(void);
#if defined I7186_E || defined I7188_E
    int ECommunitate(int NodeNum,int BytesToSend, int BytesToReceive);
#endif
    void ServTables(void);
    };

class TDO0: public TVDO {
private:
    TPLCTable *Tbl;
    int offs;
public:
    int HI;
    int Init(TPLCTable *Table,int offset);
    TDO0(void);
    ~TDO0(void);
    int On(void);
    int Off(void);
    int Set(unsigned char Val);
    int State(void);
    int UState(void);

	//	<--Формальная реализация виртуальных функций
	//int Set(unsigned char Val) {return Val;}
	//int On(void) {return 0;}
	//int Off(void) {return 0;}
	//int State(void) {return 0;}
	float Value(void) {return 0;}
	//	--> Формальная реализация виртуальных функций
    };


class T2DO0: public TVDO {
private:
    TPLCTable *Tbl1, *Tbl2;
    int offs1, offs2;
public:
    int HI;
    int Init(TPLCTable *Table1,int offset1, TPLCTable *Table2, int offset2);
    T2DO0(void);
    ~T2DO0(void);
    int On(void);
    int Off(void);
    int Set(unsigned char Val);
    int State(void);
    int UState(void);
    };

class TVirtualFB: public TVDI
    {
    char st;
    public:
	//	<--Формальная реализация виртуальных функций
		int Set(unsigned char Val) {return Val;}
		int On(void) {return 0;}
		int Off(void) {return 0;}
		//int State(void) {return 0;}
		//float Value(void) {return 0;}
	//	--> Формальная реализация виртуальных функций

        TVirtualFB( int st = 0 ) {};
        int State() { return st; }
        int StateNow() { return st; }
        int InitTmr( int dt = 2000 ) 
            { 
            dt++;
            return 0; 
            }
        int SetState ( int st = 0 ) 
            { 
#ifdef DEBUG
            Print( "TVirtualFB::int SetState ( int st = 0 ) \n");
#endif // DEBUG
            this->st = st;
            return 0; 
            }

        int parse_cmd( char *buff  )
            {
            buff++;
            return 0;
            }
    };

class TVirtualDO: public TVDO  
    {
    public:
        TVirtualDO( unsigned int no = 0xFFFF )
            {
            nom = no;
            }

        int On(void) {return 0;}
        int Off(void){return 0;}
        int Set(unsigned char Val) {return Val;}
        int State(void) {return 1;}
        int UState(void) {return 0;}
//		float Value(void) {return 0;}
    };

class TDO1: public TVDO  
    {
    private:
        TPLCTable *TblO;
        TPLCTable *TblI;
        int offs;
        int ioffs;
        unsigned long swt;
    public:
        int checkFB; //FB on closed state.

        int HI;
        unsigned long SWT_DEL;
        int Init(TPLCTable *TableO,int offset, TPLCTable *TableI, int ioffset);
        TDO1( int checkFB = 0 );
        ~TDO1(void);
        int On(void);
        int Off(void);
        int Set(unsigned char Val);
        int State(void);
        int UState(void);
    };

class TDO2: public TVDO  
    {
    private:
        TPLCTable *TblO;
        TPLCTable *TblI0;
        TPLCTable *TblI1;
        int offs;
        int ioffs0;
        int ioffs1;
        unsigned long swt;

    public:
        int isSameFB;  //ОС на одно из состояний
        int checkFB;   //FB on closed state.

    public:
        int HI;
        unsigned long SWT_DEL;
        int Init(TPLCTable *TableO,int offset, TPLCTable *TableI0, int ioffset0, TPLCTable *TableI1, int ioffset1);
        TDO2(void);
        ~TDO2(void);
        int On(void);
        int Off(void);
        int Set(unsigned char Val);
        int State(void);
        int UState(void);
    };

class TDO3: public TVDO  
    {
    private:
        TPLCTable *TblO;
        TPLCTable *TblI0;
        TPLCTable *TblI1;
        TPLCTable *TblI2;
        int offs;
        int ioffs0;
        int ioffs1;
        int ioffs2;
        unsigned long swt;
    public:
        int checkFB; //FB on closed state.

        int HI;
        unsigned long SWT_DEL;
        int Init(TPLCTable *TableO,int offset, TPLCTable *TableI0, int ioffset0, 
            TPLCTable *TableI1, int ioffset1, TPLCTable *TableI2, int ioffset2 );
        TDO3(void);
        ~TDO3(void);
        int On(void);
        int Off(void);
        int Set(unsigned char Val);
        int State(void);
        int UState(void);
    };


class T2DO2: public TVDO  {
private:
    TPLCTable *TblO0;
    TPLCTable *TblO1;
    TPLCTable *TblI0;
    TPLCTable *TblI1;
    int offs0;
    int offs1;
    int ioffs0;
    int ioffs1;
    unsigned long swt;
public:
    int HI;
    unsigned long SWT_DEL;
    int Init(TPLCTable *TableO0,int offset0, TPLCTable *TableO1,int offset1, TPLCTable *TableI0, int ioffset0, TPLCTable *TableI1, int ioffset1);
    T2DO2(void);
    ~T2DO2(void);
    int On(void);
    int Off(void);
    int Set(unsigned char Val);
    int State(void);
    int UState(void);
    };


class TMIX: public TVDO  {
private:
    TPLCTable *TblO;
    TPLCTable *TblI0;
    TPLCTable *TblI1;
    TPLCTable *TblOU;
    TPLCTable *TblOL;
    int offs;
    int ioffs0;
    int ioffs1;
    int offsU;
    int offsL;
    unsigned long swt;
public:
    int HI;
    unsigned long SWT_DEL;
    int Init(TPLCTable *TableO,int offset,
        TPLCTable *TableOU,int offsetU,
        TPLCTable *TableOL,int offsetL,
        TPLCTable *TableI0,int ioffset0,
        TPLCTable *TableI1,int ioffset1);
    TMIX(void);
    ~TMIX(void);
    int On(void);
    int Off(void);
    int OpenLSeat(void);
    int OpenUSeat(void);
    int Set(unsigned char Val);
    int State(void);
    int UState(void);
    };

class TDI: public TVDI   {
private:
    int dt;  
    unsigned long time;

    TPLCTable *Tbl;
    int offs;
    int state;
public:
    int Init(TPLCTable *Table,int offset);  
    TDI();
    ~TDI(void);
    int InitTmr( int dt = 2000 );
    int GetDt();
    int State();
    int StateNow();
    int SetState( int st );
    //Выполнение команды, хранящейся в буфере.
    int parse_cmd( char *buff  )
        {
#ifdef DEBUG
        Print( "Set cmd %lu\n", ( ( u_int_4* ) buff )[ 0 ] );
#endif // DEBUG

        SetDITable( Tbl, offs, ( ( u_int_4* ) buff )[ 0 ] );
        return 4;
        }
    };


class TAO: public TVAO {
public:
    TPLCTable *Tbl;
    int offs;
    int HI;
    int Init(TPLCTable *Table,int offset);
    TAO(void);
    ~TAO(void);
    int Set(float Val);
    float Value(void);
    };


class TAI: public TVAI {
public:
    TPLCTable *Tbl;

	int offs;

    int Init(TPLCTable *Table,int offset);
    TAI(void);
    ~TAI(void);
    float Value(void);

	//	<--Формальная реализация виртуальных функций
	int State(void) {return 0;}
	//	--> Формальная реализация виртуальных функций

    int parse_cmd( char *buff )
        {
        float val = ( ( float* ) ( buff ) )[ 0 ];
        SetAITable( Tbl, offs, val ); 
        return 0;
        }
    };

class TTE: public TVAI {
public:
    TPLCTable *Tbl;
    TParams* par;
	
	int offs;

    int Init(TPLCTable *Table,int offset);
    TTE(void);
    virtual ~TTE(void);
    float Value(void);

	//int		AI_Filter_Time;
	float	AI_Filtr_Val;		//Filtred TE
	float	AI_Last_Val;		//Last values
	unsigned long Last_Time;
	char initpassed;

    int parse_cmd( char *buff )
        {
        float val = ( ( float* ) ( buff ) )[ 0 ];
        SetAITable( Tbl, offs, val ); 
        return 0;
        }

    virtual int State()
        {
        if ( Value() == -1000 )
            {
            return -1;
            }

        return 1;
        }
    };

class TLE: public TVAI {
private:
    TPLCTable *Tbl;
    int offs;
    int type;
    float h1, h2, r;
public:
    int Init(int TYPE, TPLCTable *Table,int offset, float R, float H1, float H2);
    TLE(void);
    ~TLE(void);
    float Value(void);

	//	<--Формальная реализация виртуальных функций
	int State(void) {return 0;}
	//	--> Формальная реализация виртуальных функций

    int parse_cmd( char *buff )
        {
        float val = ( ( float* ) ( buff ) )[ 0 ];
        SetAITable( Tbl, offs, val ); 
        return 0;
        }
    };

class TIAI {
private:
    TPLCTable *Tbl;
    int offs;
public:
    int Init(TPLCTable *Table,int offset);
    TIAI(void);
    ~TIAI(void);
    unsigned long Value(void);
    };


class TPID {
private:
    TPLCTable *TblI;
    TPLCTable *TblO;
    TParams *par;
    int offs;
    int ioffs;
    int k, Ti, Td, Z, razg;
    float uk_1;
    float ek_1, ek_2;
    float dmn, dmx;
    unsigned long lt, start;
public:
    int HI;
	int state;
    TPID(void);
    ~TPID(void);
    int Init(TPLCTable *TableO,int offset, TPLCTable *TableI,int ioffset,int dmin, int dmax);  //AO and //AI
    int InitP(TParams *p, int kidx, int Tiidx, int Tdidx, int Zidx, int raz);
    void On(void);
    void Off(void);
    void Eval(void);
    void Reset(void);
    };


class TSav {
private:
    double  cn;
    unsigned long n;
    unsigned long integrator;
public:
    TSav(void);
    ~TSav(void);
    void Add(float val, unsigned long inegr);
    void R(void);
    float Q(void);
    };


class TTimer {
private:
    int b;          //Состояние, 0 - не работает, 1 - работа, 2 - пауза
    unsigned long z;//Задание таймера.
public:
    int save( char *buff );
    int load( char *buff );
    int get_saved_size();

    unsigned long lt;   //Время, когда таймер был поставлен на паузу.
    unsigned long ac;   //Время работы таймера.

    TTimer(void);
    ~TTimer(void);
    void S(void);
    void R(void);
    void P(void);
    unsigned long T(void);
    int Q(void);
    void SZ(unsigned long zad);  
    unsigned long Z(void);
    int State(void);
    };

class TCounter: public TVDO
    {
	unsigned long prev_q;
private:
    unsigned long s, lt;
    unsigned long MAX_VAL;
    int b;

    //-Интерфейс TVDO.
    char state;

public:
    //-Для реализации ошибки счетчика.
    int  pumps_count; // Количество связанных насосов. 
    TVDO **pumps;      // Связанные насосы. 
    //-Для реализации ошибки счетчика.

    //-Интерфейс TVDO.
    int Set(unsigned char Val)
        {
        Val++; // Чтобы не было Warning'а.
        return 0;
        }
    int On(void)  { return 0; }
    int Off(void) { return 0; }
    int State(void);
    int UState(void) { return 0; }
    //-Интерфейс TVDO.

    int offs;
    TPLCTable *Tbl;
    TCounter(void);
    int Init(TPLCTable *Table,int offset, unsigned long mv);
    ~TCounter(void);
    void P(void);
    void R(void);
    void S(void);
    void SetVal( float stVal );
    unsigned long Q(void);

		//Сохранение измененного состояния устройства в буфер.
        int save_changed_state( char *buff )
        {
			if ( prev_q != Q() )
				{
				( ( unsigned long* ) buff )[ 0 ] = Q();				
				prev_q = Q();
				return 4;
				}
                
            return 0;
            }

    //Сохранение состояния устройства в буфер.
    int save_state( char *buff  )
        {
        ( ( unsigned long* ) buff )[ 0 ] = Q();
		prev_q = Q();
        return 4;
        }

    //Выполнение команды, хранящейся в буфере.
    int parse_cmd( char *buff  )
        {
        SetVal( ( ( u_int_4* ) buff )[ 0 ] );
        return 4;
        }
    };

class filter_param: public i_simple_device
	{
	public:
	    filter_param( int n, TParams *par): par( par ), n( n )
            {
            }

       int save_changed_state( char *buff )
           {
           return save_state( buff );
           }

		int save_state( char *buff )
           {
		   ( ( float* ) buff )[ 0 ] = par->getParam( 0 );
           return 4;
           }

        int save_device( char *buff  )
            {
            ( ( unsigned long* ) buff )[ 0 ] = n;
            return 4;
            }
#pragma  argsused
        int load_state( char *buff  )
            {
            return 0;
            }
#pragma  argsused
        int load_changed_state( char *buff )
            {
            return 0;
            }
#pragma  argsused
        int load_device( char *buff  ) 
            {
            return 0;
            }

        int parse_cmd( char *buff  )
            {
			par->setParam( 0, ( ( float* ) buff )[ 0 ] );            
            return 4;
            }

        u_int_4 get_idx()
            {
            return 0;
            }

        void set_idx( u_int_4 new_idx )
            {
            new_idx++;
            }

        u_int_4 get_n() const
            {
            return n;
            }

         void print() const
             {
             Print( "%d \n", n );
             }

    protected:
        TParams* par;
		int n;
	};
/*
class temperature_state: public i_simple_device
    {
    public:
        temperature_state( complex_device* temper ): temper( temper )
            {
            }

       int save_changed_state( char *buff )
           {
           return save_state( buff );
           }

        int save_device( char *buff  )
            {
            ( ( unsigned long* ) buff )[ 0 ] = temper->get_n();
            return 4;
            }

        int load_state( char *buff  )
            {
            return temper->load_state( buff );
            }

        int load_changed_state( char *buff )
            {
            return temper->load_changed_state( buff );
            }

        int load_device( char *buff  ) 
            {
            return temper->load_device( buff );
            }

        int parse_cmd( char *buff  )
            {
            buff++;
            return 0;
            }

        u_int_4 get_idx()
            {
            return 0;
            }

        void set_idx( u_int_4 new_idx )
            {
            new_idx++;
            }

        u_int_4 get_n() const
            {
            return temper->get_n();
            }

         void print() const
             {
             temper->print();
             }

    protected:
        complex_device* temper;
    };
*/

class counter_state: public i_simple_device
    {
    public:
        counter_state( TCounter* ctr ): ctr( ctr )
            {
            }

       int save_state( char *buff  )
           {
           buff[ 0 ] = ctr->State();          
           return 1;
           }

       int save_changed_state( char *buff )
           {
           return save_state( buff );
           }

        int save_device( char *buff  )
            {
            ( ( unsigned long* ) buff )[ 0 ] = ctr->nom;
            return 4;
            }


        int load_state( char *buff  )
            {
            return ctr->load_state( buff );
            }

        int load_changed_state( char *buff )
            {
            return ctr->load_changed_state( buff );
            }

        int load_device( char *buff  ) 
            {
            return ctr->load_device( buff );
            }

        int parse_cmd( char *buff  )
            {
            buff++;
            return 0;
            }

        u_int_4 get_idx()
            {
            return 0;
            }

        void set_idx( u_int_4 new_idx )
            {
            new_idx++;
            }

        u_int_4 get_n() const
            {
            return ctr->get_n();
            }

         void print() const
             {
             ctr->print();
             }

    protected:
        TCounter* ctr;
    };

#endif