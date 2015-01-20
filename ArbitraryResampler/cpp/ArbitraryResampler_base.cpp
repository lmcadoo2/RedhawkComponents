#include "ArbitraryResampler_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

ArbitraryResampler_base::ArbitraryResampler_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    ThreadedComponent()
{
    loadProperties();

    dataFloatIn = new bulkio::InFloatPort("dataFloatIn");
    addPort("dataFloatIn", dataFloatIn);
    dataFloatOut = new bulkio::OutFloatPort("dataFloatOut");
    addPort("dataFloatOut", dataFloatOut);
}

ArbitraryResampler_base::~ArbitraryResampler_base()
{
    delete dataFloatIn;
    dataFloatIn = 0;
    delete dataFloatOut;
    dataFloatOut = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void ArbitraryResampler_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Resource_impl::start();
    ThreadedComponent::startThread();
}

void ArbitraryResampler_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Resource_impl::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void ArbitraryResampler_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Resource_impl::releaseObject();
}

void ArbitraryResampler_base::loadProperties()
{
    addProperty(resampling_rate,
                2,
                "resampling_rate",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


