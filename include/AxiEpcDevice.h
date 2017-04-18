#ifndef __AXIEPCDEVICD_H_
#define __AXIEPCDEVICD_H_


#include <sys/types.h>
#include <stdint.h>

#define DEVNAME "/dev/Image_m"

namespace Xilinx{

class AxiEpcDevice{
    public:
        typedef struct AXI_EPC_DATA{
            uint32_t address;
            uint32_t value;
        }AxiRegister;

        AxiEpcDevice();
        ~AxiEpcDevice();

        void setRegisterValue(uint32_t address, uint32_t value);
        uint32_t getRegisterValue(uint32_t address);

    private:
        char dev[128];
        AxiRegister Rtemp;
        int devfd;

};


}

#endif //__AXIEPCDEVICD_H_
