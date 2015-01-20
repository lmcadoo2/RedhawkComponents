#include <iostream>
#include "ossie/ossieSupport.h"

#include "OFDMReceiver.h"
int main(int argc, char* argv[])
{
    OFDMReceiver_i* OFDMReceiver_servant;
    Resource_impl::start_component(OFDMReceiver_servant, argc, argv);
    return 0;
}

