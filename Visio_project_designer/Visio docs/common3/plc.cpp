#if defined I7186_E || defined I7188_E || defined uPAC5000
#include "tcpCmctr.h"
#else
#include "cmmctr.h"
#endif //defined I7186_E || defined I7188_E

#include <stdlib.h>
#include "utils.h"
#include <malloc.h>
#include <mem.h>
#include <string.h>

#include "plc.h"
#include "init.h"
#include "priem.h"

#if !defined USE_NO_TANK_COMB_DEVICE && !defined MSA
#include "priem1.h"
#endif // USE_NO_TANK_COMB_DEVICE

#ifdef ASI
#include "asi.h"
#endif // ASI
#ifdef ASIWAGO
#include "asiwago.h"
#endif

#include "g_device.h"

//0-err
#define _do  0
#define _di  1
#define _ao  2
#define _ai  3

TVirtualDO *g_tvirt = new TVirtualDO();
TVirtualFB *g_tvirt_fb = new TVirtualFB();

#ifdef DEBUG
char g_is_FB = 1;   //  Флаг учитывания обратной связи (1 - не учитывать; 0 - учитывать)
#else
char g_is_FB = 0;
#endif

//  Флаг учитывания обратной связи для клапанов с одной обратной связи 
//  (1 - не учитывать; 0 - учитывать)
int g_no_valves_fb_1 = 0;

//  Флаг учитывания обратной связи для клапанов с двумя обратными связями 
//  (1 - не учитывать; 0 - учитывать)
int g_no_valves_fb = 0;

#if defined ASI || defined ASIWAGO
extern TASigateways*    ASigateways;
#endif // ASI

#ifndef USE_NO_TANK_COMB_DEVICE
device_communicator *g_dev_cmmctr = new device_communicator();
#endif // USE_NO_TANK_COMB_DEVICE

#ifdef USE_SIMPLE_DEV_ERRORS
#include "errors.h"

extern dev_errors_manager *g_dev_errors_manager; 

#endif // USE_SIMPLE_DEV_ERRORS

#ifdef USE_PAC_SYSTEM_INFO
#include "PAC_info.h"

extern PAC_info *g_PAC_system;
#endif // USE_PAC_SYSTEM_INFO

extern int G_PROJECT_ID;

extern unsigned long glob_last_trans;
////////////////////////////////////////////////////////
//Devices
////////////////////////////////////////////////////////
struct TDevices g_devs;
///////////////////////////////////////////////////////
extern int DEB;

extern int TPMS;
extern unsigned long CHAR_DEL;
extern unsigned long FRAME_DEL;

extern TTank	**g_tanks;
extern TMyComb	**g_combs;
extern TParams	*tpar;

#if !( defined USE_NO_TANK_COMB_DEVICE || defined USE_NO_COMB )
comb		*g_greb;
#endif // !( defined USE_NO_TANK_COMB_DEVICE || defined USE_NO_COMB )


//---file procedures for i7188------

FILE7188 *fopen7188(char *filename) {
    FILE7188 *f;
    f=(FILE7188 *)malloc(sizeof(FILE7188));
    if (f!=NULL) {
        f->fd=GetFileInfoByName(filename);
        if (f->fd!=NULL) {
            f->data=GetFilePositionByName(filename);
            if (f->data!=NULL) {
                f->pos=0;
                return f;
                } else {
                    free(f);
                    return NULL;
                };
            } else {
                free(f);
                return NULL;
            };
        } else {
            return NULL;
        };
    };

void fclose7188(FILE7188 *f) {
    free(f);
    };

int fgetc7188(FILE7188 *f) {
    if ( f->pos < f->fd->size )
		{
        return f->data[f->pos++];
        }
	else 
		{
        return EOF;
        }
    }

int BlockRead(FILE7188 *f, char *data,int col) {
    int i;
    if ( col >= 0 && f->pos + col <= f->fd->size ) 
		{
        for (i=0; i<col; i++) data[ i ]=f->data[f->pos++];
        return 0;
        } else return EOF;
    // return -1;
    };


//----------------------------------

struct TPLCTables Tables;

void InitTables(void) {
    int i;
    Tables.TablesCnt=0;
    for (i=0; i<MAX_TABLES_CNT; i++) Tables.Table[ i ]=NULL;
    };

void ModbusCRC(unsigned char *Buf,int bc) {
    int idx, bufidx;
    unsigned int CRC;
    int datlen;
    char Flag;

    CRC=65535;
    datlen=bc;
    bufidx=0;
    while (datlen > 0) {
        CRC = CRC ^ Buf[bufidx];
        for (idx=0; idx<=7; idx++) {
            Flag = CRC & 1;
            CRC = CRC >> 1;
            if (Flag) CRC = CRC ^ 0x0A001;
            };
        datlen--;
        bufidx++;
        };
    Buf[bc]=CRC & 0xFF;
    Buf[bc+1]=CRC >> 8;
    };

//Table functions
int GetDITable(TPLCTable *tptr, int N) {
    if (N<0) return 0;
    if (N>=tptr->DIcnt) return 0;
    return (tptr->DI[N]) & 1;
    };

int SetDOTable(TPLCTable *tptr, int N, char val) {
    char b;
    if (N<0) return -1;
    if (N<tptr->DOcnt) {
        if (val>0) {b=1;} else {b=0;};
        tptr->_DO[N]=b;
        return 0;
        } else return -1;
    };

int SetDITable(TPLCTable *tptr, int N, char val) 
    {
    char b;
    if ( N < 0 ) return -1;
    if ( N < tptr->DIcnt ) 
        {
        b = val > 0 ? 1 : 0;
        tptr->DI[ N ] = b;
        return 0;
        }
    else return -1;
    }
int GetDOTable(TPLCTable *tptr, int N) {
    if (N<0) return 0;
    if (N<tptr->DOcnt) {
        return tptr->_DO[N];
        } else return 0;
    };

int SetAOTable(TPLCTable *tptr, int N, float val) {
    if (N<0) return -1;
    if (N<tptr->AOcnt) {
        tptr->_AO[N]=val;
        return 0;
        } else return -1;
    };

float GetAOTable(TPLCTable *tptr, int N) {
    if (N<0) return -1;
    if (N<tptr->AOcnt) {
        return tptr->_AO[N];
        } else return -1;
    };

int SetAITable( TPLCTable *tptr, int N, float val ) 
    {  
    if ( N < 0 ) return -1;
    if ( N < tptr->AIcnt ) 
        {
        tptr->AI[ N ] = val;
        return 0;
        } 
    else return -1;
    };
float GetAITable(TPLCTable *tptr, int N) {
    if (N<0) return -1;
    if (N<tptr->AIcnt) {
        return tptr->AI[N];
        } else return -1;
    };


int SetAOTableN(int nom, int offs, float val) {
    if (nom<Tables.TablesCnt) {
        if (offs<Tables.Table[nom]->AOcnt) {
            Tables.Table[nom]->_AO[offs]=val;
            //    Print("\n\rOK");
            return 0;
            };
        };
    return -1;
    };

int SetAITableN(int nom, int offs, float val) {
    if (nom<Tables.TablesCnt) {
        if (offs<Tables.Table[nom]->AIcnt) {
            Tables.Table[nom]->AI[offs]=val;
            return 0;
            };
        };
    return -1;
    };

int SetDOTableN(int nom, int offs, char val) {
    char b;
    if (nom<Tables.TablesCnt) {
        if (offs<Tables.Table[nom]->DOcnt) {
            if (val>0) {b=1;} else {b=0;};
            Tables.Table[nom]->_DO[offs]=b;
            return 0;
            };
        };
    return -1;
    };

int SetDITableN(int nom, int offs, char val) {
    char b;
    if (nom<Tables.TablesCnt) {
        if (offs<Tables.Table[nom]->DIcnt) {
            if (val>0) {b=1;} else {b=0;};
            Tables.Table[nom]->DI[offs]=b;
            return 0;
            };
        };
    return -1;
    };





int RegisterTable(TPLCTable *tptr) {
    if (Tables.TablesCnt==MAX_TABLES_CNT) {
        return -1;
        };
    Tables.Table[Tables.TablesCnt]=tptr;
    Tables.TablesCnt++;
    return 0;
    };

int UnregisterTable(TPLCTable *tptr) {
    int i, idx;
    idx=-1;
    for (i=0; i<Tables.TablesCnt; i++) if (Tables.Table[ i ]==tptr) idx=i;
    if (idx==-1) {
        return -1;
        };
    for (i=idx; i<Tables.TablesCnt-1; i++) Tables.Table[ i ]=Tables.Table[i+1];
    Tables.TablesCnt--;
    return 0;
    };

TPLCTable *GetTableByID(int Id) {
    int i, idx;
    idx=-1;
    for (i=0; i<Tables.TablesCnt; i++) if (Tables.Table[ i ]->ID==Id) idx=i;
    if (idx==-1) {
        return NULL;
        };
    return Tables.Table[idx];
    };



//Wago Driver
TWagoDrv::TWagoDrv(void)
    {
    retrCnt=10;
    IsRTS=0;
    };

TWagoDrv::~TWagoDrv(void) {
    _FreeNodes();
    // ClosePort(port);
    RestoreCom(port);
    };

unsigned long TWagoDrv::GetDrvID(void) {
    return 1;
    };

int __readRestStr(FILE7188 *fp) {
    int ch;
    ch=fgetc7188(fp);
    //read rest of string. It's not manage string
    while (!(ch=='\n' || ch=='\r' || ch==EOF)) ch=fgetc7188(fp);
    return ch;
    };

long __readVal(FILE7188 *fp) {
    char buf[12];
    int i;
    int ch;
    ch=fgetc7188(fp);
    while (ch==' ' || ch=='\t') ch=fgetc7188(fp);
    i=0;
    while (i<12 && (ch>='0' && ch<='9')) {
        buf[ i ]=ch;
        ch=fgetc7188(fp);
        i++;
        };
    if (i>0) {
        buf[ i ]=0;
        return atol(buf);
        } else return -1;
    };

long __readVal1(FILE7188 *fp) {
    char buf[12];
    int i;
    int ch;
    ch=fgetc7188(fp);
    while ((ch<'0' || ch>'9') && (ch!=EOF)) ch=fgetc7188(fp);
    i=0;
    while (i<12 && (ch>='0' && ch<='9') && ch!=EOF) {
        buf[ i ]=ch;
        ch=fgetc7188(fp);
        i++;
        };
    if (i>0) {
        buf[ i ]=0;
        return atol(buf);
        } else return -1;
    };


#define EDRVID         0
#define EDRVID_EXP    -1
#define EPORT         -2
#define EPORT_EXP     -3
#define ENODE_CNT     -4
#define ENODE_MEM     -5
#define ENODE_CNT_EXP -6
#define ENODE         -7
#define EMEM          -8
#define EREGISTER     -9


void __ErrDscr(int err) 
	{
    if (DEB)  { Print("Failed \n\rWrong drv file (Wago):");
    switch (err) {
    case (EDRVID): {
        Print("DrvID do not match");
        break;
        };
    case (EDRVID_EXP): {
        Print("DrvID expected");
        break;
        };
    case (EPORT): {
        Print("Port parameters");
        break;
        };
    case (EPORT_EXP): {
        Print("Port parameters expected");
        break;
        };
    case (ENODE_CNT): {
        Print("Node count");
        break;
        };
    case (ENODE_MEM): {
        Print("Node memory allocating");
        break;
        };
    case (ENODE_CNT_EXP): {
        Print("Node count expected");
        break;
        };
    case (ENODE): {
        Print("Node reading");
        break;
        };
    case (EMEM): {
        Print("Memory allocating error");
        break;
        };
    case (EREGISTER): {
        Print("Registering table error");
        break;
        };
        };
        };
    Print("\n\r");
    };

void TWagoDrv::_FreeNodes (void) {
    int i;
    for (i=0; i<NodeCnt; i++) {
        UnregisterTable(Nodes[ i ].Table);
        //free table data
        if (Nodes[ i ].Table->AOcnt>0) {
            free(Nodes[ i ].Table->AO);
            free(Nodes[ i ].Table->_AO);
            free(Nodes[ i ].Table->AOR);
            };
        if (Nodes[ i ].Table->DOcnt>0) {
            free(Nodes[ i ].Table->DO);
            free(Nodes[ i ].Table->_DO);
            };
        if (Nodes[ i ].Table->AIcnt>0) {
            free(Nodes[ i ].Table->AI);
            free(Nodes[ i ].Table->AIR);
            };
        if (Nodes[ i ].Table->DIcnt>0) {
            free(Nodes[ i ].Table->DI);
            };
        if (Nodes[ i ].mAIcnt>0) {
            free(Nodes[ i ].mAI);
            };
        if (Nodes[ i ].mAOcnt>0) {
            free(Nodes[ i ].mAO);
            };
        };
    free(Nodes);
    free(tbls);
    };

void TWagoDrv::ReadRanges(FILE7188 *df) {
    int ch, tid, chen, kind;
    long rl, rh;
    ch=fgetc7188(df);
    while (ch!=EOF) {
        if (ch=='%') {
            tid=__readVal(df);
            kind=__readVal(df);
            chen=__readVal(df);
            rl=__readVal(df);
            rh=__readVal(df);
            //      printf("\n\rRANGE %d %d %d %ld %ld", tid, kind, chen, rl, rh);
            if (kind==0) {//AI
                Nodes[tid].Table->AIR[chen].rl=rl;
                Nodes[tid].Table->AIR[chen].rh=rh;
                } else if (kind==1) { //AO
                    Nodes[tid].Table->AOR[chen].rl=rl;
                    Nodes[tid].Table->AOR[chen].rh=rh;
                };
            };
        ch=fgetc7188(df);
        };
    };

#ifndef USE_NO_TANK_COMB_DEVICE
int TWagoDrv::LoadDescrB(char *fn) 
    {
#define ST_NON           1
#define ST_DRV_ID        2
#define ST_PORT_PARAMS   3
#define ST_NCNT          4
#define ST_NODES         5
    FILE7188	*df;
    int		state, t1, o1, t2, o2, t3, o3, t4, o4, t5, o5;
    int		ch, wc, dt;
    long		itmp;
    int		err, mc;
    int		i, j, k, l, m, len3, len4;
    int		mdls[100];
    long		devpos;
    TDO0		*do0;
    TDI		*di;
    T2DO0		*d2o;
    TDO1		*do1;
    TDO2		*do2;
    T2DO2		*d2o2;
    TAI		*ai;
	TTE		*te;
    TLE		*le;
    float		f1, f2, f3;
    TCounter	*ctr;
    TAO		*ao;
    TMIX		*mix;
    TDO3		*do3;

    TMyComb	*Comb1;
    TTank	*Tank1;

    unsigned char	b, b1, b2, b3;
    unsigned int	w, w1, devcnt, bait, state_t;

    //load description and init object
    //1.try to open file fn

    if (DEB)   Print( "Reading Wago description from %s....\n\r", fn);
    if ( ( df = fopen7188( fn ) ) != NULL ) 
        {
        //0.

        //-ID - 'WG {4B714C08-9602-4130-8563-4B51E08BB9D7}'
        char id[42];
        id[41] = 0;
        BlockRead( df, id, 41 );

        if (	( strcmp( id, "WG {4B714C08-9602-4130-8563-4B51E08BB9D7}" ) != 0 )
			&&	( strcmp( id, "WG {4B714C08-9602-4130-8563-4B51E08BB9D8}" ) != 0 ) )
            {
#ifdef DEBUG
            Print( "Not correct file format!!!\n\r" );            
#endif
            while ( 1 ) { }
            }

        TTank::TankCnt = 0;                      

        //-Editor version
        BlockRead( df, &b, 1);
        int versionN = b;
        BlockRead( df, &b, 1);
        versionN +=  256 * b;

        if ( versionN < 11 )
            {
#ifdef DEBUG
            Print( "\nNot correct editor version - %d, must be 11!!!\n",
                versionN );
#endif
            while ( 1 ) { }
            }

        //-File version
        BlockRead( df, &b, 1);
        int fileVersion = b;
        BlockRead( df, &b, 1);
        fileVersion += 256 * b;

        //1.Read header
        //port
        err=0;
        BlockRead(df, &b, 1);
        port=b;
        speed=115200;
        dataBits=8;
        parity=0;
        stopBits=1;
        //node count
        BlockRead(df, &b, 1);
        NodeCnt=b;
        if (DEB) Print("Port %d, nodes %d \n\r", (int)port, (int)NodeCnt);
        //allocate memory for nodes structure
        Nodes=(TNode *)malloc(sizeof(TNode)*NodeCnt);
#if defined I7186_E || defined I7188_E 
        if (NodeCnt > 0)
            {
            for (i=0;i<NodeCnt;i++)
                {
                Nodes[i].clientID = tcpCommunicator::tcpipClientID++;
                }
            }
#endif
        tbls=(TPLCTable *)malloc(sizeof(TPLCTable)*NodeCnt);
        if (Nodes!=NULL && tbls!=NULL) 
            {
            if (DEB) Print("Allocating nodes memory...OK\n\r");
            //now allocate tables for each node
            for (i=0; i<NodeCnt; i++) 
                {
                Nodes[ i ].Table=&tbls[ i ];
                Nodes[ i ].Table->DrvID=GetDrvID();
                Nodes[ i ].Table->state=TS_INIT;
                Nodes[ i ].trcnt=retrCnt;
                Nodes[ i ].nodeType = N_315;
                Nodes[ i ].connectedstate = 0;
                Nodes[ i ].socknumber = -1;
                Nodes[ i ].lastconnecttry = 0;
                strcpy( Nodes[ i ].NodeIp, "0.0.0.0" );                
                }
            } 
        else 
            {
            __ErrDscr(ENODE_MEM);
            _FreeNodes();
            fclose7188(df);
            return -1;
            }
        //2.Read nodes
        for (j=0; j<NodeCnt; j++) 
            {
            //Type
            BlockRead(df, &b, 1);
            Nodes[ j ].nodeType = NODE_TYPE( b );               


#if defined I7186_E || defined I7188_E
            //IP
            if (( Nodes[ j ].nodeType == N_341 )
                ||( Nodes[ j ].nodeType == N_841 )
                ||( Nodes[ j ].nodeType == N_PET ))
                {
                struct in_addr ip;
                unsigned long tmp;

                ip.s_addr = 0;            

                for ( i = 0; i < 4; i++ )
                    {  
                    BlockRead( df, &b, 1 );
                    tmp = b;
                    tmp <<= 8 * i;
                    ip.s_addr = ip.s_addr | tmp;
                    }

#ifndef DEBUG                     
                strcpy( Nodes[ j ].NodeIp, inet_ntoa( ip ) );
                //   xxx             Nodes[ j ].NodeIp = inet_ntoa( ip );
#endif                    

#ifdef DEBUG
                //                strcpy( Nodes[ j ].NodeIp, inet_ntoa( ip ) );
                //   xxx             Nodes[ j ].NodeIp = inet_ntoa( ip );
                //   xxx             Print( "Node IP: %s \n", Nodes[ j ].NodeIp );
                //   xxx             Getch();
#endif                   
                }
#endif // defined I7186_E || defined I7188_E

            //addr
            BlockRead(df, &b, 1);
            Nodes[j].Table->ID=b;
            if (DEB) Print("Node adr: %d\n", (int)b);

            //Read modules count
            BlockRead(df, &b, 1);
            mc=b;
            if (DEB) Print("Modules count: %d\n", (int)b);
            //Sort and create table image
            //Init tables
            Nodes[j].Table->DOcnt=0;
            Nodes[j].Table->DIcnt=0;
            Nodes[j].Table->AIcnt=0;
            Nodes[j].Table->AOcnt=0;
            Nodes[j].mAIcnt=0;
            Nodes[j].mAOcnt=0;
            Nodes[j].wAOcnt=0;
            Nodes[j].wAIcnt=0;
            //sort and prepare tables
            //3. read modules in temp buffer
            if (DEB) Print(" modules:");
            wc=ch=0;
            for (i=0; i<mc; i++) 
                {
                BlockRead(df, (unsigned char *)&w, 2);
                if (DEB) Print(" %d", w);
                mdls[ i ]=w;
                itmp=GetModuleType(w);
                ch=GetChCnt(w);
                wc=GetABC(w,Nodes[j].nodeType);
                if (itmp!=TYPE_UNKN && ch>=0) 
                    {
                    switch (itmp) 
                        {
                        case TYPE_AO: 
                            Nodes[j].Table->AOcnt+=ch;
                            Nodes[j].mAOcnt+=1;
                            Nodes[j].wAOcnt+=wc*ch;
                            break;

                        case TYPE_AI: 
                            Nodes[j].Table->AIcnt+=ch;
                            Nodes[j].mAIcnt+=1;
                            Nodes[j].wAIcnt+=wc*ch;
                            break;

                        case TYPE_DO: 
                            Nodes[j].Table->DOcnt+=ch;
                            break;

                        case TYPE_DI: 
                            Nodes[j].Table->DIcnt+=ch;
                            break;

                        case TYPE_AIAO: 
                            Nodes[j].Table->AIcnt+=ch;
                            Nodes[j].mAIcnt+=1;
                            Nodes[j].wAIcnt+=wc*ch;
                            Nodes[j].Table->AOcnt+=ch;
                            Nodes[j].mAOcnt+=1;
                            Nodes[j].wAOcnt+=wc*ch;
                            break;

                        }
                    } 
                else 
                    {
                    err=1;
                    break;
                    }
#ifdef ASIWAGO
                if (655 == w)
                    {
                    TASigateways::gatecnt++;
                    TASigateways::gatesatnodes[j]++;
#ifdef DEBUG
                    Print("Wago ASI gateway at node %u used\n\r",j);
#endif // DEBUG
                    }
#endif
                }
            //      if (DEB) Print("\n\rPrepare Table...OK");
            if ( DEB ) Print( "\n\rDI:%d,DO:%d,AO:%d,AI:%d\n\r",
                Nodes[ j ].Table->DIcnt, Nodes[ j ].Table->DOcnt,
                Nodes[ j ].Table->AOcnt,Nodes[ j ].Table->AIcnt );
            //4.3. allocate memory for node elements.
            if ( Nodes[ j ].mAOcnt > 0 ) 
                {
                Nodes[j].mAO=(int *)malloc(sizeof(int)*Nodes[j].mAOcnt);
                if (Nodes[j].mAO==NULL) 
                    {
                    _FreeNodes();
                    __ErrDscr(EMEM);
                    fclose7188(df);
                    return -1;
                    }
                }
            if (Nodes[j].mAIcnt>0) 
                {
                Nodes[j].mAI=(int *)malloc(sizeof(int)*Nodes[j].mAIcnt);
                if (Nodes[j].mAI==NULL) 
                    {
                    _FreeNodes();
                    __ErrDscr(EMEM);
                    fclose7188(df);
                    return -1;
                    };
                };
            //fill node structure
            wc=0; //incriment for AO
            ch=0; //incriment for AI
            for (i=0; i<mc; i++) 
                {
                itmp=GetModuleType(mdls[ i ]);
                switch (itmp) 
                    {
                    case TYPE_AO: 
                        Nodes[j].mAO[wc]=mdls[ i ];
                        wc++;
                        break;

                    case TYPE_AI: 
                        Nodes[j].mAI[ch]=mdls[ i ];
                        ch++;
                        break;

                    case TYPE_AIAO: 
                        Nodes[j].mAI[ch]=mdls[ i ];
                        Nodes[j].mAO[wc]=mdls[ i ];
                        wc++;
                        ch++;
                        break;

                    }
                }
            if (DEB) Print("mAOcnt: %d, wAOcnt %d\n\r",Nodes[j].mAOcnt,Nodes[j].wAOcnt);
            if (DEB) Print("mAIcnt: %d, wAIcnt %d\n\r",Nodes[j].mAIcnt,Nodes[j].wAIcnt);
            //4.4 create Table
            if (Nodes[j].Table->DOcnt>0) 
                {
                Nodes[j].Table->DO=(unsigned char *)malloc(Nodes[j].Table->DOcnt);
                Nodes[j].Table->_DO=(unsigned char *)malloc(Nodes[j].Table->DOcnt);
                if (Nodes[j].Table->DO==NULL || Nodes[j].Table->_DO==NULL) 
                    {
                    _FreeNodes();
                    __ErrDscr(EMEM);
                    fclose7188(df);
                    return -1;
                    }
                }
            if (Nodes[j].Table->DIcnt>0) 
                {
                Nodes[j].Table->DI=(unsigned char *)malloc(Nodes[j].Table->DIcnt);
                if (Nodes[j].Table->DI==NULL) 
                    {
                    _FreeNodes();
                    __ErrDscr(EMEM);
                    fclose7188(df);
                    return -1;
                    }
                }
            if (Nodes[j].Table->AIcnt>0) 
                {
                Nodes[j].Table->AI=(float *)malloc(sizeof(float)*Nodes[j].Table->AIcnt);
                Nodes[j].Table->AIR=(TRange *)malloc(sizeof(TRange)*Nodes[j].Table->AIcnt);
                if (Nodes[j].Table->AI==NULL || Nodes[j].Table->AIR==NULL) 
                    {
                    _FreeNodes();
                    __ErrDscr(EMEM);
                    fclose7188(df);
                    return -1;
                    }
                }
            if (Nodes[j].Table->AOcnt>0) 
                {
                Nodes[j].Table->_AO=(float *)malloc(sizeof(float)*Nodes[j].Table->AOcnt);
                Nodes[j].Table->AO=(float *)malloc(sizeof(float)*Nodes[j].Table->AOcnt);
                Nodes[j].Table->AOR=(TRange *)malloc(sizeof(TRange)*Nodes[j].Table->AOcnt);
                if (Nodes[j].Table->AO==NULL || Nodes[j].Table->_AO==NULL || Nodes[j].Table->AOR==NULL) 
                    {
                    _FreeNodes();
                    __ErrDscr(EMEM);
                    fclose7188(df);
                    return -1;
                    }
                }
            if (DEB) Print("\n\rCreating Table...OK");
            //init Table
            for (i=0; i<Nodes[j].Table->DOcnt; i++) 
                {
                Nodes[j].Table->DO[ i ]=0; 
                Nodes[j].Table->_DO[ i ]=0;
                }
            for (i=0; i<Nodes[j].Table->AIcnt; i++) 
                {
                Nodes[j].Table->AI[ i ]=0;
                Nodes[j].Table->AIR[ i ].rl=0;
                Nodes[j].Table->AIR[ i ].rh=100;
                }
            for (i=0; i<Nodes[j].Table->DIcnt; i++) Nodes[j].Table->DI[ i ]=0;
            /*
            for (i=0, k=0; i<Nodes[j].mAOcnt; i++) {
            ch=GetChCnt(Nodes[j].mAO[ i ]);
            for (l=0; l<ch; l++) {
            Nodes[j].Table->AO[k]=0;
            Nodes[j].Table->_AO[k]=GetDefVal(Nodes[j].mAO[ i ]);
            Nodes[j].Table->AIR[k].rl=0;
            Nodes[j].Table->AIR[k].rh=100;
            k++;
            };
            };
            */
            for (i=0; i<Nodes[j].Table->AOcnt; i++) 
                {
                Nodes[j].Table->AOR[ i ].rl=0;
                Nodes[j].Table->AOR[ i ].rh=100;
                Nodes[j].Table->AO[ i ]=0;
                Nodes[j].Table->_AO[ i ]=0;
                };

            if (DEB) Print("Init Tables...OK\n\r");
            if (err==0) 
                {
                //all OK register table
                if (RegisterTable(Nodes[j].Table)!=0) 
                    {
                    _FreeNodes();
                    __ErrDscr(EREGISTER);
                    fclose7188(df);
                    return -1;
                    };
                if (DEB) Print("Registering table no %d of %d\n\r", j, NodeCnt);
                } 
            else 
                {
                Print("Error %d\n\r", (int)err);
                //Getch();
                _FreeNodes();
                __ErrDscr(ENODE);
                fclose7188(df);
                return -1;
                }
            }

        //read devices
        devpos=df->pos;
        BlockRead(df,(unsigned char *)&w, 2);	// cnt dev
        devcnt=w;
        if (DEB) 
            { 
            Print("Dev cnt %u\n\r", devcnt); 
            //Getch(); 
            }

        for (i=0; i<50; i++) mdls[ i ]=0;

		for (i=0; i<devcnt; i++) 
			{
			BlockRead(df, &b, 1);
			mdls[b]+=1; 

			BlockRead(df, &b, 1);     
			df->pos += b + 2;

			BlockRead(df, &b, 1);
			df->pos+=b*7;
			}        


//if ( DEB ) Print("================ df->pos = %u \n", df->pos );
//
//		for (i=0; i<devcnt; i++) 
//			{
//			BlockRead(df, &b, 1); // type
//			mdls[b]+=1; 
//
//			Print("%i [%d] - ", i, b );
//
// 			byte str_len = 0;
// 			BlockRead( df, &str_len, 1 );
// 
// 			char *str_name = new char[ str_len + 1 ];
// 			BlockRead( df, str_name, str_len );
// 
// 			BlockRead(df, (unsigned char *)&w1, 2); // no
//
//			BlockRead(df, &b, 1);
//			df->pos+=b*7;
//
//
//			Print("%u - ", w1 );
//
//			for (j=0; j<str_len; j++) 
//				{
//				Print("%c", str_name[ j ] );
//				}
//
//			Print(" ( %d )  %lu \n\r", b, df->pos );
//			};

        wc=0;
        for (i=0; i<=4; i++) wc+=mdls[ i ];
        wc += mdls[ 17 ];
        wc += mdls[ dtV1DO3DI ];
        wc += mdls[ dtV1DO2DI_S ];
        wc += mdls[ dtASMix ];        

#ifdef USE_SIMPLE_DEV_ERRORS
        // Создаем ошибки для клапанов, насосов, мешалок + счетчиков и танков.
        g_dev_errors_manager = new dev_errors_manager( wc + 
            mdls[ dtN1DO1DI ] + mdls[ dtM1DO1DI ] + mdls[ dtCTR ] + 
            mdls[ dtTE ] + 20 );
#endif // USE_SIMPLE_DEV_ERRORS

        g_dev_cmmctr->add_device( new complex_device( 0, "GLB", VD_TYPES_COUNT, 0 ) );

        //-Добавляем клапаны для передачи серверу;    
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ] = 
            new complex_device( VD_V, "V", wc, 1 );
        //-!>
        //Valves
        //allocate memory
        g_devs.VCnt=wc;
        if (wc>0) 
            {
            if (DEB)Print("V %u\n\r", wc);
            g_devs.VDev=(TVDO **)malloc(wc*sizeof(void *));
            if (g_devs.VDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.VCnt=0;

        //PUMPS
        wc=mdls[5];
        g_devs.NCnt=wc;
        //-Добавляем насосы для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_N ] = 
            new complex_device( VD_N, "N", wc, 1 );
        //-!>
        //Print("After add\n");
        //Getch();
        if (wc>0) 
            {
            if (DEB)Print("\n\rN %u", wc);
            g_devs.NDev=(TVDO **)malloc(wc*sizeof(void *));
            if (g_devs.NDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.NCnt=0;

        //MOTORS
        wc=mdls[6];
        g_devs.MCnt=wc;
        //-Добавляем мешалки для передачи серверу     
        ( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_M ] = 
            new complex_device( VD_M, "M", wc, 1 );
        //-!>
        if (wc>0) 
            {
            if (DEB)Print("\n\rM %u", wc);
            g_devs.MDev=(TVDO **)malloc(wc*sizeof(void *));
            if (g_devs.MDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.MCnt=0;

        //LEVELS
        wc = mdls[ dtLS ] + mdls[ dtLS_ex ];
        g_devs.LSCnt=wc;
        //-Добавляем уровни для передачи серверу     
        ( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_LS ] = 
            new complex_device( VD_LS, "LS", wc, 1 );
        //-!>
        if (wc>0) 
            {
            if (DEB)Print("\n\rLS %u", wc);
            g_devs.LSDev=(TVDI **)malloc(wc*sizeof(void *));
            if (g_devs.LSDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.LSCnt=0;

        //TEMPERATURE
        wc=mdls[8];
        g_devs.TECnt=wc;

        //-Добавляем температуры для передачи серверу     
        ( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_TE ] = 
            new complex_device( VD_TE, "TE", wc, 4 );
        //-!>

		//-Добавляем "значение времени" фильтра температуры для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_TE_FILTER ] = 
            new complex_device( 1, "TE_FILTER", wc, i_complex_device::GROUP_DEV_FLOAT );

        if (wc>0) 
            {
            if (DEB) Print("\n\rTE %u", wc);
            g_devs.TEDev=(TVAI **)malloc(wc*sizeof(void *));
            if (g_devs.TEDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.TECnt=0;

        //CUR FLOW
        wc=mdls[9];
        g_devs.FECnt=wc;
        //-Добавляем расход для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_FE ] = 
            new complex_device( VD_FE, "FE", wc, 4 );
        //-!>
        if (wc>0) 
            {
            if (DEB) Print("\n\rFE %u", wc);
            g_devs.FEDev=(TVAI **)malloc(wc*sizeof(void *));
            if (g_devs.FEDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.FECnt=0;

        //FLOW
        wc=mdls[10];
        g_devs.FSCnt=wc;
        //-Добавляем поток для передачи серверу     
        ( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_FS ] = 
            new complex_device( VD_FS, "FS", wc, 1 );
        //-!>
        if (wc>0) 
            {
            if (DEB) Print("\n\rFS %u", wc);
            g_devs.FSDev=(TVDI **)malloc(wc*sizeof(void *));
            if (g_devs.FSDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.FSCnt=0;

        //CTR
        wc=mdls[11];
        g_devs.CTRCnt=wc;
        //-Добавляем учетчики для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_CTR ] = 
            new complex_device( VD_CTR, "CTR", wc, i_complex_device::GROUP_DEV_LONG );
        //-!>

        //-Добавляем состояние счетчиков для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_CTR_STATE ] = 
            new complex_device( VD_CTR_STATE, "CTR_STATE", wc, i_complex_device::GROUP_DEV_BYTE );
        //-!>
        if (wc>0) 
            {
            if (DEB) Print("\n\rCTR %u", wc);
            g_devs.CTRDev=(TCounter **)malloc(wc*sizeof(void *));
            if (g_devs.CTRDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.CTRCnt=0;

        //AO
        wc=mdls[12];
        g_devs.AOCnt=wc;

        //-Добавляем AO для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_AO ] = 
            new complex_device( VD_AO, "AO", wc, 4 );
        //-!>
        if (wc>0) {
            if (DEB) Print("\n\rAO %u", wc);
            g_devs.AODev=(TVAO **)malloc(wc*sizeof(void *));
            if (g_devs.AODev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.AOCnt=0;

        //LE
        wc=mdls[13]+mdls[14]+mdls[15]+mdls[16];
        g_devs.LECnt=wc;
        //-Добавляем текущие уровни для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_LE ] = 
            new complex_device( VD_LE, "LE", wc, 4 );
        //-!>
        if (wc>0) {
            if (DEB) Print("\n\rLE %u", wc);
            g_devs.LEDev=(TVAI **)malloc(wc*sizeof(void *));
            if (g_devs.LEDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.LECnt=0;

        //FB
        wc=mdls[18];
        g_devs.FBCnt=wc;
        //-Добавляем FB для передачи серверу     
        ( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_FB ] = 
            new complex_device( VD_FB, "FB", wc, 1 );
        //-!>
        if (wc>0) 
            {
            if (DEB) Print("\n\rFB %u", wc);
            g_devs.FBDev=(TVDI **)malloc(wc*sizeof(void *));
            if (g_devs.FBDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.FBCnt=0;

        //UPR
        wc=mdls[19];
        g_devs.UPRCnt=wc;
        //-Добавляем UPR для передачи серверу      
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_UPR ] = 
            new complex_device( VD_UPR, "UPR", wc, 1 );
        //-!>
        if (wc>0) 
            {
            if (DEB) Print("\n\rUPR %u", wc);
            g_devs.UPRDev=(TVDO **)malloc(wc*sizeof(void *));
            if (g_devs.UPRDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.UPRCnt=0;

        //QE
        wc=mdls[20];
        g_devs.QECnt=wc;
        //-Добавляем QE для передачи серверу     
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_QE ] = 
            new complex_device( VD_QE, "QE", wc, 4 );
        //-!>
        if (wc>0) 
            {
            if (DEB) Print("\n\rQE %u", wc);
            g_devs.QEDev=(TVAI **)malloc(wc*sizeof(void *));
            if (g_devs.QEDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.QECnt=0;

        //AI
        wc=mdls[25];	
        g_devs.AICnt=wc;
        //-Добавляем AI для передачи серверу;    
        ( ( complex_device* ) g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_AI ] = 
            new complex_device( VD_AI, "AI", wc, 4 );
        //-!>
        if (wc>0) {
            if (DEB) Print("\n\rAI %u", wc);
            g_devs.AIDev=(TVAI **)malloc(wc*sizeof(void *));
            if (g_devs.AIDev==NULL) 
                {
                __ErrDscr(EMEM);
#pragma warn -eff
                while (1) ;
#pragma warn .eff
                }
            }
        g_devs.AICnt=0;


        //fill dev structures
        df->pos=devpos;
        BlockRead(df,(unsigned char *)&w, 2);
        devcnt=w;

        for (i=0; i<devcnt; i++) 
            {
            BlockRead(df, &b, 1); // type
            dt=b;

            //description      
            BlockRead(df, &b, 1);                  
#ifdef USE_SIMPLE_DEV_ERRORS
            char tmp_str[ 256 ] = { 0 };            
            BlockRead( df, tmp_str, b );
#else 
            df->pos += b;     
#endif // USE_SIMPLE_DEV_ERRORS

            static int valve_n = 0;
            static int pump_n = 0;
            static int mesh_n = 0;
            static int LS_n = 0;
            static int LE_n = 0;
            static int TE_n = 0;
            static int FS_n = 0;
            static int FB_n = 0;
            static int FE_n = 0;
            static int CTR_n = 0;
            static int UPR_n = 0;
            static int QE_n = 0;
            static int AO_n = 0;
            static int AI_n = 0;

            static int CTR_STATE_n = 0;
			static int TE_FILTER_n = 0;
            switch (dt) 
                {
                //dtV
                case 0: 
                    {
                    do0=new TDO0;
#ifdef USE_SIMPLE_DEV_ERRORS
                    do0->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS

                    do0->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=do0;

                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=11;
                    do0->Init(Tables.Table[t1], o1);

                    //-Добавляем клапан для передачи серверу как устройство.              
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];
                    tmp->sub_dev[ valve_n++ ] = do0;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    do0->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( do0 ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };
                    //dtV2DO

                case 1: 
                    {
                    d2o=new T2DO0;
#ifdef USE_SIMPLE_DEV_ERRORS
                    d2o->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS                    

                    d2o->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=d2o;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=4;
                    d2o->Init(Tables.Table[t1], o1, Tables.Table[t2], o2);
                    d2o->Off();
                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];          
                    tmp->sub_dev[ valve_n++ ] = d2o;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    d2o->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( d2o ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dtV1DO1DI
                case 2: 
                    {
                    do1=new TDO1;
#ifdef USE_SIMPLE_DEV_ERRORS
                    do1->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS                    

                    do1->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=do1;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;

                    df->pos+=2;    

                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=4;
                    do1->Init(Tables.Table[t1], o1, Tables.Table[t2], o2);

                    df->pos += 3;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    do1->checkFB = f1;
                    //-Добавляем клапан для передачи серверу как устройство.
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];
                    tmp->sub_dev[ valve_n++ ] = do1;          
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    do1->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( do1 ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dtV1DO2DI
                case 3: 
                    {
                    do2=new TDO2;
#ifdef USE_SIMPLE_DEV_ERRORS
                    do2->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS                    
                    
                    do2->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=do2;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t3=b;
                    BlockRead(df, &b, 1);
                    o3=b;
                    df->pos+=4;
                    do2->Init(Tables.Table[t1], o1, Tables.Table[t2], o2, Tables.Table[t3], o3);

                    //-Добавляем клапан для передачи серверу как устройство.   
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];
                    tmp->sub_dev[ valve_n++ ] = do2;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    do2->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( do2 ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dt2DO2DI
                case 4: 
                    {
                    d2o2=new T2DO2;
#ifdef USE_SIMPLE_DEV_ERRORS
                    d2o2->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS                    
                    
                    d2o2->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=d2o2;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t3=b;
                    BlockRead(df, &b, 1);
                    o3=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t4=b;
                    BlockRead(df, &b, 1);
                    o4=b;
                    df->pos+=4;
                    d2o2->Init(Tables.Table[t1], o1, Tables.Table[t2], o2, Tables.Table[t3], o3, Tables.Table[t4], o4);
                    d2o2->Off();

                    //-Добавляем клапан для передачи серверу как устройство.     
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];
                    tmp->sub_dev[ valve_n++ ] = d2o2;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    d2o2->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( d2o2 ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dtN1DO1DI
                case 5: 
                    {
                    do1=new TDO1( 1 );
#ifdef USE_SIMPLE_DEV_ERRORS
                    do1->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                                                          
                    do1->HI=0;
                    g_devs.NDev[g_devs.NCnt++]=do1;
                    BlockRead(df,(unsigned char *)&w, 2);

                    g_devs.NDev[g_devs.NCnt-1]->nom=w;

                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=4;
                    do1->Init(Tables.Table[t1], o1, Tables.Table[t2], o2);                 
                    //-Добавляем клапан для передачи серверу как устройство.     
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_N ];          
                    tmp->sub_dev[ pump_n++ ] = do1;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    do1->set_type( TVDO::DT_PUMP );
                    g_dev_errors_manager->add_error( new simple_error( do1 ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dtM1DO1DI
                case 6:
                    {			
                    do1=new TDO1( 1 );
#ifdef USE_SIMPLE_DEV_ERRORS
                    do1->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    do1->HI=0;
                    g_devs.MDev[g_devs.MCnt++]=do1;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.MDev[g_devs.MCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=4;
                    do1->Init(Tables.Table[t1], o1, Tables.Table[t2], o2);

                    //-Добавляем клапан для передачи серверу как устройство.     
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_M ];          
                    tmp->sub_dev[ mesh_n++ ] = do1; 
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    do1->set_type( TVDO::DT_MIXER );
                    g_dev_errors_manager->add_error( new simple_error( do1 ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dtLS
                case 7: 
                    {
                    di=new TDI;
#ifdef USE_SIMPLE_DEV_ERRORS
                    di->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    g_devs.LSDev[g_devs.LSCnt++]=di;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.LSDev[g_devs.LSCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    di->Init(Tables.Table[t1], o1);
                    //-Добавляем клапан для передачи серверу как устройство.   
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_LS ];          
                    tmp->sub_dev[ LS_n++ ] = di;
                    //-!>

#if defined DEBUG && defined PRIEMKA
                    //Для Приемки оключаем уровни в отладочной версии.
                    SetDITable( Tables.Table[ t1 ], o1, 1 );
#endif // DEBUG          
                    break;
                    };

                //dtTE
                case 8:
                    {
                    te=new TTE;
#ifdef USE_SIMPLE_DEV_ERRORS
                    te->set_name( tmp_str );
                    te->set_type( TVDO::DT_T );
                    g_dev_errors_manager->add_error( new simple_error( te ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    g_devs.TEDev[g_devs.TECnt++]=te;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.TEDev[g_devs.TECnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    te->Init(Tables.Table[t1], o1);
                    //-Добавляем клапан для передачи серверу как устройство.
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_TE ];          
                    tmp->sub_dev[ TE_n++ ] = te;
                    //-!>

                    //-Добавляем "значение врмени" фильтра температуры для передачи серверу как устройство.                    
					tmp = ( complex_device* )( 
                        ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_TE_FILTER ];          
                    tmp->sub_dev[ TE_FILTER_n++ ] = new filter_param( te->get_n(), te->par );
                    //-!>
                    break;
                    };

                //dtFE
                case 9: 
                    {
                    ai=new TAI;
#ifdef USE_SIMPLE_DEV_ERRORS
                    ai->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    g_devs.FEDev[g_devs.FECnt++]=ai;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.FEDev[g_devs.FECnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    //read params
                    df->pos+=7;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    df->pos+=3;
                    BlockRead(df, (unsigned char *)&f2, 4);
                    ai->Init(Tables.Table[t1], o1);
                    Tables.Table[t1]->AIR[o1].rl=f1;
                    Tables.Table[t1]->AIR[o1].rh=f2;
                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_FE ];          
                    tmp->sub_dev[ FE_n++ ] = ai;
                    //-!>
                    break;
                    };

                    //dtFS
                case 10: 
                    {
                    di=new TDI;
#ifdef USE_SIMPLE_DEV_ERRORS
                    di->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    g_devs.FSDev[g_devs.FSCnt++]=di;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.FSDev[g_devs.FSCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    di->Init(Tables.Table[t1], o1);

                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_FS ];          
                    tmp->sub_dev[ FS_n++ ] = di;
                    //-!>
                    break;
                    };

                    //dtCTR
                case 11: 
                    {
                    ctr=new TCounter;
#ifdef USE_SIMPLE_DEV_ERRORS
                    ctr->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    g_devs.CTRDev[g_devs.CTRCnt++]=ctr;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.CTRDev[g_devs.CTRCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    ctr->Init(Tables.Table[t1], o1, 65535);

                    //-Добавляем счетчик для передачи серверу как устройство.
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_CTR ];          
                    tmp->sub_dev[ CTR_n++ ] = ctr;
                    //-!>

                    //-Добавляем состояние счетчика для передачи серверу как устройство.                    
                    tmp = ( complex_device* )( 
                        ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_CTR_STATE ];          
                    counter_state* counter_state_tmp = new counter_state( ctr );
                    tmp->sub_dev[ CTR_STATE_n++ ] = counter_state_tmp;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    ctr->set_type( TVDO::DT_COUNTER );
                    g_dev_errors_manager->add_error( new simple_error( ctr ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dtAO
                case 12: 
                    {
                    ao=new TAO;
#ifdef USE_SIMPLE_DEV_ERRORS
                    ao->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    g_devs.AODev[g_devs.AOCnt++]=ao;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.AODev[g_devs.AOCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    ao->Init(Tables.Table[t1], o1);
                    //-Добавляем клапан для передачи серверу как устройство.  
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_AO ];
                    tmp->sub_dev[ AO_n++ ] = ao;
                    //-!>
                    break;
                    };

                    //dtLE
                case 13:
                case 14:
                case 15: 
                    {
                    le=new TLE;
#ifdef USE_SIMPLE_DEV_ERRORS
                    le->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    g_devs.LEDev[g_devs.LECnt++]=le;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.LEDev[g_devs.LECnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    //read params
                    df->pos+=7;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    df->pos+=3;
                    BlockRead(df, (unsigned char *)&f2, 4);
                    df->pos+=3;
                    BlockRead(df, (unsigned char *)&f3, 4);
                    le->Init(dt, Tables.Table[t1], o1, f1, f2, f3);
                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_LE ];
                    tmp->sub_dev[ LE_n++ ] = le;          
                    //-!>
                    break;
                    };

                case 16: 
                    {
                    le=new TLE;
#ifdef USE_SIMPLE_DEV_ERRORS
                    le->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS                    

                    g_devs.LEDev[g_devs.LECnt++]=le;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.LEDev[g_devs.LECnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    //read params
                    df->pos+=7;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    df->pos+=3;
                    BlockRead(df, (unsigned char *)&f2, 4);
                    le->Init(dt, Tables.Table[t1], o1, f1, f2, 0);
                    //-Добавляем клапан для передачи серверу как устройство.  
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_LE ];
                    tmp->sub_dev[ LE_n++ ] = le;
                    //-!>
                    break;
                    };
					   
                    //dtMix
                case 17:
                    {
                    mix=new TMIX;
#ifdef USE_SIMPLE_DEV_ERRORS
                    mix->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    mix->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=mix;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;
                    df->pos+=2;

                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;

                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=5;

                    BlockRead(df, &b, 1);
                    t3=b;
                    BlockRead(df, &b, 1);
                    o3=b;
                    df->pos+=5;

                    BlockRead(df, &b, 1);
                    t4=b;
                    BlockRead(df, &b, 1);
                    o4=b;
                    df->pos+=5;

                    BlockRead(df, &b, 1);
                    t5=b;
                    BlockRead(df, &b, 1);
                    o5=b;
                    df->pos+=4;

                    mix->Init(Tables.Table[t1], o1, Tables.Table[t2], o2, Tables.Table[t3], o3, Tables.Table[t4], o4, Tables.Table[t5], o5);
                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];          
                    tmp->sub_dev[ valve_n++ ] = mix;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    mix->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( mix ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
                    };

                    //dtOS
                case 18: 
                    {
                    di=new TDI;
#ifdef USE_SIMPLE_DEV_ERRORS
                    di->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS                   

                    g_devs.FBDev[g_devs.FBCnt++]=di;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.FBDev[g_devs.FBCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    di->Init(Tables.Table[t1], o1);
                    //-Добавляем клапан для передачи серверу как устройство.  
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_FB ];
                    tmp->sub_dev[ FB_n++ ] = di;          
                    //-!>
                    break;
                    };

                    //dtUPR
                case 19: 
                    {
                    do0=new TDO0;
#ifdef USE_SIMPLE_DEV_ERRORS
                    do0->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    do0->HI=0;
                    g_devs.UPRDev[g_devs.UPRCnt++]=do0;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.UPRDev[g_devs.UPRCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    do0->Init(Tables.Table[t1], o1);
                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_UPR ];          
                    tmp->sub_dev[ UPR_n++ ] = do0;
                    //-!>
                    break;
                    };

                    //dtQE
                case 20: 
                    {
                    ai=new TAI;
#ifdef USE_SIMPLE_DEV_ERRORS
                    ai->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                                                            
                    g_devs.QEDev[g_devs.QECnt++]=ai;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.QEDev[g_devs.QECnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    //read params
                    df->pos+=7;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    df->pos+=3;
                    BlockRead(df, (unsigned char *)&f2, 4);
                    ai->Init(Tables.Table[t1], o1);
                    Tables.Table[t1]->AIR[o1].rl=f1;
                    Tables.Table[t1]->AIR[o1].rh=f2;
                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_QE ];
                    tmp->sub_dev[ QE_n++ ] = ai;
                    //-!>
                    break;
                    };  

                case dtV1DO3DI:
					{
                    do3=new TDO3;
#ifdef USE_SIMPLE_DEV_ERRORS
                    do3->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    do3->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=do3;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t3=b;
                    BlockRead(df, &b, 1);
                    o3=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t4=b;
                    BlockRead(df, &b, 1);
                    o4=b;          
                    do3->Init(Tables.Table[t1], o1, Tables.Table[t2], o2, 
                        Tables.Table[t3], o3, Tables.Table[t4], o4 );

                    df->pos += 7;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    do3->checkFB = f1;
                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];
                    tmp->sub_dev[ valve_n++ ] = do3;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    do3->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( do3 ) );
#endif // USE_SIMPLE_DEV_ERRORS
					}
                    break;

                case dtV1DO2DI_S:
					{
                    do2=new TDO2;
#ifdef USE_SIMPLE_DEV_ERRORS
                    do2->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS
                    
                    do2->HI=0;
                    g_devs.VDev[g_devs.VCnt++]=do2;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.VDev[g_devs.VCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t2=b;
                    BlockRead(df, &b, 1);
                    o2=b;
                    df->pos+=5;
                    BlockRead(df, &b, 1);
                    t3=b;
                    BlockRead(df, &b, 1);
                    o3=b;                 
                    do2->Init(Tables.Table[t1], o1, Tables.Table[t2], o2, 
                        Tables.Table[t3], o3 );

                    df->pos += 7;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    do2->checkFB = f1;

                    do2->isSameFB = 1;

                    //-Добавляем клапан для передачи серверу как устройство. 
                    complex_device* tmp = ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];
                    tmp->sub_dev[ valve_n++ ] = do2;
                    //-!>

#ifdef USE_SIMPLE_DEV_ERRORS
                    do2->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( new simple_error( do2 ) );
#endif // USE_SIMPLE_DEV_ERRORS
					}
                    break;

                case dtASMix:
					{
#if defined ASI || defined ASIWAGO
                    BYTE gateway_addr;
                    BYTE dev_addr;
                    unsigned int nom;
#endif // ASI
                    BlockRead( df,( unsigned char* )&w, 2 );
#if defined ASI || defined ASIWAGO
                    nom = w;
#endif // ASI
                    df->pos += 4;                               //Пропускаем ненужную информацию.
                    BlockRead( df, ( unsigned char* )&f1, 4 );
#if defined ASI || defined ASIWAGO
                    dev_addr = f1;
#endif // ASI
                    df->pos += 3;                           //Пропускаем ненужную информацию.
                    BlockRead( df, ( unsigned char* )&f1, 4 );
#if defined ASI || defined ASIWAGO
                    gateway_addr = f1;
                    g_devs.VDev[ g_devs.VCnt ] = new TAsiValve( gateway_addr, dev_addr );
#ifdef USE_SIMPLE_DEV_ERRORS
                    g_devs.VDev[ g_devs.VCnt ]->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS

                    g_devs.VDev[ g_devs.VCnt++ ]->nom = nom;
                    complex_device* tmp = ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_V ];
                    tmp->sub_dev[ valve_n++ ] = g_devs.VDev[ g_devs.VCnt - 1];                     
#endif // ASI

#ifdef USE_SIMPLE_DEV_ERRORS
                    g_devs.VDev[ g_devs.VCnt - 1 ]->set_type( TVDO::DT_VALVE );
                    g_dev_errors_manager->add_error( 
                        new simple_error( g_devs.VDev[ g_devs.VCnt - 1 ] ) );
#endif // USE_SIMPLE_DEV_ERRORS
                    break;
					}
                   
					//dtAI
                case dtAI: 
                    {
                    ai=new TAI;
#ifdef USE_SIMPLE_DEV_ERRORS
                    ai->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS                    

                    g_devs.AIDev[g_devs.AICnt++]=ai;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.AIDev[g_devs.AICnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    ai->Init(Tables.Table[t1], o1);
					
                    //-Добавляем для передачи серверу как устройство. 
                    complex_device* tmp = ( complex_device* )( 
                        ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_AI ];
                    tmp->sub_dev[ AI_n++ ] = ai;
                    };
					break;

                //dtLS_ex
                case dtLS_ex: 
                    {
                    di=new TDI;
#ifdef USE_SIMPLE_DEV_ERRORS
                    di->set_name( tmp_str );
#endif // USE_SIMPLE_DEV_ERRORS

                    g_devs.LSDev[g_devs.LSCnt++]=di;
                    BlockRead(df,(unsigned char *)&w, 2);
                    g_devs.LSDev[g_devs.LSCnt-1]->nom=w;
                    df->pos+=2;
                    BlockRead(df, &b, 1);
                    t1=b;
                    BlockRead(df, &b, 1);
                    o1=b;
                    df->pos+=4;
                    di->Init(Tables.Table[t1], o1);

                    //-Добавляем для передачи серверу как устройство.   
                    complex_device* tmp = 
                        ( complex_device* )( ( complex_device* )g_dev_cmmctr->dev[ 0 ] )->sub_dev[ VD_LS ];          
                    tmp->sub_dev[ LS_n++ ] = di;
                    //-!>
                            
                    df->pos += 3;
                    BlockRead(df, (unsigned char *)&f1, 4);
                    di->active_state = f1;                  
                    break;
                    };

                default:     
                    BlockRead(df,(unsigned char *)&w, 2);
					if (DEB) Print("\n\rUnknown device %u (type = %d) \n", w, dt );
					//if (DEB) Print("\n\rUnknown device %d\n", dt );
					//df->pos+=2;

					BlockRead(df, &b, 1);
                    df->pos+=b*7;
                    break;       
                }

//if ( DEB ) Print(" --- %d no[%u] (  )  %lu\n", i, w, df->pos );
            }

//if ( DEB ) Print("================ df->pos = %lu \n", df->pos );

        //read paths

        //fclose7188(df);
        //return 0;

        //read paths   

        if (df->pos >= df->fd->size ) 
            {
            fclose7188(df);
            return 0;
            }; 

        //	Считывание устройств с AS интерфейсом   
        //Read header
        //port        
        BlockRead( df, &b, 1 );
        //int AS_port = b;        
        //node count
        BlockRead( df, &b, 1 );
        int gates_cnt = b;

#ifdef DEBUG
        Print( "Reading AS gateways info, gateways - %i, ", gates_cnt );
#endif

#ifdef ASI
        BYTE* gates_info = new BYTE[ gates_cnt ];
#endif // ASI

        for ( i = 0; i < gates_cnt; i++ )
            {
            BlockRead( df, &b, 1 );

			Print( "gates_info %d = %d \n", i, b );

#ifdef ASI
            gates_info[ i ] = b;
#endif // ASI
            }        

#ifdef ASI
        ASigateways = new TASigateways( gates_cnt, gates_info );
        delete gates_info;
#endif // ASI

#ifdef ASIWAGO
        if (TASigateways::gatecnt>0)
            {
            ASigateways = new TASigateways();
#ifdef DEBUG 
			Print("\n\rUsing total %u wago ASi gateways",TASigateways::gatecnt);
#endif
            }
#endif // ASIWAGO

#ifdef DEBUG
        Print( "...reading Ok!\n" );
        Print( "\n Mem[1] free: %lu bytes. ", ( unsigned long ) coreleft() );
#ifdef KEY_CONFIRM
        Print( "Press any key to continue..." );
        Getch();
        Print( "\n" );
#endif // KEY_CONFIRM
#endif    

        //Read paths
        if (df->pos >= df->fd->size ) 
            {
            fclose7188(df);
            return 0;
            };    

#ifndef MSA
        ////////////////////////////////////////////////////////////////////////////////////////////////
        //=====	Читаем данные по гребенкам	=====================================
        TParams::InitParams( G_PROJECT_ID );

        BlockRead(df, &b, 1);	//	Comb count
        TMyComb::CombCnt = b;   

#ifdef DEBUG
		Print( "Total comb count %d\n", b );
#endif // DEBUG

        g_combs = new TMyComb* [ TMyComb::CombCnt ];

        for ( m = 0; m < TMyComb::CombCnt; m++ )
            { 

			if ( strcmp( id, "WG {4B714C08-9602-4130-8563-4B51E08BB9D8}" ) == 0 )
				{
				//procedure TWagoDrv::Read_obj_data( file7188* df, int m );
				BlockRead(df, &b1, 1);	    //	Mode SubType
				//Print( "Mode SubType = %d \n", b1 );

				BlockRead(df, &b, 1);		//	Mode count 
				l = b;
				Comb1->statesCnt = l;
				//Print( "Mode count = %i \n", l );

				int comb_n;
				BlockRead( df, ( u_char* ) &comb_n, 2 );	//	Comb number
				//Print( "Tank number = %i \n", comb_n );

				int comb_par_cnt;
				BlockRead( df, ( u_char* ) &comb_par_cnt, 2 );	
				//Print( "comb_par_cnt = %i \n", comb_par_cnt );

				int comb_work_par_cnt;
				BlockRead( df, ( u_char* ) &comb_work_par_cnt, 2 );
				//Print( "comb_work_par_cnt = %i \n", comb_work_par_cnt );

				int comb_tmr_cnt;
				BlockRead( df, ( u_char* ) &comb_tmr_cnt, 2 );
				//Print( "comb_tmr_cnt = %i \n", comb_tmr_cnt );


				Comb1 = init_comb( b1, comb_n, b, comb_par_cnt,
					comb_work_par_cnt, comb_tmr_cnt );

				//	Счииываем значения параметров объекта
				for ( i = 0; i < comb_par_cnt; i++ )
					{
					BlockRead( df, (unsigned char *)&w, 2 );
					Comb1->par->setParam( i, w );
					}
				Comb1->par->Save();
				
				Comb1->paths = new TPathRec[ l ];  

				for ( i = 0; i < l; i++ ) 
					{
					//	Для включения режима
					Comb1->paths[ i ].FB_need_Cnt		= 0;
					Comb1->paths[ i ].FB_on_mode_Cnt		= 0;
					
					//	Для работы режима
					Comb1->paths[ i ].OVCnt				= 0;
					Comb1->paths[ i ].CVCnt				= 0;
					Comb1->paths[ i ].USCnt				= 0;
					Comb1->paths[ i ].LSCnt				= 0;

					Comb1->paths[ i ].UPRCnt				= 0;
					Comb1->paths[ i ].FB_control_Cnt		= 0;
					Comb1->paths[ i ].UPR_control_Cnt	= 0;

					Comb1->paths[ i ].Key_signal_Cnt		= 0;
					Comb1->paths[ i ].Wait_modes_Cnt		= 0;

					//	Для выключения режима
					Comb1->paths[ i ].Dev_off_Cnt		= 0;
					Comb1->paths[ i ].FVCnt				= 0;

					//	Дополнительные параметры режима
					Comb1->paths[ i ].work_time_par		= 0;
					Comb1->paths[ i ].next_mode			= 0;

					Comb1->paths[ i ].Block_cm_cnt		= 0;
					Comb1->paths[ i ].Block_tm_cnt		= 0;
					Comb1->paths[ i ].Block_dev_Cnt		= 0;

					Comb1->paths[ i ].On_mode_Cnt		= 0;
					Comb1->paths[ i ].Off_mode_Cnt		= 0;	
					}

				for ( i = 0; i < l; i++ )
					{
#ifdef DEBUG
					Print( "MODE %d :::::\n", i );
#endif
					//--------------------------------------------------------------
 					Read_list_dev( df, i, &Comb1->paths[ i ].FB_need, Comb1->paths[ i ].FB_need_Cnt );
 					Read_list_dev( df, i, &Comb1->paths[ i ].FB_on_mode, Comb1->paths[ i ].FB_on_mode_Cnt );
			
					//--------------------------------------------------------------
					Read_list_dev( df, i, &Comb1->paths[ i ].OV, Comb1->paths[ i ].OVCnt );
					Read_list_dev( df, i, &Comb1->paths[ i ].CV, Comb1->paths[ i ].CVCnt );

#ifdef DEBUG
					Print( "MODE[ %d ] - Open Device: ", i );
					for ( j = 0; j < Comb1->paths[ i ].OVCnt; j++ )
						{	
						Print( " %d, ", Comb1->paths[ i ].OV[ j ].no );
						}

					Print( "\nMODE[ %d ] - Close Device: ", i );
					for ( j = 0; j < Comb1->paths[ i ].CVCnt; j++ )
						{	
						Print( " %d, ", Comb1->paths[ i ].CV[ j ].no );
						}
#endif

					Read_list_dev( df, i, &Comb1->paths[ i ].US, Comb1->paths[ i ].USCnt );
					Read_list_dev( df, i, &Comb1->paths[ i ].LS, Comb1->paths[ i ].LSCnt );
					Read_list_dev( df, i, &Comb1->paths[ i ].UPR, Comb1->paths[ i ].UPRCnt );
					Read_dual_list( df, i, 
						&Comb1->paths[ i ].FB_control, &Comb1->paths[ i ].FB_control_dev, 
						Comb1->paths[ i ].FB_control_Cnt	 );
					Read_dual_list( df, i, 
						&Comb1->paths[ i ].UPR_control_dev, &Comb1->paths[ i ].UPR_control, 
						Comb1->paths[ i ].UPR_control_Cnt );
					Read_list_dev( df, i, &Comb1->paths[ i ].Key_signal, Comb1->paths[ i ].Key_signal_Cnt );
					Read_list_mode( df, i, &Comb1->paths[ i ].Wait_modes, Comb1->paths[ i ].Wait_modes_Cnt );

					//--------------------------------------------------------------
					Read_list_dev( df, i, &Comb1->paths[ i ].Dev_off, Comb1->paths[ i ].Dev_off_Cnt );
					Read_list_dev( df, i, &Comb1->paths[ i ].FV, Comb1->paths[ i ].FVCnt );

					//--------------------------------------------------------------
					Read_Value( df, Comb1->paths[ i ].work_time_par );
					Read_Value( df, Comb1->paths[ i ].next_mode );
				  
					Read_list_dev( df, i, &Comb1->paths[ i ].Block_dev, Comb1->paths[ i ].Block_dev_Cnt );
					Read_list_mode( df, i, &Comb1->paths[ i ].Block_cm, Comb1->paths[ i ].Block_cm_cnt );
				    Read_list_mode( df, i, &Comb1->paths[ i ].Block_tm, Comb1->paths[ i ].Block_tm_cnt );
					Read_list_mode( df, i, &Comb1->paths[ i ].On_mode, Comb1->paths[ i ].On_mode_Cnt );
					Read_list_mode( df, i, &Comb1->paths[ i ].Off_mode, Comb1->paths[ i ].Off_mode_Cnt );

					//	Задание количества переменных (для исключения реакции на дребезг контактов)
					Comb1->paths[ i ].time_for_off_UPR = new ulong[ Comb1->paths[ i ].UPR_control_Cnt ];
					Comb1->paths[ i ].time_for_on_mode = new ulong[ Comb1->paths[ i ].Dev_off_Cnt ];
					Comb1->paths[ i ].time_for_off_mode = new ulong[ Comb1->paths[ i ].FB_on_mode_Cnt ];

#ifdef DEBUG
					Print( "-------------------------------------------\n" );
#endif
					//	Данные по маршрутам (отсутствуют)
					Comb1->paths[ i ].in_x = 0;
					Comb1->paths[ i ].in_y = 0;
					Comb1->paths[ i ].out_x = 0;
					Comb1->paths[ i ].out_y = 0; 
					}
				}
			else
				{
				//	if ( strcmp( id, "WG {4B714C08-9602-4130-8563-4B51E08BB9D7}" ) == 0 )
/*
				BlockRead(df, &b1, 1);	    //	Mode SubType
				//Print( "Mode SubType = %d \n", b1 );

				BlockRead(df, &b, 1);		//	Mode count / 32 
				l = b*32;
				//Print( "Mode count = %i \n", l );

				int comb_n;
				BlockRead( df, ( u_char* ) &comb_n, 2 );	//	Tank number
				//Print( "Tank number = %i \n", comb_n );

				int comb_par_cnt;
				BlockRead( df, ( u_char* ) &comb_par_cnt, 2 );	
				//Print( "comb_par_cnt = %i \n", comb_par_cnt );

				int comb_work_par_cnt;
				BlockRead( df, ( u_char* ) &comb_work_par_cnt, 2 );
				//Print( "comb_work_par_cnt = %i \n", comb_work_par_cnt );

				int comb_tmr_cnt;
				BlockRead( df, ( u_char* ) &comb_tmr_cnt, 2 );
				//Print( "comb_tmr_cnt = %i \n", comb_tmr_cnt );


				Comb1 = init_comb( b1, comb_n, b, comb_par_cnt,
					comb_work_par_cnt, comb_tmr_cnt );

				Comb1->paths = new TPathRec[ l ];  

				for ( i = 0; i < l; i++ ) 
					{
					Comb1->paths[ i ].OVCnt=0;
					Comb1->paths[ i ].CVCnt=0;
					Comb1->paths[ i ].FVCnt=0;
					Comb1->paths[ i ].USCnt=0;
					Comb1->paths[ i ].LSCnt=0;
					Comb1->paths[ i ].FBCnt=0;

					Comb1->paths[ i ].washN = 0;
					Comb1->paths[ i ].washV = 0; 
					Comb1->paths[ i ].washFB = 0;

					Comb1->paths[ i ].washUPRCnt = 0;

					Comb1->paths[ i ].in_FB_cnt = 0;
					Comb1->paths[ i ].in_FB_val = 0;
					Comb1->paths[ i ].out_UPR_cnt = 0;
					Comb1->paths[ i ].out_UPR_val = 0;
					}

				for ( i = 0; i < l; i++ )
					{
					//skip description
					//BlockRead(df, &b, 1);      
					//df->pos += b;
					//ov length             
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].OVCnt=b;
#ifdef DEBUG
					if ( Comb1->paths[ i ].OVCnt ) Print( "[%3d] O- ", i );	
#endif	

					//alloc memory
					Comb1->paths[ i ].OV = (int *)malloc(Comb1->paths[ i ].OVCnt*sizeof(int));
					if (Comb1->paths[ i ].OV == NULL && Comb1->paths[ i ].OVCnt > 0) 
						{
						__ErrDscr(EMEM); 
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}

					for ( j = 0; j < Comb1->paths[ i ].OVCnt; j++ ) 
						{
						BlockRead(df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].OV[ j ] = w;
#ifdef DEBUG
						Print( "%u;\t", w );	
#endif
						};
#ifdef DEBUG
					if ( j ) Print( "\n" );	
#endif              
					//cv length
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].CVCnt=b;
					//alloc memory
					Comb1->paths[ i ].CV = (int *)malloc(Comb1->paths[ i ].CVCnt*sizeof(int));
					if (Comb1->paths[ i ].CV == NULL && Comb1->paths[ i ].CVCnt>0) 
						{
						__ErrDscr(EMEM);
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}

#ifdef DEBUG
					if ( 0 == Comb1->paths[ i ].OVCnt && Comb1->paths[ i ].CVCnt ) Print( "[%3d] C- ", i );
					if ( Comb1->paths[ i ].OVCnt && Comb1->paths[ i ].CVCnt ) Print( "      C- ", i );

#endif	
					for ( j = 0; j < Comb1->paths[ i ].CVCnt; j++ )
						{
						BlockRead(df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].CV[j]=w;
#ifdef DEBUG
						Print( "%u;\t", w );	
#endif
						};
#ifdef DEBUG
					if ( j ) Print( "\n" );	
#endif
					//fv length
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].FVCnt=b;
					//alloc mem
					Comb1->paths[ i ].FV = (int *)malloc(Comb1->paths[ i ].FVCnt*sizeof(int));
					if (Comb1->paths[ i ].FV == NULL && Comb1->paths[ i ].FVCnt>0) 
						{
						__ErrDscr(EMEM); 
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}

					for (j=0; j<Comb1->paths[ i ].FVCnt; j++) 
						{
						BlockRead(df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].FV[j]=w;
						};

					//us length
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].USCnt=b;
					//alloc mem
					Comb1->paths[ i ].US=(int *)malloc(Comb1->paths[ i ].USCnt*sizeof(int));
					if (Comb1->paths[ i ].US==NULL && Comb1->paths[ i ].USCnt>0) 
						{
						__ErrDscr(EMEM); 
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}

					for (j=0; j<Comb1->paths[ i ].USCnt; j++) 
						{
						BlockRead(df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].US[j]=w;
						};

					//ls length
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].LSCnt=b;
					//alloc mem
					Comb1->paths[ i ].LS=(int *)malloc(Comb1->paths[ i ].LSCnt*sizeof(int));
					if (Comb1->paths[ i ].LS==NULL && Comb1->paths[ i ].LSCnt>0)
						{
						__ErrDscr(EMEM); 
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}

					for (j=0; j<Comb1->paths[ i ].LSCnt; j++) 
						{
						BlockRead(df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].LS[j]=w;
						};

					//fb length
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].FBCnt=b;      
					//alloc mem
					Comb1->paths[ i ].FB=(int *)malloc(Comb1->paths[ i ].FBCnt*sizeof(int));
					if (Comb1->paths[ i ].FB==NULL && Comb1->paths[ i ].FBCnt>0)
						{
						__ErrDscr(EMEM); 
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}				

					for (j=0; j<Comb1->paths[ i ].FBCnt; j++)
						{
						BlockRead(df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].FB[j]=w;        
						}      

					// N length
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].washNCnt = b;

					//alloc mem
					Comb1->paths[ i ].washN =(int *)malloc( Comb1->paths[ i ].washNCnt * sizeof(int) );
					if (Comb1->paths[ i ].washN == NULL && Comb1->paths[ i ].washNCnt > 0)
						{
						__ErrDscr(EMEM); 
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}

					for ( j = 0; j < Comb1->paths[ i ].washNCnt; j++) 
						{
						BlockRead( df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].washN[ j ] = w;

#ifdef DEBUG
						Print( "Path[ %d ].N[ %d ] : %u \n", i, j, w );
#endif        
						}

					// V length
					BlockRead(df, &b, 1);
					Comb1->paths[ i ].washVCnt = b;

					//alloc mem
					Comb1->paths[ i ].washV =(int *)malloc( Comb1->paths[ i ].washVCnt * sizeof(int) );
					if (Comb1->paths[ i ].washV == NULL && Comb1->paths[ i ].washVCnt > 0)
						{
						__ErrDscr(EMEM); 
#pragma warn -eff
						while (1) ;
#pragma warn .eff
						}				

					for ( j = 0; j < Comb1->paths[ i ].washVCnt; j++) 
						{
						BlockRead( df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].washV[ j ] = w;

#ifdef DEBUG
						Print( "Path[ %d ].V[ %d ] : %u \n", i, j, w );
#endif        
						}

					BlockRead(df, (unsigned char *)&w, 2);
					Comb1->paths[ i ].washFB = w;  

					BlockRead(df, (unsigned char *)&b, 1);
					Comb1->paths[ i ].washUPRCnt = b;  
					Comb1->paths[ i ].washUPR = new int[ b ];
					for ( j = 0; j < Comb1->paths[ i ].washUPRCnt; j++) 
						{
						BlockRead( df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].washUPR[ j ] = w;

#ifdef DEBUG
						Print( "Path[ %d ].UPR mode OK [ %d ] : %u \n", i, j, w );
#endif        
						}

					BlockRead(df, (unsigned char *)&b, 1);

					Comb1->paths[ i ].in_FB_cnt = b;  
					Comb1->paths[ i ].in_FB_val = new int[ b ];
					for ( j = 0; j < Comb1->paths[ i ].in_FB_cnt; j++) 
						{
						BlockRead( df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].in_FB_val[ j ] = w;
#ifdef DEBUG
						Print( "Path[ %d ].in FB [ %d ] : %u \n", i, j, w );
#endif        
						}

					BlockRead(df, (unsigned char *)&b, 1);
					Comb1->paths[ i ].out_UPR_cnt = b;  
					Comb1->paths[ i ].out_UPR_val = new int[ b ];
					for ( j = 0; j < Comb1->paths[ i ].out_UPR_cnt; j++) 
						{
						BlockRead( df, (unsigned char *)&w, 2);
						Comb1->paths[ i ].out_UPR_val[ j ] = w;
#ifdef DEBUG
						Print( "Path[ %d ].out UPR [ %d ] : %u \n", i, j, w );
#endif        
						}

					// Cчитывание параметров режима, связанных с маршрутом гребенки.
					BlockRead(df, &b1, 1);
					if ( b1 == 1 )
						{
						BlockRead(df, &b, 1);
						Comb1->paths[ i ].in_x = b;
						BlockRead(df, &b, 1);
						Comb1->paths[ i ].in_y = b;
						BlockRead(df, &b, 1);
						Comb1->paths[ i ].out_x = b;
						BlockRead(df, &b, 1);
						Comb1->paths[ i ].out_y = b; 
						}
					else
						{
						Comb1->paths[ i ].in_x = 0;
						Comb1->paths[ i ].in_y = 0;
						Comb1->paths[ i ].out_x = 0;
						Comb1->paths[ i ].out_y = 0; 
						}	
#ifdef DEBUG
					//  Выводим маршрут режима 
					Print( "%d ::: in: %d - %d   out: %d - %d \n", i, Comb1->paths[ i ].in_x, Comb1->paths[ i ].in_y,
						Comb1->paths[ i ].out_x, Comb1->paths[ i ].out_y );														
#endif		
					}
*/
				}

			g_combs[m] = Comb1;

#ifdef USE_SIMPLE_DEV_ERRORS
            g_dev_errors_manager->add_error( new tech_dev_error( Comb1 ) );
#endif // USE_SIMPLE_DEV_ERRORS

#ifdef DEBUG
            Print("...reading OK.\n");
            Print( "\n 2_Mem free: %lu bytes. ", ( unsigned long ) coreleft() );
#ifdef KEY_CONFIRM
            Print( "Press any key to continue..."  );
            Getch();
            Print( "\n"  );
#endif // KEY_CONFIRM                   
#endif // DEBUG 
            }	//	for ( m = 0; m < TMyComb::CombCnt; m++ )

        if (df->pos >= df->fd->size ) 
            {
            fclose7188(df);
            return 0;
            };																  

        //=====	Читаем данные по танкам	=====================================														  													
        //Tank paths	
        BlockRead( df, &b, 1 );	//	Tank count

#ifdef DEBUG
        Print( "Total tank count %d\n", b );

		//devpos = df->pos;
		//for ( j = 0; j < 50; j++ )
		//	{
		//	BlockRead( df, &b, 1 );
		//	Print("%d ", b );
		//	}
		//Getch();
		//df->pos=devpos;

#endif // DEBUG

        TTank::TankCnt = b;                 

        g_tanks = new TTank* [ TTank::TankCnt ];

        dev_elem open_device[ 256 ]; 
        dev_elem close_device[ 256 ];
        for ( m = 0; m < TTank::TankCnt; m++ )
			{	
			if ( strcmp( id, "WG {4B714C08-9602-4130-8563-4B51E08BB9D8}" ) == 0 )
				{				

				char tank_sub_type;
				BlockRead(df, &tank_sub_type, 1);	//	Tank SubType                
				char tank_comb_type;
				BlockRead(df, &tank_comb_type, 1);	//	Comb type for Tank                
				char tank_mode_cnt;
				BlockRead( df, &tank_mode_cnt, 1 );	//	Mode count                
#ifdef DEBUG
					Print( "Tank mode count = %d \n", tank_mode_cnt  );
#endif 

				int tank_n;
				BlockRead( df, ( u_char* ) &tank_n, 2 );	//	Tank number
				int tank_par_cnt;
				BlockRead( df, ( u_char* ) &tank_par_cnt, 2 );	
				int tank_work_par_cnt;
				BlockRead( df, ( u_char* ) &tank_work_par_cnt, 2 );
				int tank_tmr_cnt;
				BlockRead( df, ( u_char* ) &tank_tmr_cnt, 2 );

#ifdef DEBUG
				Print( "Before init tank\n" );
				Print( " %d %d %d %d %d\n", tank_sub_type, tank_n, tank_par_cnt,
					tank_work_par_cnt, tank_tmr_cnt );
				//Getch();
#endif
				Tank1 = init_tank( tank_sub_type, tank_n, tank_par_cnt,
					tank_work_par_cnt, tank_tmr_cnt );
				Tank1->comb_n = tank_comb_type;
				Tank1->PodTip = tank_sub_type;

				Tank1->mode_cnt = tank_mode_cnt;

#ifdef DEBUG
				Print("[%d] tank N[%d], type[%d]\n", m, Tank1->no, tank_sub_type );
				Print( "Memory free: %lu bytes.\n", ( unsigned long ) coreleft() );
#endif

				//	Счииываем значения параметров объекта
				for ( i = 0; i < tank_par_cnt; i++ )
					{
					BlockRead( df, (unsigned char *)&w, 2 );
					Tank1->par->setParam( i, w );
					}
				Tank1->par->Save();

				Tank1->paths = new TPathRec[ tank_mode_cnt ];  

				for ( i = 0; i < tank_mode_cnt; i++ ) 
					{
					//	Для включения режима
					Tank1->paths[ i ].FB_need_Cnt		= 0;
					Tank1->paths[ i ].FB_on_mode_Cnt		= 0;

					//	Для работы режима
					Tank1->paths[ i ].OVCnt				= 0;
					Tank1->paths[ i ].CVCnt				= 0;
					Tank1->paths[ i ].USCnt				= 0;
					Tank1->paths[ i ].LSCnt				= 0;

					Tank1->paths[ i ].UPRCnt				= 0;
					Tank1->paths[ i ].FB_control_Cnt		= 0;
					Tank1->paths[ i ].UPR_control_Cnt	= 0;

					Tank1->paths[ i ].Key_signal_Cnt		= 0;

					//	Для выключения режима
					Tank1->paths[ i ].Dev_off_Cnt		= 0;
					Tank1->paths[ i ].FVCnt				= 0;

					//	Дополнительные параметры режима
					Tank1->paths[ i ].work_time_par		= 0;
					Tank1->paths[ i ].next_mode			= 0;

					Tank1->paths[ i ].Block_cm_cnt		= 0;
					Tank1->paths[ i ].Block_tm_cnt		= 0;
					Tank1->paths[ i ].Block_dev_Cnt		= 0;

					Tank1->paths[ i ].On_mode_Cnt		= 0;
					Tank1->paths[ i ].Off_mode_Cnt		= 0;	
					}
				
				int step_count;
				for ( i = 0; i < tank_mode_cnt; i++ ) 
					{ 
					//	Описание характеристик режима
#ifdef DEBUG
					Print( "MODE %d :::::\n", i );
#endif
					//--------------------------------------------------------------
					Read_list_dev( df, i, &Tank1->paths[ i ].FB_need, Tank1->paths[ i ].FB_need_Cnt );
					Read_list_dev( df, i, &Tank1->paths[ i ].FB_on_mode, Tank1->paths[ i ].FB_on_mode_Cnt );

					//--------------------------------------------------------------
					Read_list_dev( df, i, &Tank1->paths[ i ].OV, Tank1->paths[ i ].OVCnt );
					Read_list_dev( df, i, &Tank1->paths[ i ].CV, Tank1->paths[ i ].CVCnt );
					Read_list_dev( df, i, &Tank1->paths[ i ].US, Tank1->paths[ i ].USCnt );
					Read_list_dev( df, i, &Tank1->paths[ i ].LS, Tank1->paths[ i ].LSCnt );
					Read_list_dev( df, i, &Tank1->paths[ i ].UPR, Tank1->paths[ i ].UPRCnt );
					Read_dual_list( df, i, 
						&Tank1->paths[ i ].FB_control, &Tank1->paths[ i ].FB_control_dev, 
						Tank1->paths[ i ].FB_control_Cnt	 );
					Read_dual_list( df, i, 
						&Tank1->paths[ i ].UPR_control_dev, &Tank1->paths[ i ].UPR_control, 
						Tank1->paths[ i ].UPR_control_Cnt	 );
					Read_list_dev( df, i, &Tank1->paths[ i ].Key_signal, Tank1->paths[ i ].Key_signal_Cnt );
					Read_list_mode( df, i, &Tank1->paths[ i ].Wait_modes, Tank1->paths[ i ].Wait_modes_Cnt );

					//--------------------------------------------------------------
					Read_list_dev( df, i, &Tank1->paths[ i ].Dev_off, Tank1->paths[ i ].Dev_off_Cnt );
					Read_list_dev( df, i, &Tank1->paths[ i ].FV, Tank1->paths[ i ].FVCnt );

					//--------------------------------------------------------------
					Read_Value( df, Tank1->paths[ i ].work_time_par );
					Read_Value( df, Tank1->paths[ i ].next_mode );

					Read_list_dev( df, i, &Tank1->paths[ i ].Block_dev, Tank1->paths[ i ].Block_dev_Cnt );
					Read_list_mode( df, i, &Tank1->paths[ i ].Block_cm, Tank1->paths[ i ].Block_cm_cnt );
					Read_list_mode( df, i, &Tank1->paths[ i ].Block_tm, Tank1->paths[ i ].Block_tm_cnt );
					Read_list_mode( df, i, &Tank1->paths[ i ].On_mode, Tank1->paths[ i ].On_mode_Cnt );
					Read_list_mode( df, i, &Tank1->paths[ i ].Off_mode, Tank1->paths[ i ].Off_mode_Cnt );

					//	Задание количества переменных (для исключения реакции на дребезг контактов)
					Tank1->paths[ i ].time_for_off_UPR = new ulong[ Tank1->paths[ i ].UPR_control_Cnt ];
					Tank1->paths[ i ].time_for_on_mode = new ulong[ Tank1->paths[ i ].Dev_off_Cnt ];
					Tank1->paths[ i ].time_for_off_mode = new ulong[ Tank1->paths[ i ].FB_on_mode_Cnt ];

#ifdef DEBUG
					Print( "-------------------------------------------\n" );
#endif					
					  
					BlockRead( df, &b, 1 );	//	Step count
					step_count = b;

#ifdef DEBUG
					Print( "step_count = %d \n", step_count );
#endif	

#if !defined NO_TANKS_MODE
					if ( step_count ) Tank1->modes_manager->set_mode_config( i, step_count );
#endif //NO_TANKS_MODE                

					for ( int k = 0; k < step_count; k++ )
						{
						//	Описание характеристик шага	
							  
						BlockRead( df, &b, 1 );
						u_char open_dev_cnt = b;

						for ( j = 0; j < open_dev_cnt; j++ ) 
							{
							BlockRead( df, &b, 1 );	//	dev type
							open_device[ j ].type = ( devType ) b; 

							BlockRead( df, ( u_char* )&w, 2 );
							open_device[ j ].no = w; 
							}

						BlockRead( df, &b, 1 );
						u_char close_dev_cnt = b;

						for ( j = 0; j < close_dev_cnt; j++ ) 
							{
							BlockRead( df, &b, 1 );	//	dev type
							open_device[ j ].type = ( devType ) b; 
							
							BlockRead( df, ( u_char* )&w, 2 );
							close_device[ j ].no = w; 
							}

						unsigned char	x1, y1, x2, y2; 
						uint temp, temp1;

						Read_Value( df, temp );		//	Время режима (номер параметра)
						Read_Value( df, temp1 );	//	Номер следующего шага

						b = ( byte ) temp;
						b1 = ( byte ) temp1;
											 
						// Cчитывание параметров шага, связанных с маршрутом гребенки.
						x1 = 0;
						y1 = 0;
						x2 = 0;
						y2 = 0;
              		   
#if !defined NO_TANKS_MODE
						Tank1->modes_manager->set_step_config( 
							i,                          //Режим.
							k,                          //Шаг.
							close_dev_cnt,              //Количество закрываемых устройств.
							open_dev_cnt,               //Количество открываемых устройств.
							b1,                         //Следующий шаг при завершении времени текущего шага.
							b,                          //Номер параметра, содержащий время шага, мин.
							x1,                         //Координаты входа
							y1,                         //
							x2,                         //Координаты выхода
							y2                          //
							);

						for ( j = 0; j < open_dev_cnt; j++ ) 
							{
							Tank1->modes_manager->add_opened_dev( i, k, (TVDO*) DEV( open_device[ j ] ) );
							}   

						for ( j = 0; j < close_dev_cnt; j++ ) 
							{
							Tank1->modes_manager->add_closed_dev( i, k, (TVDO*) DEV( close_device[ j ] ) );
							}  
#endif //NO_TANKS_MODE                

						}   //  Проходим по шагам
// 					Print("mode -----\n");
// 					Getch();
					}   //  Проходим по режимам
				g_tanks[ m ] = Tank1;     
				   
// #ifdef DEBUG
// 				Print( "===================== TANK %d \n", m );
// 				Getch();
// #endif // DEBUG
					   

#ifdef USE_SIMPLE_DEV_ERRORS
				g_dev_errors_manager->add_error( new tech_dev_error( Tank1 ) );
#endif // USE_SIMPLE_DEV_ERRORS

				}
			else
				{
				//	Для "WG {4B714C08-9602-4130-8563-4B51E08BB9D7}" 
/*
				char tank_sub_type;
				BlockRead(df, &tank_sub_type, 1);	//	Tank SubType                
				char tank_comb_type;
				BlockRead(df, &tank_comb_type, 1);	//	Comb type for Tank                
				char tank_mod_cnt;
				BlockRead( df, &tank_mod_cnt, 1 );	//	Mode count                
				if ( tank_mod_cnt < 32 ) 
					{
					tank_mod_cnt = tank_mod_cnt * 32; 
#ifdef DEBUG
					Print( "Tank mode count = %d \n", tank_mod_cnt  );
#endif 
					}                

				int tank_n;
				BlockRead( df, ( u_char* ) &tank_n, 2 );	//	Tank number
				int tank_par_cnt;
				BlockRead( df, ( u_char* ) &tank_par_cnt, 2 );	
				int tank_work_par_cnt;
				BlockRead( df, ( u_char* ) &tank_work_par_cnt, 2 );
				int tank_tmr_cnt;
				BlockRead( df, ( u_char* ) &tank_tmr_cnt, 2 );

#ifdef DEBUG
				Print( "Before init tank\n" );
				Print( " %d %d %d %d %d\n", tank_sub_type, tank_n, tank_par_cnt,
					tank_work_par_cnt, tank_tmr_cnt );
				//Getch();
#endif
				Tank1 = init_tank( tank_sub_type, tank_n, tank_par_cnt,
					tank_work_par_cnt, tank_tmr_cnt );
				Tank1->comb_n = tank_comb_type;
				Tank1->PodTip = tank_sub_type;

#ifdef DEBUG
				Print("[%d] tank N[%d], type[%d]\n", m, Tank1->no, tank_sub_type );
				Print( "Memory free: %lu bytes.\n", ( unsigned long ) coreleft() );
				//Getch();
#endif

				int step_count;
				for ( i = 0; i < tank_mod_cnt; i++ ) 
					{   
					BlockRead( df, &b, 1 );	//	Step count
					step_count = b;

#if !defined NO_TANKS_MODE
					if ( step_count ) Tank1->modes_manager->set_mode_config( i, step_count );
#endif //NO_TANKS_MODE                

					for ( int k = 0; k < step_count; k++ )
						{		  
						BlockRead( df, &b, 1 );
						u_char open_dev_cnt = b;

						for ( j = 0; j < open_dev_cnt; j++ ) 
							{
							BlockRead( df, ( unsigned char * )&w, 2 );
							open_device[ j ] = w; 
							}

						BlockRead( df, &b, 1 );
						u_char close_dev_cnt = b;

						for ( j = 0; j < close_dev_cnt; j++ ) 
							{
							BlockRead( df, ( unsigned char * )&w, 2 );
							close_device[ j ] = w; 
							}
						//#ifdef DEBUG
						//                    Print( "mode %d, step %d, cnt_close %d, cnt_open %d\n",
						//                        i,                         //Режим.
						//                        k,                         //Шаг.
						//                        close_dev_cnt,             //Количество закрываемых устройств.
						//                        open_dev_cnt               //Количество открываемых устройств.
						//                        );
						//#endif // DEBUG

						unsigned char	x1, y1, x2, y2; 

						BlockRead( df, &b, 1 );     //Следующий шаг при завершении времени текущего шага
						BlockRead( df, &b1, 1 );    //Номер параметра, содержащий время шага, мин.                    

						// Cчитывание параметров шага, связанных с маршрутом гребенки.
						BlockRead(df, &b2, 1);
						if ( b2 == 1 )
							{
							BlockRead( df, &x1, 1 );
							BlockRead( df, &y1, 1 );
							BlockRead( df, &x2, 1 );
							BlockRead( df, &y2, 1 );
							}
						else
							{
							x1 = 0;
							y1 = 0;
							x2 = 0;
							y2 = 0;
							}	                 

#if !defined NO_TANKS_MODE
						Tank1->modes_manager->set_step_config( 
							i,                          //Режим.
							k,                          //Шаг.
							close_dev_cnt,              //Количество закрываемых устройств.
							open_dev_cnt,               //Количество открываемых устройств.
							b,                          //Следующий шаг при завершении времени текущего шага.
							b1,                         //Номер параметра, содержащий время шага, мин.
							x1,                         //Координаты входа
							y1,                         //
							x2,                         //Координаты выхода
							y2                          //
							);

						for ( j = 0; j < open_dev_cnt; j++ ) 
							{
							Tank1->modes_manager->add_opened_dev( i, k, V( open_device[ j ] ) );
							}   

						for ( j = 0; j < close_dev_cnt; j++ ) 
							{
							Tank1->modes_manager->add_closed_dev( i, k, V( close_device[ j ] ) );
							}  
#endif //NO_TANKS_MODE                

						}   //  Проходим по шагам
					}   //  Проходим по режимам
				g_tanks[ m ] = Tank1;     


#ifdef DEBUG
				Print( "===================== TANK %d \n", m );
				Getch();
#endif // DEBUG



#ifdef USE_SIMPLE_DEV_ERRORS
				g_dev_errors_manager->add_error( new tech_dev_error( Tank1 ) );
#endif // USE_SIMPLE_DEV_ERRORS
				}
*/
				}
			}	//	for m = [0..TankCnt]

#ifdef DEBUG
        Print( "...g_tanks reading ok! TankCnt = %d  \n", TTank::TankCnt ); 
        Print( "\n 3_Mem free: %lu bytes. ", ( unsigned long ) coreleft() );
		
#ifdef KEY_CONFIRM
        Print( "Press any key to continue..." );
        Getch();
        Print( "\n" );
#endif // KEY_CONFIRM
#endif		

#ifndef USE_NO_COMB
        //	Считывание структуры гребенки.
        unsigned char	iX_max, iY_max;

        BlockRead( df, &b, 1 );
        iY_max = b;
        BlockRead( df, &b, 1 );
        iX_max = b;

        g_greb = new comb( iY_max, iX_max );

#ifdef DEBUG                                          
        Print( "\n iY_max( rows_cnt ) = %d   iX_max( columns_cnt ) = %d \n", 
            g_greb->rows_cnt, g_greb->columns_cnt ); 
#endif	

        //	Перемещаем элементы структуры в массив ( mMatrix[][] );
        for ( i = 0; i < iY_max; i++ )
            {
            for ( j = 0; j < iX_max; j++ )
                {
                BlockRead( df, &b, 1 );
                g_greb->mMatrix[ i ][ j ].type = b; 			
                }
            }			  	

        //	Производим считывание идентификаторов устройств (клапанов);
        for ( i = 0; i < iY_max; i++ )
            {
            for ( j = 0; j < iX_max; j++ )
                {
                BlockRead( df, ( unsigned char * )&w, 2 );
                g_greb->mMatrix[ i ][ j ].id = w;
                }
            }
        g_greb->init();

#ifdef DEBUG
        g_greb->print();
        Print( "\n 3_1 Mem free: %lu bytes. ", ( unsigned long ) coreleft() );
		//Getch();
#ifdef KEY_CONFIRM
        Print( "Press any key to continue..." );
        Getch();
        Print( "\n" );
#endif // KEY_CONFIRM 
#endif // DEBUG

#endif //USE_NO_COMB

#endif // MSA

        if ( df->pos >= df->fd->size ) 
            {
            fclose7188(df);
            return 0;
            };


		if ( strcmp( id, "WG {4B714C08-9602-4130-8563-4B51E08BB9D8}" ) )
			{
			fclose7188(df);
			return 0;
			}

        //Имена режимов гребенок.
        //[ 0 ] - количество гребенок
        //[ 1 ] - индекс гребенки
        //[ 2 ] - количество режимов гребенки
        //[ 3 ] - номер режима
        //[ 4 ] - длина названия
        //[ 5 ]
        //[ 6 ] - название режима
        //[ ... ]
        char combs_cnt = 0;
        BlockRead( df, &combs_cnt, 1 );

        for ( int i = 0; i < combs_cnt; i++ )
            {
            char comb_idx = 0;
            BlockRead( df, &comb_idx, 1 );

            char modes_cnt = 0;
            BlockRead( df, &modes_cnt, 1 );

            for ( int j = 0; j < modes_cnt; j++ )
                {
                char mode_n =  0;
                BlockRead( df, &mode_n, 1 );

                int str_len = 0;
                BlockRead( df, (char*) &str_len, 2 );

                char *str_name = new char[ str_len + 1 ];
				BlockRead( df, str_name, str_len );
                str_name[ str_len ] = 0;
                g_combs[ comb_idx ]->set_mode_name( j, str_name );
                }
            }

        //Имена режимов танков.
        //[ 0 ] - количество танков
        //[ 1 ] - индекс танка
        //[ 2 ] - количество режимов танка
        //[ 3 ] - номер режима
        //[ 4 ] - длина названия
        //[ 5 ]
        //[ 6 ] - название режима
        //[ ... ]

        char tanks_cnt = 0;
        BlockRead( df, &tanks_cnt, 1 );

        for ( i = 0; i < tanks_cnt; i++ )
            {
            char tank_idx = 0;
            BlockRead( df, &tank_idx, 1 );

            char modes_cnt = 0;
            BlockRead( df, &modes_cnt, 1 );

            for ( int j = 0; j < modes_cnt; j++ )
                {
                char mode_n =  0;
                BlockRead( df, &mode_n, 1 );

                int str_len = 0;
                BlockRead( df, (char*) &str_len, 2 );

                char *str_name = new char[ str_len + 1 ];
                BlockRead( df, str_name, str_len );
                str_name[ str_len ] = 0;
                g_tanks[ tank_idx ]->set_mode_name( j, str_name );
				}
            }

        fclose7188(df);
        return 0;
        };
    return 0;
    }
#endif // USE_NO_TANK_COMB_DEVICE
//-----------------------------------------------------------------------------

// 	void TWagoDrv::Read_mode_proportis( FILE7188 *df, int i, TTank *obj )
// 		{
// #ifdef DEBUG
// 		Print( "MODE %d :::::\n", i );
// #endif
// 
// 		//--------------------------------------------------------------
// 		Read_list_dev( df, i, obj->paths[ i ].FB_need, obj->paths[ i ].FB_need_Cnt );
// 		Read_list_dev( df, i, obj->paths[ i ].FB_on_mode, obj->paths[ i ].FB_on_mode_Cnt );
// 
// 		//--------------------------------------------------------------
// 		Read_list_dev( df, i, obj->paths[ i ].OV, obj->paths[ i ].OVCnt );
// 		Read_list_dev( df, i, obj->paths[ i ].CV, obj->paths[ i ].CVCnt );
// 		Read_list_dev( df, i, obj->paths[ i ].US, obj->paths[ i ].USCnt );
// 		Read_list_dev( df, i, obj->paths[ i ].LS, obj->paths[ i ].LSCnt );
// 		Read_list_dev( df, i, obj->paths[ i ].UPR, obj->paths[ i ].UPRCnt );
// 		Read_dual_list( df, i, 
// 			obj->paths[ i ].FB_control, obj->paths[ i ].FB_control_dev, 
// 			obj->paths[ i ].FB_control_Cnt	 );
// 		Read_list_dev( df, i, obj->paths[ i ].Key_signal, obj->paths[ i ].Key_signal_Cnt );
// 
// 		//--------------------------------------------------------------
// 		Read_list_dev( df, i, obj->paths[ i ].Dev_off, Comb1->paths[ i ].Dev_off_Cnt );
// 		Read_list_dev( df, i, obj->paths[ i ].FV, Comb1->paths[ i ].FVCnt );
// 
// 		//--------------------------------------------------------------
// 		Read_Value( df, i, obj->paths[ i ].work_time_par );
// 		Read_Value( df, i, obj->paths[ i ].next_mode );
// 
// 		Read_list_dev( df, i, obj->paths[ i ].Block_dev, obj->paths[ i ].Block_dev_Cnt );
// 		Read_list_mode( df, i, obj->paths[ i ].Block_cm, obj->paths[ i ].Block_cm_cnt );
// 		Read_list_mode( df, i, obj->paths[ i ].Block_tm, obj->paths[ i ].Block_tm_cnt );
// 		Read_list_mode( df, i, obj->paths[ i ].On_mode, obj->paths[ i ].On_mode_Cnt );
// 		Read_list_mode( df, i, obj->paths[ i ].Off_mode, obj->paths[ i ].Off_mode_Cnt );
// 
// #ifdef DEBUG
// 		Print( "-------------------------------------------\n" );
// 		Getch();
// #endif
// 		}
//-----------------------------------------------------------------------------


	void TWagoDrv::Read_Value( FILE7188	*df, uint &value )
		{
		unsigned char b;
		unsigned int w;

		BlockRead(df, &b, 1);

		if ( b == 0 )
			{
			value = 0;
			}
		else
			{
			if ( b == 1 )
				{
				BlockRead(df, (u_char *)&w, 2);
				value = w;
				} 
			else
				{
				for ( int i = 0; i < b; i++ )
					{
					BlockRead(df, (u_char *)&w, 2);
					}

				value = 0;
				}
			}			
#ifdef DEBUG
			//Print( "    value = %u \n", value );
#endif  
		
		}
//-----------------------------------------------------------------------------

	void TWagoDrv::Read_list_mode( FILE7188	*df, int mode, uint **Mode_list, int &Mode_cnt )
		{
		unsigned char b;
		unsigned int w;

		BlockRead(df, &b, 1);
		Mode_cnt = b; 
		*Mode_list = new uint[ Mode_cnt ];

#ifdef DEBUG
		Print( "Path[ %d ]Mode_cnt = %d \n", mode, Mode_cnt );
#endif

		for ( int j = 0; j < Mode_cnt; j++) 
			{
			//BlockRead(df, &b, 1);	//	none
									   
			BlockRead(df, (unsigned char *)&w, 2);	//	mode kod (value)
			(*Mode_list)[ j ] = w;

			//------------------
			//	для Priem.cpp
			//for ( int i = 0; i < Mode_cnt; i++ )
			//	{
			//	if ( Mode_list[ i ] < 1000 )
			//		{
			//		Mode_list[ i ] / 1000; //	obj no
			//		Mode_list[ i ] % 1000; //	mode
			//															
			//		}
			//	}
			//------------------

#ifdef DEBUG
			Print( "    value[ %d ]: %u \n", j, w );
#endif  
			}
		}
//-----------------------------------------------------------------------------

	void TWagoDrv::Read_list_dev( FILE7188 *df, int mode, dev_elem **Dev_list, int &Dev_cnt )
		{
		unsigned char b;
		unsigned int w;

		BlockRead(df, &b, 1);
		Dev_cnt = b; 
		*Dev_list = new dev_elem[ Dev_cnt ];

#ifdef DEBUG
		Print( "MODE[ %d ].Dev_cnt = %d \n", mode, Dev_cnt );
#endif

		for ( int j = 0; j < Dev_cnt; j++) 
			{
			BlockRead(df, &b, 1);					//	device type
			(*Dev_list)[ j ].type = devType( b );

			BlockRead(df, (unsigned char *)&w, 2);	//	device number
			(*Dev_list)[ j ].no = w;
#ifdef DEBUG
			Print( "    DEV[ %d ]( %d ): %u \n", j, b, w );
#endif        
			}
		}
//-----------------------------------------------------------------------------

	void TWagoDrv::Read_dual_list( 
		FILE7188 *df, int mode, dev_elem **Dev_list, dev_elem **Dev_list_2, int &Dev_cnt )
		{
		unsigned char b;
		unsigned int w;

		BlockRead(df, &b, 1);
		Dev_cnt = b; 
		*Dev_list = new dev_elem[ Dev_cnt ];
		*Dev_list_2 = new dev_elem[ Dev_cnt ];

#ifdef DEBUG
		Print( "MODE[ %d ].Dev_cnt = %d \n", mode, Dev_cnt );
#endif

		for ( int j = 0; j < Dev_cnt; j++) 
			{
			BlockRead(df, &b, 1);					//	device type
			(*Dev_list)[ j ].type = devType( b );

			BlockRead(df, (unsigned char *)&w, 2);	//	device number
			(*Dev_list)[ j ].no = w;

#ifdef DEBUG
			Print( "    DEV[ %d ]( %d ): %u ---> ", j, b, w );
#endif

			BlockRead(df, &b, 1);					//	device type
			(*Dev_list_2)[ j ].type = devType( b );

			BlockRead(df, (unsigned char *)&w, 2);	//	device number
			(*Dev_list_2)[ j ].no = w;

#ifdef DEBUG
			Print( " DEV[ %d ]( %d ): %u \n", j, b, w );
#endif        
			}
		} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef USE_SIMPLE_DEV_ERRORS
void TVDEV::set_name( char *new_name )
    {
    strlcpy( name, new_name, C_MAX_NAME_LENGTH );

    char *p_short_name = short_name;
    char *p_name = name;

    while ( *p_name && *p_name != ' '&& *p_name != '-' )
        {
        *p_short_name++ = *p_name++;
        }

    *p_short_name = 0;
    }
#endif // USE_SIMPLE_DEV_ERRORS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int TWagoDrv::Run(void) {
    int rc;
    int i;
    int hasComNodes = 0;
    for (i = 0; i < NodeCnt; i++)
        {
        if (( Nodes[i].nodeType == N_315 ) 
            ||( Nodes[i].nodeType == N_815 ))
            {
            hasComNodes++;
            }
        }

    if (hasComNodes)
        {
        if (port==2 || port==1) IsRTS=1;
        rc=InstallCom(port, speed, dataBits, parity, stopBits);
        if (rc==0) {
#ifdef DEBUG_7LED
            ResetGlobalError( 2, 0, 3 );
#endif    
            } else {
                if (DEB) Print("\n\rWAGO ERROR:Unable to open port %d", port);
#ifdef DEBUG_7LED
                SetGlobalError( 2, 0, 3 );
#endif

            };
        }

#if defined I7186_E || defined I7188_E
    for (i = 0; i<NodeCnt; i++)
        {
        if (( Nodes[i].nodeType == N_341 )
          ||( Nodes[i].nodeType == N_841 )
          ||( Nodes[i].nodeType == N_PET ) )
            {
            Nodes[i].socknumber = tcpCommunicator::Ethernet_Connect(Nodes[i].NodeIp,502,500);
            if (Nodes[i].socknumber>=0)
                {
                Nodes[i].connectedstate = 1;
                Nodes[i].lastconnecttry = MyGetMS();
                }
            else
                {
                Nodes[i].connectedstate = 0;
                }
            }
        }
#endif
    return 0;
    };

//int TWagoDrv::LoadTables(char *fn) {
//    FILE7188 *df;
//    int ch;
//    int intb, outtb, ino, outo, i;
//    unsigned long t1, t2;
//    char b;
//    TTblRec r;
//
//    if (DEB)  Print("\n\rReading tables description from %s....", fn);
//    if ((df=fopen7188(fn))!=NULL) {
//        //alloc mem
//
//        TRS.R=(struct TTblRec *)malloc(sizeof(TTblRec)*300);
//        TRS.RecCnt=0;
//
//        while (df->pos<df->fd->size) {
//            intb=__readVal1(df); if (intb<0) break;
//            ino=__readVal1(df);  if (ino<0) break;
//            outtb=__readVal1(df);if (outtb<0) break;
//            outo=__readVal1(df); if (outo<0) break;
//            TRS.R[TRS.RecCnt].inid=intb;
//            TRS.R[TRS.RecCnt].inoffs=ino;
//            TRS.R[TRS.RecCnt].outid=outtb;
//            TRS.R[TRS.RecCnt].outoffs=outo;
//            //       if (DEB) Print("\n\r(%d %d %d %d)", TRS.R[TRS.RecCnt].inid, TRS.R[TRS.RecCnt].inoffs, TRS.R[TRS.RecCnt].outid, TRS.R[TRS.RecCnt].outoffs);
//            TRS.RecCnt++;
//            };
//        fclose7188(df);
//
//        //sort TRS
//        if (DEB) Print("OK\n\rSorting tables...");
//
//        if (DEB) Print("OK\n\rChecking som.dsr...");
//
//        b=0;
//        for (i=0; i<TRS.RecCnt; i++) {
//            b=0;
//            if (TRS.R[ i ].inid<NodeCnt && TRS.R[ i ].outid<NodeCnt)  {
//                if (TRS.R[ i ].inoffs<tbls[TRS.R[ i ].inid].DIcnt && TRS.R[ i ].outoffs<tbls[TRS.R[ i ].outid].DOcnt) {
//                    b=1;
//                    };
//                };
//            if (b==0) {
//                if (DEB) Print("\n\rError in line (%d %d %d %d)", TRS.R[ i ].inid, TRS.R[ i ].inoffs, TRS.R[ i ].outid, TRS.R[ i ].outoffs);
//                };
//            };
//
//        b=1;
//        while (b) {
//            b=0;
//            for (i=0; i<TRS.RecCnt-1; i++) {
//                t1=(long)TRS.R[ i ].outid*256+TRS.R[ i ].outoffs;
//                t2=(long)TRS.R[i+1].outid*256+TRS.R[i+1].outoffs;
//                if (t1>t2) {
//                    b=1;
//                    r=TRS.R[ i ];
//                    TRS.R[ i ]=TRS.R[i+1];
//                    TRS.R[i+1]=r;
//                    };
//                };
//            };
//
//
//        if (DEB) Print("OK\n\r");
//        } else {
//            if (DEB) Print("Failed");
//            return -1;
//        };
//        return 0;
//    };

int TWagoDrv::ReadInputs(void) {
    int i,j,k, res, bc, idx, pidx;
    //  unsigned char b;
    float f;
    //read DI

    for (i=0; i<NodeCnt; i++) {
        if (( Nodes[i].nodeType == N_315 ) 
            ||( Nodes[i].nodeType == N_815 ))
            {
            if (IsRTS==0) WaitTiks(1);
            if (Nodes[i].Table->DIcnt>0) {
                Buf[0]=Nodes[i].Table->ID;
                Buf[1]=0x02;
                Buf[2]=0x0;
                Buf[3]=0x0;
                Buf[4]=0x0;
                Buf[5]=(unsigned char)Nodes[i].Table->DIcnt;
                ModbusCRC(Buf, 6);
#if !defined I7186_E && !defined I7188_E && !defined uPAC5000
                if (IsRTS) Set485DirToTransmit(port);
#endif
                //write to port
                res=ToComBufn(port, Buf, 8);

#ifdef DEBUG_7LED
                ResetGlobalError( 2, 0, 4 );
#endif
                if (res!=0) {
                    if (DEB) Print("Wago driver error: Unable to write port %d", port);

#ifdef DEBUG_7LED
                    SetGlobalError( 2, 0, 4 );
#endif
                    };
                if (Nodes[i].Table->DIcnt % 8 == 0) {
                    bc=Nodes[i].Table->DIcnt/8;
                    } else {
                        bc=(unsigned char)Nodes[i].Table->DIcnt/8+1;
                    };

                    if (GetAnswer(i, bc+5)==0) {
                        //ok, parse data
                        for (j=0, idx=0; j<bc; j++) {
                            for (k=0; k<8; k++) {
                                if (idx<Nodes[i].Table->DIcnt) {
                                    Nodes[i].Table->DI[idx]=(Buf[j+3]>>k) & 1;
                                    idx++;
                                    };
                                };
                            };
                        };
                };
            }
        else //ethernet wago
            {
            switch ( Nodes[i].nodeType ) 
                {            
                case N_341:
                case N_841:
                case N_PET:
#if defined I7186_E || defined I7188_E
                    if (Nodes[i].Table->DIcnt>0) {
                        Buf[0] = 's';
                        Buf[1] = 's';
                        Buf[2] = 0;
                        Buf[3] = 0;
                        Buf[4] = 0;
                        Buf[5] = 6;
                        Buf[6] = Nodes[i].Table->ID;
                        Buf[7] = 0x02;
                        Buf[8] = 0;
                        Buf[9] = 0;
                        Buf[10] = (unsigned char)Nodes[i].Table->DIcnt >> 8;
                        Buf[11] = (unsigned char)Nodes[i].Table->DIcnt & 0xFF;
                        if (Nodes[i].Table->DIcnt % 8 == 0)
                            {
                            bc=Nodes[i].Table->DIcnt/8;
                            } 
                        else 
                            {
                            bc=(unsigned char)Nodes[i].Table->DIcnt/8+1;
                            };

                        if (ECommunitate(i,12,bc+9) > 0)
                            {
                            if (Buf[7] == 0x02 && Buf[8] == bc)
                                {
                                for (j=0, idx=0; j<bc; j++) {
                                    for (k=0; k<8; k++) {
                                        if (idx<Nodes[i].Table->DIcnt) {
                                            Nodes[i].Table->DI[idx]=(Buf[j+9]>>k) & 1;
                                            idx++;
                                            };
                                        };
                                    };
                                } 
                            else
                                {
#ifdef DEBUG
                                Print("\n\rRead DI:Wago returned error...");
#endif // DEBUG
                                }
                            }
                        }
#endif
                    break;

                    //                case N_PET:    //   ICP CON PET-7051
                    //                    break;  
                }  
            }

        //read AI
        if (( Nodes[i].nodeType == N_315 ) 
            ||( Nodes[i].nodeType == N_815 ))
            {
            if (Nodes[i].Table->AIcnt>0) {
                Buf[0]=Nodes[i].Table->ID;
                Buf[1]=0x04;
                Buf[2]=0x0;
                Buf[3]=0x0;
                Buf[4]=0x0;
                bc=(unsigned char)Nodes[i].wAIcnt/2;
                Buf[5]=bc;
                ModbusCRC(Buf, 6);
#if !defined I7188_E && !defined I7186_E && !defined uPAC5000
                if (IsRTS) Set485DirToTransmit(port);
#endif //!defined I7188_E && !defined I7186_E
                //write to port
                res=ToComBufn(port, Buf, 8);
#ifdef DEBUG_7LED
                ResetGlobalError( 2, 0, 5 );
#endif
                if (res!=0) {
                    if (DEB) Print("Wago driver error: Unable to write port %d",port);
#ifdef DEBUG_7LED
                    SetGlobalError( 2, 0, 5 );
#endif
                    };

                if (GetAnswer(i, bc*2+5)==0) {
                    idx=0;
                    pidx=3;

                    for (j=0; j<Nodes[i].mAIcnt; j++) {
                        bc=GetChCnt(Nodes[i].mAI[j]);
                        for (k=0; k<bc; k++) {
                            f=GetReal(&pidx, Nodes[i].mAI[j], k, Nodes[i].Table->AIR[idx],Nodes[i].nodeType);
                            //	  if ((long)f!=-1000)
                            Nodes[i].Table->AI[idx]=f;
                            idx++; //chennel
                            };
                        };

                    };
                };
            }
        else //Ethernet WAGO
            {
#if defined I7186_E || defined I7188_E
#ifdef ASIWAGO
            int gatn = 0;
            int curgn;
            if (Nodes[i].Table->AIcnt>0 || TASigateways::gatesatnodes[i]>0) 
#else
            if (Nodes[i].Table->AIcnt>0) 
#endif // ASIWAGO
                {                
                Buf[0] = 's';
                Buf[1] = 's';
                Buf[2] = 0;
                Buf[3] = 0;
                Buf[4] = 0;
                Buf[5] = 6;
                Buf[6] = Nodes[i].Table->ID;
                Buf[7] = 0x04;
                Buf[8] = 0;
                Buf[9] = 0;
#ifdef ASIWAGO
                bc=Nodes[i].wAIcnt/2 + TASigateways::gatesatnodes[i]*12;
                Buf[10] = ((unsigned char)bc) >> 8;
                Buf[11] = ((unsigned char)bc) & 0xFF;
#else
                bc=(unsigned char)Nodes[i].wAIcnt/2;
                Buf[10] = ((unsigned char)bc) >> 8;
                Buf[11] = ((unsigned char)bc) & 0xFF;
#endif // ASIWAGO
                if (ECommunitate(i,12,bc*2+9) > 0)
                    {
                    if (Buf[7] == 0x04 && Buf[8] == bc*2)
                        {
                        idx=0;
                        pidx=9;
                        for (j=0; j<Nodes[i].mAIcnt; j++) {
#ifdef ASIWAGO
                            if (655 == Nodes[i].mAI[j])
                                {
                                k = 0;
                                curgn = 0;
                                while (k<i)
                                    {
                                    curgn += TASigateways::gatesatnodes[k];
                                    }
                                curgn += gatn++;
                                pidx+=8;
                                for (k = 0;k < 16; k++)
                                    {
                                    if (k % 2)
                                        {
                                        ASigateways->ASigate[curgn]->ASDI[k] = (unsigned char)(Buf[pidx + k - 1] << 4) + (unsigned char)(Buf[pidx + k - 1] >> 4);
                                        }
                                    else
                                        {
                                        ASigateways->ASigate[curgn]->ASDI[k] = (unsigned char)(Buf[pidx + k + 1] << 4) + (unsigned char)(Buf[pidx + k + 1] >> 4);
                                        }
                                    }
                                pidx+=16;
                                continue;
                                }
#endif // ASIWAGO
                            bc=GetChCnt(Nodes[i].mAI[j]);
                            for (k=0; k<bc; k++) {
                                f=GetReal(&pidx, Nodes[i].mAI[j], k, Nodes[i].Table->AIR[idx],Nodes[i].nodeType);
                                Nodes[i].Table->AI[idx]=f;
                                idx++; //chennel
                                }
                            }
                        } 
                    else
                        {
#ifdef DEBUG
                        Print("\n\rRead AI:Wago returned error...");
#endif // DEBUG
                        }
                    }
                }
#endif
            }
        }
    return 0;
    }


int TWagoDrv::WriteOutputs(void) {
    int i,j,k, res, bc, idx, pidx;
    unsigned char b;
    float f;

    //DO
    for (i=0; i<NodeCnt; i++) {
        if (( Nodes[i].nodeType == N_315 ) 
            ||( Nodes[i].nodeType == N_815 ))
            {
            //for each node
            if (IsRTS==0) WaitTiks(1);

            if (Nodes[i].Table->DOcnt>0) {
                Buf[0]=Nodes[i].Table->ID;
                Buf[1]=0x0F;
                Buf[2]=0x0;
                Buf[3]=0x0;
                Buf[4]=0x0;
                Buf[5]=(unsigned char)Nodes[i].Table->DOcnt;
                if (Nodes[i].Table->DOcnt % 8 == 0) {
                    bc=Nodes[i].Table->DOcnt/8;
                    } else {
                        bc=(unsigned char)Nodes[i].Table->DOcnt/8+1;
                    };
                    Buf[6]=bc;
                    for (b=0, j=0, idx=0; j<bc; j++) {
                        for (k=0, b=0; k<8; k++) {
                            if (idx<Nodes[i].Table->DOcnt) {
                                b=b | ((Nodes[i].Table->_DO[idx] & 1)<<k);
                                idx++;
                                };
                            };
                        Buf[j+7]=b;
                        };
                    ModbusCRC(Buf, bc+7);
                    //write to port
#if !defined I7188_E && !defined I7186_E && !defined uPAC5000
                    if (IsRTS) Set485DirToTransmit(port);
#endif //!defined I7188_E && !defined I7186_E
                    res=ToComBufn(port, Buf, bc+9);
                    if (res!=0) {
                        if (DEB) Print("Wago driver error: Unable to write port %d", port);
#ifdef DEBUG_7LED
                        ResetGlobalError( 2, 0, 6 );
#endif
#ifdef DEBUG_7LED
                        SetGlobalError( 2, 0, 6 );
#endif
                        };
                    //check answer if ok, replace DO with _DO if needed

                    if (GetAnswer(i, 8)==0) {
                        bc=Nodes[i].Table->DOcnt;
                        for (j=0; j<bc; j++) {
                            b=Nodes[ i ].Table->_DO[j] & 1;
                            if ((Nodes[ i ].Table->DO[j] & 1) !=b) {
                                if (b==1) {
                                    Nodes[ i ].Table->DO[j]|=1;
                                    } else {
                                        Nodes[ i ].Table->DO[j]&=0xFE;
                                    };
                                };
                            };
                        };
                };
            }
        else //Ethernet wago
            {
#if defined I7186_E || defined I7188_E
            if (Nodes[i].Table->DOcnt>0) {
                if (Nodes[i].Table->DOcnt % 8 == 0) {
                    bc=Nodes[i].Table->DOcnt/8;
                    } else {
                        bc=(unsigned char)Nodes[i].Table->DOcnt/8+1;
                    };
                    Buf[0] = 's';
                    Buf[1] = 's';
                    Buf[2] = 0;
                    Buf[3] = 0;
                    Buf[4] = 0;
                    Buf[5] = 7+bc;
                    Buf[6] = Nodes[i].Table->ID;
                    Buf[7] = 0x0F;
                    Buf[8] = 0;
                    Buf[9] = 0;
                    Buf[10] = ((unsigned char)Nodes[i].Table->DOcnt) >> 8;
                    Buf[11] = ((unsigned char)Nodes[i].Table->DOcnt) & 0xFF;
                    Buf[12] = bc;
                    for (b=0, j=0, idx=0; j<bc; j++) {
                        for (k=0, b=0; k<8; k++) {
                            if (idx<Nodes[i].Table->DOcnt) {
                                b=b | ((Nodes[i].Table->_DO[idx] & 1)<<k);
                                idx++;
                                }
                            }
                        Buf[j+13]=b;
                        }

                    if (ECommunitate(i,bc+13,12) > 0)
                        {
                        if (Buf[7] == 0x0F)
                            {
                            for (j=0; j<bc; j++) {
                                b=Nodes[i].Table->_DO[j] & 1;
                                if ((Nodes[i].Table->DO[j] & 1) !=b) {
                                    if (b==1) {
                                        Nodes[i].Table->DO[j]|=1;
                                        } else {
                                            Nodes[i].Table->DO[j]&=0xFE;
                                        }
                                    }
                                }
                            } 
                        else
                            {
#ifdef DEBUG
                            Print("\n\rWrite DO:Wago returned error...");
#endif // DEBUG
                            }
                        }
                }
#endif
            }



        //write AO
        if (( Nodes[i].nodeType == N_315 ) 
            ||( Nodes[i].nodeType == N_815 ))
            {
            if (Nodes[i].Table->AOcnt>0) {
                Buf[0]=Nodes[i].Table->ID;
                Buf[1]=0x10;
                Buf[2]=0x0;
                Buf[3]=0x0;
                Buf[4]=0x0;
                bc=(unsigned char)Nodes[i].wAOcnt/2;
                Buf[5]=bc;
                Buf[6]=bc*2;
                //prepare data
                idx=0;
                pidx=7;
                for (j=0; j<Nodes[i].mAOcnt; j++) {
                    bc=GetChCnt(Nodes[i].mAO[j]);
                    for (k=0; k<bc; k++) {
                        SetWord(&pidx, Nodes[i].mAO[j], Nodes[i].Table->_AO[idx], Nodes[i].Table->AOR[idx],Nodes[i].nodeType);
                        idx++; //chennel
                        };
                    };
                ModbusCRC(Buf, pidx);
#if !defined I7188_E && !defined I7186_E && !defined uPAC5000
                if (IsRTS) Set485DirToTransmit(port);
#endif //!defined I7188_E && !defined I7186_E
                //write to port
                res=ToComBufn(port, Buf, pidx+2);
                if (res!=0) {
                    if (DEB) Print("Wago driver error: Unable to write port %d", port);
                    };
                //check answer if ok, replace AO with _AO if needed
                if (GetAnswer(i, 8)==0) {
                    //      Print("\\");
                    for (j=0; j<Nodes[i].Table->AOcnt; j++) {
                        //       if (Nodes[i].Table->AO[j]!=0) Print("-- %f", Nodes[i].Table->AO[j]);
                        if (Nodes[i].Table->AO[j]!=Nodes[i].Table->_AO[j]) {
                            Nodes[i].Table->AO[j]=Nodes[i].Table->_AO[j];
                            };
                        };
                    };
                };
            }
        else //Ethernet wago
            {
#if defined I7186_E || defined I7188_E
#ifdef ASIWAGO
            int gatn = 0;
            int curgn;
            if (Nodes[i].Table->AOcnt>0 || TASigateways::gatesatnodes[i]>0) 
#else
            if (Nodes[i].Table->AOcnt>0)               
#endif // ASIWAGO
                {
#ifdef ASIWAGO
                bc = (unsigned char)Nodes[i].wAOcnt/2 + TASigateways::gatesatnodes[i] * 12;
#else
                bc = (unsigned char)Nodes[i].wAOcnt/2;
#endif // ASIWAGO
                Buf[0] = 's';
                Buf[1] = 's';
                Buf[2] = 0;
                Buf[3] = 0;
                Buf[4] = 0;
                Buf[5] = 7+bc*2;
                Buf[6] = Nodes[i].Table->ID;
                Buf[7] = 0x10;
                Buf[8] = 0;
                Buf[9] = 0;
                Buf[10] = bc >> 8;
                Buf[11] = bc & 0xFF;
                Buf[12] = bc*2;
                idx=0;
                pidx=13;
                for (j=0; j<Nodes[i].mAOcnt; j++) {
#ifdef ASIWAGO
                    if (655 == Nodes[i].mAO[j])
                        {
                        k = 0;
                        curgn = 0;
                        while (k<i)
                            {
                            curgn += TASigateways::gatesatnodes[k];
                            }
                        curgn += gatn++;
						for (k = 0; k < 8; k++)
							{
							Buf[pidx + k] = 0; //забиваем mailbox и контрольный байт нулями
							}
                        pidx+=8;
                        for (k = 0;k < 16; k++)
                            {
                            if (k % 2)
                                {
                                Buf[pidx + k] = (unsigned char)(ASigateways->ASigate[curgn]->ASDO[k-1] << 4) + (unsigned char)(ASigateways->ASigate[curgn]->ASDO[k-1] >> 4);
                                }
                            else
                                {
                                Buf[pidx + k] = (unsigned char)(ASigateways->ASigate[curgn]->ASDO[k+1] << 4) + (unsigned char)(ASigateways->ASigate[curgn]->ASDO[k+1] >> 4);
                                }
                            }
                        pidx+=16;
                        continue;
                        }
#endif // ASIWAGO
                    bc=GetChCnt(Nodes[i].mAO[j]);
                    for (k=0; k<bc; k++) {
                        SetWord(&pidx, Nodes[i].mAO[j], Nodes[i].Table->_AO[idx], Nodes[i].Table->AOR[idx],Nodes[i].nodeType);
                        idx++; //chennel
                        };
                    };
#ifdef ASIWAGO
                bc = (unsigned char)Nodes[i].wAOcnt/2 + TASigateways::gatesatnodes[i] * 12;
#else
                bc = (unsigned char)Nodes[i].wAOcnt/2;
#endif // ASIWAGO
                if (ECommunitate(i,bc*2+13,12) > 0)
                    {
                    if (Buf[7] == 0x10)
                        {
                        for (j=0; j<Nodes[i].Table->AOcnt; j++) {
                            if (Nodes[i].Table->AO[j]!=Nodes[i].Table->_AO[j]) {
                                Nodes[i].Table->AO[j]=Nodes[i].Table->_AO[j];
                                }
                            }
                        } 
                    else
                        {
#ifdef DEBUG
                        Print("\n\rWrite AO:Wago returned error... Error number %X. AO count %d",Buf[7],Nodes[i].Table->AOcnt);
#endif // DEBUG
                        }
                    }
                }
#endif
            }
        };
    return 0;
    };

int TWagoDrv::GetChar(void) {
    unsigned long st, del;
    int res;
    // del=CHAR_DEL;
    del=15;
    st=TikMult();
    res=IsCom(port);

    //Print( " com = %d\n", port );
    while (res==0 && getDeltaTiks(st)<=del) res=IsCom(port);
    res=IsCom(port);
    if (res<=0) {
        //error
        if (res!=0) return res;
        if (res==0) return -8;
        };
    return ReadCom(port);
    };

int TWagoDrv::GetAnswer(int i, int bc) {
    int err, res, j;
    unsigned char b, b1;
    unsigned long curt, ticks = 100;

    if (IsRTS) {
        //      ticks=Post->par->getParam(P_WAGO_TICKS);
        WaitTransmitOver(port);
        //      WaitTiks(1);
        //      Delay(1);
        for (curt=0; curt<ticks; curt++) 
            {
            err=err+1;
            if (err==1000) err=0;
            }
#if !defined I7186_E && !defined I7188_E && !defined uPAC5000
        Set485DirToReceive(port);
#endif
        };

    //    Print("In Answer ");

    //bc bytes should receive
    err=0;
    for (j=0; j<bc; j++) {
        res=GetChar();
        if (res>=0) {
            Buf[j]=res;
            } else {
                err=1;
                res=j;
                break;
            };
            if (j==0 && (unsigned long)Buf[0]!=Nodes[ i ].Table->ID) {err=3; break;};
            if (j==1) {
                if (Buf[1]==0x0F || Buf[1]==02 || Buf[1]==0x10 || Buf[1]==04) {
                    } else {err=3;break;};
                };
        };
    if (err==0) {
        //check crc
        b=Buf[bc-1];
        b1=Buf[bc-2];
        ModbusCRC(Buf, bc-2);
        //was crc error
        if (b!=Buf[bc-1] || b1!=Buf[bc-2]) err=2;
        };
    curt=TikMult();
    if ( err > 0 ) 
        {
        if ( err == 3 )
            {
            Wait( 10 );
            }

        //dec node err count
        if ( Nodes[ i ].trcnt == 0 && Nodes[ i ].Table->state != TS_DISABLED )
            {
            Nodes[ i ].Table->state = TS_DISABLED;
            SetGlobalError( EC_NO_CONNECTION, ES_WAGO, Nodes[ i ].Table->ID );
            Wait( 30 );
            } 
        else 
            {
            if ( Nodes[ i ].trcnt > 0 ) Nodes[ i ].trcnt--;
            }

            while ( GetChar() >= 0 );
            return -1;
        } 
    else 
        {
        //All OK.
        //Reset error if was.
        if ( Nodes[ i ].trcnt <= 0 ) 
            {
            ResetGlobalError( EC_NO_CONNECTION, ES_WAGO, Nodes[ i ].Table->ID );            
            }
        Nodes[ i ].trcnt = retrCnt;
        Nodes[ i ].Table->state = TS_ENABLED;
        }

    while (getDeltaTiks(curt)<FRAME_DEL) ;

    //  Print("Out Answer ");

    return 0;
    };

//TDO0
TDO0::TDO0(void) {
    Tbl=NULL;
    offs=-1;
    HI=0;
    };

TDO0::~TDO0(void) { };

int TDO0::Init(TPLCTable *Table,int offset) {
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->DOcnt) {
            Tbl=Table;
            offs=offset;
            return 0;
            };
        };
    return -1;
    };

int TDO0::On(void) {
    return SetDOTable(Tbl, offs, 1);
    };

int TDO0::Off(void) {
    return SetDOTable(Tbl, offs, 0);
    };

int TDO0::Set(unsigned char Val) {
    int b;
    if (Val>=1) {b=1;} else {b=0;};
    return SetDOTable(Tbl, offs, b);
    };

int TDO0::State(void) {
    return GetDOTable(Tbl, offs);
    };

int TDO0::UState(void) {
    return GetDOTable(Tbl, offs);
    };



//T2DO0
T2DO0::T2DO0(void) {
    Tbl1=NULL;
    Tbl2=NULL;
    offs1=-1;
    offs2=-1;
    HI=0;
    };

T2DO0::~T2DO0(void) { };

int T2DO0::Init(TPLCTable *Table1,int offset1, TPLCTable *Table2,int offset2) {
    if (Table1!=NULL && Table2!=NULL) {
        if (offset1>=0 && offset1<Table1->DOcnt && offset2>=0 && offset2<Table2->DOcnt) {
            Tbl1=Table1;
            offs1=offset1;
            Tbl2=Table2;
            offs2=offset2;
            return 0;
            };
        };
    return -1;
    };

int T2DO0::On(void) {
    SetDOTable(Tbl1, offs1, 0);
    SetDOTable(Tbl2, offs2, 1);
    return 0;
    };

int T2DO0::Off(void) {
    SetDOTable(Tbl1, offs1, 1);
    SetDOTable(Tbl2, offs2, 0);
    return 0;
    };

int T2DO0::Set(unsigned char Val) {
    if (Val>=1) {On();} else {Off();};
    return 0;
    };

int T2DO0::State(void) {
    unsigned char b1, b2;
    b1=GetDOTable(Tbl1, offs1);
    b2=GetDOTable(Tbl2, offs2);
    if (b1==b2) return -1;
    return b2;
    };

int T2DO0::UState(void) {
    unsigned char b1, b2;
    b1=GetDOTable(Tbl1, offs1);
    b2=GetDOTable(Tbl2, offs2);
    if (b1==b2) return -1;
    return b2;
    };



//TDO1
TDO1::TDO1( int checkFB ): checkFB( checkFB )
    {
    TblO=NULL;
    TblI=NULL;
    offs=-1;
    ioffs=-1;
    swt=MyGetMS();
    SWT_DEL=5000;
    HI=0;
    };

TDO1::~TDO1(void) { };

int TDO1::Init(TPLCTable *TableO,int offset, TPLCTable *TableI, int ioffset) {
    if (TableO!=NULL && TableI!=NULL) {
        if (offset>=0 && offset<TableO->DOcnt && ioffset>=0 && ioffset<TableI->DIcnt) {
            TblO=TableO;
            TblI=TableI;
            offs=offset;
            ioffs=ioffset;
            return 0;
            };
        };
    return -1;
    };

int TDO1::On(void) {
    int o;
    o=GetDOTable(TblO, offs);
    if (o==0) {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 1);
        };
    return 0;
    };

int TDO1::Off(void) {
    int o;
    o=GetDOTable(TblO, offs);
    if (o!=0) {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 0);
        };
    return 0;
    };

int TDO1::Set(unsigned char Val) {
    int b;
    if (Val>=1) {b=On();} else {b=Off();};
    return b;
    };

int TDO1::State(void) 
    {
    int i,o;
    o = GetDOTable(TblO, offs);
    i = GetDITable(TblI, ioffs);

    if ( g_is_FB 
#ifdef TAKE_ACCOUNT_FB
		|| g_no_valves_fb_1 
#endif	//	TAKE_ACCOUNT_FB   
     
#ifdef USE_SIMPLE_DEV_ERRORS
        || DE_IS_INHIBIT == ( DE_IS_INHIBIT & ( int ) par->getParam( 0 ) )
#endif // USE_SIMPLE_DEV_ERRORS
        ) return o;

    if ( checkFB == 0 )
        {
        if ( ( o == 0 && i == 1 ) || ( o == 1 && i == 0 ) ) 
            {
            swt = MyGetMS();
            return o;
            }
        }
    else
        {
        if ( o == i ) 
            {
            swt = MyGetMS();
            return i;
            }
        }   

    if ( getDeltaT(swt) > SWT_DEL ) 
        {
        return -1;
        }

    if ( checkFB == 0 ) return !i;
    return i;
    }

int TDO1::UState(void) 
    {
    return GetDOTable(TblO, offs);
    };


//TDO2
TDO2::TDO2(void): isSameFB( 0 ), checkFB( 0 )
    {
    TblO=NULL;
    TblI0=NULL;
    TblI1=NULL;
    offs=-1;
    ioffs0=-1;
    ioffs1=-1;
    swt=MyGetMS();
    SWT_DEL=5000;
    HI=0;
    };

TDO2::~TDO2(void) { };

int TDO2::Init(TPLCTable *TableO,int offset, TPLCTable *TableI0, int ioffset0, TPLCTable *TableI1, int ioffset1) {
    if (TableO!=NULL && TableI0!=NULL && TableI1!=NULL) {
        if (offset>=0 && offset<TableO->DOcnt && ioffset0>=0 && ioffset0<TableI0->DIcnt && ioffset1>=0 && ioffset1<TableI1->DIcnt) {
            TblO=TableO;
            TblI0=TableI0;
            TblI1=TableI1;
            offs=offset;
            ioffs0=ioffset0;
            ioffs1=ioffset1;
            return 0;
            };
        };
    return -1;
    };

int TDO2::On(void) {
    int o;
    o=GetDOTable(TblO, offs);
    if (o==0) {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 1);
        };
    return 0;
    };

int TDO2::Off(void) {
    int o;
    o=GetDOTable(TblO, offs);
    if (o!=0) {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 0);
        };
    return 0;
    };

int TDO2::Set(unsigned char Val) {
    int b;
    if (Val>=1) {b=On();} else {b=Off();};
    return b;
    };


int TDO2::State(void) {
    int i0, i1, o;
    o=GetDOTable(TblO, offs);
    i0=GetDITable(TblI0, ioffs0);
    i1=GetDITable(TblI1, ioffs1);

    if ( g_is_FB || g_no_valves_fb 
#ifdef USE_SIMPLE_DEV_ERRORS
        || DE_IS_INHIBIT == ( DE_IS_INHIBIT & ( int ) par->getParam( 0 ) )
#endif // USE_SIMPLE_DEV_ERRORS        
        ) return o;

    if ( isSameFB == 1 )
        {
        if ( checkFB == 0 )
            {    
            if ( ( o == 0 && i0 == 1 && i1 == 1 ) || 
                ( o == 1 && i1 == 0 && i0 == 0 ) ) 
                {
                swt = MyGetMS();
                return o;
                }
            }
        else
            {
            if ( ( o == 0 && i0 == 0 && i1 == 0 ) || 
                ( o == 1 && i1 == 1 && i0 == 1 ) )  
                {
                swt = MyGetMS();
                return o;
                }
            }  
        }
    else
        {
        if ((o==0 && i0==1 && i1==0) || (o==1 && i1==1 && i0==0)) 
            {
            swt=MyGetMS();
            return o;
            }
        }

    if (getDeltaT(swt)>SWT_DEL) {
        return -1;
        };
    return o;
    };

int TDO2::UState(void) {
    return GetDOTable(TblO, offs);
    };

//TDO3
TDO3::TDO3(): checkFB( 0 ) 
    {
    TblO=NULL;
    TblI0=NULL;
    TblI1=NULL;
    TblI2=NULL;
    offs=-1;
    ioffs0=-1;
    ioffs1=-1;
    ioffs2=-1;
    swt=MyGetMS();
    SWT_DEL=5000;
    HI=0;
    };

TDO3::~TDO3(void) { };

int TDO3::Init(TPLCTable *TableO,int offset, TPLCTable *TableI0, int ioffset0, 
               TPLCTable *TableI1, int ioffset1, TPLCTable *TableI2, int ioffset2 ) 
    {
    if (TableO!=NULL && TableI0!=NULL && TableI1!=NULL && TableI2!=NULL ) 
        {
        if (offset>=0 && offset<TableO->DOcnt && ioffset0>=0 && ioffset0<TableI0->DIcnt && 
            ioffset1>=0 && ioffset1<TableI1->DIcnt && ioffset2 >= 0 && ioffset2 < TableI2->DIcnt ) 
            {
            TblO=TableO;
            TblI0=TableI0;
            TblI1=TableI1;
            TblI2 = TableI2;

            offs=offset;
            ioffs0=ioffset0;
            ioffs1=ioffset1;
            ioffs2 = ioffset2;
            return 0;
            }
        }
    return -1;
    }

int TDO3::On(void) 
    {
    int o;
    o = GetDOTable(TblO, offs);
    if ( o == 0 ) 
        {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 1);
        }
    return 0;
    }

int TDO3::Off(void) 
    {
    int o;
    o = GetDOTable(TblO, offs);
    if (o != 0 ) 
        {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 0);
        }
    return 0;
    }

int TDO3::Set(unsigned char Val) 
    {
    int b;
    if (Val>=1) 
        {
        b=On();
        } 
    else 
        {
        b=Off();
        }
    return b;
    }


int TDO3::State(void) 
    {
    int i0, i1, i2, o;
    o=GetDOTable(TblO, offs);
    i0=GetDITable(TblI0, ioffs0);
    i1=GetDITable(TblI1, ioffs1);
    i2=GetDITable(TblI2, ioffs2);

    if ( g_is_FB 
#ifdef USE_SIMPLE_DEV_ERRORS
        || DE_IS_INHIBIT == ( DE_IS_INHIBIT & ( int ) par->getParam( 0 ) )
#endif // USE_SIMPLE_DEV_ERRORS        
        ) return o;

    if ( checkFB == 0 )
        {    
        if ( ( o == 0 && i0 == 1 && i1 == 1 && i2 == 1 ) || 
            ( o == 1 && i1 == 0 && i0 == 0 && i2 == 0 ) ) 
            {
            swt = MyGetMS();
            return o;
            }
        }
    else
        {
        if ( ( o == 0 && i0 == 0 && i1 == 0 && i2 == 0 ) || 
            ( o == 1 && i1 == 1 && i0 == 1 && i2 == 1 ) )  
            {
            swt = MyGetMS();
            return o;
            }
        }  

    if ( getDeltaT( swt ) > SWT_DEL ) 
        {
        return -1;
        }

    return o;
    }

int TDO3::UState(void) 
    {
    return GetDOTable(TblO, offs);
    }

//T2DO2
T2DO2::T2DO2(void) {
    TblO0=NULL;
    TblO1=NULL;
    TblI0=NULL;
    TblI1=NULL;
    offs0=-1;
    offs1=-1;
    ioffs0=-1;
    ioffs1=-1;
    swt=MyGetMS();
    SWT_DEL=5000;
    HI=0;
    };

T2DO2::~T2DO2(void) { };

int T2DO2::Init(TPLCTable *TableO0,int offset0, TPLCTable *TableO1,int offset1,TPLCTable *TableI0, int ioffset0, TPLCTable *TableI1, int ioffset1) {
    if (TableO0!=NULL && TableO1!=NULL && TableI0!=NULL && TableI1!=NULL) {
        if (offset0>=0 && offset0<TableO0->DOcnt && offset1>=0 && offset1<TableO1->DOcnt &&ioffset0>=0 && ioffset0<TableI0->DIcnt && ioffset1>=0 && ioffset1<TableI1->DIcnt) {
            TblO0=TableO0;
            TblO1=TableO1;
            TblI0=TableI0;
            TblI1=TableI1;
            offs0=offset0;
            offs1=offset1;
            ioffs0=ioffset0;
            ioffs1=ioffset1;
            return 0;
            };
        };
    return -1;
    };

int T2DO2::On(void) {
    int o;
    o=GetDOTable(TblO1, offs1);
    if (o==0) {
        swt=MyGetMS();
        SetDOTable(TblO1, offs1, 1);
        SetDOTable(TblO0, offs0, 0);
        };
    return 0;
    };

int T2DO2::Off(void) {
    int o;
    o=GetDOTable(TblO0, offs0);
    if (o==0) {
        swt=MyGetMS();
        SetDOTable(TblO1, offs1, 0);
        SetDOTable(TblO0, offs0, 1);
        };
    return 0;
    };

int T2DO2::Set(unsigned char Val) {
    int b;
    if (Val>=1) {b=On();} else {b=Off();};
    return b;
    };


int T2DO2::State(void) {
    int i0, i1, o1, o0;
    o1=GetDOTable(TblO1, offs1);
    o0=GetDOTable(TblO0, offs0);
    i0=GetDITable(TblI0, ioffs0);
    i1=GetDITable(TblI1, ioffs1);

    if ( g_is_FB
#ifdef USE_SIMPLE_DEV_ERRORS
        || DE_IS_INHIBIT == ( DE_IS_INHIBIT & ( int ) par->getParam( 0 ) )
#endif // USE_SIMPLE_DEV_ERRORS        
        ) return o1;

    if ((o1==i1) && (o0==i0)) {
        swt=MyGetMS();
        return o1;
        };
    if (getDeltaT(swt)>SWT_DEL) {
        return -1;
        };
    return o1;
    };

int T2DO2::UState(void) {
    unsigned char b1, b2;
    b1=GetDOTable(TblO1, offs1);
    b2=GetDOTable(TblO0, offs0);
    if (b1==b2) return -1;
    return b1;
    };


//TDI
TDI::TDI(): dt( 0 ), state( 0 )
    {
    active_state = 1;

    Tbl=NULL;
    offs=-1; 
    time = MyGetMS();        
    };

TDI::InitTmr ( int dt )
    {
    this->dt = dt;
    return 0;
    }

int TDI::GetDt()
    {
    return dt;
    }

TDI::~TDI(void) { };

int TDI::Init(TPLCTable *Table,int offset) {
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->DIcnt) {
            Tbl=Table;
            offs=offset;
            return 0;
            };
        };
    return -1;
    };

int TDI::SetState( int st )
    {
#ifdef DEBUG
    SetDITable(Tbl,offs,st);
#endif
    state = st;
    time = MyGetMS();
    return 0;
    }

int TDI::StateNow()
    {
#ifdef DEBUG
    Print( "int TDI::StateNow()\n" );
#endif // DEBUG

    return GetDITable( Tbl, offs );
    }

int TDI::State() 
    {
    if ( dt > 0 )
        {
        if ( state != GetDITable( Tbl, offs ) )
            {
            if ( getDeltaT( time ) > dt  ) 
                {
                state = GetDITable( Tbl, offs );
                time = MyGetMS();
                }
            }
        else 
            {
            time = MyGetMS();
            }
        }
    else state = GetDITable( Tbl, offs );

    return state;
    };

//T2DO2
TMIX::TMIX(void) {
    TblO=NULL;
    TblI0=NULL;
    TblI1=NULL;
    TblOU=NULL;
    TblOL=NULL;
    offs=-1;
    offsU=-1;
    offsL=-1;
    ioffs0=-1;
    ioffs1=-1;
    swt=MyGetMS();
    SWT_DEL=5000;
    HI=0;
    };

TMIX::~TMIX(void) { };

int TMIX::Init(TPLCTable *TableO,int offset,
               TPLCTable *TableOU,int offsetU,
               TPLCTable *TableOL,int offsetL,
               TPLCTable *TableI0,int ioffset0,
               TPLCTable *TableI1,int ioffset1) {

                   if (TableO!=NULL && TableI0!=NULL && TableI1!=NULL && TableOU!=NULL && TableOL!=NULL ) {
                       if (offset>=0 && offset<TableO->DOcnt &&
                           ioffset0>=0 && ioffset0<TableI0->DIcnt &&
                           ioffset1>=0 && ioffset1<TableI1->DIcnt &&
                           offsetU>=0 && offsetU<TableOU->DOcnt &&
                           offsetL>=0 && offsetL<TableOL->DOcnt) {
                               TblO=TableO;
                               TblI0=TableI0;
                               TblI1=TableI1;
                               TblOU=TableOU;
                               TblOL=TableOL;
                               offs=offset;
                               ioffs0=ioffset0;
                               ioffs1=ioffset1;
                               offsU=offsetU;
                               offsL=offsetL;
                               return 0;
                           };
                       };
                   return -1;
    };


int TMIX::On(void) {
    int o;
    //off lifts
    SetDOTable(TblOU, offsU, 0);
    SetDOTable(TblOU, offsL, 0);
    o=GetDOTable(TblO, offs);
    if (o==0) {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 1);
        };
    return 0;
    };

int TMIX::Off(void) {
    int o;
    SetDOTable(TblOU, offsU, 0);
    SetDOTable(TblOU, offsL, 0);
    o=GetDOTable(TblO, offs);
    if (o!=0) {
        swt=MyGetMS();
        return SetDOTable(TblO, offs, 0);
        };
    return 0;
    };

int TMIX::Set(unsigned char Val) {
    int b;
    switch (Val) {
  case 0: {
      b=Off();
      break;
      };
  case 1: {
      b=On();
      break;
      };
  case 2: {
      b=OpenUSeat();
      break;
      };
  case 3: {
      b=OpenLSeat();
      break;
      };
  case 4: {
      b=SetDOTable(TblOU, offsU, 1);
      break;
      };
  case 5: {
      b=SetDOTable(TblOL, offsL, 1);
      break;
      };
  default: {
      b=On();
      break;
      };
        };
    return b;
    };


int TMIX::State(void) 
    {
    int i0, i1, o;
    o=GetDOTable(TblO, offs);
    i0=GetDITable(TblI0, ioffs0);
    i1=GetDITable(TblI1, ioffs1);
  
    if ( g_is_FB 
#ifdef USE_SIMPLE_DEV_ERRORS
        || DE_IS_INHIBIT == ( DE_IS_INHIBIT & ( int ) par->getParam( 0 ) )
#endif // USE_SIMPLE_DEV_ERRORS
        ) 
        {
        if (o==0 && GetDOTable(TblOU, offsU)==1) return 2;
        if (o==0 && GetDOTable(TblOL, offsL)==1) return 3;
        return o;
        }

    if ((o==0 && i0==1 && i1==0) || (o==1 && i1==1 && i0==0)) {
        swt=MyGetMS();
        if (o==0 && GetDOTable(TblOU, offsU)==1) return 2;
        if (o==0 && GetDOTable(TblOL, offsL)==1) return 3;
        return o;
        };
    if (getDeltaT(swt)>SWT_DEL) {
        return -1;
        };
    return o;
    };

int TMIX::UState(void) {
    return GetDOTable(TblO, offs);
    };


int TMIX::OpenLSeat(void) {
    Off();
    return SetDOTable(TblOL, offsL, 1);
    };

int TMIX::OpenUSeat(void) {
    Off();
    return SetDOTable(TblOU, offsU, 1);
    };


//TAO
TAO::TAO(void) {
    Tbl=NULL;
    offs=-1;
    HI=0;
    };

TAO::~TAO(void) { };

int TAO::Init(TPLCTable *Table,int offset) {
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->AOcnt) {
            Tbl=Table;
            offs=offset;
            return 0;
            };
        };
    return -1;
    };


int TAO::Set(float Val) {
    return SetAOTable(Tbl, offs, Val);
    };


float TAO::Value(void) {
    return GetAOTable(Tbl, offs);
    };

//TAI
TAI::TAI(void)
	{
    Tbl=NULL;
    offs=-1;
    };

TAI::~TAI(void) { };

int TAI::Init(TPLCTable *Table,int offset) {
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->AIcnt) {
            Tbl=Table;
            offs=offset;
            return 0;
            };
        };
    return -1;
    };

float TAI::Value(void) 
	{
	return GetAITable(Tbl, offs);
    };

//TTE
TTE::TTE(void) 
	{
	par = new TParams(1,1);
    Tbl=NULL;
    offs=-1;
	initpassed = 0;
    };

TTE::~TTE(void) 
	{
	delete par;
	};

int TTE::Init(TPLCTable *Table,int offset) {
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->AIcnt) {
            Tbl=Table;
            offs=offset;
			AI_Last_Val = 0;
            return 0;
            };
        };
    return -1;
    };

float TTE::Value(void) 
	{
	if ( par->getParam( 0 ) > 0 )
		{
		if (!initpassed)
			{
			AI_Last_Val = GetAITable(Tbl, offs);
			if (AI_Last_Val !=0)
				{
				Last_Time = MyGetMS();
				initpassed = 1;
				}
			return AI_Last_Val;
			} 
		else
			{
			//	Реализация фильтра
			if (getDeltaT(Last_Time) > 100)
				{
				AI_Filtr_Val	= ( (( GetAITable(Tbl, offs) - AI_Last_Val ) * ( getDeltaT(Last_Time) ) ) / par->getParam( 0 ) );
				AI_Last_Val		= AI_Last_Val + AI_Filtr_Val;
				Last_Time		= MyGetMS();
				}
			return AI_Last_Val;
			}
		}
	else
		{
		initpassed = 0;
		return GetAITable(Tbl, offs);
		}
	};

//TLE
TLE::TLE(void) {
    Tbl=NULL;
    offs=-1;
    };

TLE::~TLE(void) { };

int TLE::Init(int TYPE, TPLCTable *Table,int offset, float R, float H1, float H2) {
    type=TYPE;
    r=R;
    h1=H1;
    h2=H2;
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->AIcnt) {
            Tbl=Table;
            offs=offset;
            return 0;
            };
        };
    return -1;
    };


float TLE::Value(void) 
    {
    return GetAITable(Tbl, offs);
    };

//TIAI
TIAI::TIAI(void) {
    Tbl=NULL;
    offs=-1;
    };

TIAI::~TIAI(void) { };

int TIAI::Init(TPLCTable *Table,int offset) {
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->AIcnt) {
            Tbl=Table;
            offs=offset;
            return 0;
            };
        };
    return -1;
    };


unsigned long TIAI::Value(void) {
    return (unsigned long)GetAITable(Tbl, offs);
    };


//TPID
TPID::TPID(void) {
    HI=0;
    TblI=NULL;
    TblO=NULL;
    par=NULL;
    offs=-1;
    ioffs=-1;
    k=-1;
    Ti=-1;
    Td=-1;
    Z=-1;
    state=-1;
    uk_1=0;
    ek_1=0;
    ek_2=0;
    dmn=0;
    dmx=100;
    uk_1=0;
    razg=1;
    lt=MyGetMS();
    start=lt;
    };

TPID::~TPID(void) { };

int TPID::Init(TPLCTable *TableO,int offset, TPLCTable *TableI,int ioffset,int dmin, int dmax) {  //AO and //AI
    if (TableO!=NULL && TableI!=NULL) {
        if (offset>=0 && offset<TableO->AOcnt && ioffset>=0 && ioffset<TableI->AIcnt) {
            TblO=TableO;
            TblI=TableI;
            offs=offset;
            ioffs=ioffset;
            dmn=dmin;
            dmx=dmax;
            return 0;
            };
        };
    return -1;
    };

int TPID::InitP(TParams *p, int kidx, int Tiidx, int Tdidx, int Zidx, int raz) {
    int pcnt;
    if (p!=NULL) {
        pcnt=p->getParamCnt();
        if (kidx>=0 && kidx<pcnt && Tiidx>=0 && Tiidx<pcnt && Tdidx>=0 && Tdidx<pcnt && Zidx>=0 && Zidx<pcnt) {
            par=p;
            state=0;
            k=kidx;
            Ti=Tiidx;
            Td=Tdidx;
            Z=Zidx;
            if (raz!=0) {
                razg=raz;
                } else {
                    razg=1;
                };
                return 0;
            };
        };
    return -1;
    };

void TPID::On(void) {
    if (state!=1 && state!=5) {
        lt=MyGetMS();
        start=lt;
        state=1;
        };
    };

void TPID::Off(void) {
    if (state!=0) {
        state=0;
        SetAOTable(TblO, offs, 0);
        };
    };

void TPID::Eval(void) {
    unsigned long now;
    float cur, dt, dlta;
    float K, TI, TD;
    float ek, Uk, dUk, q0, q1, q2;
    int st;

    if (state>=0) {
        st=state;
        state=5;
        now=MyGetMS();
        if (now!=lt) {
            cur=GetAITable(TblI, ioffs);
            K=par->getParam(k);
            TI=par->getParam(Ti);
            TD=par->getParam(Td);
#ifdef MSA
            ek=100*(par->getParam(Z)-cur)/(dmx-dmn);
#else
            ek=100*(cur-par->getParam(Z))/(dmx-dmn);
#endif // MSA
            dt=(float)getDeltaT(lt)/1000;

            if (dt!=0 && TI!=0) {
                lt=MyGetMS();
                q0=K*(1+TD/dt);
                q1=K*(-1-2*TD/dt+2*dt/TI);
                q2=K*TD/dt;
                dUk=q0*ek+q1*ek_1+q2*ek_2;
                Uk=uk_1+dUk;
                if (Uk > 100) Uk=100;
                if (Uk < 0) Uk=0;
                uk_1=Uk;
                ek_2=ek_1;
                ek_1=ek;
                if (st==1) {
                    dlta=getDeltaT(start)/1000;
                    if (dlta<=razg) {
                        if (Uk>=100*dlta/razg) {
                            SetAOTable(TblO, offs, 100*dlta/razg);
                            } else {
                                SetAOTable(TblO, offs, Uk);
                            };
                        } else {
                            SetAOTable(TblO, offs, Uk);
                        };
                    };

                };
            };
        state=st;
        };

    };

void TPID::Reset(void) {
    lt=MyGetMS();
    start=lt;
    uk_1=0;
    ek_1=0;
    ek_2=0;
    };

//TTimer
TTimer::TTimer() {
    b=0;
    lt=0;
    z=0;
    ac=0;
    };

TTimer::~TTimer() { };

void TTimer::S() 
    {
    if ( b == 0 )
        {
        b = 1;
        ac = 0;
        lt = MyGetMS();
        } 
    else 
        if ( b == 2 ) //pause
            {  
            b = 1;
            lt = MyGetMS();
            }
    }

void TTimer::R() 
    {
    b = 0;
    ac = 0;
    }

void TTimer::P() 
    {
    if ( b == 1 ) 
        {
        ac += getDeltaT( lt );
        }
    b = 2;
    }

unsigned long TTimer::T()
    {
    if ( b == 1) 
        {
        return ac + getDeltaT( lt );
        }
    return ac;
    }

int TTimer::Q(void) 
    {
    if ( b != 0 ) 
        {
		if ( ac + (b == 1 ? getDeltaT(lt) : 0) < z )
            {
            return 0;
            } 
        else 
            {
            return 1;
            }
        }
    return 0;
    }

void TTimer::SZ( unsigned long zad ) 
    {
#ifdef DEBUG
    if ( 0 == zad )
        {
        Print( "Error void TTimer::SZ( unsigned long zad ), zad = %lu!\n", zad );
        }
#endif

    z = zad;
    }

int TTimer::State() 
    {
    return b;
    }

unsigned long TTimer::Z() 
    {
    return z;
    }

int TTimer::save( char *buff )
    {
	if (b == 1)
		{
		ac += getDeltaT(lt);
		lt = MyGetMS();
		}
    buff[ 0 ] = b;    
    ( ( u_int_4* ) ( buff + 1 ) )[ 0 ] = z;
    int answer_size = 5;
    if ( b != 0 )
        {
        ( ( u_int_4* ) ( buff + 1 ) )[ 2 ] = ac;
        answer_size += 8;
        }

    return answer_size;
    }

int TTimer::load( char *buff )
    {
    b = buff[ 0 ];
    z = ( ( u_int_4* ) ( buff + 1 ) )[ 0 ];
    int answer_size = 5;
    if ( b != 0 )
        {
		lt = MyGetMS();
        ac = ( ( u_int_4* ) ( buff + 1 ) )[ 2 ];
        answer_size += 8;
        }
    return answer_size;
    }

int TTimer::get_saved_size()
    {
    if ( b != 0 ) return 4 + 1 + 4 + 4;
    return 4 + 1;
    }
//TCounter

TCounter::TCounter(): pumps_count( 0 ),
    pumps( 0 ),
    state( 0 )
    {
    Tbl=NULL;
    b=0;
    s=0;
    MAX_VAL=0;
    offs=0;
    lt=0;

    //R();
    b = 1;  //S();
    };

TCounter::~TCounter(void) { };

int TCounter::Init(TPLCTable *Table,int offset, unsigned long mv) {
    if (Table!=NULL) {
        if (offset>=0 && offset<Table->AIcnt) {
            Tbl=Table;
            offs=offset;
            MAX_VAL=mv;
            return 0;
            };
        };
    return -1;
    };

void TCounter::SetVal( float stVal )
    {  
    s = stVal;   
    };

void TCounter::P(void) {
    unsigned long delta, cur;
    if (b==1) {
        cur=(unsigned long)GetAITable(Tbl, offs);
        if (cur<lt) {
            delta=MAX_VAL-lt+cur;
            } else {
                delta=cur-lt;
            };
            if (delta>0) {
                s+=delta;
                lt=cur;
                };
        };
    b=2;
    };

void TCounter::S(void) {
    if (b==0) {
        b=1;
        s=0;
        lt=GetAITable(Tbl, offs);
        } else if (b==2) {  //pause
            b=1;
            lt=GetAITable(Tbl, offs);
        };
    };

unsigned long TCounter::Q(void) 
    {
#ifdef DEBUG
    //Print( "Ctr::Q(), b = %d", b );
#endif

    unsigned long delta, cur;
    if ( b == 2 ) return s;
    if ( b == 1 ) 
        {
        cur = GetAITable(Tbl, offs);
        if ( cur < lt ) 
            {
            delta=MAX_VAL-lt+cur;
            } 
        else 
            {
            delta=cur-lt;
            }
        if (delta>0) 
            {
            s+=delta;
            lt=cur;
            }
        return s;
        }
    return 0;
    }

void TCounter::R(void)  {
    if (b!=0) b=0;
    };

int TCounter::State( void )
    {
    if ( pumps_count && pumps )
        {
        char           is_pump_working         = 0;
        static u_int_4 start_pump_working_time = 0; 
        static u_int_4 counter_prev_value      = 0;

        for ( int i = 0; i < pumps_count; i++ )
            {                
            if ( pumps[ i ]->State() == 1 )
                {
                is_pump_working = 1;
                if ( 0 == start_pump_working_time )
                    {
                    start_pump_working_time = MyGetMS();
                    counter_prev_value      = Q();
                    state                   = 1;
                    }
                }
            }

        if ( 0 == is_pump_working )
            {
            // Насос не работает.

            start_pump_working_time = 0;
            state = 0;
            }
        else
            {
            // Насос работает.

            if ( b == 2 ) // Пауза.
            	{
                state = 2;// Пауза счетчика.
                start_pump_working_time = MyGetMS();
            	}
            else          // Работа.
                {
                state = 1; // Работа счетчика.

                if ( getDeltaT( start_pump_working_time ) >
#ifdef USE_PAC_SYSTEM_INFO
                    g_PAC_system->par->getParam( PAC_info::P_CTR_ERROR_TIME ) 
#else
                    10000
#endif
                    )
                    {
                    // Проверяем счетчик на ошибку - он должен изменить свои показания.
                    if ( Q() == counter_prev_value )
                        {
                        state = -1;
                        }
                    else
                        {                        
                        start_pump_working_time = MyGetMS();
                        counter_prev_value      = Q();                        
                        }
                    }
                }
            }
        } // if ( pumps_count && pumps )

    return state;
    }


TSav::TSav(void) {
    n=0;
    cn=0;
    integrator=0;
    };

TSav::~TSav(void) { };

void TSav::Add(float val, unsigned long inegr) {
    unsigned long delta, i;
    if (inegr>integrator) {
        delta=inegr-integrator;
        integrator=inegr;
        for (i=0; i<delta; i++) {
            n++;
            cn=(cn*(n-1)+val)/n;
            };
        };
    };

void TSav::R(void) {
    n=0;
    cn=0;
    integrator=0;
    };

float TSav::Q(void) {
    return cn;
    };


int getDevIDX(TVDEV **dev,unsigned int K, int cnt)
    {
    int i, l, u;
    l=0;
    u=cnt-1;

//        Print( "K = %d  ", K );

    while (l<=u) 
        {
        i = ( l + u ) / 2;
//                Print( " i = %d  ", i);
//                Print( " dev[ i ]->nom = %d  ", dev[ i ]->nom );        

        if (dev[ i ]->nom==K) return i;
        if (K>dev[ i ]->nom) {
            l=i+1;
            } else {
                u=i-1;
            };
        };
    return -1;
    };

TVDO *V(unsigned int nom
#ifdef DEBUG
        , char is_print_error
#endif
        ) 
    {
    int res;
    res=getDevIDX((TVDEV **)g_devs.VDev, nom, g_devs.VCnt);
    if (res>=0) return g_devs.VDev[res];
    //not found
#ifdef DEBUG
    if ( is_print_error ) Print("\n\rError: V%u not found", nom);
#endif
    return g_tvirt;
    };

TVDO *N(unsigned int nom) 
    {
    int res;
    res = getDevIDX((TVDEV **)g_devs.NDev, nom, g_devs.NCnt);
    if (res>=0) return g_devs.NDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: N%u not found", nom);
#endif
    return g_tvirt;
    };

TVDO *M(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.MDev, nom, g_devs.MCnt);
    if (res>=0) return g_devs.MDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: M%u not found", nom);
#endif
    return g_tvirt;
    };

TVDI *LS(unsigned int nom) 
    {
    int res;
    res=getDevIDX((TVDEV **)g_devs.LSDev, nom, g_devs.LSCnt);
    if ( res >= 0 ) return g_devs.LSDev[res];  
    //not found
#ifdef DEBUG
    Print("\n\rError: LS%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_LS, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    }

TVDI *FS(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.FSDev, nom, g_devs.FSCnt);
    if (res>=0) return g_devs.FSDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: FS%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_FS, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    }

TVDI *FB(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.FBDev, nom, g_devs.FBCnt);
    if (res>=0) return g_devs.FBDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: OS%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_FB, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    }

TVAO *AO(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.AODev, nom, g_devs.AOCnt);
    if (res>=0) return g_devs.AODev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: AO%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_AO, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    }

TVAI *AI(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.AIDev, nom, g_devs.AICnt);
    if (res>=0) return g_devs.AIDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: AI%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_AI, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    }

TCounter *CTR(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.CTRDev, nom, g_devs.CTRCnt);
    if (res>=0) return g_devs.CTRDev[res];

    //not found
#ifdef DEBUG
    Print("\n\rError: CTR%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_CTR, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    };

TVAI *TE(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.TEDev, nom, g_devs.TECnt);
    if (res>=0) return g_devs.TEDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: TE%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_TE, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    }

TVAI *FE(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.FEDev, nom, g_devs.FECnt);
    if (res>=0) return g_devs.FEDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: FE%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_FE, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    }

TVAI *LE(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.LEDev, nom, g_devs.LECnt);
    if (res>=0) return g_devs.LEDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: LE%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_LE, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    };

TVAI *QE(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.QEDev, nom, g_devs.QECnt);
    if (res>=0) return g_devs.QEDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: QE%u not found", nom);
#endif
    SetGlobalError( EC_DEVICE_DEFINITION, ES_WRONG_QE, nom ); 
    while ( 1 ) 
        {
        ShowErrors();        
        }
    };

TVDO *UPR(unsigned int nom) {
    int res;
    res=getDevIDX((TVDEV **)g_devs.UPRDev, nom, g_devs.UPRCnt);
    if (res>=0) return g_devs.UPRDev[res];
    //not found
#ifdef DEBUG
    Print("\n\rError: UPR%u not found", nom);
#endif
    return g_tvirt;
    };

//----------------------------------------------------------------------
void TWagoDrv::ServTables(void) {
    int i;
    TPLCTable TI, TO;
    int res;
    TTblRec r;

    res=0;
    r=TRS.R[0];

    for (i=0; i<TRS.RecCnt; i++) {
        if (TRS.R[ i ].inid<NodeCnt && TRS.R[ i ].outid<NodeCnt)  {
            if (TRS.R[ i ].inoffs<tbls[TRS.R[ i ].inid].DIcnt && TRS.R[ i ].outoffs<tbls[TRS.R[ i ].outid].DOcnt) {

                if (TRS.R[ i ].outoffs==r.outoffs && TRS.R[ i ].outid==r.outid) {
                    res=res | GetDITable(&tbls[TRS.R[ i ].inid], TRS.R[ i ].inoffs);
                    } else {
                        SetDOTable(&tbls[r.outid], r.outoffs, res);
                        r=TRS.R[ i ];
                        res=GetDITable(&tbls[TRS.R[ i ].inid], TRS.R[ i ].inoffs);
                    };
                };
            };
        SetDOTable(&tbls[r.outid], r.outoffs, res);
        };
    };


TChanger::TChanger(int count) {
    int i;
    data=(TChangerDev *)malloc(count*sizeof(TChangerDev));
    cnt=count;
    if (data==NULL) {
        cnt=0;
        __ErrDscr(EMEM);
        };
    for (i=0; i<cnt; i++) {
        data[ i ].no=0;
        data[ i ].last_v=0;
        data[ i ].on_change=NULL;
        };
    };

TChanger::~TChanger(void) {
    delete data;
    };

TChanger::Evaluate(void) {
    int i;
    for (i=0; i<cnt; i++) {
        if (data[ i ].no!=0 && data[ i ].on_change!=NULL) {
            if (data[ i ].last_v!=FB(data[ i ].no)->State()) {
                data[ i ].last_v=FB(data[ i ].no)->State();
                data[ i ].on_change(data[ i ].no, data[ i ].last_v);
                };
            };
        };
    return 0;
    };



//specific Wago function

int TWagoDrv::GetChCnt(int module) {
    switch (module) {
    case 461:
	case 46102:
    case 466:
    case 512:
    case 554:
    case 638: return 2;
    case 455:
    case 460:
    case 402:
    case 504:
	case 1420: return 4;
    case 430:
    case 530:
	case 1415:
	case 1515: return 8;
	case 1405:
	case 1504: return 16;
    case 613:
    case 612:
    case 602:
    case 627:
    case 628:
    case 655:
    case 600: return 0;
    default: {
        if (DEB) Print("\n\rGetChCnt Unknown module %d", module);
        return -1;
        };
        };
    };

int TWagoDrv::GetModuleType(int module) {
    switch (module) {
    case 402:
	case 430:
	case 1405:
	case 1415:
	case 1420: return TYPE_DI;
	case 504:
	case 512:
    case 530:
	case 1504:
	case 1515: return TYPE_DO;
    case 455:
    case 460:
    case 461: 
	case 46102:
    case 466: return TYPE_AI;
    case 554: return TYPE_AO;
    case 638: 
    case 655: return TYPE_AIAO;
    case 613: 
    case 612: 
    case 602: 
    case 627:
    case 628:
    case 600: return TYPE_STUB;
    default: {
        if (DEB==1) Print("\n\rGet Mod type: Unknown module %d", module);
        return TYPE_UNKN;
        };
        };
    };

//  Возвращает количество байт на каждый канал
int TWagoDrv::GetABC(int module,NODE_TYPE nt) {
    switch (module) {
    case 455:
    case 460:
    case 461:
	case 46102:
    case 466:
    case 554: return 2;
    case 638: 

        if ( nt == N_341 || nt == N_841 || nt == N_PET ) 
            {
            return 4;
            } 
        else
            {
            return 3;
            }
    case 655: return 0;
    default: {
        return -1;
        };
        };
    };

float TWagoDrv::GetDefVal(int module) {
    switch(module) {
   case 554: return 0;
   case 638: return 0;
   default: return 0;
        };
    };

float TWagoDrv::GetReal(int *pidx, int module, int ch, TRange r, NODE_TYPE nt) {
    unsigned int w;
    int wi;
    float v;
    switch(module) {
   case 461:
	   {
       wi=Buf[*pidx]*256+Buf[*pidx+1];
       *pidx+=2;
       v=(float)wi/10;
       if (v > -200 && v < 850) {
           return v;
           } else {
               return -1000;
           };
       };

   case 46102: 
	   {
	   wi=Buf[*pidx]*256+Buf[*pidx+1];
	   *pidx+=2;
	   v=(float)wi/10;
	   if ( v == 850 )				return -1000;
	   if ( v == -512 )				return 0;
	   if ( v >= 10 && v <= 1200 )	return v;
	   return v;
	   };

   case 466: {
       w=Buf[*pidx]*256+Buf[*pidx+1];
       *pidx+=2;
       if (r.rh==0 && r.rl==0) {
           if (w<7) return 0;
           return (4+w/2047.5);
           } else {
               if (w<7) {
                   v=4;
                   } else {
                       v=4+w/2047.5;
                   };
                   return r.rl+(v-4)*(r.rh-r.rl)/16;
           };
       };
   case 638: 
       {
       if (ch==0) 
           {
           switch (nt)
               {
               case N_315:
                   w=Buf[*pidx+1]*256+Buf[*pidx+2];
                   *pidx+=3;
                   break;
               case N_815:
                   w=Buf[*pidx]+Buf[*pidx+3]*256;
                   *pidx+=3;
                   break;
               case N_341:
               case N_841:
               case N_PET:
                   w = Buf[*pidx+2]*256+Buf[*pidx+3];
                   *pidx+=4;
                   break;
               }
           return (float)w;
           } 
       else 
           {
           if (N_315 == nt || N_815 == nt)
               {
               w=Buf[*pidx+1]*256+Buf[*pidx+2];
               *pidx+=3;
               }
           else
               {
               w = Buf[*pidx+2]*256+Buf[*pidx+3];
               *pidx+=4;
               }

           return (float)w;
           };     

       };
   default: 
       {
       if (DEB==1) Print("Wago->GetReal: Unknown module %d", module);
       return 0;
       };
        };
    };

int TWagoDrv::SetWord(int *pidx, int module, float val, TRange r, NODE_TYPE nt) {
    unsigned int w;
    float v;
    switch(module) {
   case 554: {
       if (r.rh==0 && r.rl==0) {
           v=val;
           } else {
               v=4+16*(val-r.rl)/(r.rh-r.rl);
           };
           if (v<4) v=4;
           if (v>20) v=20;
           v=2047.5*(v-4);
           w=(unsigned int)v;
           Buf[*pidx]=w >> 8;
           Buf[*pidx+1]=w & 0xFF;
           *pidx+=2;
           return 1;
       };
   case 638: {
       if (N_315 == nt || N_815 == nt)
           {
           Buf[*pidx]=0;
           Buf[*pidx+1]=0;
           Buf[*pidx+2]=0;
           *pidx+=3;
           }
       else
           {
           Buf[*pidx]=0;
           Buf[*pidx+1]=0;
           Buf[*pidx+2]=0;
           Buf[*pidx+3]=0;
           *pidx+=4;
           }
       return 1;
       };
   default: {
       if (DEB==1) Print("Wago->SetWord: Unknown module %d", module);
       return 0;
       };
        };
    };

#if defined I7186_E || defined I7188_E
int TWagoDrv::ECommunitate( int NodeNum,int BytesToSend, int BytesToReceive )
    {
    int iRet, iLength;
    if (Nodes[NodeNum].connectedstate == 0)
        {
        if ((Nodes[NodeNum].socknumber>=0) && (tcpCommunicator::sst[Nodes[NodeNum].socknumber].clID == Nodes[NodeNum].clientID))
            {
            tcpCommunicator::SocketClear(Nodes[NodeNum].socknumber);
            closesocket(Nodes[NodeNum].socknumber);
            Nodes[NodeNum].socknumber = -1;
            } 
        if (getDeltaT(Nodes[NodeNum].lastconnecttry) > WAGO_RECONNECT_TIMEOUT) //таймаут повторного соединения с узлом WAGO
            {
            Nodes[NodeNum].lastconnecttry = MyGetMS();
            iRet = tcpCommunicator::Ethernet_Connect(Nodes[NodeNum].NodeIp,502,250);
            if (iRet>=0)
                {
                Nodes[NodeNum].socknumber = iRet;
                tcpCommunicator::sst[iRet].clID = Nodes[NodeNum].clientID;
                Nodes[NodeNum].connectedstate = 1;
                }
			else
				{
				goto evalConnectionErrors;
				}

			} 
		else
			{
			goto evalConnectionErrors;
			}
		}
    if (tcpCommunicator::sst[Nodes[NodeNum].socknumber].clID != Nodes[NodeNum].clientID)
        {
        Nodes[NodeNum].connectedstate = 0;
        return -1;
        }
    iRet = send(Nodes[NodeNum].socknumber,Buf,BytesToSend,0);
    YIELD();
    if (iRet<0)
        {
		goto evalConnectionErrors;
        }
    iLength=tcpCommunicator::Ethernet_Receive(Nodes[NodeNum].socknumber,Buf, BytesToReceive, 250);
    if(iLength>0)
        {
        Buf[iLength]=0;
        ResetGlobalError( EC_NO_CONNECTION, ES_WAGO, Nodes[NodeNum].Table->ID);
		Nodes[NodeNum].lastsuccessfullexchange = MyGetMS();
        return iLength;
        }
    else
        {
		goto evalConnectionErrors;
        }
evalConnectionErrors:
	if (Nodes[NodeNum].connectedstate != 0)
		{
		Nodes[NodeNum].connectedstate = 0;
		}
	if (getDeltaT(Nodes[NodeNum].lastsuccessfullexchange) > DISCONNECT_TIME_FOR_ERROR)
		{
		SetGlobalError( EC_NO_CONNECTION, ES_WAGO, Nodes[NodeNum].Table->ID);
		}
	return -1;
    }
#endif
