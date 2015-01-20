#ifndef OFDMRECEIVER_IMPL_BASE_H
#define OFDMRECEIVER_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class OFDMReceiver_base : public Resource_impl, protected ThreadedComponent
{
    public:
        OFDMReceiver_base(const char *uuid, const char *label);
        ~OFDMReceiver_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        short subcarriers;
        short cyclic_prefix;
        std::vector<short> pilot_frequencies;
        std::vector<short> null_frequencies;

        // Ports
        bulkio::InFloatPort *dataFloatIn;
        bulkio::OutCharPort *dataCharOut;

    private:
};
#endif // OFDMRECEIVER_IMPL_BASE_H
