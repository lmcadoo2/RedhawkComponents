#include <iostream>
#include "ossie/ossieSupport.h"

#include "OFDMTransmitter.h"
int main(int argc, char* argv[])
{
    OFDMTransmitter_i* OFDMTransmitter_servant;
    Resource_impl::start_component(OFDMTransmitter_servant, argc, argv);
    return 0;
}

