#ifndef ANALOGINPUTS_H
#define ANALOGINPUTS_H

#include "ecrt.h"
#include <sys/mman.h>
#include <stdio.h>
#include <cstring>
#include "AnalogIOData.h"

namespace ElmoMotor{

// Four-channel Analog inputs card Beckhoff EL3104 via Ethercat
class CAnalogInputsEL3104
{
public:
    CAnalogInputsEL3104();
    ~CAnalogInputsEL3104();

    // assign the device's position in a Ethercat net according to the network's topology, register domain, pdos and sync masters for this driver
    int SetEtherCATPosition(uint16_t position,
        uint32_t product_code =  0x0c203052,
        uint32_t vender_id = 0x00000002,
        uint16_t alias = 0);

    // function to initialize the device, mush be called after ecrt_request_master(0) which is defined in the EthercatMaster object
    int Initialize(ec_master_t **p_master);

    // Activate the slave's cyclic task, return value 0 means successfully activated, otherwise failed
    int Activate();

    // Retrieve analog inputs data
    int GetAnalogInputs(CAnalogInputsData* p_data) const;

    // Set the analog inputs to zero, this method requests the device to
    // start a zeroing procedure. This procedure needs ZEROING_DATA_COUNT
    // cycles to complete. In the following cycles, device averages
    // the readout data and reset its zero offset base value.
    void RequestZeroing();
    
    void Upload();//call at the beginning of a cycle, retrieve data from the card
    void Download(); // call at the end of a cycle, download data to the card

    static const int ZEROING_DATA_COUNT = 500; // indicates the number of data used for averaging when zeroing

private:
    
    int ReadData(CAnalogInputsData *p_data);//cyclic tasks
    
    CAnalogInputsData m_InputsData; // original device readout data
    int32_t m_baseValue[4];       // base value to be considered as zero offset
    double m_summedValue[4];        // summed value for zeroing
    int m_zeroingCountLast;
    
    ec_master_t* m_pMaster;
    //configuration for multi-domain
    ec_domain_t* m_pDomainAll;// = NULL;
    uint8_t* m_pDomainAllPd;

    //slave
    uint16_t m_alias; //= 0;
    uint16_t m_position;
    uint32_t m_vender_id; //= 0x00000002;
    uint32_t m_product_code; //= 0x0c203052;

    ec_slave_config_t* m_slaveConfigEL3104;
    ec_pdo_entry_reg_t m_domainAllRegs[5];
    ec_pdo_entry_info_t m_slavePdoEntries[44];
    ec_pdo_info_t m_slaveTxPdos[4];
    ec_sync_info_t m_slaveSyncs[5];

    unsigned int m_offsetValueChannel1;		
    unsigned int m_offsetValueChannel2;			
    unsigned int m_offsetValueChannel3;		
    unsigned int m_offsetValueChannel4;		
};

}

#endif // ANALOGINPUTS_H
