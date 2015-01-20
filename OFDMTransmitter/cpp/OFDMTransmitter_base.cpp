#include "OFDMTransmitter_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

OFDMTransmitter_base::OFDMTransmitter_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    ThreadedComponent()
{
    loadProperties();

    dataCharIn = new bulkio::InCharPort("dataCharIn");
    addPort("dataCharIn", dataCharIn);
    dataFloatOut = new bulkio::OutFloatPort("dataFloatOut");
    addPort("dataFloatOut", dataFloatOut);
}

OFDMTransmitter_base::~OFDMTransmitter_base()
{
    delete dataCharIn;
    dataCharIn = 0;
    delete dataFloatOut;
    dataFloatOut = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void OFDMTransmitter_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Resource_impl::start();
    ThreadedComponent::startThread();
}

void OFDMTransmitter_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Resource_impl::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void OFDMTransmitter_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Resource_impl::releaseObject();
}

void OFDMTransmitter_base::loadProperties()
{
    addProperty(subcarriers,
                64,
                "subcarriers",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(cyclic_prefix,
                16,
                "cyclic_prefix",
                "",
                "readwrite",
                "bits",
                "external",
                "configure");

    addProperty(error_correction_scheme_codec_inner,
                0,
                "error_correction_scheme_codec_inner",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(error_detection_scheme_codec,
                0,
                "error_detection_scheme_codec",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(error_correction_scheme_codec_outer,
                3,
                "error_correction_scheme_codec_outer",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    // Set the sequence with its initial values
    pilot_frequencies.push_back(4);
    pilot_frequencies.push_back(12);
    pilot_frequencies.push_back(20);
    pilot_frequencies.push_back(44);
    pilot_frequencies.push_back(52);
    pilot_frequencies.push_back(60);
    addProperty(pilot_frequencies,
                pilot_frequencies,
                "pilot_frequencies",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    // Set the sequence with its initial values
    null_frequencies.push_back(0);
    null_frequencies.push_back(26);
    null_frequencies.push_back(27);
    null_frequencies.push_back(28);
    null_frequencies.push_back(29);
    null_frequencies.push_back(30);
    null_frequencies.push_back(31);
    null_frequencies.push_back(32);
    null_frequencies.push_back(33);
    null_frequencies.push_back(34);
    null_frequencies.push_back(35);
    null_frequencies.push_back(36);
    null_frequencies.push_back(37);
    null_frequencies.push_back(38);
    addProperty(null_frequencies,
                null_frequencies,
                "null_frequencies",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


