#ifndef ARBITRARYRESAMPLER_IMPL_H
#define ARBITRARYRESAMPLER_IMPL_H

#include "ArbitraryResampler_base.h"

//PLEASE NOTE: This Component requires the use of the liquid library

#include "liquid.h"

class ArbitraryResampler_i : public ArbitraryResampler_base
{
    ENABLE_LOGGING
    public:
        ArbitraryResampler_i(const char *uuid, const char *label);
        ~ArbitraryResampler_i();
        int serviceFunction();

    private:
		std::map<std::string, resamp_rrrf> resamplers_real;
		std::map<std::string, resamp_crcf> resamplers_complex;

};

#endif // ARBITRARYRESAMPLER_IMPL_H
