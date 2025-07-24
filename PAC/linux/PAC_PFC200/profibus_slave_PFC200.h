/// @file profibus_slave_PFC200.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC
/// по работе с Profibus Slave.
///
/// This module uses the handling of the PROFIBUS slave device. That contains starting and stopping
/// of the device functionality as well as the process data exchange between the fieldbus an PLC process
/// data images.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef PROFIBUS_SLAVE_PFC200
#define PROFIBUS_SLAVE_PFC200

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

#ifdef USE_PROFIBUS
extern "C"
    {
#include <dal/adi_application_interface.h>    /* application device interface (ADI) */

#include "dpsTypes.h"                         /* type definitions of the PROFIBUS slave device interface */
    }
#endif // USE_PROFIBUS

#include "profibus_slave.h"
#include "smart_ptr.h"
#include "log.h"

#ifndef USE_PROFIBUS
class profibus_slave_PFC200: public profibus_slave
    {
private:
    static auto_smart_ptr < profibus_slave_PFC200 > instance;

public:
    enum CONSTANTS
    {
    MAX_DEVICE_LIST_ENTRIES = 10,
    };

    /// @brief Получение единственного экземпляра класса для работы.
    ///
    /// @return - указатель на единственный объект класса @ref
    /// profibus_slave_PFC200.
    static profibus_slave_PFC200* get_instance();

    virtual ~profibus_slave_PFC200() {}

    int init() { return 0; }

    /// @brief This function is the main loop. It exchanges the process data between the input an output process images.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    /// @param iDpsDeviceId - identification of the PROFIBUS slave device.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int eval() { return 0; }

    //Lua functions.

    double get_double( int offset ) { return 0.0; }

    bool get_bool( int byte_offset, int bit_offset ) { return false; }
    void set_bool( int byte_offset, int bit_offset, bool val ) {}

    int get_int( int byte_offset ) { return 0; }
    int get_int4( int byte_offset ) { return 0; }
    void set_int( int byte_offset, int val ) {}
    };

#else // USE_PROFIBUS

//-----------------------------------------------------------------------------
/// @brief Работа с Profibus Slave.


class profibus_slave_PFC200: public profibus_slave
    {
public:
    enum CONSTANTS
    {
    MAX_DEVICE_LIST_ENTRIES = 10,
    };

    /// @brief Получение единственного экземпляра класса для работы.
    ///
    /// @return - указатель на единственный объект класса @ref
    /// profibus_slave_PFC200.
    static profibus_slave_PFC200* get_instance();

    virtual ~profibus_slave_PFC200()
        {
        close();
        }

    int init();

    /// @brief This function is the main loop. It exchanges the process data between the input an output process images.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    /// @param iDpsDeviceId - identification of the PROFIBUS slave device.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int eval();

    //Lua functions.

    double get_double( int offset );

    bool get_bool( int byte_offset, int bit_offset );
    void set_bool( int byte_offset, int bit_offset, bool val );

    int get_int( int byte_offset );
    void set_int( int byte_offset, int val );

    int get_int4( int byte_offset );

private:
    profibus_slave_PFC200()
        {
        pstAdi = NULL;
        iDpsDeviceId = 0;
        }

    void close();

    static auto_smart_ptr < profibus_slave_PFC200 > instance;///< Экземпляр класса.

    /// @brief This function returns a reference to the ADI.
    ///
    /// @param pstAdi - pointer to the memory where the ADI structure should
    /// be returned.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t get_ADI( tApplicationDeviceInterface** ppstAdi );

    /// @brief This function initializes the ADI.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t init_ADI( tApplicationDeviceInterface* pstAdi );

    ///  @brief This function scans for devices accessible by the DAL.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t scan_for_devices( tApplicationDeviceInterface* pstAdi );

    /// @brief This function returns the list of devices accessible by the DAL.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    /// @param pulNrDevicesFound - pointer to the memory where the number of
    /// found devices should be returned.
    /// @param pastDeviceList - pointer to the device list to be filled by this
    /// function.
    /// @param ulMaxNumOfDevicesInList - maximum number of device that can be
    /// handled by the list.
    /// @param uiDpsDeviceId - identification of the PROFIBUS slave device.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t get_device_list( tApplicationDeviceInterface* pstAdi,
    size_t* pulNrDevicesFound, tDeviceInfo* pastDeviceList,
    size_t ulMaxNumOfDevicesInList );

    /// @brief This function returns the PROFIBUS slave device ID.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    /// @param puiDpsDeviceId - pointer to the memory where the ID of the
    /// PROFIBUS slave device should be returned.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t get_device_id( size_t ulNrDevicesFound,
    tDeviceInfo* pastDeviceList, tDeviceId* piDpsDeviceId );

    /// @brief This function opens the PROFIBUS slave device.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    /// @param iDpsDeviceId - identification of the PROFIBUS slave device.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t open_device( tApplicationDeviceInterface* pstAdi,
        tDeviceId iDpsDeviceId );

    /// @brief This function sets up the PLC watchdog.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t setup_watchdog( tApplicationDeviceInterface* pstAdi );

    /// @brief This function configures the PROFIBUS slave device with the
    /// device specific parameter.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure..
    /// @param iDpsDeviceId -identification of the PROFIBUS slave device.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t configure_device( tApplicationDeviceInterface* pstAdi,
        tDeviceId iDpsDeviceId );

    /// @brief This function reports that the application has changed to the
    /// running state.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t report_app_state_running_evt( tApplicationDeviceInterface* pstAdi );

    /// @brief This function reports the start of PLC watchdog.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t start_watchdog( tApplicationDeviceInterface* pstAdi );


    /// @brief This function stops the watchdog.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t stop_watchdog( tApplicationDeviceInterface* pstAdi );

    /// @brief This function closes the PROFIBUS slave device.
    ///
    /// @param pstAdi - pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t close_device( tApplicationDeviceInterface* pstAdi );

    /// @brief This function releases the Application-Data-Interface "ADI".
    ///
    /// @param pstAdi  pointer to the initialized ADI structure.
    ///
    /// @return - \ref DAL_SUCCESS when the function has successfully
    /// processed. In other cases it returns \ref DAL_FAILURE.
    int32_t release_ADI( tApplicationDeviceInterface* pstAdi );


    tApplicationDeviceInterface* pstAdi; //Pointer to the application interface.
    tDeviceId iDpsDeviceId;		 //PROFIBUS DP slave device ID.

    uint8_t aucPlcPrcImgInp[244]; 	//Input process image of the PLC.
    uint8_t aucPlcPrcImgOutp[244]; 	//Output process image of the PLC.
    };
//-----------------------------------------------------------------------------
#endif //USE_PROFIBUS

#endif // PROFIBUS_SLAVE_PFC200
