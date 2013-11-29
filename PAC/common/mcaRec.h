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
		RV_RESERV_START, //������ ��������� ����������
		RV_FIRSTVALVEON = 41,
		RV_LASTVALVEON = 50,
		RV_FIRSTVALVEOFF = 51,
		RV_LASTVALVEOFF = 55,
		};
	private:
#ifdef WIN_OS
		FILE* memFile;
		char* memFileName;
#endif // WIN_OS
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
		int OnRecipeDevices(int recipeNo);
		/// @fn  int TRecipeManager::OffRecipeDevices(int recipeNo)
		/// @brief ��������� ���������� �������
		/// @param recipeNo ����� �������
		/// @return   int 0 - ������ !0 - ��
		int OffRecipeDevices(int recipeNo);
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


#endif // mcaRecipes_h__
