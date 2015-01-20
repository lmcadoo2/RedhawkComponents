#ifndef REEDSOLOMONENCODERDECODER_IMPL_BASE_H
#define REEDSOLOMONENCODERDECODER_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class ReedSolomonEncoderDecoder_base : public Resource_impl, protected ThreadedComponent
{
    public:
        ReedSolomonEncoderDecoder_base(const char *uuid, const char *label);
        ~ReedSolomonEncoderDecoder_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        short scheme_codec;
        bool encode;

        // Ports
        bulkio::InCharPort *dataCharIn;
        bulkio::OutCharPort *dataCharOut;

    private:
};
#endif // REEDSOLOMONENCODERDECODER_IMPL_BASE_H
