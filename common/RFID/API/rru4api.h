#ifndef RRU4API_H
#define RRU4API_H
// ----------------------------------------------------------------------------
//
// RRU4 API-IDs
// ------------
//
//
//
// Version v1.0
//
// Author:	Dipl.-Ing. (FH) Holger Busse
//			Dipl.-Ing. Sebastian Dreyßig
//
// (c) 2008 - 2018 KATHREIN Sachsen GmbH
// ----------------------------------------------------------------------------

// $Id: rru4api.h,v 1.64 2018/01/30 11:03:14 hbusse Exp $

// ============================================================================
// Includes

#include "globtype.h"

// ============================================================================
// Makros (allgemein)

#define EPCGLOBAL_GEN2_MSK_PC_T			0x0100
#define EPCGLOBAL_GEN2_MSK_PC_XI		0x0200
#define EPCGLOBAL_GEN2_MSK_PC_UMI		0x0400
#define EPCGLOBAL_GEN2_MSK_XPCW1_H		0x0001
#define EPCGLOBAL_GEN2_MSK_XPCW1_NR		0x0002
#define EPCGLOBAL_GEN2_MSK_XPCW1_K		0x0004
#define EPCGLOBAL_GEN2_MSK_XPCW1_U		0x0008
#define EPCGLOBAL_GEN2_MSK_XPCW1_TN		0x0010
#define EPCGLOBAL_GEN2_MSK_XPCW1_SLI	0x0020
#define EPCGLOBAL_GEN2_MSK_XPCW1_C		0x0040
#define EPCGLOBAL_GEN2_MSK_XPCW1_B		0x0080
#define EPCGLOBAL_GEN2_MSK_XPCW1_XEB	0x8000

// ============================================================================
// Datentypen

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TWord
#else
	typedef enum
#endif
{
	// RRU4 Befehle
	CMD_GetMode = 0x0001,								// 0x0001
	CMD_SetMode,										// 0x0002
	CMD_GetCommunicationStandard,						// 0x0003
	CMD_SetCommunicationStandard,						// 0x0004
	CMD_GetPortPower,									// 0x0005
	CMD_SetPortPower,									// 0x0006
	CMD_GetCarrierFollowUpTime,							// 0x0007 (nur Generation 2)
	CMD_SetCarrierFollowUpTime,							// 0x0008 (nur Generation 2)
	CMD_GetAntennaMode,									// 0x0009 (nur Generation 2)
	CMD_SetAntennaMode,									// 0x000A (nur Generation 2)
	CMD_GetPortMultiplexSequenceAndExposureTime,		// 0x000B
	CMD_SetPortMultiplexSequenceAndExposureTime,		// 0x000C
	CMD_GetCableLossAndAntennaGain,						// 0x000D
	CMD_SetCableLossAndAntennaGain,						// 0x000E
	CMD_GetETSIPortChannelList,							// 0x000F
	CMD_SetETSIPortChannelList,							// 0x0010
	CMD_GetETSIPortChannelSwitchingMode,				// 0x0011 (nur Generation 2)
	CMD_SetETSIPortChannelSwitchingMode,				// 0x0012 (nur Generation 2)
	CMD_GetProfileList,									// 0x0013
	CMD_GetProfile,										// 0x0014
	CMD_SetProfile,										// 0x0015
	CMD_GetModulationType,								// 0x0016 (nur Generation 2)
	CMD_SetModulationType,								// 0x0017 (nur Generation 2)
	CMD_GetExtendedResultFlag,							// 0x0018
	CMD_SetExtendedResultFlag,							// 0x0019
	CMD_GetErrorStatus,									// 0x001A
	CMD_GetDefaultParameterSet,							// 0x001B
	CMD_SetDefaultParameterSet,							// 0x001C
	CMD_GetActiveParameterSet,							// 0x001D
	CMD_SetActiveParameterSet,							// 0x001E
	CMD_SaveActiveParameterSet,							// 0x001F
	CMD_GetParameterByConfigId,							// 0x0020
	CMD_SetParameterByConfigId,							// 0x0021
	CMD_GetIOCardHwConfig,								// 0x0022 (nur Generation 2)
	CMD_SetIOCardHwConfig,								// 0x0023 (nur Generation 2)
	CMD_GetIOCardProtocolConfig,						// 0x0024 (nur Generation 2)
	CMD_SetIOCardProtocolConfig,						// 0x0025 (nur Generation 2)
	CMD_GetTime,										// 0x0026
	CMD_SetTime,										// 0x0027
	CMD_GetTemperature,									// 0x0028
	CMD_GetSwVersion,									// 0x0029
	CMD_GetHwVersion,									// 0x002A
	CMD_GetLicenseKey,									// 0x002B
	CMD_SetLicenseKey,									// 0x002C
	CMD_GetIOCardExtendedInfo,							// 0x002D (nur Generation 2)
	CMD_GetAntennaMapping,								// 0x002E (nur Generation 2)
	CMD_SetAntennaMapping,								// 0x002F (nur Generation 2)
	CMD_GetASyncPowerRampOffset,						// 0x0030 (nur Generation 2)
	CMD_GetMaxPowerLimits,								// 0x0031
	CMD_SetConfigPassword = 0x003E,						// 0x003E
	CMD_EnterConfigPassword,							// 0x003F
	CMD_LoadFactoryDefaults = 0x0040,					// 0x0040
	CMD_FirmwareUpdatePrepare,							// 0x0041
	CMD_FirmwareUpdateTransmitData,						// 0x0042
	CMD_FirmwareUpdateFlash,							// 0x0043
	CMD_RebootSystem,									// 0x0044
	CMD_ActivateBuzzer,									// 0x0045
	CMD_GetDeviceSerialNumber,							// 0x0046
	CMD_GetDeviceTypeNumber,							// 0x0047
	CMD_GetLEDMode,										// 0x0048
	CMD_SetLEDMode,										// 0x0049
	CMD_GetUptimeInMilliseconds,						// 0x004A
	CMD_GetSelSessionAndTarget = 0x0080,				// 0x0080
	CMD_SetSelSessionAndTarget,							// 0x0081
	CMD_GetInitialQ,									// 0x0082
	CMD_SetInitialQ,									// 0x0083
	CMD_GetMaxAirCommErrors,							// 0x0084
	CMD_SetMaxAirCommErrors,							// 0x0085
	CMD_GetASyncObservedListParameters,					// 0x0086
	CMD_SetASyncObservedListParameters,					// 0x0087
	CMD_GetSelectFilterOnOff,							// 0x0088
	CMD_SetSelectFilterOnOff,							// 0x0089
	CMD_GetSelectFilterData,							// 0x008A
	CMD_SetSelectFilterData,							// 0x008B
	CMD_GPIOGetIOData,									// 0x008C
	CMD_GPIOSetOutput,									// 0x008D
	CMD_GPIOAddActionToActionlist,						// 0x008E
	CMD_GPIOClearActionlist,							// 0x008F
	CMD_GPIOAssignInputToActionlist,					// 0x0090
	CMD_AddTagToBlacklist,								// 0x0091 (nur Generation 2)
	CMD_DeleteBlacklist,								// 0x0092 (nur Generation 2)
	CMD_GetBlacklistEntries,							// 0x0093 (nur Generation 2)
	CMD_GetBlacklistEntriesExtended,					// 0x0094 (nur Generation 2)
	CMD_SetEPCMemoryBankFilter = 0x0096,				// 0x0096 (nur Generation 2)
	CMD_GPIOGetActionlist,								// 0x0097
	CMD_GPIOSetActionlist,								// 0x0098
	CMD_SetLED = 0x00A0,								// 0x00A0 (nur Generation 3)
	CMD_KRAIInitializePort = 0x00D0,					// 0x00D0
	CMD_KRAIGetPortInfo,								// 0x00D1
	CMD_KRAISetLED,										// 0x00D2
	CMD_KRAIGetSwVersion,								// 0x00D3 (nur Generation 3)
	CMD_SyncGetEPCs = 0x0101,							// 0x0101
	CMD_SyncBulkGetEPCs,								// 0x0102
	CMD_SyncReadDataAny,								// 0x0103
	CMD_SyncReadDataSpecific,							// 0x0104
	CMD_SyncWriteDataAny,								// 0x0105
	CMD_SyncWriteDataSpecific,							// 0x0106
	CMD_SyncWriteMaskedDataAny,							// 0x0107
	CMD_SyncWriteMaskedDataSpecific,					// 0x0108
	CMD_SyncBlockEraseAny,								// 0x0109
	CMD_SyncBlockEraseSpecific,							// 0x010A
	CMD_SyncLockAny,									// 0x010B
	CMD_SyncLockSpecific,								// 0x010C
	CMD_SyncKillAny,									// 0x010D
	CMD_SyncKillSpecific,								// 0x010E
	CMD_SyncWriteEPCToSingleTag,						// 0x010F
	CMD_ASyncGetRawEPCs,								// 0x0110
	CMD_ASyncGetEPCs,									// 0x0111
	CMD_ASyncReadDataAny,								// 0x0112
	CMD_ASyncWriteDataAny,								// 0x0113
	CMD_ASyncWriteMaskedDataAny,						// 0x0114
	CMD_ASyncBlockEraseAny,								// 0x0115
	CMD_ASyncLockAny,									// 0x0116
	CMD_ASyncKillAny,									// 0x0117
	CMD_ASyncStopCommand,								// 0x0118
	CMD_ASyncGetTagRate,								// 0x0119
	CMD_SyncWriteEPCSpecific,							// 0x011A
	CMD_SyncReadDataUntilEndOfBankAny,					// 0x011B
	CMD_SyncReadDataUntilEndOfBankSpecific,				// 0x011C
	CMD_ASyncReadDataUntilEndOfBankAny,					// 0x011D
	CMD_SyncBlockPermalockAny = 0x0120,					// 0x0120
	CMD_SyncBlockPermalockSpecific,						// 0x0121
	CMD_ASyncBlockPermalockAny,							// 0x0122
	CMD_SyncAuthenticateAny,							// 0x0123
	CMD_SyncAuthenticateSpecific,						// 0x0124
	CMD_ASyncAuthenticateAny,							// 0x0125
	CMD_SyncUntraceableAny,								// 0x0126
	CMD_SyncUntraceableSpecific,						// 0x0127
	CMD_ASyncUntraceableAny,							// 0x0128
	CMD_SyncSJ5511ActivateSecureModeAny = 0x0170,		// 0x0170 (nur Generation 3)
	CMD_SyncSJ5511ActivateSecureModeSpecific,			// 0x0171 (nur Generation 3)
	CMD_ASyncSJ5511ActivateSecureModeAny,				// 0x0172 (nur Generation 3)
	CMD_SyncSJ5511AuthenticateOBUAny,					// 0x0173 (nur Generation 3)
	CMD_SyncSJ5511AuthenticateOBUSpecific,				// 0x0174 (nur Generation 3)
	CMD_ASyncSJ5511AuthenticateOBUAny,					// 0x0175 (nur Generation 3)
	CMD_SyncNXPG2XReadProtectAny = 0x0180,				// 0x0180 (nur Generation 2)
	CMD_SyncNXPG2XReadProtectSpecific,					// 0x0181 (nur Generation 2)
	CMD_SyncNXPG2XResetReadProtectAny,					// 0x0182 (nur Generation 2)
	CMD_SyncNXPG2XResetReadProtectSpecific,				// 0x0183 (nur Generation 2)
	CMD_SyncNXPG2XChangeEASAny,							// 0x0184 (nur Generation 2)
	CMD_SyncNXPG2XChangeEASSpecific,					// 0x0185 (nur Generation 2)
	CMD_SyncNXPG2XEASAlarm,								// 0x0186 (nur Generation 2)
	CMD_ASyncNXPG2XReadProtectAny,						// 0x0187 (nur Generation 2)
	CMD_ASyncNXPG2XResetReadProtectAny,					// 0x0188 (nur Generation 2)
	CMD_ASyncNXPG2XChangeEASAny,						// 0x0189 (nur Generation 2)
	CMD_ASyncNXPG2XEASAlarm,							// 0x018A (nur Generation 2)
	CMD_SyncRSSILedBarSpecific = 0x01A0,				// 0x01A0 (nur Generation 3)
	CMD_GetAntennaList = 0x0201,						// 0x0201
	CMD_SetAntenna,										// 0x0202
	CMD_SetFrequency,									// 0x0203
	CMD_GetNoiseValue,									// 0x0204
	CMD_GetETSILBTReferenceNoiseValue,					// 0x0205 (nur Generation 2)
	CMD_CarrierOnOff,									// 0x0206
	CMD_RandomModulationOnOff,							// 0x0207 (nur Generation 2)
	CMD_MeasureTxPower,									// 0x0208
	CMD_MeasureRxPower,									// 0x0209
	CMD_MeasureVSWR,									// 0x020A
	CMD_StartInventory,									// 0x020B
	CMD_OpenNoninventoriedTag,							// 0x020C
	CMD_OpenSpecificTag,								// 0x020D
	CMD_TxGen2CommandSelect,							// 0x020E
	CMD_TxGen2CommandACK,								// 0x020F
	CMD_TxGen2CommandNAK,								// 0x0210
	CMD_TxGen2CommandReqRN,								// 0x0211
	CMD_TxGen2CommandAccess,							// 0x0212
	CMD_TxGen2CommandRead,								// 0x0213
	CMD_TxGen2CommandWrite,								// 0x0214
	CMD_TxGen2CommandKill,								// 0x0215
	CMD_TxGen2CommandLock,								// 0x0216
	CMD_TxGen2CommandBlockWrite,						// 0x0217
	CMD_TxGen2CommandBlockErase,						// 0x0218
	CMD_TxGen2CommandQuery,								// 0x0219
	CMD_TxGen2CommandQueryAdjust,						// 0x021A
	CMD_TxGen2CommandQueryRep,							// 0x021B
	CMD_TxGen2CommandCustomCommand,						// 0x021C
	CMD_EN302208TestmodulationOnOff,					// 0x021D (nur Generation 2)
	CMD_DirectBulkGetEPCs,								// 0x021E
	CMD_SelectFrequencyAndSwitchCarrierOn,				// 0x021F
	CMD_ExecuteAccessSequence,							// 0x0220
	CMD_ExecuteKillSequence,							// 0x0221
	CMD_ExecuteWriteSequence,							// 0x0222

	// HSM
	CMD_HSM_Encrypt = 0x1000,							// 0x1000 (nur Generation 3)
	CMD_HSM_Decrypt,									// 0x1001 (nur Generation 3)
	CMD_HSM_GetRandomData,								// 0x1002 (nur Generation 3)
	CMD_HSM_GetChallenge,								// 0x1003 (nur Generation 3)
	CMD_HSM_Authenticate,								// 0x1004 (nur Generation 3)
	CMD_HSM_SetPublicCertificate,						// 0x1005 (nur Generation 3)
	CMD_HSM_SetWrappingKey,								// 0x1006 (nur Generation 3)
	CMD_HSM_SetAESKey,									// 0x1007 (nur Generation 3)

	// Befehl für Protokolltunnel Interprozesskommunikation
	CMD_KBRPTInterProcComm = 0x3FFF,					// 0x3FFF

	// RRU4 Befehlsantworten
	RES_GetMode = 0x8001,								// 0x8001
	RES_SetMode,										// 0x8002
	RES_GetCommunicationStandard,						// 0x8003
	RES_SetCommunicationStandard,						// 0x8004
	RES_GetPortPower,									// 0x8005
	RES_SetPortPower,									// 0x8006
	RES_GetCarrierFollowUpTime,							// 0x8007 (nur Generation 2)
	RES_SetCarrierFollowUpTime,							// 0x8008 (nur Generation 2)
	RES_GetAntennaMode,									// 0x8009 (nur Generation 2)
	RES_SetAntennaMode,									// 0x800A (nur Generation 2)
	RES_GetPortMultiplexSequenceAndExposureTime,		// 0x800B
	RES_SetPortMultiplexSequenceAndExposureTime,		// 0x800C
	RES_GetCableLossAndAntennaGain,						// 0x800D
	RES_SetCableLossAndAntennaGain,						// 0x800E
	RES_GetETSIPortChannelList,							// 0x800F
	RES_SetETSIPortChannelList,							// 0x8010
	RES_GetETSIPortChannelSwitchingMode,				// 0x8011 (nur Generation 2)
	RES_SetETSIPortChannelSwitchingMode,				// 0x8012 (nur Generation 2)
	RES_GetProfileList,									// 0x8013
	RES_GetProfile,										// 0x8014
	RES_SetProfile,										// 0x8015
	RES_GetModulationType,								// 0x8016 (nur Generation 2)
	RES_SetModulationType,								// 0x8017 (nur Generation 2)
	RES_GetExtendedResultFlag,							// 0x8018
	RES_SetExtendedResultFlag,							// 0x8019
	RES_GetErrorStatus,									// 0x801A
	RES_GetDefaultParameterSet,							// 0x801B
	RES_SetDefaultParameterSet,							// 0x801C
	RES_GetActiveParameterSet,							// 0x801D
	RES_SetActiveParameterSet,							// 0x801E
	RES_SaveActiveParameterSet,							// 0x801F
	RES_GetParameterByConfigId,							// 0x8020
	RES_SetParameterByConfigId,							// 0x8021
	RES_GetIOCardHwConfig,								// 0x8022 (nur Generation 2)
	RES_SetIOCardHwConfig,								// 0x8023 (nur Generation 2)
	RES_GetIOCardProtocolConfig,						// 0x8024 (nur Generation 2)
	RES_SetIOCardProtocolConfig,						// 0x8025 (nur Generation 2)
	RES_GetTime,										// 0x8026
	RES_SetTime,										// 0x8027
	RES_GetTemperature,									// 0x8028
	RES_GetSwVersion,									// 0x8029
	RES_GetHwVersion,									// 0x802A
	RES_GetLicenseKey,									// 0x802B
	RES_SetLicenseKey,									// 0x802C
	RES_GetIOCardExtendedInfo,							// 0x802D (nur Generation 2)
	RES_GetAntennaMapping,								// 0x802E (nur Generation 2)
	RES_SetAntennaMapping,								// 0x802F (nur Generation 2)
	RES_GetASyncPowerRampOffset,						// 0x8030 (nur Generation 2)
	RES_GetMaxPowerLimits,								// 0x8031
	RES_SetConfigPassword = 0x803E,						// 0x803E
	RES_EnterConfigPassword,							// 0x803F
	RES_LoadFactoryDefaults = 0x8040,					// 0x8040
	RES_FirmwareUpdatePrepare,							// 0x8041
	RES_FirmwareUpdateTransmitData,						// 0x8042
	RES_FirmwareUpdateFlash,							// 0x8043
	RES_RebootSystem,									// 0x8044
	RES_ActivateBuzzer,									// 0x8045
	RES_GetDeviceSerialNumber,							// 0x8046
	RES_GetDeviceTypeNumber,							// 0x8047
	RES_GetLEDMode,										// 0x8048
	RES_SetLEDMode,										// 0x8049
	RES_GetUptimeInMilliseconds,						// 0x804A
	RES_GetSelSessionAndTarget = 0x8080,				// 0x8080
	RES_SetSelSessionAndTarget,							// 0x8081
	RES_GetInitialQ,									// 0x8082
	RES_SetInitialQ,									// 0x8083
	RES_GetMaxAirCommErrors,							// 0x8084
	RES_SetMaxAirCommErrors,							// 0x8085
	RES_GetASyncObservedListParameters,					// 0x8086
	RES_SetASyncObservedListParameters,					// 0x8087
	RES_GetSelectFilterOnOff,							// 0x8088
	RES_SetSelectFilterOnOff,							// 0x8089
	RES_GetSelectFilterData,							// 0x808A
	RES_SetSelectFilterData,							// 0x808B
	RES_GPIOGetIOData,									// 0x808C
	RES_GPIOSetOutput,									// 0x808D
	RES_GPIOAddActionToActionlist,						// 0x808E
	RES_GPIOClearActionlist,							// 0x808F
	RES_GPIOAssignInputToActionlist,					// 0x8090
	RES_AddTagToBlacklist,								// 0x8091 (nur Generation 2)
	RES_DeleteBlacklist,								// 0x8092 (nur Generation 2)
	RES_GetBlacklistEntries,							// 0x8093 (nur Generation 2)
	RES_GetBlacklistEntriesExtended,					// 0x8094 (nur Generation 2)
	RES_SetEPCMemoryBankFilter = 0x8096,				// 0x8096 (nur Generation 2)
	RES_GPIOGetActionlist,								// 0x8097
	RES_GPIOSetActionlist,								// 0x8098
	RES_SetLED = 0x80A0,								// 0x80A0 (nur Generation 3)
	RES_KRAIInitializePort = 0x80D0,					// 0x80D0
	RES_KRAIGetPortInfo,								// 0x80D1
	RES_KRAISetLED,										// 0x80D2
	RES_KRAIGetSwVersion,								// 0x80D3 (nur Generation 3)
	RES_SyncGetEPCs = 0x8101,							// 0x8101
	RES_SyncBulkGetEPCs,								// 0x8102
	RES_SyncReadDataAny,								// 0x8103
	RES_SyncReadDataSpecific,							// 0x8104
	RES_SyncWriteDataAny,								// 0x8105
	RES_SyncWriteDataSpecific,							// 0x8106
	RES_SyncWriteMaskedDataAny,							// 0x8107
	RES_SyncWriteMaskedDataSpecific,					// 0x8108
	RES_SyncBlockEraseAny,								// 0x8109
	RES_SyncBlockEraseSpecific,							// 0x810A
	RES_SyncLockAny,									// 0x810B
	RES_SyncLockSpecific,								// 0x810C
	RES_SyncKillAny,									// 0x810D
	RES_SyncKillSpecific,								// 0x810E
	RES_SyncWriteEPCToSingleTag,						// 0x810F
	RES_ASyncGetRawEPCs,								// 0x8110
	RES_ASyncGetEPCs,									// 0x8111
	RES_ASyncReadDataAny,								// 0x8112
	RES_ASyncWriteDataAny,								// 0x8113
	RES_ASyncWriteMaskedDataAny,						// 0x8114
	RES_ASyncBlockEraseAny,								// 0x8115
	RES_ASyncLockAny,									// 0x8116
	RES_ASyncKillAny,									// 0x8117
	RES_ASyncStopCommand,								// 0x8118
	RES_ASyncGetTagRate,								// 0x8119
	RES_SyncWriteEPCSpecific,							// 0x811A
	RES_SyncReadDataUntilEndOfBankAny,					// 0x811B
	RES_SyncReadDataUntilEndOfBankSpecific,				// 0x811C
	RES_ASyncReadDataUntilEndOfBankAny,					// 0x811D
	RES_SyncBlockPermalockAny = 0x8120,					// 0x8120
	RES_SyncBlockPermalockSpecific,						// 0x8121
	RES_ASyncBlockPermalockAny,							// 0x8122
	RES_SyncAuthenticateAny,							// 0x8123
	RES_SyncAuthenticateSpecific,						// 0x8124
	RES_ASyncAuthenticateAny,							// 0x8125
	RES_SyncUntraceableAny,								// 0x8126
	RES_SyncUntraceableSpecific,						// 0x8127
	RES_ASyncUntraceableAny,							// 0x8128
	RES_SyncSJ5511ActivateSecureModeAny = 0x8170,		// 0x8170 (nur Generation 3)
	RES_SyncSJ5511ActivateSecureModeSpecific,			// 0x8171 (nur Generation 3)
	RES_ASyncSJ5511ActivateSecureModeAny,				// 0x8172 (nur Generation 3)
	RES_SyncSJ5511AuthenticateOBUAny,					// 0x8173 (nur Generation 3)
	RES_SyncSJ5511AuthenticateOBUSpecific,				// 0x8174 (nur Generation 3)
	RES_ASyncSJ5511AuthenticateOBUAny,					// 0x8175 (nur Generation 3)
	RES_SyncNXPG2XReadProtectAny = 0x8180,				// 0x8180 (nur Generation 2)
	RES_SyncNXPG2XReadProtectSpecific,					// 0x8181 (nur Generation 2)
	RES_SyncNXPG2XResetReadProtectAny,					// 0x8182 (nur Generation 2)
	RES_SyncNXPG2XResetReadProtectSpecific,				// 0x8183 (nur Generation 2)
	RES_SyncNXPG2XChangeEASAny,							// 0x8184 (nur Generation 2)
	RES_SyncNXPG2XChangeEASSpecific,					// 0x8185 (nur Generation 2)
	RES_SyncNXPG2XEASAlarm,								// 0x8186 (nur Generation 2)
	RES_ASyncNXPG2XReadProtectAny,						// 0x8187 (nur Generation 2)
	RES_ASyncNXPG2XResetReadProtectAny,					// 0x8188 (nur Generation 2)
	RES_ASyncNXPG2XChangeEASAny,						// 0x8189 (nur Generation 2)
	RES_ASyncNXPG2XEASAlarm,							// 0x818A (nur Generation 2)
	RES_SyncRSSILedBarSpecific = 0x81A0,				// 0x81A0 (nur Generation 3)
	RES_GetAntennaList = 0x8201,						// 0x8201
	RES_SetAntenna,										// 0x8202
	RES_SetFrequency,									// 0x8203
	RES_GetNoiseValue,									// 0x8204
	RES_GetETSILBTReferenceNoiseValue,					// 0x8205 (nur Generation 2)
	RES_CarrierOnOff,									// 0x8206
	RES_RandomModulationOnOff,							// 0x8207 (nur Generation 2)
	RES_MeasureTxPower,									// 0x8208
	RES_MeasureRxPower,									// 0x8209
	RES_MeasureVSWR,									// 0x820A
	RES_StartInventory,									// 0x820B
	RES_OpenNoninventoriedTag,							// 0x820C
	RES_OpenSpecificTag,								// 0x820D
	RES_TxGen2CommandSelect,							// 0x820E
	RES_TxGen2CommandACK,								// 0x820F
	RES_TxGen2CommandNAK,								// 0x8210
	RES_TxGen2CommandReqRN,								// 0x8211
	RES_TxGen2CommandAccess,							// 0x8212
	RES_TxGen2CommandRead,								// 0x8213
	RES_TxGen2CommandWrite,								// 0x8214
	RES_TxGen2CommandKill,								// 0x8215
	RES_TxGen2CommandLock,								// 0x8216
	RES_TxGen2CommandBlockWrite,						// 0x8217
	RES_TxGen2CommandBlockErase,						// 0x8218
	RES_TxGen2CommandQuery,								// 0x8219
	RES_TxGen2CommandQueryAdjust,						// 0x821A
	RES_TxGen2CommandQueryRep,							// 0x821B
	RES_TxGen2CommandCustomCommand,						// 0x821C
	RES_EN302208TestmodulationOnOff,					// 0x821D (nur Generation 2)
	RES_DirectBulkGetEPCs,								// 0x821E
	RES_SelectFrequencyAndSwitchCarrierOn,				// 0x821F
	RES_ExecuteAccessSequence,							// 0x8220
	RES_ExecuteKillSequence,							// 0x8221
	RES_ExecuteWriteSequence,							// 0x8222

	// HSM
	RES_HSM_Encrypt = 0x9000,							// 0x9000 (nur Generation 3)
	RES_HSM_Decrypt,									// 0x9001 (nur Generation 3)
	RES_HSM_GetRandomData,								// 0x9002 (nur Generation 3)
	RES_HSM_GetChallenge,								// 0x9003 (nur Generation 3)
	RES_HSM_Authenticate,								// 0x9004 (nur Generation 3)
	RES_HSM_SetPublicCertificate,						// 0x9005 (nur Generation 3)
	RES_HSM_SetWrappingKey,								// 0x9006 (nur Generation 3)
	RES_HSM_SetAESKey,									// 0x9007 (nur Generation 3)

	// Antwort vom Protokolltunnel Interprozesskommunikation
	RES_KBRPTInterProcComm = 0xBFFF,					// 0xBFFF

	// RRU4 asynchrone Befehlsantworten
	REA_ASyncGetRawEPCs = 0xC110,						// 0xC110
	REA_ASyncGetEPCs,									// 0xC111
	REA_ASyncReadDataAny,								// 0xC112
	REA_ASyncWriteDataAny,								// 0xC113
	REA_ASyncWriteMaskedDataAny,						// 0xC114
	REA_ASyncBlockEraseAny,								// 0xC115
	REA_ASyncLockAny,									// 0xC116
	REA_ASyncKillAny,									// 0xC117
	REA_ASyncReadDataUntilEndOfBankAny = 0xC11D,		// 0xC11D
	REA_ASyncBlockPermalockAny = 0xC122,				// 0xC122
	REA_ASyncAuthenticateAny = 0xC125,					// 0xC125
	REA_ASyncUntraceableAny = 0xC128,					// 0xC128
	REA_ASyncSJ5511ActivateSecureModeAny = 0xC172,		// 0xC172 (nur Generation 3)
	REA_ASyncSJ5511AuthenticateOBUAny = 0xC175,			// 0xC175 (nur Generation 3)
	REA_ASyncNXPG2XReadProtectAny = 0xC187,				// 0xC187 (nur Generation 2)
	REA_ASyncNXPG2XResetReadProtectAny,					// 0xC188 (nur Generation 2)
	REA_ASyncNXPG2XChangeEASAny,						// 0xC189 (nur Generation 2)
	REA_ASyncNXPG2XEASAlarm,							// 0xC18A (nur Generation 2)

	// RRU4 asynchrone Meldungen
	REA_ReaderMessage = 0xC301							// 0xC301
} tHostCommCommand;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	RF_NoError = 0,								// 0x00
	RF_NoData,									// 0x01
	RF_CRCError,								// 0x02
	RF_NoLicense,								// 0x03
	RF_OutOfRange,								// 0x04
	RF_NoStandard,								// 0x05
	RF_NoAntenna,								// 0x06
	RF_NoFrequency,								// 0x07
	RF_NoCarrier,								// 0x08
	RF_AntennaError,							// 0x09
	RF_NoTag,									// 0x0A
	RF_MoreThanOneTagInField,					// 0x0B
	RF_WrongLicenseKey,							// 0x0C
	RF_FirmwareRejected,						// 0x0D
	RF_WrongCommandFunctionMode,				// 0x0E
	RF_NoHandle,								// 0x0F
	RF_NoProfile,								// 0x10
	RF_AccessDenied,							// 0x11
	RF_TransmissionTimeExpired,					// 0x12
	RF_DatabaseError,							// 0x13
	RF_ParameterError,							// 0x14
	RF_SignatureVerificationFailed,				// 0x15
	RF_OutOfMemory = 0x7F,						// 0x7F
	RF_Nonspecific = 0x80						// 0x80
} tResultFlag;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	CFM_Normal = 0,
	CFM_Direct,
	CFM_Last
} tCommandFunctionMode;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	CS_None = 0,
	CS_AutoDetect,
	CS_ETSI_EN302208,
	CS_ETSI_EN302208_LBT,
	CS_FCC,
	CS_Special,
	CS_China,
	CS_Thailand,
	CS_Brazil,
	CS_SouthKorea,
	CS_Peru,
	CS_Singapore_High,
	CS_Australia,
	CS_Japan,
	CS_Malaysia,
	CS_Indonesia,
	CS_Guatemala_Low,		// Gen3 only
	CS_Guatemala_High,		// Gen3 only
	CS_Singapore_Low,
	CS_Last
} tCommunicationStandard;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	AM_All = 0,
	AM_Antenna1 = 0x10,
	AM_Antenna2,
	AM_Antenna3,
	AM_Antenna4,
	AM_Last
} tAntennaMode;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	EPCSW_Mode0 = 0,
	EPCSW_Mode1,
	EPCSW_Last
} tETSIPortChannelSwitchingMode;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	MT_DSB = 0,		// Zweiseitenbandmodulation
	MT_PRA,			// PR-ASK Modulation
	MT_SSB,			// Einseitenbandmodulation (momentan nicht unterstützt)
	MT_EXT,			// R1000-externe Zweiseitenbandmodulation (momentan nicht unterstützt)
	MT_Last
} tModulationType;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	OOF_Off = 0x00,
	OOF_On = 0xFF
} tOnOffFlag;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	CGF_Going = 0x00,
	CGF_DataChanging = 0x80,
	CGF_Coming = 0xFF
} tComingGoingFlag;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	SSHBF_Stop = 0x00,
	SSHBF_Heartbeat = 0x80,
	SSHBF_Start = 0xFF
} tStartStopHeartbeatFlag;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	RLBCF_Start = 0,
	RLBCF_Do,
	RLBCF_Stop,
	RLBCF_Last
} tRSSILedBarControlFlag;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	EMID_Error_NoFreeChannel = 0,
	EMID_Error_Antenna1,
	EMID_Error_Antenna2,
	EMID_Error_Antenna3,
	EMID_Error_Antenna4,
	EMID_Error_Antenna5,
	EMID_Error_Antenna6,
	EMID_Error_Antenna7,
	EMID_Error_Antenna8,
	EMID_Error_ETSIPowerCheck1,
	EMID_Error_ETSIPowerCheck2,
	EMID_Error_ETSIPowerCheck3,
	EMID_Error_ETSIPowerCheck4,
	EMID_Error_ETSIPowerCheck5,
	EMID_Error_ETSIPowerCheck6,
	EMID_Error_ETSIPowerCheck7,
	EMID_Error_ETSIPowerCheck8,
	EMID_Error_KRAIOverCurrent1,
	EMID_Error_KRAIOverCurrent2,
	EMID_Error_KRAIOverCurrent3,
	EMID_Error_KRAIOverCurrent4,
	EMID_Error_KRAIOverCurrent5,
	EMID_Error_KRAIOverCurrent6,
	EMID_Error_KRAIOverCurrent7,
	EMID_Error_KRAIOverCurrent8,
	EMID_Error_Last
} tErrorMessageId;

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	IM_Last = 0,
	IM_StartUserMessages = 0xC0
} tInfoMessage;

// ----------------------------------------------------------------------------
// Extended-Result-Flag
// (Das Extended-Result-Flag legt fest, welche Informationen innerhalb der
// Erweiterten Ergebnisdaten übertragen werden. Ist ein Bit gesetzt, wird
// die entsprechende Information übertragen.)

// Makros für den Zugriff auf das Extended-Result-Flag
#define ERF_MASK_A		0x01	// Antenne
#define ERF_MASK_R		0x02	// RSSI-Wert
#define ERF_MASK_T		0x04	// Zeitstempel
#define ERF_MASK_PC		0x08	// Protocol-Control-Word(s)
#define ERF_MASK_FRTP	0x10	// Frequenz und Tag-Phase
#define ERF_MASK_RDBM	0x20	// RSSI-Wert in dBm
#define ERF_MASK_ALL	0x3F

// ----------------------------------------------------------------------------

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	TEC_NoError = 0x00,
	TEC_NoAnswer,
	TEC_AccessDenied,
	TEC_VerifyFailed,
	TEC_Unspecified,
	TEC_Other = 0x80,
	TEC_NotSupported = 0x81,
	TEC_InsufficientPrivileges = 0x82,
	TEC_MemoryOverrun = 0x83,
	TEC_MemoryLocked = 0x84,
	TEC_CryptoSuiteError = 0x85,
	TEC_CommandNotEncapsulated = 0x86,
	TEC_ResponseBufferOverflow = 0x87,
	TEC_SecurityTimeout = 0x88,
	TEC_InsufficientPower = 0x8B,
	TEC_NonSpecific = 0x8F
} tTagErrorCode;

// ----------------------------------------------------------------------------

// Tag Speicherbereiche
#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	TMB_RFU = 0,
	TMB_EPC,
	TMB_TID,
	TMB_User,
	TMB_Last
} tTagMemoryBank;

// ============================================================================
// Kathrein RFID Antenna Interface (KRAI)

// Makros für die Einstellung der Antennenpolarisation

// WiRa 70° KRAI
#define KRAI_POLARIZATION_RHCP		0x01
#define KRAI_POLARIZATION_LHCP		0x02
#define KRAI_POLARIZATION_HP		0x04
#define KRAI_POLARIZATION_VP		0x08

// WiRa 30° CSB KRAI
#define KRAI_DIRECTION_LEFT			0x01
#define KRAI_DIRECTION_CENTER		0x02
#define KRAI_DIRECTION_RIGHT		0x04

// Enum für die LED-Funktion der KRAI-Antennen mit konfigurierbaren LEDs

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	KOF_Off = 0,
	KOF_On,
	KOF_RFOn,
	KOF_AntennaError,
	KOF_TagFound,
	KOF_TagOperationSuccess,
	KOF_ProtocolAccess,
	KOF_Last
} tKRAIOutputFunction;

// ============================================================================
// Enum zur Ansteuerung der LED auf dem Controller-Board (sofern LED vorhanden)

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	LM_Auto = 0,
	LM_Off,
	LM_Green,
	LM_Red,
	LM_Orange,
	LM_Last
} tLEDMode;

// ============================================================================
// Enum zur Ansteuerung der LEDs eines Gen3-Readers

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	LEDOF_Off = 0,
	LEDOF_On,
	LEDOF_Frequency1Hz,
	LEDOF_Frequency2Hz,
	LEDOF_Frequency4Hz,
	LEDOF_Frequency8Hz,
	LEDOF_RFOn,
	LEDOF_AntennaError,
	LEDOF_TagFound,
	LEDOF_TagOperationSuccess,
	LEDOF_ProtocolAccess,
	LEDOF_KRAIStatus = 32,
	LEDOF_WiFiStatus,
	LEDOF_GSMStatus,
	LEDOF_BluetoothStatus,
	LEDOF_EthernetLinkStatus,
	LEDOF_Last
} tLEDOutputFunction;

// ============================================================================
// Enums zur Arbeit mit dem HSM-Modul eines Gen3-Readers

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	AESM_ECB128 = 0,
	AESM_CBC128,
	AESM_Last
} tAESMode;

// ============================================================================
// allgemeine Daten für die Konfiguration von I/O-Karten

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	IOCT_AutoDetect = 0,
	IOCT_CardNotUsed,
	IOCT_RS232,
	IOCT_RS485,
	IOCT_Ethernet,
	IOCT_CAN,
	IOCT_GPIO,
	IOCT_GPIORRUMOD,
	IOCT_LED,
	IOCT_FTDI,
	IOCT_Last
} tIOCardType;

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	IOCS_NotSpecified = 0,
	IOCS_KBP,					// *** veraltet, wird ab Firmwareversion 1.50.00 nicht mehr unterstützt ***
	IOCS_STDPRTCL,				// *** veraltet, wird ab Firmwareversion 1.50.00 nicht mehr unterstützt ***
	IOCS_KBRP,					// Kathrein Burgstädt Reader Protokoll (KBRP)
	IOCS_KBRPNHS,				// Kathrein Burgstädt Reader Protokoll (KBRP) ohne Handshake
	IOCS_KBRPETH,				// Kathrein Burgstädt Reader Protokoll (KBRP) für Ethernetverbindungen
	IOCS_Last
} tIOCardService;

// ----------------------------------------------------------------------------
// Daten für den Zugriff auf I/O-Module mit UART

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	UDB_5 = 0,
	UDB_6,
	UDB_7,
	UDB_8,
	UDB_Last
} tUartDataBits;

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	UP_None = 0,
	UP_Odd,
	UP_Even,
	UP_Mark,
	UP_Space,
	UP_Last
} tUartParity;

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	USB_1 = 0,
	USB_15,
	USB_2,
	USB_Last
} tUartStopBits;

#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	UFC_None = 0,
	UFC_RTSCTS,
	UFC_RS485,
	UFC_Last
} tUartFlowControl;

// Konfigurationsdaten für den UART
typedef struct
{
	TDWord udwBaudRate;
	tUartDataBits enDataBits;
	tUartParity enParity;
	tUartStopBits enStopBits;
	tUartFlowControl enFlowControl;
} tUartConfig;

// ============================================================================
// Daten für den Zugriff auf das GPIO-Modul

#define GPIOMAXNUMBEROFINPUTS		16
#define GPIOMAXNUMBEROFOUTPUTS		16

// Funktionen für digitalen Ausgang
#if defined _MSC_VER || defined __GNUG__ || defined __BCPLUSPLUS__
	typedef enum : TByte
#else
	typedef enum
#endif
{
	GPIOOF_Off = 0,
	GPIOOF_On,
	GPIOOF_Frequency1Hz,
	GPIOOF_Frequency2Hz,
	GPIOOF_Frequency4Hz,
	GPIOOF_Frequency8Hz,
	GPIOOF_RFOn,
	GPIOOF_AntennaError,
	GPIOOF_TagFound,
	GPIOOF_TagOperationSuccess,
	GPIOOF_ProtocolAccess,
	GPIOOF_Last
} tGPIOOutputFunction;

typedef struct
{
	TWord uwDebounceTimeValue;
} tGPIOInputConfig;

typedef struct
{
	tGPIOOutputFunction enGPIOOutputFunction;
	TByte ubFirstAndLastAntenna;
	TWord uwTimeToSwitchOff;
} tGPIOOutputConfig;

// Konfigurationsstruktur (abzurufen über die Funktion "GetIOCardHwConfig")
typedef struct
{
	// Strukturgröße
	TWord uwSizeOfStruct;

	// Invertierungsflags
	TWord uwLogicalInputInvert;
	TWord uwLogicalOutputInvert;

	// Konfigurationsdaten für die Eingänge
	tGPIOInputConfig stGPIOInputConfig[GPIOMAXNUMBEROFINPUTS];

	// Konfigurationsdaten für die Ausgänge
	tGPIOOutputConfig stGPIOOutputConfig[GPIOMAXNUMBEROFOUTPUTS];

	// Prüfsumme
	TWord uwCRC;
} tGPIOConfig;

// ============================================================================
// Makro für I/O-Karten welche eine LED-Ausgabe enthalten

#define IOCARDMAXNUMBEROFLEDS			5

// ============================================================================
// Konfigurationsdaten für I/O-Module welche nur LED-Ausgänge als vom Reader
// ansteuerbare Funktionalität bereitstellen (z.B. GPIO-KaCom Generation 2).

typedef struct
{
	// Strukturgröße
	TWord uwSizeOfStruct;

	// Konfigurationsdaten für die Ausgänge
	tGPIOOutputConfig stLEDOutputConfig[IOCARDMAXNUMBEROFLEDS];

	// Prüfsumme
	TWord uwCRC;
} tIOModLEDConfig;

// ============================================================================
// Konfigurationsdaten für den Zugriff auf das Ethernet-Modul

#define ETHERNETCONFIG_CONFIGBIT_USEDHCP		0x01
#define ETHERNETCONFIG_CONFIGBIT_USEIPV6		0x02
#define ETHERNETCONFIG_MAXNETWORKNAMELENGTH		16

typedef struct
{
	// Strukturgröße
	TWord uwSizeOfStruct;

	// IPv6 und DHCP verwenden
	TByte ubConfigBits;

	// Netzwerkname
	char rgcNetworkName[ETHERNETCONFIG_MAXNETWORKNAMELENGTH];

	// Keep-Alive-Zeit
	TDWord udwKeepAliveTime;

	// IP Einstellungen
	union
	{
		// IPv4 Einstellungen
		struct
		{
			TByte rgubIPAddress[4];
			TByte rgubSubnetMask[4];
			TByte rgubGateway[4];
		} stIPv4;

		// IPv6 Einstellungen
		struct
		{
			TWord rguwIPAddress[8];
			TByte ubSubnetPrefixLength;
		} stIPv6;
	} ustIPSettings;

	// Konfigurationsdaten für die Ausgänge
	tGPIOOutputConfig stLEDOutputConfig[IOCARDMAXNUMBEROFLEDS];

	// Prüfsumme
	TWord uwCRC;
} tEthernetConfig;

// ============================================================================
// Konfigurationsdaten für den Zugriff auf das RS232-Modul

typedef struct
{
	// Strukturgröße
	TWord uwSizeOfStruct;

	// Konfigurationsdaten für den UART
	tUartConfig stUartConfig;

	// Konfigurationsdaten für die Ausgänge
	tGPIOOutputConfig stLEDOutputConfig[IOCARDMAXNUMBEROFLEDS];

	// Prüfsumme
	TWord uwCRC;
} tRS232Config;

// ============================================================================
// Konfigurationsdaten für den Zugriff auf das RS485/422-Modul

typedef struct
{
	// Strukturgröße
	TWord uwSizeOfStruct;

	// Umschaltung RS485 <-> RS422, Aktivierung Pull-Ups und Abschlusswiderstand
	struct
	{
		TBool bRS422;
		TBool bPullUp;
		TBool bTerminatingResistor;
	} stCardConfig;

	// Konfigurationsdaten für den UART
	tUartConfig stUartConfig;

	// Konfigurationsdaten für die Ausgänge
	tGPIOOutputConfig stLEDOutputConfig[IOCARDMAXNUMBEROFLEDS];

	// Prüfsumme
	TWord uwCRC;
} tRS485Config;

// ============================================================================
// GPIO-Konfiguration für das RRU RFID-Modul

#define RRUMODNUMBEROFINPUTANDOUTPUT		4

// Konfigurationsstruktur (abzurufen über die Funktion "GetIOCardHwConfig")
typedef struct
{
	// Strukturgröße
	TWord uwSizeOfStruct;

	// Invertierungsflags
	TByte ubLogicalInputInvert;
	TByte ubLogicalOutputInvert;

	// Konfigurationsdaten für die Eingänge
	struct
	{
		TWord uwDebounceTimeValue;
	} stGPIOInputConfig[RRUMODNUMBEROFINPUTANDOUTPUT];

	// Konfigurationsdaten für die Ausgänge
	tGPIOOutputConfig stGPIOOutputConfig[RRUMODNUMBEROFINPUTANDOUTPUT];

	// Prüfsumme
	TWord uwCRC;
} tRRUModGPIOConfig;

// ============================================================================

#endif
