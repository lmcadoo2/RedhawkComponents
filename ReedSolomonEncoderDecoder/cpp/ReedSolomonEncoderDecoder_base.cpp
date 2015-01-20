#include "ReedSolomonEncoderDecoder_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

ReedSolomonEncoderDecoder_base::ReedSolomonEncoderDecoder_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    ThreadedComponent()
{
    loadProperties();

    dataCharIn = new bulkio::InCharPort("dataCharIn");
    addPort("dataCharIn", dataCharIn);
    dataCharOut = new bulkio::OutCharPort("dataCharOut");
    addPort("dataCharOut", dataCharOut);
}

ReedSolomonEncoderDecoder_base::~ReedSolomonEncoderDecoder_base()
{
    delete dataCharIn;
    dataCharIn = 0;
    delete dataCharOut;
    dataCharOut = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void ReedSolomonEncoderDecoder_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Resource_impl::start();
    ThreadedComponent::startThread();
}

void ReedSolomonEncoderDecoder_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Resource_impl::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void ReedSolomonEncoderDecoder_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Resource_impl::releaseObject();
}

void ReedSolomonEncoderDecoder_base::loadProperties()
{
    addProperty(scheme_codec,
                16,
                "scheme_codec",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(encode,
                true,
                "encode",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


