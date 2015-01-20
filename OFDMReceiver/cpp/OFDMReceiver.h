#ifndef OFDMRECEIVER_IMPL_H
#define OFDMRECEIVER_IMPL_H

#include "OFDMReceiver_base.h"
#include "OFDMReceiverCallback.h"
#include <liquid/liquid.h>
#include "fec.h"

class OFDMReceiver_i : public OFDMReceiver_base
{
    ENABLE_LOGGING
    public:
        OFDMReceiver_i(const char *uuid, const char *label);
        ~OFDMReceiver_i();
        int serviceFunction();
    private:
        boost::mutex mutex_lock;
};

#endif // OFDMRECEIVER_IMPL_H
