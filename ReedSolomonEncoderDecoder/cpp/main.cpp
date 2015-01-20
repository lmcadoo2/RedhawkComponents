#include <iostream>
#include "ossie/ossieSupport.h"

#include "ReedSolomonEncoderDecoder.h"
int main(int argc, char* argv[])
{
    ReedSolomonEncoderDecoder_i* ReedSolomonEncoderDecoder_servant;
    Resource_impl::start_component(ReedSolomonEncoderDecoder_servant, argc, argv);
    return 0;
}

