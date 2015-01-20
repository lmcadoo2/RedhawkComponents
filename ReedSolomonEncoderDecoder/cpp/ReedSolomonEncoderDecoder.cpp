/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "ReedSolomonEncoderDecoder.h"

PREPARE_LOGGING(ReedSolomonEncoderDecoder_i)

ReedSolomonEncoderDecoder_i::ReedSolomonEncoderDecoder_i(const char *uuid, const char *label) :
    ReedSolomonEncoderDecoder_base(uuid, label)
{
	scheme_list = {LIQUID_FEC_CONV_V27, LIQUID_FEC_CONV_V29, LIQUID_FEC_CONV_V39, LIQUID_FEC_CONV_V615,
		LIQUID_FEC_CONV_V27P23, LIQUID_FEC_CONV_V27P34, LIQUID_FEC_CONV_V27P45, LIQUID_FEC_CONV_V27P56,
		LIQUID_FEC_CONV_V27P67, LIQUID_FEC_CONV_V27P78, LIQUID_FEC_CONV_V29P23, LIQUID_FEC_CONV_V29P34,
		LIQUID_FEC_CONV_V29P45, LIQUID_FEC_CONV_V29P56, LIQUID_FEC_CONV_V29P67, LIQUID_FEC_CONV_V29P78,
		LIQUID_FEC_RS_M8};
}

ReedSolomonEncoderDecoder_i::~ReedSolomonEncoderDecoder_i()
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
        (ReedSolomonEncoderDecoder_base).
    
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
        addPropertyChangeListener(<property name>, this, &ReedSolomonEncoderDecoder_i::<callback method>)
        in the constructor.

        Callback methods should take two arguments, both const pointers to the value
        type (e.g., "const float *"), and return void.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            
        //Add to ReedSolomonEncoderDecoder.cpp
        ReedSolomonEncoderDecoder_i::ReedSolomonEncoderDecoder_i(const char *uuid, const char *label) :
            ReedSolomonEncoderDecoder_base(uuid, label)
        {
            addPropertyChangeListener("scaleValue", this, &ReedSolomonEncoderDecoder_i::scaleChanged);
        }

        void ReedSolomonEncoderDecoder_i::scaleChanged(const float *oldValue, const float *newValue)
        {
            std::cout << "scaleValue changed from" << *oldValue << " to " << *newValue
                      << std::endl;
        }
            
        //Add to ReedSolomonEncoderDecoder.h
        void scaleChanged(const float* oldValue, const float* newValue);
        

************************************************************************************************/
int ReedSolomonEncoderDecoder_i::serviceFunction()
{
	// Lock properties and then grab them. This prevents changes midway through the function having an effect on functionality
	mutex_lock.lock();
	fec_scheme fs = scheme_list[scheme_codec];
	bool should_encode = encode;
	mutex_lock.unlock();

	bulkio::InCharPort::dataTransfer *input = dataCharIn->getPacket(bulkio::Const::BLOCKING);

	if(not input)
	{
		return NOOP;
	}

	std::vector<signed char> data = input->dataBuffer;

	// Put the input into a useful format
	unsigned int length_of_input = data.size();
	unsigned char received_message[length_of_input];
	for(unsigned int i = 0; i<length_of_input; i++)
	{
		received_message[i] = reinterpret_cast<unsigned char &>(data[i]);
	}

	// Create the fec object
	fec fec_obj = fec_create(fs, NULL);
	// Output
	std::vector<signed char> outputData;

	if(should_encode)	// Have to encode the message
	{
		// Necessary calculations
		unsigned int length_of_enc_msg = fec_get_enc_msg_length(fs, length_of_input);
		unsigned char encoded_message[length_of_enc_msg];

		fec_encode(fec_obj, length_of_input, received_message, encoded_message);

		// Add a "msg_len" field to the SRI so that the decoder on the other end has the initial length
		int keywords_length =input->SRI.keywords.length();
		int index = keywords_length;
		for(int i = 0; i < keywords_length; i++)
		{
			if(strcmp(input->SRI.keywords[i].id, "msg_len") == 0)
			{
				index = i;
			}
		}
		if(index == keywords_length)
		{
			input->SRI.keywords.length(index+1);
		}
		input->SRI.keywords[index].id = "msg_len";
		input->SRI.keywords[index].value <<= length_of_input;

		// Prepare output
		outputData.resize(length_of_enc_msg);
		for(unsigned int i = 0; i<length_of_enc_msg; i++)
		{
			outputData[i] = reinterpret_cast<signed char &>(encoded_message[i]);
		}
	}
	else	// Decode
	{
		// First get the length of initial message
		int keywords_length = input->SRI.keywords.length();
		int index = keywords_length;
		for(int i = 0; i<keywords_length; i++)
		{
			if(strcmp(input->SRI.keywords[i].id,"msg_len") == 0)
			{
				index = i;
			}
		}
		unsigned int length_of_initial_msg;
		if(index == keywords_length) // If there was no field "msg_len"
		{
			std::cout<<"No length of initial message, I can do nothing."<<std::endl;
			delete input;
			return NOOP;
		}
		std::cout<<"performed decode"<<std::endl;
		input->SRI.keywords[index].value >>= length_of_initial_msg;

		//Set value of the keyword to NULL, it is no longer useful or necessary
		input->SRI.keywords[index].value <<= NULL;

		// Preparations for decode
		unsigned char decoded_message[length_of_initial_msg];
		fec_decode(fec_obj, length_of_initial_msg, received_message, decoded_message);

		outputData.resize(length_of_initial_msg);
		for(unsigned int i = 0; i<length_of_initial_msg; i++)
		{
			outputData[i] = reinterpret_cast<signed char &>(decoded_message[i]);
		}
	}

	// SRI has always changed, push every time
	dataCharOut->pushSRI(input->SRI);

	// Push data out
	dataCharOut->pushPacket(outputData, input->T, input->EOS, input->streamID);

	// Get rid of FEC object. It is recreated every time. Perhaps not the most efficient way to do this (see Arbitrary Resampler),
	//		definitely the simplest.
	fec_destroy(fec_obj);

	delete input; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
	return NORMAL;
}
