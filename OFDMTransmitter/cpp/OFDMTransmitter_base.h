#ifndef OFDMTRANSMITTER_IMPL_BASE_H
#define OFDMTRANSMITTER_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class OFDMTransmitter_base : public Resource_impl, protected ThreadedComponent
{
    public:
        OFDMTransmitter_base(const char *uuid, const char *label);
        ~OFDMTransmitter_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        short subcarriers;
        short taper_length;
        short cyclic_prefix;
        short error_correction_scheme_codec_inner;
        short error_detection_scheme_codec;
        short error_correction_scheme_codec_outer;
        short modulation_scheme_codec;
        std::vector<short> pilot_frequencies;
        std::vector<short> null_frequencies;

        // Ports
        bulkio::InCharPort *dataCharIn;
        bulkio::OutFloatPort *dataFloatOut;

    private:
};
#endif // OFDMTRANSMITTER_IMPL_BASE_H
