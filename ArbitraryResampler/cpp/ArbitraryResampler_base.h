#ifndef ARBITRARYRESAMPLER_IMPL_BASE_H
#define ARBITRARYRESAMPLER_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class ArbitraryResampler_base : public Resource_impl, protected ThreadedComponent
{
    public:
        ArbitraryResampler_base(const char *uuid, const char *label);
        ~ArbitraryResampler_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        float resampling_rate;

        // Ports
        bulkio::InFloatPort *dataFloatIn;
        bulkio::OutFloatPort *dataFloatOut;

    private:
};
#endif // ARBITRARYRESAMPLER_IMPL_BASE_H
