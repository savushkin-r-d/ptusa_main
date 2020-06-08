/// @file mcaRec.h
/// @brief ���������� ������ @ref TRecipeManager ��� �������� �������� �������������� ������ �������
/// � ����������������� ������ �����������.
///
/// @author  ��������� ����� ��������
///
/// @par ���������� � ������ �����
/// @$Rev: 3612 $.\n
/// @$Author: artem $.\n
/// @$Date: 2013-09-17 15:30:53 +0300 (Tue, 17 Sep 2013) $.
#ifndef mcaRecipes_h__
#define mcaRecipes_h__
#include "param_ex.h"
#include "dtime.h"



///@brief ������������ ����� ����� ��� �������
#define MAX_REC_NAME_LENGTH 32L
///@brief ������ ����� ������ � ������
#define BLOCK_SIZE 128L
///@brief �������� �������� ��������� � �������
#define RECIPE_SAVE_INTERVAL 300000L

///@class TRecipeManager mcaRec.h
///@brief ����� ��� �������� � ������ � ��������� � ����������������� ������ ����������� ��� ���
class TRecipeManager
    {
    public:
    enum RecipeValues
        {
        RV_IS_USED = 0,
        RV_TO_DEFAULTS,
        RV_V1,			//����� V1 (�� ������ �� �������)
        RV_V2,			//����� V2 (�� ������� �� ������)
        RV_OBJ_TYPE,		//��� ������� (����, ����� � ��.)
        RV_FLOW,			//������
        RV_PODP_CIRC,	//����������� �� ����� �� ����� ����������
        RV_DELTA_TR,		//������� ���������� �� ������ � �������� (���� ������ ��������, �� ���������� ������ ������� ����������)
        RV_T_WP,	//����������� ���������������� �������������
        RV_T_WSP,	//����������� ������������� �������� ����� ������
        RV_T_WKP,	//����������� ������������� �������� ����� �������
        RV_T_WOP,	//����������� �������������� �������������
        RV_T_S,	//����������� ����� �������
        RV_T_K,	//����������� ����� ��������
        RV_T_D,	//����������� ����������� �� ����
        RV_T_DEZSR,	//����������� ����������� ��������������� ���������
        RV_DOP_V_PR_OP,	//�������������� ����� ���������������� �������������
        RV_DOP_V_AFTER_S,	//�������������� ����� ������������� ����� ������
        RV_DOP_V_AFTER_K,	//�������������� ����� ������������� ����� �������
        RV_DOP_V_OK_OP,		//�������������� ����� �������������� �������������
        RV_RET_STOP,	//����� �� ����� V1 ��� ��������� ����������� ������
        RV_V_RAB_ML,   //���������� ������ ���� �������� ������ ��� ������� ������������� ���������� ����� ����������
        RV_V_RET_DEL,	//����� ������� ������ ����� ������ ������ V2 �������� ���������� �����
        RV_TM_OP,	//����� ���������� ��� ������������� (�������� �� ������������)
        RV_TM_S,	//����� ���������� �� ������
        RV_TM_K,	//����� ���������� �� �������
        RV_TM_S_SK,	//����� ���������� �� ������ ��� ����� �������+������
        RV_TM_K_SK,	//����� ���������� �� ������� ��� ����� �������+������
        RV_TM_D,	//����� ����������� �� ����
        RV_TM_DEZSR, //����� ����������� ��������������� ���������
        RV_TM_DEZSR_INJECT, //����� �������� ���������������� �������� ��� �����������
        RV_N_RET,		//����� ����������� ������
        RV_N_UPR,		//����� ������� ��������� ����������� ������
        RV_OS,			//�������� �����
        RV_OBJ_EMPTY,   //������ "������ ���������"
        RV_PROGRAM_MASK, //����� ������� ������
        RV_T_RINSING_CLEAN, //����������� ������������� ������ �����
        RV_V_RINSING_CLEAN, //����� ������������� ������ �����

        RV_T_SANITIZER_RINSING,	//����������� ������������� ����� ����� ��������������� ���������
        RV_V_SANITIZER_RINSING,	//����� ������������� ����� ����� ��������������� ���������
        RV_TM_MAX_TIME_OPORBACHOK,	//������������ ����� �� ���������� ������� ������ � ����� �� ���������� �������� ����������� �����

        RV_TM_RET_IS_EMPTY, //����� ���������� ������� �� �������� ��� ��������� ����������� �������
        RV_V_LL_BOT,		//���������� ������ �� ���������� ������� ������ � ����� �� ���������� �������� "����������� �����"
        RV_R_NO_FLOW,		//����������� ������ ��� ������� �������, ��� ��� ���
        RV_TM_R_NO_FLOW,	//����� ������������� ������ "��� ������� �� ������"

        RV_TM_NO_FLOW_R,	//����� ��������� ������ "��� ������� �� ��������"
        RV_TM_NO_CONC,		//����� ��������� ������ "��� ������������ � ���������� �����"

        //-PID1
        RV_PIDP_Z = 47,                 //��������� ��������.
        RV_PIDP_k,                 //�������� k.
        RV_PIDP_Ti,                //�������� Ti.
        RV_PIDP_Td,                //�������� Td.
        RV_PIDP_dt,                //�������� �������
        RV_PIDP_dmax,              //�ax �������� ������� ��������.
        RV_PIDP_dmin,              //�in �������� ������� ��������.
        RV_PIDP_AccelTime,         //����� ������ �� ����� �������������.
        RV_PIDP_IsManualMode,      //������ �����.
        RV_PIDP_UManual,           //�������� ������ �������� ��������� �������.
        RV_PIDP_Uk,                //����� ���.
        //-PID1-!>
        //-PID2
        RV_PIDF_Z,                 //��������� ��������.
        RV_PIDF_k,                 //�������� k.
        RV_PIDF_Ti,                //�������� Ti.
        RV_PIDF_Td,                //�������� Td.
        RV_PIDF_dt,                //�������� �������
        RV_PIDF_dmax,              //�ax �������� ������� ��������.
        RV_PIDF_dmin,              //�in �������� ������� ��������.
        RV_PIDF_AccelTime,         //����� ������ �� ����� �������������.
        RV_PIDF_IsManualMode,      //������ �����.
        RV_PIDF_UManual,           //�������� ������ �������� ��������� �������.
        RV_PIDF_Uk,                //����� ���.
        //-PID2-!>
        RV_TM_MAX_TIME_OPORCIP,	//������������ ����� �������� "����������� ������� CIP"
        RV_SIGNAL_MEDIUM_CHANGE,		//������ out "����� �����"
        RV_SIGNAL_CAUSTIC,			//������ out"������ � �����"
        RV_SIGNAL_ACID,				//������ out"������� � �����"
        RV_SIGNAL_CIP_IN_PROGRESS,	//������ out"����� ����"
        RV_SIGNAL_CIPEND,			//������ out"����� ��������"
        RV_SIGNAL_CIP_READY,			//������ out "����� ������"
        RV_SIGNAL_OBJECT_READY,		//������ in "���������� ������� � �����"
        RV_SIGNAL_SANITIZER_PUMP,	//������ out ���������� ��������� ���������������� ��������
        RV_RESUME_CIP_ON_SIGNAL,		//�������������� ������������� ����� ��� ���������� ������ ������� CIP
        RV_SIGNAL_PUMP_CONTROL,		//������ in analog "���������� ������������������� ��������� ������"
        RV_SIGNAL_DESINSECTION,      //������ out "�����������"
        RV_SIGNAL_OBJECT_PAUSE,      //������ in "�����"
        RV_SIGNAL_CIRCULATION,       //������ out ����������.
        RV_SIGNAL_PUMP_CAN_RUN,      //������ in "�������� ��������� ��������� ������"
        RV_SIGNAL_PUMP_CONTROL_FEEDBACK,    //������ in analog "�������������� ������� ��� ������ ������"
        RV_SIGNAL_RET_PUMP_SENSOR,		//������ ������� ������ ��� ������ ����������� ������
        RV_RET_PUMP_SENSOR_DELAY,        //�������� ���������� ����������� ������ ��� ���������� ������� ������� ������
        RV_SIGNAL_IN_CIP_READY,          //������ in "����� ����"
        RV_SIGNAL_CIPEND2,               //������ "����� �������� 2"
        RV_SIGNAL_CAN_CONTINUE,          //������ ����� ���������� ����� ��� �������� ���������� � ��������
        RV_SIGNAL_WATER,                 //������ "���� � �����"
        RV_RESERV_START, //������ ��������� ����������

        RV_WORKCENTER = 104,		//����� �������� ������ ������� �����
        RV_FIRSTVALVEON = 105,
        RV_LASTVALVEON = 114,
        RV_FIRSTVALVEOFF = 115,
        RV_LASTVALVEOFF = 119,
        };
    private:
        ///@brief ����, ��������������� �� ��������� ���������� �������
        int recipechanged;
        unsigned long recipechangechecktime;
        int lineNo;
        int currentRecipe;
        int curRecipeStartBlock;
        unsigned long lastEvalTime;
        unsigned long recipeStartAddr;
        void SaveRecipeName();

        void FormRecipeList();
        unsigned long startAddr();
        unsigned long startAddr(int recNo);
        unsigned char* recipeMemory;
        unsigned long recipeMemorySize;
        int ReadMem(unsigned long startaddr, unsigned long length, unsigned char* buf);
        int WriteMem(unsigned long startaddr, unsigned long length, unsigned char* buf);
    public:
        ///@brief ��� ����� � ���������
        char* defaultfilename;
        ///@brief ��������� ���� ��� ���� ���������� ��������
        static int startRecipeBlock;
        ///@brief ���������� �������� �� �����
        static int recipePerLine;
        ///@brief ����� ������� � ������
        static int blocksPerRecipe;
        ///@brief ����� ����� �������
        static int recipeNameLength;
        ///@brief ������������� ����� ������ ���������� (�� ���������� ������ �������)
        static int startRecipeParamsOffset;
        ///@brief ����� ��� ����������� ��������
        static unsigned char* recipeCopyBuffer;
        ///@brief ��� �������� �������
        char* currentRecipeName;
        ///@brief ������ �������� ��� �������
        char* recipeList;
        /// @fn  int TRecipeManager::LoadRecipeToParams(int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, TParams* par)
        /// @brief ��������� ��������� ����� ���������� �� ���������� ������� � ��������� ������� � ��������� ���������
        /// @param recipeNo - ����� �������
        /// @param recipeStartPos - � ������� ��������� ������� ���������� ��������
        /// @param paramsStartPos - ��������� ������� � �������
        /// @param parQuantity - ���������� ����������� ����������
        /// @param par - ��������� �� ����� ���������� TParams
        /// @return   int - ���������� 0 ��� ������ � !0 ��� �������� ����������
        int LoadRecipeToParams(int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, run_time_params_float* par );
        /// @fn  int TRecipeManager::getCurrentRecipe()
        /// @brief ���������� ����� �������� �������
        /// @return   int ����� �������
        int getCurrentRecipe();
        /// @fn  int TRecipeManager::setCurrentRecipe(int recipeNo)
        /// @brief ������ ������� ������
        /// @param recipeNo ����� �������
        /// @return   int 0 - �������� ����� ������� > ������������� ���������� ��������
        int setCurrentRecipe(int recipeNo);
        /// @fn  float TRecipeManager::getRecipeValue(int recNo, int valueNo)
        /// @brief ���������� �������� ��������� ���������� �������
        /// @param recNo ����� �������
        /// @param valueNo ����� ���������
        /// @return   float �������� ���������
        float getRecipeValue(int recNo, int valueNo);
        /// @fn  float TRecipeManager::getValue(int valueNo)
        /// @brief ���������� �������� ��������� �������� �������
        /// @param valueNo ����� ���������
        /// @return   float �������� ���������
        float getValue(int valueNo);
        /// @fn  int TRecipeManager::getRecipeName(int recNO, char* recName)
        /// @brief �������� ������ � ������ �������
        /// @param recNO ����� �������
        /// @param recName ��������� �� ������, � ������� ����� ������� ������
        /// @return   int 0 - ������ !0 - ��
        int getRecipeName(int recNO, char* recName);
        /// @fn  int TRecipeManager::setRecipeValue(int recNo, int valueNo, float newValue)
        /// @brief ������������� �������� ��������� ��������� ��������� �������
        /// @param recNo ����� �������
        /// @param valueNo ����� ���������
        /// @param newValue ����� ��������
        /// @return   int 0 - ������ !0 - ��
        int setRecipeValue(int recNo, int valueNo, float newValue);
        /// @fn  int TRecipeManager::setValue(int valueNo, float newValue)
        /// @brief ������������� �������� ��������� ��������� �������� �������
        /// @param valueNo ����� ���������
        /// @param newValue ����� ��������
        /// @return   int 0 - ������ !0 - ��
        int setValue(int valueNo, float newValue);
        /// @fn  int TRecipeManager::NextRecipe()
        /// @brief ������� � ���������� �������
        /// @return   int 0 - ������ !0 - ��
        int NextRecipe();
        /// @fn  int TRecipeManager::PrevRecipe()
        /// @brief ������� � ����������� �������
        /// @return   int 0 - ������ !0 - ��
        int PrevRecipe();
        /// @fn  int TRecipeManager::ToRecipe(int recNo)
        /// @brief ������� � ���������� �������
        /// @param recNo - ����� �������
        /// @return   int 0 - ������ !0 - ��
        int ToRecipe(int recNo);
        /// @fn  int TRecipeManager::OnRecipeDevices(int recipeNo)
        /// @brief �������� ���������� �������
        /// @param recipeNo ����� �������
        /// @return   int 0 - ������ !0 - ��
        int OnRecipeDevices(int recipeNo, int msaline = 1);
        /// @fn  int TRecipeManager::OffRecipeDevices(int recipeNo)
        /// @brief ��������� ���������� �������
        /// @param recipeNo ����� �������
        /// @return   int 0 - ������ !0 - ��
        int OffRecipeDevices(int recipeNo, int msaline = 1);
        /// @fn  int TRecipeManager::GetParamsCount()
        /// @brief ���������� ���������� ���������� ��� �������
        /// @return   int ���������� ���������� �������
        int GetParamsCount();
        /// @fn  int TRecipeManager::ResetRecipeToDefaults(int recipeNo)
        /// @brief ���������� �������� ���������� ���������� ������� �� �������� ��-���������
        /// @param recipeNo ����� �������
        /// @return   int 0 - ������ !0 - ��
        int ResetRecipeToDefaults(int recipeNo);
        /// @fn  void TRecipeManager::LoadRecipeName()
        /// @brief ��������� ��� ������� �� ����������������� ������
        /// @return   void
        void LoadRecipeName();
        /// @fn  void TRecipeManager::EvalRecipe()
        /// @brief ��������� ��������, ������������ ��������� ������� ��� ������� � ����������������� ������ � ��������� ������ ��������
        /// @return   void
        void EvalRecipe();
        /// @fn  void TRecipeManager::CopyRecipe()
        /// @brief �������� ������� ������ � �����
        /// @return   void
        void CopyRecipe();
        /// @fn  void TRecipeManager::PasteRecipe()
        /// @brief ������������ ������� ������ ���������� �� �������
        /// @return   void
        void PasteRecipe();
        /// @fn int TRecipeManager::SaveToFile()
        /// @brief ���������� �������� ������ � ����
        /// @return ���������� 0 � ������ ��������� ����������
        int SaveToFile(const char* filename);
        /// @fn int TRecipeManager::LoadFromFile()
        /// @brief �������� �������� �� ������������ �����
        /// @return ���������� 0 � ������ ��������� ����������
        int LoadFromFile(const char* filename);
        /// @fn  void TRecipeManager::NullifyRecipe()
        /// @brief �������� ������� ������
        /// @return   void
        void NullifyRecipe();
        /// @fn   TRecipeManager::TRecipeManager(int lineNo)
        /// @brief ����������� ������
        /// @param lineNo ����� ����� �����, ���������� � 0. �� ���� ������� ������������ �������� � ������
        /// @return
        TRecipeManager(int lineNo);
        ~TRecipeManager();
    };


    class TMediumRecipeManager
    {
    public:
        enum RecipeValues
        {
            RV_IS_USED = 0,
            RV_TO_DEFAULTS,
            RV_P_TYPE,                       //��� ��������
            RV_P_CZAD,                       //������������ �������� ��������
            RV_P_CMIN,                       //����������� ������������ �������� ��������
            RV_P_CKANAL,                     //������������ ������������ �������� ��� �����������
            RV_P_ALF,                        //����������� ������������� �����������
            RV_P_K,                          //������������ ������������������ ��������
            RV_P_RO,                         //��������� ������������������ ��������
            RV_P_CONC25,                     //������������ � �������� �����
            RV_P_MS25,                     //������������ � �������� �����
        };

        enum MediumTypes
        {
            MT_CAUSTIC = 0,
            MT_ACID,
        };
    private:
        ///@brief ����, ��������������� �� ��������� ���������� �������
        int recipechanged;
        int mediumType;
        unsigned long recipechangechecktime;
        int currentRecipe;
        int curRecipeStartBlock;
        unsigned long lastEvalTime;
        unsigned long recipeStartAddr;
        void SaveRecipeName();

        void FormRecipeList();
        unsigned long startAddr();
        unsigned long startAddr(int recNo);
        unsigned char* recipeMemory;
        unsigned long recipeMemorySize;
        int ReadMem(unsigned long startaddr, unsigned long length, unsigned char* buf);
        int WriteMem(unsigned long startaddr, unsigned long length, unsigned char* buf);
    public:
        char* defaultfilename;
        ///@brief ��������� ���� ��� ���� ���������� ��������
        static int startRecipeBlock;
        ///@brief ���������� �������� �� �����
        static int recipePerLine;
        ///@brief ����� ������� � ������
        static int blocksPerRecipe;
        ///@brief ����� ����� �������
        static int recipeNameLength;
        ///@brief ������������� ����� ������ ���������� (�� ���������� ������ �������)
        static int startRecipeParamsOffset;
        ///@brief ����� ��� ����������� ��������
        static unsigned char* recipeCopyBuffer;
        ///@brief ��� �������� �������
        char* currentRecipeName;
        ///@brief ������ �������� ��� �������
        char* recipeList;
        /// @fn  int TRecipeManager::LoadRecipeToParams(int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, TParams* par)
        /// @brief ��������� ��������� ����� ���������� �� ���������� ������� � ��������� ������� � ��������� ���������
        /// @param recipeNo - ����� �������
        /// @param recipeStartPos - � ������� ��������� ������� ���������� ��������
        /// @param paramsStartPos - ��������� ������� � �������
        /// @param parQuantity - ���������� ����������� ����������
        /// @param par - ��������� �� ����� ���������� TParams
        /// @return   int - ���������� 0 ��� ������ � !0 ��� �������� ����������
        int LoadRecipeToParams(int recipeNo, saved_params<float, true> par);
        /// @fn  int TRecipeManager::getCurrentRecipe()
        /// @brief ���������� ����� �������� �������
        /// @return   int ����� �������
        int getCurrentRecipe();
        /// @fn  int TRecipeManager::setCurrentRecipe(int recipeNo)
        /// @brief ������ ������� ������
        /// @param recipeNo ����� �������
        /// @return   int 0 - �������� ����� ������� > ������������� ���������� ��������
        int setCurrentRecipe(int recipeNo);
        /// @fn  float TRecipeManager::getRecipeValue(int recNo, int valueNo)
        /// @brief ���������� �������� ��������� ���������� �������
        /// @param recNo ����� �������
        /// @param valueNo ����� ���������
        /// @return   float �������� ���������
        float getRecipeValue(int recNo, int valueNo);
        /// @fn  float TRecipeManager::getValue(int valueNo)
        /// @brief ���������� �������� ��������� �������� �������
        /// @param valueNo ����� ���������
        /// @return   float �������� ���������
        float getValue(int valueNo);
        /// @fn  int TRecipeManager::getRecipeName(int recNO, char* recName)
        /// @brief �������� ������ � ������ �������
        /// @param recNO ����� �������
        /// @param recName ��������� �� ������, � ������� ����� ������� ������
        /// @return   int 0 - ������ !0 - ��
        int getRecipeName(int recNO, char* recName);
        /// @fn  int TRecipeManager::setRecipeValue(int recNo, int valueNo, float newValue)
        /// @brief ������������� �������� ��������� ��������� ��������� �������
        /// @param recNo ����� �������
        /// @param valueNo ����� ���������
        /// @param newValue ����� ��������
        /// @return   int 0 - ������ !0 - ��
        int setRecipeValue(int recNo, int valueNo, float newValue);
        /// @fn  int TRecipeManager::setValue(int valueNo, float newValue)
        /// @brief ������������� �������� ��������� ��������� �������� �������
        /// @param valueNo ����� ���������
        /// @param newValue ����� ��������
        /// @return   int 0 - ������ !0 - ��
        int setValue(int valueNo, float newValue);
        /// @fn  int TRecipeManager::NextRecipe()
        /// @brief ������� � ���������� �������
        /// @return   int 0 - ������ !0 - ��
        int NextRecipe();
        /// @fn  int TRecipeManager::PrevRecipe()
        /// @brief ������� � ����������� �������
        /// @return   int 0 - ������ !0 - ��
        int PrevRecipe();
        /// @fn  int TRecipeManager::ToRecipe(int recNo)
        /// @brief ������� � ���������� �������
        /// @param recNo - ����� �������
        /// @return   int 0 - ������ !0 - ��
        int ToRecipe(int recNo);
        /// @fn  int TRecipeManager::OnRecipeDevices(int recipeNo)
        /// @brief �������� ���������� �������
        /// @param recipeNo ����� �������
        /// @return   int 0 - ������ !0 - ��
        int OnRecipeDevices(int recipeNo, int msaline = 1);
        /// @fn  int TRecipeManager::OffRecipeDevices(int recipeNo)
        /// @brief ��������� ���������� �������
        /// @param recipeNo ����� �������
        /// @return   int 0 - ������ !0 - ��
        int OffRecipeDevices(int recipeNo, int msaline = 1);
        /// @fn  int TRecipeManager::GetParamsCount()
        /// @brief ���������� ���������� ���������� ��� �������
        /// @return   int ���������� ���������� �������
        int GetParamsCount();
        /// @fn  int TRecipeManager::ResetRecipeToDefaults(int recipeNo)
        /// @brief ���������� �������� ���������� ���������� ������� �� �������� ��-���������
        /// @param recipeNo ����� �������
        /// @return   int 0 - ������ !0 - ��
        int ResetRecipeToDefaults(int recipeNo);
        /// @fn  void TRecipeManager::LoadRecipeName()
        /// @brief ��������� ��� ������� �� ����������������� ������
        /// @return   void
        void LoadRecipeName();
        /// @fn  void TRecipeManager::EvalRecipe()
        /// @brief ��������� ��������, ������������ ��������� ������� ��� ������� � ����������������� ������ � ��������� ������ ��������
        /// @return   void
        void EvalRecipe();
        /// @fn  void TRecipeManager::CopyRecipe()
        /// @brief �������� ������� ������ � �����
        /// @return   void
        void CopyRecipe();
        /// @fn  void TRecipeManager::PasteRecipe()
        /// @brief ������������ ������� ������ ���������� �� �������
        /// @return   void
        void PasteRecipe();
        /// @fn int TRecipeManager::SaveToFile()
        /// @brief ���������� �������� ������ � ����
        /// @return ���������� 0 � ������ ��������� ����������
        int SaveToFile(const char* filename);
        /// @fn int TRecipeManager::LoadFromFile()
        /// @brief �������� �������� �� ������������ �����
        /// @return ���������� 0 � ������ ��������� ����������
        int LoadFromFile(const char* filename);
        /// @fn  void TRecipeManager::NullifyRecipe()
        /// @brief �������� ������� ������
        /// @return   void
        void NullifyRecipe();
        /// @fn   TRecipeManager::TRecipeManager(int lineNo)
        /// @brief ����������� ������
        /// @param lineNo ����� ����� �����, ���������� � 0. �� ���� ������� ������������ �������� � ������
        /// @return
        TMediumRecipeManager(MediumTypes mType);
        ~TMediumRecipeManager();
    };


#endif // mcaRecipes_h__
    
