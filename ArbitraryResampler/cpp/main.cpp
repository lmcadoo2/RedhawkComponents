#include <iostream>
#include "ossie/ossieSupport.h"

#include "ArbitraryResampler.h"
int main(int argc, char* argv[])
{
    ArbitraryResampler_i* ArbitraryResampler_servant;
    Resource_impl::start_component(ArbitraryResampler_servant, argc, argv);
    return 0;
}

