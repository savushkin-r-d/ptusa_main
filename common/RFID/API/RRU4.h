#ifndef RRU4_H
#define RRU4_H

#ifdef __GNUC__
	#define __declspec(x)		__attribute__((visibility("default")))
#endif

// ---------------------------------------------------------------------------
//
// Reader DLL
// -----------
//
// Defines the interface to the Kathrein Sachsen UHF RFID Reader
//
//
// Version v1.0
//
// Autor:	Dipl.-Inf. Maik H?nig
//			Dipl.-Ing. Sebastian Drey?ig
//			Dipl.-Ing. (FH) Holger Busse
//			M.Sc. Jan Kuhnert
//
// (c) 2006 - 2018 KATHREIN Sachsen GmbH
// ---------------------------------------------------------------------------

// $Id: RRU4.h,v 1.89 2018/01/30 11:10:09 hbusse Exp $

// ---------------------------------------------------------------------------
// includes

#include "globtype.h"
#include "rru4api.h"

// ===========================================================================
// defines

// version
#define VER_DATA	"VER_HVER:3 VER_LVER:02 VER_SVER:00 VER_BUILD:1212"

#define EPCWORDLENGTH	31
#define PCWORDLENGTH	3

#define RRU4			void *

#ifdef STDCALL
#define CALLCONVENTION	__stdcall
#else
#define CALLCONVENTION
#endif

// ===========================================================================
// typedefs

typedef struct
{
	TByte ubEPCWordLength;
	TWord rguwEPC[EPCWORDLENGTH];
} tEPC;

typedef struct
{
	TDWord udwMessageLength;
	TByte *pubMessage;
} tGen2V2Message;

typedef struct
{
	TBool bExecutionStatus;
	TBool bTransmissionCounter;
	TByte rgubCiphertext[32];
} tSJ5511Data;

typedef enum : TByte
{
	CT_RS232 = 0,						//RS232 connection
	CT_USB,								//USB connection
	CT_EthernetGen2,					//Ethernet connection generation 2 reader
	CT_EthernetGen3,					//Ethernet connection generation 3 reader
	CT_EthernetAuto,					//Ethernet connection, auto mode
	CT_None = 0xFF
} tCommunicationType;

typedef enum : TByte
{
	IPAT_IPv4 = 0,
	IPAT_IPv6
} tIPAddressType;

typedef struct stEPCListEntry
{
	TByte ubPort;
	TByte ubRSSI;
	TDWord udwTimestamp;
	TWord rguwPC[PCWORDLENGTH];
	tEPC stEPC;
	tTagErrorCode enTagErrorCode;
	TWord uwReadCount;
	TByte ubCountWords;
	TWord *puwData;
	struct stEPCListEntry *pNext;
	TDWord udwFrequency;
	TWord uwTagPhase;
	tGen2V2Message stGen2V2Message;
	TsWord swRSSIdBmValue;
	tSJ5511Data *pstSJ5511Data;
} tEPCListEntry;

typedef struct stPortMultiplexAndExposureTimeListEntry
{
	TByte ubPort;
	TWord uwExposureTime;
	struct stPortMultiplexAndExposureTimeListEntry *pNext;
} tPortMultiplexAndExposureTimeListEntry;

typedef struct stProfileListEntry
{
	TByte ubProfile;
	char *pcDescription;
	struct stProfileListEntry *pNext;
} tProfileListEntry;

typedef struct stModuleDescriptionListEntry
{
	char *pcModuleDescription;
	struct stModuleDescriptionListEntry *pNext;
} tModuleDescriptionListEntry;

typedef struct
{
	tCommunicationType enCommunicationType;
	union
	{
		struct
		{
			TByte ubPort;
			TDWord udwBaudRate;
			tUartParity enUartParity;
			tUartStopBits enUartStopBits;
			tUartFlowControl enUartFlowControl;
		} stRS232;

		struct
		{
			TDWord udwDeviceNumber;
		} stUSB;

		struct
		{
			tIPAddressType enIPAddressType;
			TBool bIsIPAddress;
			union
			{
				TByte rgubIPv4Address[4];
				TByte rgubIPv6Address[16];
				char rgcNetworkName[256];
			} uKindOfConnection;
			TDWord udwKeepAliveTime;
			char *pcSSHAuth;
			char *pcSSHUser;
			TWord uwPort;
		} stEthernet;
	} ustCommunicationConfigParameters;
} tCommunicationConfigData;

typedef enum : TByte
{
	REC_NoError = 0,
	REC_NotInitialized,
	REC_NoValidHandle,
	REC_CommunicationError = 5,
	REC_NoMemory = 8,
	REC_WrongParameters = 10
} tReaderErrorCode;

typedef enum : TByte
{
	IE_NoError = 0,
	IE_CommunicationLost,
	IE_CommunicationError
} tInternalError;

typedef enum : TByte
{
	C_Unknown = 0,
	C_Yes,
	C_No
} tConnectable;

typedef struct
{
	char rgcNetworkName[256];
	TBool bUseDHCP;
	TByte rgubIPAddress[4];
	TByte rgubSubnetMask[4];
	TByte rgubGateway[4];
	TByte rgubMACAddress[6];
	tConnectable enConnectable;
	char rgcInterface[16];

	struct
	{
		struct
		{
			TByte rgubIPAddress[16];
			TByte ubSubnetPrefixLength;
			tConnectable enConnectable;
		} rgstIPAddresses[2];

		TByte rgubGateway[16];
	} stIPv6;
} tFoundReaderIP;

typedef struct
{
	const char *pcConfigIdName;
	const TDWord udwConfigIdValue;
} tConfigIdNameAndValue;

typedef void (CALLCONVENTION tDllResultHandlerGetMode)(tCommandFunctionMode enCommandFunctionMode, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetMode)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetCommunicationStandard)(tCommunicationStandard enCommunicationStandard, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetCommunicationStandard)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetPortPower)(tResultFlag enResultFlag, TByte ubPort, TByte ubPortPower, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetPortPower)(tResultFlag enResultFlag, TByte ubPort, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetCarrierFollowUpTime)(TWord uwCarrierFollowUpTime, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetCarrierFollowUpTime)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetAntennaMode)(tAntennaMode enAntennaModeSync, tAntennaMode enAntennaModeAsync, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetAntennaMode)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetPortMultiplexSequenceAndExposureTime)(tPortMultiplexAndExposureTimeListEntry *pPortMultiplexAndExposureTimeList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetPortMultiplexSequenceAndExposureTime)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetCableLossAndAntennaGain)(tResultFlag enResultFlag, TByte ubPort, TByte ubCableLoss, TsByte sbAntennaGain, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetCableLossAndAntennaGain)(tResultFlag enResultFlag, TByte ubPort, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetETSIPortChannelList)(tResultFlag enResultFlag, TByte ubPort, TByte *pubChannelList, TByte ubChannelListLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetETSIPortChannelList)(tResultFlag enResultFlag, TByte ubPort, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetETSIPortChannelSwitchingMode)(tETSIPortChannelSwitchingMode enETSIPortChannelSwitchingMode, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetETSIPortChannelSwitchingMode)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetProfileList)(tProfileListEntry *pProfileList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetProfile)(TByte ubProfile, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetProfile)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetModulationType)(tModulationType enModulationType, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetModulationType)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetExtendedResultFlag)(TByte ubExtendedResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetExtendedResultFlag)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetErrorStatus)(TDWord udwErrorStatus, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetDefaultParameterSet)(TByte ubParameterSet, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetDefaultParameterSet)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetActiveParameterSet)(TByte ubParameterSet, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetActiveParameterSet)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSaveActiveParameterSet)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetParameterByConfigId)(tResultFlag enResultFlag, TDWord udwId, TDWord udwValue, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetParameterByConfigId)(tResultFlag enResultFlag, TDWord udwId, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetIOCardHwConfig)(tResultFlag enResultFlag, TByte ubIOCard, tIOCardType enIOCardType, tIOCardService enIOCardService, TByte *pubData, TDWord udwDataLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetIOCardHwConfig)(tResultFlag enResultFlag, TByte ubIOCard, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetIOCardProtocolConfig)(tResultFlag enResultFlag, TByte ubIOCard, TByte *pubData, TDWord udwDataLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetIOCardProtocolConfig)(tResultFlag enResultFlag, TByte ubIOCard, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetTime)(TsDWord sdwSeconds, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetTime)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetTemperature)(TsWord swTemperature, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetSwVersion)(TWord uwHVer, TWord uwLVer, TWord uwSubVer, TDWord udwBuild, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetHwVersion)(char *pcCPUDescription, char *pcPADescription, tModuleDescriptionListEntry *pModulDesciptionList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetLicenseKey)(char *pcLicenseKey, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetLicenseKey)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetIOCardExtendedInfo)(tResultFlag enResultFlag, TByte ubIOCard, TByte *pubData, TDWord udwDataLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetConfigPassword)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerEnterConfigPassword)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerLoadFactoryDefaults)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerFirmwareUpdatePrepare)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerFirmwareUpdateTransmitData)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerFirmwareUpdateFlash)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerRebootSystem)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerActivateBuzzer)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetDeviceSerialNumber)(tResultFlag enResultFlag, char *pcSerialNumber, char *pcRevision, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetDeviceTypeNumber)(tResultFlag enResultFlag, char *pcTypeNumber, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetLEDMode)(tResultFlag enResultFlag, tLEDMode enLEDMode, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetLEDMode)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetUptimeInMilliseconds)(TDWord udwMilliseconds, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllResultHandlerGetSelSessionAndTarget)(TByte ubSel, TByte ubSession, TByte ubTarget, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetSelSessionAndTarget)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetInitialQ)(TByte ubInitialQ, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetInitialQ)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetMaxAirCommErrorCount)(TByte ubMaxAirCommErrorCount, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetMaxAirCommErrorCount)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetASyncObservedListParameters)(TByte ubGlimpsedTimeoutCount, TByte ubObservedThresholdCount, TByte ubObservedTimeoutCount, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetASyncObservedListParameters)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetSelectFilterOnOff)(tResultFlag enResultFlag, TByte ubFilter, tOnOffFlag enOnOffFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetSelectFilterOnOff)(tResultFlag enResultFlag, TByte ubFilter, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetSelectFilterData)(tResultFlag enResultFlag, TByte ubFilter, tOnOffFlag enOnOffFlag, TByte ubTarget, TByte ubAction, tTagMemoryBank enTagMemoryBank, TDWord udwBitPointer, TByte ubMaskBitLength, TByte *pubMaskData, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetSelectFilterData)(tResultFlag enResultFlag, TByte ubFilter, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGPIOGetIOData)(tResultFlag enResultFlag, TByte ubIOCard, TWord uwPhysicalInputs, TWord uwLogicalInputs, TWord uwDebouncedInputs, TWord uwLogicalOutputs, TWord uwPhysicalOutputs, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGPIOSetOutput)(tResultFlag enResultFlag, TByte ubIOCard, TByte ubOutput, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGPIOAddActionToActionlist)(tResultFlag enResultFlag, TByte ubActionlist, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGPIOClearActionlist)(tResultFlag enResultFlag, TByte ubActionlist, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGPIOAssignInputToActionlist)(tResultFlag enResultFlag, TByte ubIOCard, TByte ubInput, TByte ubEdge, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGPIOGetActionlist)(tResultFlag enResultFlag, TByte ubActionlist, TDWord udwActionsLength, TByte *pubActions, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGPIOSetActionlist)(tResultFlag enResultFlag, TByte ubActionlist, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetLED)(tResultFlag enResultFlag, TByte ubLEDNumber, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerKRAIInitializePort)(tResultFlag enResultFlag, TByte ubPort, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerKRAIGetPortInfo)(tResultFlag enResultFlag, TByte ubPort, TWord uwAntennaId, TByte ubAntennaCount, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerKRAISetLED)(tResultFlag enResultFlag, TByte ubPort, TByte ubLEDNumber, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerKRAIGetSwVersion)(tResultFlag enResultFlag, TWord uwHVer, TWord uwLVer, TWord uwSubVer, TDWord udwBuild, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllResultHandlerSyncGetEPCs)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncBulkGetEPCs)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncReadDataAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncReadDataUntilEndOfBankAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncReadDataSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncReadDataUntilEndOfBankSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncWriteDataAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncWriteDataSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncWriteMaskedDataAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncWriteMaskedDataSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncBlockEraseAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncBlockEraseSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncLockAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncLockSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncKillAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncKillSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncBlockPermalockAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncBlockPermalockSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncAuthenticateAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncAuthenticateSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncUntraceableAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncUntraceableSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncSJ5511ActivateSecureModeAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncSJ5511ActivateSecureModeSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncSJ5511AuthenticateOBUAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncSJ5511AuthenticateOBUSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncWriteEPCSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncWriteEPCToSingleTag)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncNXPG2XReadProtectAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncNXPG2XReadProtectSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncNXPG2XResetReadProtectAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncNXPG2XResetReadProtectSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncNXPG2XChangeEASAny)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncNXPG2XChangeEASSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncNXPG2XEASAlarm)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSyncRSSILedBarSpecific)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncGetRawEPCsSync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncGetRawEPCsASync)(TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncGetEPCsSync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncGetEPCsASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncReadDataAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncReadDataAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncReadDataUntilEndOfBankAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncReadDataUntilEndOfBankAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncWriteDataAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncWriteDataAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncWriteMaskedDataAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncWriteMaskedDataAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncBlockEraseAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncBlockEraseAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncLockAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncLockAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncKillAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncKillAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncBlockPermalockAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncBlockPermalockAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncAuthenticateAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncAuthenticateAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncUntraceableAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncUntraceableAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncSJ5511ActivateSecureModeAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncSJ5511ActivateSecureModeAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncSJ5511AuthenticateOBUAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncSJ5511AuthenticateOBUAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XReadProtectAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XReadProtectAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XResetReadProtectAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XResetReadProtectAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XChangeEASAnySync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XChangeEASAnyASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XEASAlarmSync)(tResultFlag enResultFlag, tStartStopHeartbeatFlag enStartStopHeartbeatFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncNXPG2XEASAlarmASync)(tComingGoingFlag enComingGoingFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncStopCommand)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerASyncGetTagRate)(tResultFlag enResultFlag, TWord uwTagRate, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllResultHandlerGetAntennaList)(TByte ubPortMask, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetAntenna)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSelectFrequencyAndSwitchCarrierOn)(tResultFlag enResultFlag, TDWord udwFrequency, TWord uwChannelTime, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerSetFrequency)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetNoiseValue)(tResultFlag enResultFlag, TByte ubNoiseValue, TsWord swNoiseLevel, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerGetETSILBTReferenceNoiseValue)(TByte ubETSILBTReferenceNoiseValue, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerCarrierOnOff)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerRandomModulationOnOff)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerMeasureTxPower)(float fTxPower, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerMeasureRxPower)(float fRxPower, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerMeasureVSWR)(float fVSWR, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerStartInventory)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerOpenNoninventoriedTag)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerOpenSpecificTag)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerDirectBulkGetEPCs)(tResultFlag enResultFlag, TByte ubExtendedResultFlag, tEPCListEntry *pEPCList, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerExecuteAccessSequence)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerExecuteKillSequence)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerExecuteWriteSequence)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandSelect)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandACK)(tResultFlag enResultFlag, TWord *puwPC, tEPC epc, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandNAK)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandReqRN)(tResultFlag enResultFlag, TWord uwRN, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandAccess)(tResultFlag enResultFlag, TWord uwHandle, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandRead)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, TWord uwHandle, TByte ubDataLength, TWord *puwData, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandWrite)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, TWord uwHandle, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandKill)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, TWord uwHandle, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandLock)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, TWord uwHandle, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandBlockWrite)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, TWord uwHandle, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandBlockErase)(tResultFlag enResultFlag, tTagErrorCode enTagErrorCode, TWord uwHandle, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandQuery)(tResultFlag enResultFlag, TWord uwRN, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandQueryAdjust)(tResultFlag enResultFlag, TWord uwRN, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandQueryRep)(tResultFlag enResultFlag, TWord uwRN, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerTxGen2CommandCustomCommand)(tResultFlag enResultFlag, TByte *pubData, TDWord udwDataLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerEN302208TestmodulationOnOff)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllResultHandlerMessage)(TDWord udwTimestamp, TByte ubMessageType, tErrorMessageId enErrorMessageId, tComingGoingFlag enComingGoingFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerInternalError)(tInternalError enInternalError, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllResultHandlerReaderCustomCommand)(TWord uwCommandId, TByte *pubData, TDWord udwDataLength, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllEventHandlerUSBDevice)(TDWord udwNumberOfDevices, TDWord *pudwDevices);
typedef void (CALLCONVENTION tDllResultHandlerFoundReaderIP)(tFoundReaderIP *pSearchReaderIP);
typedef void (CALLCONVENTION tDllResultHandlerSendFactoryDefaultBroadcast)(TByte ubResultFlag, TByte *pubScrambledMAC, char *pcInfo);

typedef void (CALLCONVENTION tDllResultHandlerRawReceiveBlock)(TDWord udwConnectionInfo, TByte *pubData, TDWord udwDataLength, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllResultHandlerHSMEncrypt)(tResultFlag enResultFlag, TByte *pubCiphertext, TDWord udwCiphertextLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerHSMDecrypt)(tResultFlag enResultFlag, TByte *pubPlaintext, TDWord udwPlaintextLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerHSMGetRandomData)(tResultFlag enResultFlag, TByte *pubRandomData, TDWord udwRandomDataLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerHSMGetChallenge)(tResultFlag enResultFlag, TByte *pubChallenge, TDWord udwChallengeLength, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerHSMAuthenticate)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerHSMSetPublicCertificate)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerHSMSetWrappingKey)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);
typedef void (CALLCONVENTION tDllResultHandlerHSMSetAESKey)(tResultFlag enResultFlag, RRU4 rru4Handle, void *pTag);

typedef void (CALLCONVENTION tDllResultHandlerKBRPTInterProcComm)(tResultFlag enResultFlag, TWord uwClientId, TByte *pubData, TDWord udwDataLength, RRU4 rru4Handle, void *pTag);

// ===========================================================================
// functions

#ifdef __cplusplus
extern "C"
{
#endif

// ---------------------------------------------------------------------------

__declspec(dllexport) void CALLCONVENTION GetLibraryVersion(TWord *puwHVer,TWord *puwLVer,TWord *puwSubVer,TDWord *pudwBuild);

// ---------------------------------------------------------------------------

__declspec(dllexport) RRU4 CALLCONVENTION ConnectReader(tCommunicationConfigData *pCommunicationConfigData, void *pTag);
__declspec(dllexport) RRU4 CALLCONVENTION ConnectReaderEx(tCommunicationConfigData *pCommunicationConfigData, TsDWord *psdwError, void *pTag);
__declspec(dllexport) RRU4 CALLCONVENTION RawConnectReader(tCommunicationConfigData *pCommunicationConfigData, void *pTag);
__declspec(dllexport) RRU4 CALLCONVENTION RawConnectReaderEx(tCommunicationConfigData *pCommunicationConfigData, TsDWord *psdwError, void *pTag);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION DisconnectReader(RRU4 rru4Handle);
__declspec(dllexport) tCommunicationType CALLCONVENTION GetCommunicationType(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerRawReceiveBlock(tDllResultHandlerRawReceiveBlock *pDllResultHandlerRawReceiveBlock);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION RawTxBlock(RRU4 rru4Handle, TDWord udwConnectionInfo, TByte *pubData, TDWord udwDataLength);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetMode(tDllResultHandlerGetMode *pDllResultHandlerGetMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetMode(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetMode(tDllResultHandlerSetMode *pDllResultHandlerSetMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetMode(RRU4 rru4Handle, tCommandFunctionMode enCommandFunctionMode);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetCommunicationStandard(tDllResultHandlerGetCommunicationStandard *pDllResultHandlerGetCommunicationStandard);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetCommunicationStandard(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetCommunicationStandard(tDllResultHandlerSetCommunicationStandard *pDllResultHandlerSetCommunicationStandard);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetCommunicationStandard(RRU4 rru4Handle, tCommunicationStandard enCommunicationStandard);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetPortPower(tDllResultHandlerGetPortPower *pDllResultHandlerGetPortPower);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetPortPower(RRU4 rru4Handle, TByte ubPort);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetPortPower(tDllResultHandlerSetPortPower *pDllResultHandlerSetPortPower);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetPortPower(RRU4 rru4Handle, TByte ubPort, TByte ubPortPower);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetCarrierFollowUpTime(tDllResultHandlerGetCarrierFollowUpTime *pDllResultHandlerGetCarrierFollowUpTime);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetCarrierFollowUpTime(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetCarrierFollowUpTime(tDllResultHandlerSetCarrierFollowUpTime *pDllResultHandlerSetCarrierFollowUpTime);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetCarrierFollowUpTime(RRU4 rru4Handle, TWord uwCarrierFollowUpTime);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetAntennaMode(tDllResultHandlerGetAntennaMode *pDllResultHandlerGetAntennaMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetAntennaMode(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetAntennaMode(tDllResultHandlerSetAntennaMode *pDllResultHandlerSetAntennaMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetAntennaMode(RRU4 rru4Handle, tAntennaMode enAntennaModeSync, tAntennaMode enAntennaModeAsync);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetPortMultiplexSequenceAndExposureTime(tDllResultHandlerGetPortMultiplexSequenceAndExposureTime *pDllResultHandlerGetPortMultiplexSequenceAndExposureTime);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetPortMultiplexSequenceAndExposureTime(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetPortMultiplexSequenceAndExposureTime(tDllResultHandlerSetPortMultiplexSequenceAndExposureTime *pDllResultHandlerSetPortMultiplexSequenceAndExposureTime);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetPortMultiplexSequenceAndExposureTime(RRU4 rru4Handle, tPortMultiplexAndExposureTimeListEntry *pPortMultiplexAndExposureTimeList);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetCableLossAndAntennaGain(tDllResultHandlerGetCableLossAndAntennaGain *pDllResultHandlerGetCableLossAndAntennaGain);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetCableLossAndAntennaGain(RRU4 rru4Handle, TByte ubPort);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetCableLossAndAntennaGain(tDllResultHandlerSetCableLossAndAntennaGain *pDllResultHandlerSetCableLossAndAntennaGain);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetCableLossAndAntennaGain(RRU4 rru4Handle, TByte ubPort, TByte ubCableLoss, TsByte sbAntennaGain);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetETSIPortChannelList(tDllResultHandlerGetETSIPortChannelList *pDllResultHandlerGetETSIPortChannelList);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetETSIPortChannelList(RRU4 rru4Handle, TByte ubPort);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetETSIPortChannelList(tDllResultHandlerSetETSIPortChannelList *pDllResultHandlerSetETSIPortChannelList);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetETSIPortChannelList(RRU4 rru4Handle, TByte ubPort, TByte *pubChannelList, TByte ubChannelListLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetETSIPortChannelSwitchingMode(tDllResultHandlerGetETSIPortChannelSwitchingMode *pDllResultHandlerGetETSIPortChannelSwitchingMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetETSIPortChannelSwitchingMode(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetETSIPortChannelSwitchingMode(tDllResultHandlerSetETSIPortChannelSwitchingMode *pDllResultHandlerSetETSIPortChannelSwitchingMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetETSIPortChannelSwitchingMode(RRU4 rru4Handle, tETSIPortChannelSwitchingMode enETSIPortChannelSwitchingMode);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetProfileList(tDllResultHandlerGetProfileList *pDllResultHandlerGetProfileList);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetProfileList(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetProfile(tDllResultHandlerGetProfile *pDllResultHandlerGetProfile);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetProfile(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetProfile(tDllResultHandlerSetProfile *pDllResultHandlerSetProfile);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetProfile(RRU4 rru4Handle, TByte ubProfile);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetModulationType(tDllResultHandlerGetModulationType *pDllResultHandlerGetModulationType);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetModulationType(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetModulationType(tDllResultHandlerSetModulationType *pDllResultHandlerSetModulationType);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetModulationType(RRU4 rru4Handle, tModulationType enModulationType);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetExtendedResultFlag(tDllResultHandlerGetExtendedResultFlag *pDllResultHandlerGetExtendedResultFlag);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetExtendedResultFlag(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetExtendedResultFlag(tDllResultHandlerSetExtendedResultFlag *pDllResultHandlerSetExtendedResultFlag);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetExtendedResultFlag(RRU4 rru4Handle, TByte ubExtendedResultFlag);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetErrorStatus(tDllResultHandlerGetErrorStatus *pDllResultHandlerGetErrorStatus);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetErrorStatus(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetDefaultParameterSet(tDllResultHandlerGetDefaultParameterSet *pDllResultHandlerGetDefaultParameterSet);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetDefaultParameterSet(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetDefaultParameterSet(tDllResultHandlerSetDefaultParameterSet *pDllResultHandlerSetDefaultParameterSet);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDefaultParameterSet(RRU4 rru4Handle, TByte ubParameterSet);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetActiveParameterSet(tDllResultHandlerGetActiveParameterSet *pDllResultHandlerGetActiveParameterSet);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetActiveParameterSet(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetActiveParameterSet(tDllResultHandlerSetActiveParameterSet *pDllResultHandlerSetActiveParameterSet);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetActiveParameterSet(RRU4 rru4Handle, TByte ubParameterSet);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSaveActiveParameterSet(tDllResultHandlerSaveActiveParameterSet *pDllResultHandlerSaveActiveParameterSet);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SaveActiveParameterSet(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetParameterByConfigId(tDllResultHandlerGetParameterByConfigId *pDllResultHandlerGetParameterByConfigId);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetParameterByConfigId(RRU4 rru4Handle, TDWord udwId);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetParameterByConfigId(tDllResultHandlerSetParameterByConfigId *pDllResultHandlerSetParameterByConfigId);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetParameterByConfigId(RRU4 rru4Handle, TDWord udwId, TDWord udwValue);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetIOCardHwConfig(tDllResultHandlerGetIOCardHwConfig *pDllResultHandlerGetIOCardHwConfig);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetIOCardHwConfig(RRU4 rru4Handle, TByte ubIOCard);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetIOCardHwConfig(tDllResultHandlerSetIOCardHwConfig *pDllResultHandlerSetIOCardHwConfig);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetIOCardHwConfig(RRU4 rru4Handle, TByte ubIOCard, tIOCardType enIOCardType, tIOCardService enIOCardService, TByte *pubData, TDWord udwDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetIOCardProtocolConfig(tDllResultHandlerGetIOCardProtocolConfig *pDllResultHandlerGetIOCardProtocolConfig);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetIOCardProtocolConfig(RRU4 rru4Handle, TByte ubIOCard);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetIOCardProtocolConfig(tDllResultHandlerSetIOCardProtocolConfig *pDllResultHandlerSetIOCardProtocolConfig);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetIOCardProtocolConfig(RRU4 rru4Handle, TByte ubIOCard, TByte *pubData, TDWord udwDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetTime(tDllResultHandlerGetTime *pDllResultHandlerGetTime);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetTime(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetTime(tDllResultHandlerSetTime *pDllResultHandlerSetTime);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetTime(RRU4 rru4Handle, TsDWord sdwSeconds);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetTemperature(tDllResultHandlerGetTemperature *pDllResultHandlerGetTemperature);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetTemperature(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetSwVersion(tDllResultHandlerGetSwVersion *pDllResultHandlerGetSwVersion);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetSwVersion(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetHwVersion(tDllResultHandlerGetHwVersion *pDllResultHandlerGetHwVersion);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetHwVersion(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetLicenseKey(tDllResultHandlerGetLicenseKey *pDllResultHandlerGetLicenseKey);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetLicenseKey(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetLicenseKey(tDllResultHandlerSetLicenseKey *pDllResultHandlerSetLicenseKey);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetLicenseKey(RRU4 rru4Handle, char *pcLicenseKey);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetIOCardExtendedInfo(tDllResultHandlerGetIOCardExtendedInfo *pDllResultHandlerGetIOCardExtendedInfo);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetIOCardExtendedInfo(RRU4 rru4Handle, TByte ubIOCard);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetConfigPassword(tDllResultHandlerSetConfigPassword *pDllResultHandlerSetConfigPassword);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetConfigPassword(RRU4 rru4Handle, char *pcOldPassword, char *pcNewPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerEnterConfigPassword(tDllResultHandlerEnterConfigPassword *pDllResultHandlerEnterConfigPassword);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION EnterConfigPassword(RRU4 rru4Handle, char *pcPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerLoadFactoryDefaults(tDllResultHandlerLoadFactoryDefaults *pDllResultHandlerLoadFactoryDefaults);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION LoadFactoryDefaults(RRU4 rru4Handle, TBool bLoadAllParameterSetsToFactoryDefault, TByte ubParameterSet);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerFirmwareUpdatePrepare(tDllResultHandlerFirmwareUpdatePrepare *pDllResultHandlerFirmwareUpdatePrepare);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION FirmwareUpdatePrepare(RRU4 rru4Handle, TDWord udwDataLength, TWord uwCRC);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerFirmwareUpdateTransmitData(tDllResultHandlerFirmwareUpdateTransmitData *pDllResultHandlerFirmwareUpdateTransmitData);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION FirmwareUpdateTransmitData(RRU4 rru4Handle, TByte *pubData, TDWord udwDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerFirmwareUpdateFlash(tDllResultHandlerFirmwareUpdateFlash *pDllResultHandlerFirmwareUpdateFlash);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION FirmwareUpdateFlash(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerRebootSystem(tDllResultHandlerRebootSystem *pDllResultHandlerRebootSystem);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION RebootSystem(RRU4 rru4Handle, TWord uwData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerActivateBuzzer(tDllResultHandlerActivateBuzzer *pDllResultHandlerActivateBuzzer);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ActivateBuzzer(RRU4 rru4Handle, TWord uwTime);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetDeviceSerialNumber(tDllResultHandlerGetDeviceSerialNumber *pDllResultHandlerGetDeviceSerialNumber);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetDeviceSerialNumber(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetDeviceTypeNumber(tDllResultHandlerGetDeviceTypeNumber *pDllResultHandlerGetDeviceTypeNumber);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetDeviceTypeNumber(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetLEDMode(tDllResultHandlerGetLEDMode *pDllResultHandlerGetLEDMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetLEDMode(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetLEDMode(tDllResultHandlerSetLEDMode *pDllResultHandlerSetLEDMode);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetLEDMode(RRU4 rru4Handle, tLEDMode enLEDMode);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetUptimeInMilliseconds(tDllResultHandlerGetUptimeInMilliseconds *pDllResultHandlerGetUptimeInMilliseconds);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetUptimeInMilliseconds(RRU4 rru4Handle);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetSelSessionAndTarget(tDllResultHandlerGetSelSessionAndTarget *pDllResultHandlerGetSelSessionAndTarget);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetSelSessionAndTarget(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetSelSessionAndTarget(tDllResultHandlerSetSelSessionAndTarget *pDllResultHandlerSetSelSessionAndTarget);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetSelSessionAndTarget(RRU4 rru4Handle, TByte ubSel, TByte ubSession, TByte ubTarget);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetInitialQ(tDllResultHandlerGetInitialQ *pDllResultHandlerGetInitialQ);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetInitialQ(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetInitialQ(tDllResultHandlerSetInitialQ *pDllResultHandlerSetInitialQ);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetInitialQ(RRU4 rru4Handle, TByte ubInitialQ);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetMaxAirCommErrorCount(tDllResultHandlerGetMaxAirCommErrorCount *pDllResultHandlerGetMaxAirCommErrorCount);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetMaxAirCommErrorCount(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetMaxAirCommErrorCount(tDllResultHandlerSetMaxAirCommErrorCount *pDllResultHandlerSetMaxAirCommErrorCount);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetMaxAirCommErrorCount(RRU4 rru4Handle, TByte ubMaxAirCommErrorCount);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetASyncObservedListParameters(tDllResultHandlerGetASyncObservedListParameters *pDllResultHandlerGetASyncObservedListParameters);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetASyncObservedListParameters(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetASyncObservedListParameters(tDllResultHandlerSetASyncObservedListParameters *pDllResultHandlerSetASyncObservedListParameters);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetASyncObservedListParameters(RRU4 rru4Handle, TByte ubGlimpsedTimeoutCount, TByte ubObservedThresholdCount, TByte ubObservedTimeoutCount);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetSelectFilterOnOff(tDllResultHandlerGetSelectFilterOnOff *pDllResultHandlerGetSelectFilterOnOff);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetSelectFilterOnOff(RRU4 rru4Handle, TByte ubFilter);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetSelectFilterOnOff(tDllResultHandlerSetSelectFilterOnOff *pDllResultHandlerSetSelectFilterOnOff);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetSelectFilterOnOff(RRU4 rru4Handle, TByte ubFilter, tOnOffFlag enOnOffFlag);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetSelectFilterData(tDllResultHandlerGetSelectFilterData *pDllResultHandlerGetSelectFilterData);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetSelectFilterData(RRU4 rru4Handle, TByte ubFilter);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetSelectFilterData(tDllResultHandlerSetSelectFilterData *pDllResultHandlerSetSelectFilterData);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetSelectFilterData(RRU4 rru4Handle, TByte ubFilter, tOnOffFlag enOnOffFlag, TByte ubTarget, TByte ubAction, tTagMemoryBank enTagMemoryBank, TDWord udwBitPointer, TByte ubMaskLength, TByte *pubMaskData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGPIOGetIOData(tDllResultHandlerGPIOGetIOData *pDllResultHandlerGPIOGetIOData);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GPIOGetIOData(RRU4 rru4Handle, TByte ubIOCard);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGPIOSetOutput(tDllResultHandlerGPIOSetOutput *pDllResultHandlerGPIOSetOutput);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GPIOSetOutput(RRU4 rru4Handle, TByte ubIOCard, TByte ubOutput, TWord uwTime);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGPIOAddActionToActionlist(tDllResultHandlerGPIOAddActionToActionlist *pDllResultHandlerGPIOAddActionToActionlist);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GPIOAddActionToActionlist(RRU4 rru4Handle, TByte ubActionList, TByte *pubData, TDWord udwDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGPIOClearActionlist(tDllResultHandlerGPIOClearActionlist *pDllResultHandlerGPIOClearActionlist);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GPIOClearActionlist(RRU4 rru4Handle, TByte ubActionList);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGPIOAssignInputToActionlist(tDllResultHandlerGPIOAssignInputToActionlist *pDllResultHandlerGPIOAssignInputToActionlist);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GPIOAssignInputToActionlist(RRU4 rru4Handle, TByte ubIOCard, TByte ubInput, TByte ubEdge, TByte ubActionList);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGPIOGetActionlist(tDllResultHandlerGPIOGetActionlist *pDllResultHandlerGPIOGetActionlist);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GPIOGetActionlist(RRU4 rru4Handle, TByte ubActionList);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGPIOSetActionlist(tDllResultHandlerGPIOSetActionlist *pDllResultHandlerGPIOSetActionlist);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GPIOSetActionlist(RRU4 rru4Handle, TByte ubActionList, TDWord udwActionsLength, TByte* pubActions);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetLED(tDllResultHandlerSetLED *pDllResultHandlerSetLED);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetLED(RRU4 rru4Handle, TByte ubLEDNumber, TWord uwTime);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerKRAIInitializePort(tDllResultHandlerKRAIInitializePort *pDllResultHandlerKRAIInitializePort);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION KRAIInitializePort(RRU4 rru4Handle, TByte ubPort);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerKRAIGetPortInfo(tDllResultHandlerKRAIGetPortInfo *pDllResultHandlerKRAIGetPortInfo);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION KRAIGetPortInfo(RRU4 rru4Handle, TByte ubPort);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerKRAISetLED(tDllResultHandlerKRAISetLED *pDllResultHandlerKRAISetLED);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION KRAISetLED(RRU4 rru4Handle, TByte ubPort, TByte ubLEDNumber, TWord uwTime);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerKRAIGetSwVersion(tDllResultHandlerKRAIGetSwVersion *pDllResultHandlerKRAIGetSwVersion);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION KRAIGetSwVersion(RRU4 rru4Handle);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncGetEPCs(tDllResultHandlerSyncGetEPCs *pDllResultHandlerSyncGetEPCs);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncGetEPCs(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncBulkGetEPCs(tDllResultHandlerSyncBulkGetEPCs *pDllResultHandlerSyncBulkGetEPCs);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncBulkGetEPCs(RRU4 rru4Handle, TWord uwTimeToRead);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncReadDataAny(tDllResultHandlerSyncReadDataAny  *pDllResultHandlerSyncReadDataAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncReadDataAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncReadDataUntilEndOfBankAny(tDllResultHandlerSyncReadDataUntilEndOfBankAny  *pDllResultHandlerSyncReadDataUntilEndOfBankAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncReadDataUntilEndOfBankAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncReadDataSpecific(tDllResultHandlerSyncReadDataSpecific *pDllResultHandlerSyncReadDataSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncReadDataSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncReadDataUntilEndOfBankSpecific(tDllResultHandlerSyncReadDataUntilEndOfBankSpecific *pDllResultHandlerSyncReadDataUntilEndOfBankSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncReadDataUntilEndOfBankSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncWriteDataAny(tDllResultHandlerSyncWriteDataAny *pDllResultHandlerSyncWriteDataAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncWriteDataAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength, TWord *puwData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncWriteDataSpecific(tDllResultHandlerSyncWriteDataSpecific *pDllResultHandlerSyncWriteDataSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncWriteDataSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength, TWord *puwData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncWriteMaskedDataAny(tDllResultHandlerSyncWriteMaskedDataAny *pDllResultHandlerSyncWriteMaskedDataAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncWriteMaskedDataAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength, TWord *puwData, TWord *puwMask);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncWriteMaskedDataSpecific(tDllResultHandlerSyncWriteMaskedDataSpecific *pDllResultHandlerSyncWriteMaskedDataSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncWriteMaskedDataSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength, TWord *puwData, TWord *puwMask);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncBlockEraseAny(tDllResultHandlerSyncBlockEraseAny *pDllResultHandlerSyncBlockEraseAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncBlockEraseAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncBlockEraseSpecific(tDllResultHandlerSyncBlockEraseSpecific *pDllResultHandlerSyncBlockEraseSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncBlockEraseSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncLockAny(tDllResultHandlerSyncLockAny *pDllResultHandlerSyncLockAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncLockAny(RRU4 rru4Handle, TDWord udwPassword, TDWord udwPayload);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncLockSpecific(tDllResultHandlerSyncLockSpecific *pDllResultHandlerSyncLockSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncLockSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TDWord udwPayload);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncKillAny(tDllResultHandlerSyncKillAny *pDllResultHandlerSyncKillAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncKillAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubRecommissioningBits);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncKillSpecific(tDllResultHandlerSyncKillSpecific *pDllResultHandlerSyncKillSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncKillSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TByte ubRecommissioningBits);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncBlockPermalockAny(tDllResultHandlerSyncBlockPermalockAny *pDllResultHandlerSyncBlockPermalockAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncBlockPermalockAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubReadLock, tTagMemoryBank enTagMemoryBank, TDWord udwBlockPointer, TByte ubBlockRange, TWord *puwMaskData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncBlockPermalockSpecific(tDllResultHandlerSyncBlockPermalockSpecific *pDllResultHandlerSyncBlockPermalockSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncBlockPermalockSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TByte ubReadLock, tTagMemoryBank enTagMemoryBank, TDWord udwBlockPointer, TByte ubBlockRange, TWord *puwMaskData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncAuthenticateAny(tDllResultHandlerSyncAuthenticateAny *pDllResultHandlerSyncAuthenticateAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncAuthenticateAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubCSI, TWord uwMessageBitLength, TByte *pubMessage);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncAuthenticateSpecific(tDllResultHandlerSyncAuthenticateSpecific *pDllResultHandlerSyncAuthenticateSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncAuthenticateSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TByte ubCSI, TWord uwMessageBitLength, TByte *pubMessage);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncUntraceableAny(tDllResultHandlerSyncUntraceableAny *pDllResultHandlerSyncUntraceableAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncUntraceableAny(RRU4 rru4Handle, TDWord udwPassword, TBool bU, TByte ubEPC, TByte ubTID, TBool bUser, TByte ubRange);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncUntraceableSpecific(tDllResultHandlerSyncUntraceableSpecific *pDllResultHandlerSyncUntraceableSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncUntraceableSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TBool bU, TByte ubEPC, TByte ubTID, TBool bUser, TByte ubRange);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncSJ5511ActivateSecureModeAny(tDllResultHandlerSyncSJ5511ActivateSecureModeAny *pDllResultHandlerSyncSJ5511ActivateSecureModeAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncSJ5511ActivateSecureModeAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubRFU);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncSJ5511ActivateSecureModeSpecific(tDllResultHandlerSyncSJ5511ActivateSecureModeSpecific *pDllResultHandlerSyncSJ5511ActivateSecureModeSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncSJ5511ActivateSecureModeSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TByte ubRFU);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncSJ5511AuthenticateOBUAny(tDllResultHandlerSyncSJ5511AuthenticateOBUAny *pDllResultHandlerSyncSJ5511AuthenticateOBUAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncSJ5511AuthenticateOBUAny(RRU4 rru4Handle, TDWord udwPassword, TBool bTC, TByte ubRFU, TByte rgubR96[12]);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncSJ5511AuthenticateOBUSpecific(tDllResultHandlerSyncSJ5511AuthenticateOBUSpecific *pDllResultHandlerSyncSJ5511AuthenticateOBUSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncSJ5511AuthenticateOBUSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TBool bTC, TByte ubRFU, TByte rgubR96[12]);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncWriteEPCSpecific(tDllResultHandlerSyncWriteEPCSpecific *pDllResultHandlerSyncWriteEPCSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncWriteEPCSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, tEPC newEPC);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncWriteEPCToSingleTag(tDllResultHandlerSyncWriteEPCToSingleTag *pDllResultHandlerSyncWriteEPCToSingleTag);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncWriteEPCToSingleTag(RRU4 rru4Handle, tEPC epc, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncNXPG2XReadProtectAny(tDllResultHandlerSyncNXPG2XReadProtectAny *pDllResultHandlerSyncNXPG2XReadProtectAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncNXPG2XReadProtectAny(RRU4 rru4Handle, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncNXPG2XReadProtectSpecific(tDllResultHandlerSyncNXPG2XReadProtectSpecific *pDllResultHandlerSyncNXPG2XReadProtectSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncNXPG2XReadProtectSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncNXPG2XResetReadProtectAny(tDllResultHandlerSyncNXPG2XResetReadProtectAny *pDllResultHandlerSyncNXPG2XResetReadProtectAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncNXPG2XResetReadProtectAny(RRU4 rru4Handle, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncNXPG2XResetReadProtectSpecific(tDllResultHandlerSyncNXPG2XResetReadProtectSpecific *pDllResultHandlerSyncNXPG2XResetReadProtectSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncNXPG2XResetReadProtectSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncNXPG2XChangeEASAny(tDllResultHandlerSyncNXPG2XChangeEASAny *pDllResultHandlerSyncNXPG2XChangeEASAny);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncNXPG2XChangeEASAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubEASAlarmBit);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncNXPG2XChangeEASSpecific(tDllResultHandlerSyncNXPG2XChangeEASSpecific *pDllResultHandlerSyncNXPG2XChangeEASSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncNXPG2XChangeEASSpecific(RRU4 rru4Handle, tEPC epc, TDWord udwPassword, TByte ubEASAlarmBit);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncNXPG2XEASAlarm(tDllResultHandlerSyncNXPG2XEASAlarm *pDllResultHandlerSyncNXPG2XEASAlarm);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncNXPG2XEASAlarm(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSyncRSSILedBarSpecific(tDllResultHandlerSyncRSSILedBarSpecific *pDllResultHandlerSyncRSSILedBarSpecific);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SyncRSSILedBarSpecific(RRU4 rru4Handle, tEPC epc, tRSSILedBarControlFlag enRSSILedBarControlFlag, TByte ubPort);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncGetRawEPCs(tDllResultHandlerASyncGetRawEPCsSync *pDllResultHandlerASyncGetRawEPCsSync, tDllResultHandlerASyncGetRawEPCsASync *pDllResultHandlerASyncGetRawEPCsASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncGetRawEPCs(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncGetEPCs(tDllResultHandlerASyncGetEPCsSync *pDllResultHandlerASyncGetEPCsSync, tDllResultHandlerASyncGetEPCsASync *pDllResultHandlerASyncGetEPCsASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncGetEPCs(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncReadDataAny(tDllResultHandlerASyncReadDataAnySync *pDllResultHandlerASyncReadDataAnySync, tDllResultHandlerASyncReadDataAnyASync *pDllResultHandlerASyncReadDataAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncReadDataAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncReadDataUntilEndOfBankAny(tDllResultHandlerASyncReadDataUntilEndOfBankAnySync *pDllResultHandlerASyncReadDataUntilEndOfBankAnySync, tDllResultHandlerASyncReadDataUntilEndOfBankAnyASync *pDllResultHandlerASyncReadDataUntilEndOfBankAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncReadDataUntilEndOfBankAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncWriteDataAny(tDllResultHandlerASyncWriteDataAnySync *pDllResultHandlerASyncWriteDataAnySync, tDllResultHandlerASyncWriteDataAnyASync *pDllResultHandlerASyncWriteDataAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncWriteDataAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength, TWord *puwData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncWriteMaskedDataAny(tDllResultHandlerASyncWriteMaskedDataAnySync *pDllResultHandlerASyncWriteMaskedDataAnySync, tDllResultHandlerASyncWriteMaskedDataAnyASync *pDllResultHandlerASyncWriteMaskedDataAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncWriteMaskedDataAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength, TWord *puwData, TWord *puwMask);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncBlockEraseAny(tDllResultHandlerASyncBlockEraseAnySync *pDllResultHandlerASyncBlockEraseAnySync, tDllResultHandlerASyncBlockEraseAnyASync *pDllResultHandlerASyncBlockEraseAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncBlockEraseAny(RRU4 rru4Handle, TDWord udwPassword, tTagMemoryBank enTagMemoryBank, TDWord udwWordPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncLockAny(tDllResultHandlerASyncLockAnySync *pDllResultHandlerASyncLockAnySync, tDllResultHandlerASyncLockAnyASync *pDllResultHandlerASyncLockAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncLockAny(RRU4 rru4Handle, TDWord udwPassword, TDWord udwPayload);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncKillAny(tDllResultHandlerASyncKillAnySync *pDllResultHandlerASyncKillAnySync, tDllResultHandlerASyncKillAnyASync *pDllResultHandlerASyncKillAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncKillAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubRecommissioningBits);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncBlockPermalockAny(tDllResultHandlerASyncBlockPermalockAnySync *pDllResultHandlerASyncBlockPermalockAnySync, tDllResultHandlerASyncBlockPermalockAnyASync *pDllResultHandlerASyncBlockPermalockAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncBlockPermalockAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubReadLock, tTagMemoryBank enTagMemoryBank, TDWord udwBlockPointer, TByte ubBlockRange, TWord *puwMaskData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncAuthenticateAny(tDllResultHandlerASyncAuthenticateAnySync *pDllResultHandlerASyncAuthenticateAnySync, tDllResultHandlerASyncAuthenticateAnyASync *pDllResultHandlerASyncAuthenticateAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncAuthenticateAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubCSI, TWord uwMessageBitLength, TByte *pubMessage);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncUntraceableAny(tDllResultHandlerASyncUntraceableAnySync *pDllResultHandlerASyncUntraceableAnySync, tDllResultHandlerASyncUntraceableAnyASync *pDllResultHandlerASyncUntraceableAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncUntraceableAny(RRU4 rru4Handle, TDWord udwPassword, TBool bU, TByte ubEPC, TByte ubTID, TBool bUser, TByte ubRange);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncSJ5511ActivateSecureModeAny(tDllResultHandlerASyncSJ5511ActivateSecureModeAnySync *pDllResultHandlerASyncSJ5511ActivateSecureModeAnySync, tDllResultHandlerASyncSJ5511ActivateSecureModeAnyASync *pDllResultHandlerASyncSJ5511ActivateSecureModeAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncSJ5511ActivateSecureModeAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubRFU);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncSJ5511AuthenticateOBUAny(tDllResultHandlerASyncSJ5511AuthenticateOBUAnySync *pDllResultHandlerASyncSJ5511AuthenticateOBUAnySync, tDllResultHandlerASyncSJ5511AuthenticateOBUAnyASync *pDllResultHandlerASyncSJ5511AuthenticateOBUAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncSJ5511AuthenticateOBUAny(RRU4 rru4Handle, TDWord udwPassword, TBool bTC, TByte ubRFU, TByte rgubR96[12]);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncNXPG2XReadProtectAny(tDllResultHandlerASyncNXPG2XReadProtectAnySync *pDllResultHandlerASyncNXPG2XReadProtectAnySync, tDllResultHandlerASyncNXPG2XReadProtectAnyASync *pDllResultHandlerASyncNXPG2XReadProtectAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncNXPG2XReadProtectAny(RRU4 rru4Handle, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncNXPG2XResetReadProtectAny(tDllResultHandlerASyncNXPG2XResetReadProtectAnySync *pDllResultHandlerASyncNXPG2XResetReadProtectAnySync, tDllResultHandlerASyncNXPG2XResetReadProtectAnyASync *pDllResultHandlerASyncNXPG2XResetReadProtectAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncNXPG2XResetReadProtectAny(RRU4 rru4Handle, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncNXPG2XChangeEASAny(tDllResultHandlerASyncNXPG2XChangeEASAnySync *pDllResultHandlerASyncNXPG2XChangeEASAnySync, tDllResultHandlerASyncNXPG2XChangeEASAnyASync *pDllResultHandlerASyncNXPG2XChangeEASAnyASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncNXPG2XChangeEASAny(RRU4 rru4Handle, TDWord udwPassword, TByte ubEASAlarmBit);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncNXPG2XEASAlarm(tDllResultHandlerASyncNXPG2XEASAlarmSync *pDllResultHandlerASyncNXPG2XEASAlarmSync, tDllResultHandlerASyncNXPG2XEASAlarmASync *pDllResultHandlerASyncNXPG2XEASAlarmASync);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncNXPG2XEASAlarm(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncStopCommand(tDllResultHandlerASyncStopCommand *pDllResultHandlerASyncStopCommand);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncStopCommand(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerASyncGetTagRate(tDllResultHandlerASyncGetTagRate *pDllResultHandlerASyncGetTagRate);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ASyncGetTagRate(RRU4 rru4Handle);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetAntennaList(tDllResultHandlerGetAntennaList *pDllResultHandlerGetAntennaList);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetAntennaList(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetAntenna(tDllResultHandlerSetAntenna *pDllResultHandlerSetAntenna);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetAntenna(RRU4 rru4Handle, TByte ubPort);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSelectFrequencyAndSwitchCarrierOn(tDllResultHandlerSelectFrequencyAndSwitchCarrierOn *pDllResultHandlerSelectFrequencyAndSwitchCarrierOn);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SelectFrequencyAndSwitchCarrierOn(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSetFrequency(tDllResultHandlerSetFrequency *pDllResultHandlerSetFrequency);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetFrequency(RRU4 rru4Handle, TDWord udwFrequency);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetNoiseValue(tDllResultHandlerGetNoiseValue *pDllResultHandlerGetNoiseValue);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetNoiseValue(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerGetETSILBTReferenceNoiseValue(tDllResultHandlerGetETSILBTReferenceNoiseValue *pDllResultHandlerGetETSILBTReferenceNoiseValue);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION GetETSILBTReferenceNoiseValue(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerCarrierOnOff(tDllResultHandlerCarrierOnOff *pDllResultHandlerCarrierOnOff);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION CarrierOnOff(RRU4 rru4Handle, tOnOffFlag enOnOffFlag);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerRandomModulationOnOff(tDllResultHandlerRandomModulationOnOff *pDllResultHandlerRandomModulationOnOff);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION RandomModulationOnOff(RRU4 rru4Handle, tOnOffFlag enOnOffFlag);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerMeasureTxPower(tDllResultHandlerMeasureTxPower *pDllResultHandlerMeasureTxPower);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION MeasureTxPower(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerMeasureRxPower(tDllResultHandlerMeasureRxPower *pDllResultHandlerMeasureRxPower);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION MeasureRxPower(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerMeasureVSWR(tDllResultHandlerMeasureVSWR *pDllResultHandlerMeasureVSWR);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION MeasureVSWR(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerStartInventory(tDllResultHandlerStartInventory *pDllResultHandlerStartInventory);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION StartInventory(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerOpenNoninventoriedTag(tDllResultHandlerOpenNoninventoriedTag *pDllResultHandlerOpenNoninventoriedTag);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION OpenNoninventoriedTag(RRU4 rru4Handle, TByte ubFlags);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerOpenSpecificTag(tDllResultHandlerOpenSpecificTag *pDllResultHandlerOpenSpecificTag);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION OpenSpecificTag(RRU4 rru4Handle, tEPC epc);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerDirectBulkGetEPCs(tDllResultHandlerDirectBulkGetEPCs *pDllResultHandlerDirectBulkGetEPCs);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION DirectBulkGetEPCs(RRU4 rru4Handle, TWord uwTimeToRead);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerExecuteAccessSequence(tDllResultHandlerExecuteAccessSequence *pDllResultHandlerExecuteAccessSequence);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ExecuteAccessSequence(RRU4 rru4Handle, TWord uwHandle, TDWord udwPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerExecuteKillSequence(tDllResultHandlerExecuteKillSequence *pDllResultHandlerExecuteKillSequence);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ExecuteKillSequence(RRU4 rru4Handle, TWord uwHandle, TDWord udwPassword, TByte ubRecommissioningBits);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerExecuteWriteSequence(tDllResultHandlerExecuteWriteSequence *pDllResultHandlerExecuteWriteSequence);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ExecuteWriteSequence(RRU4 rru4Handle, TWord uwHandle, tTagMemoryBank enTagMemoryBank, TDWord udwPointer, TWord uwData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandSelect(tDllResultHandlerTxGen2CommandSelect *pDllResultHandlerTxGen2CommandSelect);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandSelect(RRU4 rru4Handle, TByte ubTarget, TByte ubAction, tTagMemoryBank enTagMemoryBank, TDWord udwPointer, TByte ubMaskBitLength, TByte *pubMaskData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandACK(tDllResultHandlerTxGen2CommandACK *pDllResultHandlerTxGen2CommandACK);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandACK(RRU4 rru4Handle, TWord uwHandle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandNAK(tDllResultHandlerTxGen2CommandNAK *pDllResultHandlerTxGen2CommandNAK);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandNAK(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandReqRN(tDllResultHandlerTxGen2CommandReqRN *pDllResultHandlerTxGen2CommandReqRN);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandReqRN(RRU4 rru4Handle, TWord uwHandle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandAccess(tDllResultHandlerTxGen2CommandAccess *pDllResultHandlerTxGen2CommandAccess);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandAccess(RRU4 rru4Handle, TWord uwHandle, TWord uwEcryptedPassword);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandRead(tDllResultHandlerTxGen2CommandRead *pDllResultHandlerTxGen2CommandRead);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandRead(RRU4 rru4Handle, TWord uwHandle, tTagMemoryBank enTagMemoryBank, TDWord udwPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandWrite(tDllResultHandlerTxGen2CommandWrite *pDllResultHandlerTxGen2CommandWrite);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandWrite(RRU4 rru4Handle, TWord uwHandle, tTagMemoryBank enTagMemoryBank, TDWord udwPointer, TWord uwEncryptedData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandKill(tDllResultHandlerTxGen2CommandKill *pDllResultHandlerTxGen2CommandKill);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandKill(RRU4 rru4Handle, TWord uwHandle, TWord uwEncryptedKillPassword, TByte ubRecommissioningBits, TByte ubSecondKillFlag);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandLock(tDllResultHandlerTxGen2CommandLock *pDllResultHandlerTxGen2CommandLock);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandLock(RRU4 rru4Handle, TWord uwHandle, TDWord udwPayload);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandBlockWrite(tDllResultHandlerTxGen2CommandBlockWrite *pDllResultHandlerTxGen2CommandBlockWrite);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandBlockWrite(RRU4 rru4Handle, TWord uwHandle, tTagMemoryBank enTagMemoryBank, TDWord udwPointer, TByte ubDataLength, TWord *puwData);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandBlockErase(tDllResultHandlerTxGen2CommandBlockErase *pDllResultHandlerTxGen2CommandBlockErase);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandBlockErase(RRU4 rru4Handle, TWord uwHandle, tTagMemoryBank enTagMemoryBank, TDWord udwPointer, TByte ubDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandQuery(tDllResultHandlerTxGen2CommandQuery *pDllResultHandlerTxGen2CommandQuery);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandQuery(RRU4 rru4Handle, TByte ubDivideRatio, TByte ubMillerMode, TByte ubTRext, TByte ubSelect, TByte ubSession, TByte ubTarget, TByte ubQValue);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandQueryAdjust(tDllResultHandlerTxGen2CommandQueryAdjust *pDllResultHandlerTxGen2CommandQueryAdjust);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandQueryAdjust(RRU4 rru4Handle, TByte ubSession, TByte ubAdjustValue);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandQueryRep(tDllResultHandlerTxGen2CommandQueryRep *pDllResultHandlerTxGen2CommandQueryRep);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandQueryRep(RRU4 rru4Handle, TByte ubSession);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerTxGen2CommandCustomCommand(tDllResultHandlerTxGen2CommandCustomCommand *pDllResultHandlerTxGen2CommandCustomCommand);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION TxGen2CommandCustomCommand(RRU4 rru4Handle, TByte ubBitCount, TByte *pubData, TByte ubAnswerBitCount, TByte ubFlagRegister);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerEN302208TestmodulationOnOff(tDllResultHandlerEN302208TestmodulationOnOff *pDllResultHandlerEN302208TestmodulationOnOff);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION EN302208TestmodulationOnOff(RRU4 rru4Handle, tOnOffFlag enOnOffFlag);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerMessage(tDllResultHandlerMessage *pDllResultHandlerMessage);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerInternalError(tDllResultHandlerInternalError *pDllResultHandlerInternalError);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerReaderCustomCommand(tDllResultHandlerReaderCustomCommand *pDllResultHandlerReaderCustomCommand);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION ReaderCustomCommand(RRU4 rru4Handle, TWord uwCommandId, TByte *pubData, TDWord udwDataLength);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllEventHandlerUSBDevice(tDllEventHandlerUSBDevice *pDllEventHandlerUSBDevice);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerFoundReaderIP(tDllResultHandlerFoundReaderIP *pDllResultHandlerFoundReaderIP);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION StartSearchReaderIP(void);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION StopSearchReaderIP(void);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMEncrypt(tDllResultHandlerHSMEncrypt *pDllResultHandlerHSMEncrypt);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMEncrypt(RRU4 rru4Handle, TByte ubKeyIndex, tAESMode enAESMode, TByte *pubPlaintext, TDWord udwPlaintextLength, TByte *pubIV, TByte ubIVLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMDecrypt(tDllResultHandlerHSMDecrypt *pDllResultHandlerHSMDecrypt);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMDecrypt(RRU4 rru4Handle, TByte ubKeyIndex, tAESMode enAESMode, TByte *pubCiphertext, TDWord udwCiphertextLength, TByte *pubIV, TByte ubIVLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMGetRandomData(tDllResultHandlerHSMGetRandomData *pDllResultHandlerHSMGetRandomData);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMGetRandomData(RRU4 rru4Handle, TDWord udwRandomDataLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMGetChallenge(tDllResultHandlerHSMGetChallenge *pDllResultHandlerHSMGetChallenge);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMGetChallenge(RRU4 rru4Handle);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMAuthenticate(tDllResultHandlerHSMAuthenticate *pDllResultHandlerHSMAuthenticate);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMAuthenticate(RRU4 rru4Handle, TByte *pubSignedChallenge, TDWord udwSignedChallengeLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMSetPublicCertificate(tDllResultHandlerHSMSetPublicCertificate *pDllResultHandlerHSMSetPublicCertificate);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMSetPublicCertificate(RRU4 rru4Handle, TByte *pubPublicCertificate, TDWord udwPublicCertificateLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMSetWrappingKey(tDllResultHandlerHSMSetWrappingKey *pDllResultHandlerHSMSetWrappingKey);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMSetWrappingKey(RRU4 rru4Handle, TByte *pubWrappingKey, TDWord udwWrappingKeyLength, TByte *pubIV, TByte ubIVLength);

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerHSMSetAESKey(tDllResultHandlerHSMSetAESKey *pDllResultHandlerHSMSetAESKey);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION HSMSetAESKey(RRU4 rru4Handle, TByte ubKeyIndex, TByte *pubAESKey, TDWord udwAESKeyLength, TByte *pubIV, TByte ubIVLength);

// ---------------------------------------------------------------------------

__declspec(dllexport) tResultFlag CALLCONVENTION HSMWrapKey(TByte *pubKey, TDWord udwKeyLength, TByte *pubWrappingKey, TDWord udwWrappingKeyLength, TByte **ppubWrappedKey, TDWord *pudwWrappedKeyLength, TByte **ppubIV, TByte *pubIVLength);
__declspec(dllexport) tResultFlag CALLCONVENTION HSMSignChallenge(TByte *pubChallenge, TDWord udwChallengeLength, TByte *pubPrivateCertificate,  TDWord udwPrivateCertificateLength, TByte **ppubSignedChallenge, TDWord *pudwSignedChallengeLength);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerKBRPTInterProcComm(tDllResultHandlerKBRPTInterProcComm *pDllResultHandlerKBRPTInterProcComm);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION KBRPTInterProcComm(RRU4 rru4Handle, TWord uwClientId, TByte *pubData, TDWord udwDataLength);

// ---------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SetDllResultHandlerSendFactoryDefaultBroadcast(tDllResultHandlerSendFactoryDefaultBroadcast *pDllResultHandlerSendFactoryDefaultBroadcast);
__declspec(dllexport) tReaderErrorCode CALLCONVENTION SendFactoryDefaultBroadcast(TByte *pubData);

// ----------------------------------------------------------------------------

__declspec(dllexport) tReaderErrorCode CALLCONVENTION SendBeepBroadcast(TByte *pubData, TWord uwBuzzerTime);

// ----------------------------------------------------------------------------

__declspec(dllexport) void CALLCONVENTION GetConfigIdNamesAndValues(tConfigIdNameAndValue **prgstConfigIdNamesAndValues, TDWord *pudwConfigIdNamesAndValuesCount);

// ----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
