#ifndef REEDSOLOMONENCODERDECODER_IMPL_H
#define REEDSOLOMONENCODERDECODER_IMPL_H

#include "ReedSolomonEncoderDecoder_base.h"

//Dependencies
#include <liquid/liquid.h>
#include "fec.h"

class ReedSolomonEncoderDecoder_i : public ReedSolomonEncoderDecoder_base
{
    ENABLE_LOGGING
    public:
        ReedSolomonEncoderDecoder_i(const char *uuid, const char *label);
        ~ReedSolomonEncoderDecoder_i();
        int serviceFunction();
    private:
        fec_scheme scheme_list[17];
        boost::mutex mutex_lock;
};

#endif // REEDSOLOMONENCODERDECODER_IMPL_H
