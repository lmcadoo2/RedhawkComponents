#ifndef OFDMTRANSMITTER_IMPL_H
#define OFDMTRANSMITTER_IMPL_H

#include "OFDMTransmitter_base.h"
#include "liquid.h"
#include "fec.h"

class OFDMTransmitter_i : public OFDMTransmitter_base
{
    ENABLE_LOGGING
    public:
        OFDMTransmitter_i(const char *uuid, const char *label);
        ~OFDMTransmitter_i();
        int serviceFunction();
    private:
        fec_scheme scheme_list[27];
        crc_scheme ed_scheme_list[6];
        modulation_scheme mod_scheme_list[49];
        boost::mutex mutex_lock;
};

#endif // OFDMTRANSMITTER_IMPL_H
