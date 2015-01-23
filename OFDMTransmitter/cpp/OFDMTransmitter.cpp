/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "OFDMTransmitter.h"

PREPARE_LOGGING(OFDMTransmitter_i)

OFDMTransmitter_i::OFDMTransmitter_i(const char *uuid, const char *label) :
    OFDMTransmitter_base(uuid, label)
{
	scheme_list = {LIQUID_FEC_NONE, LIQUID_FEC_REP3, LIQUID_FEC_REP5, LIQUID_FEC_HAMMING74, LIQUID_FEC_HAMMING84,
			LIQUID_FEC_HAMMING128, LIQUID_FEC_GOLAY2412, LIQUID_FEC_SECDED2216, LIQUID_FEC_SECDED3932,
			LIQUID_FEC_SECDED7264, LIQUID_FEC_CONV_V27, LIQUID_FEC_CONV_V29, LIQUID_FEC_CONV_V39,
			LIQUID_FEC_CONV_V615, LIQUID_FEC_CONV_V27P23, LIQUID_FEC_CONV_V27P34, LIQUID_FEC_CONV_V27P45,
			LIQUID_FEC_CONV_V27P56, LIQUID_FEC_CONV_V27P67, LIQUID_FEC_CONV_V27P78, LIQUID_FEC_CONV_V29P23,
			LIQUID_FEC_CONV_V29P34, LIQUID_FEC_CONV_V29P45, LIQUID_FEC_CONV_V29P56, LIQUID_FEC_CONV_V29P67,
			LIQUID_FEC_CONV_V29P78, LIQUID_FEC_RS_M8};
	ed_scheme_list = {LIQUID_CRC_NONE, LIQUID_CRC_CHECKSUM, LIQUID_CRC_8, LIQUID_CRC_16, LIQUID_CRC_24, LIQUID_CRC_32};
	mod_scheme_list = {LIQUID_MODEM_PSK2, LIQUID_MODEM_PSK4, LIQUID_MODEM_PSK8, LIQUID_MODEM_PSK16, LIQUID_MODEM_PSK32,
			LIQUID_MODEM_PSK64, LIQUID_MODEM_PSK128, LIQUID_MODEM_PSK256, LIQUID_MODEM_DPSK2, LIQUID_MODEM_DPSK4,
			LIQUID_MODEM_DPSK8, LIQUID_MODEM_DPSK16, LIQUID_MODEM_DPSK32, LIQUID_MODEM_DPSK64, LIQUID_MODEM_DPSK128,
			LIQUID_MODEM_DPSK256, LIQUID_MODEM_ASK2, LIQUID_MODEM_ASK4, LIQUID_MODEM_ASK8, LIQUID_MODEM_ASK16,
			LIQUID_MODEM_ASK32, LIQUID_MODEM_ASK64, LIQUID_MODEM_ASK128, LIQUID_MODEM_ASK256, LIQUID_MODEM_QAM4,
			LIQUID_MODEM_QAM8, LIQUID_MODEM_QAM16, LIQUID_MODEM_QAM32, LIQUID_MODEM_QAM64, LIQUID_MODEM_QAM128,
			LIQUID_MODEM_QAM256, LIQUID_MODEM_APSK4, LIQUID_MODEM_APSK8, LIQUID_MODEM_APSK16, LIQUID_MODEM_APSK32,
			LIQUID_MODEM_APSK64, LIQUID_MODEM_APSK128, LIQUID_MODEM_APSK256, LIQUID_MODEM_BPSK, LIQUID_MODEM_QPSK,
			LIQUID_MODEM_OOK, LIQUID_MODEM_SQAM32, LIQUID_MODEM_SQAM128, LIQUID_MODEM_V29, LIQUID_MODEM_ARB16OPT,
			LIQUID_MODEM_ARB32OPT, LIQUID_MODEM_ARB64OPT, LIQUID_MODEM_ARB128OPT, LIQUID_MODEM_ARB256OPT};
}

OFDMTransmitter_i::~OFDMTransmitter_i()
{
}

/***********************************************************************************************

    Basic functionality:

        The service function is called by the serviceThread object (of type ProcessThread).
        This call happens immediately after the previous call if the return value for
        the previous call was NORMAL.
        If the return value for the previous call was NOOP, then the serviceThread waits
        an amount of time defined in the serviceThread's constructor.
        
    SRI:
        To create a StreamSRI object, use the following code:
                std::string stream_id = "testStream";
                BULKIO::StreamSRI sri = bulkio::sri::create(stream_id);

	Time:
	    To create a PrecisionUTCTime object, use the following code:
                BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();

        
    Ports:

        Data is passed to the serviceFunction through the getPacket call (BULKIO only).
        The dataTransfer class is a port-specific class, so each port implementing the
        BULKIO interface will have its own type-specific dataTransfer.

        The argument to the getPacket function is a floating point number that specifies
        the time to wait in seconds. A zero value is non-blocking. A negative value
        is blocking.  Constants have been defined for these values, bulkio::Const::BLOCKING and
        bulkio::Const::NON_BLOCKING.

        Each received dataTransfer is owned by serviceFunction and *MUST* be
        explicitly deallocated.

        To send data using a BULKIO interface, a convenience interface has been added 
        that takes a std::vector as the data input

        NOTE: If you have a BULKIO dataSDDS or dataVITA49  port, you must manually call 
              "port->updateStats()" to update the port statistics when appropriate.

        Example:
            // this example assumes that the component has two ports:
            //  A provides (input) port of type bulkio::InShortPort called short_in
            //  A uses (output) port of type bulkio::OutFloatPort called float_out
            // The mapping between the port and the class is found
            // in the component base class header file

            bulkio::InShortPort::dataTransfer *tmp = short_in->getPacket(bulkio::Const::BLOCKING);
            if (not tmp) { // No data is available
                return NOOP;
            }

            std::vector<float> outputData;
            outputData.resize(tmp->dataBuffer.size());
            for (unsigned int i=0; i<tmp->dataBuffer.size(); i++) {
                outputData[i] = (float)tmp->dataBuffer[i];
            }

            // NOTE: You must make at least one valid pushSRI call
            if (tmp->sriChanged) {
                float_out->pushSRI(tmp->SRI);
            }
            float_out->pushPacket(outputData, tmp->T, tmp->EOS, tmp->streamID);

            delete tmp; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
            return NORMAL;

        If working with complex data (i.e., the "mode" on the SRI is set to
        true), the std::vector passed from/to BulkIO can be typecast to/from
        std::vector< std::complex<dataType> >.  For example, for short data:

            bulkio::InShortPort::dataTransfer *tmp = myInput->getPacket(bulkio::Const::BLOCKING);
            std::vector<std::complex<short> >* intermediate = (std::vector<std::complex<short> >*) &(tmp->dataBuffer);
            // do work here
            std::vector<short>* output = (std::vector<short>*) intermediate;
            myOutput->pushPacket(*output, tmp->T, tmp->EOS, tmp->streamID);

        Interactions with non-BULKIO ports are left up to the component developer's discretion

    Properties:
        
        Properties are accessed directly as member variables. For example, if the
        property name is "baudRate", it may be accessed within member functions as
        "baudRate". Unnamed properties are given the property id as its name.
        Property types are mapped to the nearest C++ type, (e.g. "string" becomes
        "std::string"). All generated properties are declared in the base class
        (OFDMTransmitter_base).
    
        Simple sequence properties are mapped to "std::vector" of the simple type.
        Struct properties, if used, are mapped to C++ structs defined in the
        generated file "struct_props.h". Field names are taken from the name in
        the properties file; if no name is given, a generated name of the form
        "field_n" is used, where "n" is the ordinal number of the field.
        
        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            //  - A boolean called scaleInput
              
            if (scaleInput) {
                dataOut[i] = dataIn[i] * scaleValue;
            } else {
                dataOut[i] = dataIn[i];
            }
            
        Callback methods can be associated with a property so that the methods are
        called each time the property value changes.  This is done by calling 
        addPropertyChangeListener(<property name>, this, &OFDMTransmitter_i::<callback method>)
        in the constructor.

        Callback methods should take two arguments, both const pointers to the value
        type (e.g., "const float *"), and return void.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            
        //Add to OFDMTransmitter.cpp
        OFDMTransmitter_i::OFDMTransmitter_i(const char *uuid, const char *label) :
            OFDMTransmitter_base(uuid, label)
        {
            addPropertyChangeListener("scaleValue", this, &OFDMTransmitter_i::scaleChanged);
        }

        void OFDMTransmitter_i::scaleChanged(const float *oldValue, const float *newValue)
        {
            std::cout << "scaleValue changed from" << *oldValue << " to " << *newValue
                      << std::endl;
        }
            
        //Add to OFDMTransmitter.h
        void scaleChanged(const float* oldValue, const float* newValue);
        

************************************************************************************************/
int OFDMTransmitter_i::serviceFunction()
{

	// Lock to grab properties
	mutex_lock.lock();
	unsigned int num_subcarriers = subcarriers;
	unsigned int cyclic_prefix_len = cyclic_prefix;
	unsigned int tap_length = taper_length;
	short ec_outer = error_correction_scheme_codec_outer;
	short ec_inner = error_correction_scheme_codec_inner;
	short ed_codec = error_detection_scheme_codec;
	short mod_codec = modulation_scheme_codec;
	std::vector<short> pilot_freqs = pilot_frequencies;
	std::vector<short> null_freqs = null_frequencies;
	mutex_lock.unlock();

	// Assign the subcarriers to pilot frequencies, null frequencies, and data frequencies.
	// Ideally make this 2 properties, pilot and null (sequences), and data is the rest
    unsigned char p[num_subcarriers];
    for(unsigned int i = 0; i< num_subcarriers; i++)
    {
    	if(std::find(null_freqs.begin(), null_freqs.end(), i) != null_freqs.end())
    	{
    		p[i] = OFDMFRAME_SCTYPE_NULL;
    	}
    	else if(std::find(pilot_freqs.begin(), pilot_freqs.end(), i) != pilot_freqs.end())
    	{
    		p[i] = OFDMFRAME_SCTYPE_PILOT;
    	}
    	else
    	{
    		p[i] = OFDMFRAME_SCTYPE_DATA;
    	}
    }

    // Other properties, like the error correction codec
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
	fgprops.check = ed_scheme_list[ed_codec];
	fgprops.fec0 = scheme_list[ec_inner];
	fgprops.fec1 = scheme_list[ec_outer];
	fgprops.mod_scheme =  mod_scheme_list[mod_codec];


	// Get input data and put in usable (to liquid) form
	bulkio::InCharPort::dataTransfer * input = dataCharIn->getPacket(bulkio::Const::BLOCKING);
	if(not input)
	{
		return NOOP;
	}
	std::vector<signed char> input_data = input->dataBuffer;
	unsigned int payload_len = input_data.size();
	unsigned char payload[payload_len];
	for(unsigned int i = 0; i<payload_len; i++)
	{
		payload[i] = (unsigned char) input_data[i];
	}

	// Create a header, for I have no idea what. So I stole the liquid header
	unsigned char header[8];
	for (unsigned int i=0; i<8; i++)
	{
		header[i]  = i      & 0xff;
	}

	// Assemble output structures
	unsigned int symbol_length = num_subcarriers+cyclic_prefix_len;
	std::complex<float> symbol[symbol_length];
	std::vector<float> output;

	// Change SRI mode, since we will be pushing out complex data
	input->SRI.mode = true;
	dataFloatOut->pushSRI(input->SRI);

	// Create actual frame generator
	ofdmflexframegen generator = ofdmflexframegen_create(num_subcarriers, cyclic_prefix_len, tap_length, p, &fgprops);
	ofdmflexframegen_assemble(generator, header, payload, payload_len);

	// Loop to write symbols to a buffer, push them out, and to signify the EOS
	int last = 0;
	unsigned int num_written = 0;
	bool EOS = false;
	while(not last)
	{
		num_written = symbol_length;
		last = ofdmflexframegen_writesymbol(generator, symbol);

		// Only num_written complex samples written, creating 2*num_written floats
		output.resize(2*num_written);
		for(unsigned int i = 0; i<num_written*2; i+=2)
		{
			output[i] = symbol[i/2].real();
			output[i+1] = symbol[i/2].imag();
		}
		// If there is no more input and we have generated the last symbol, EOS
		if(input->EOS and last)
		{
			EOS = true;
		}
		dataFloatOut->pushPacket(output, input->T, EOS, input->streamID);

		// Clear the output buffer for the next symbol
		output.clear();
	}

	ofdmflexframegen_destroy(generator);

	delete input;

	return NORMAL;
}

