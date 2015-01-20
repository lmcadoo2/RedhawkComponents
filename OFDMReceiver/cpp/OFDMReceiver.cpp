/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "OFDMReceiver.h"

PREPARE_LOGGING(OFDMReceiver_i)

OFDMReceiver_i::OFDMReceiver_i(const char *uuid, const char *label) :
    OFDMReceiver_base(uuid, label)
{

}

OFDMReceiver_i::~OFDMReceiver_i()
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
        (OFDMReceiver_base).
    
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
        addPropertyChangeListener(<property name>, this, &OFDMReceiver_i::<callback method>)
        in the constructor.

        Callback methods should take two arguments, both const pointers to the value
        type (e.g., "const float *"), and return void.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            
        //Add to OFDMReceiver.cpp
        OFDMReceiver_i::OFDMReceiver_i(const char *uuid, const char *label) :
            OFDMReceiver_base(uuid, label)
        {
            addPropertyChangeListener("scaleValue", this, &OFDMReceiver_i::scaleChanged);
        }

        void OFDMReceiver_i::scaleChanged(const float *oldValue, const float *newValue)
        {
            std::cout << "scaleValue changed from" << *oldValue << " to " << *newValue
                      << std::endl;
        }
            
        //Add to OFDMReceiver.h
        void scaleChanged(const float* oldValue, const float* newValue);
        

************************************************************************************************/


int OFDMReceiver_i::serviceFunction()
{
	// Values hard coded, but will be properties eventually
	mutex_lock.lock();
	unsigned int num_subcarriers = subcarriers;
	unsigned int cyclic_prefix_len = cyclic_prefix;
	std::vector<short> pilot_freqs = pilot_frequencies;
	std::vector<short> null_freqs = null_frequencies;
    mutex_lock.unlock();

    // Create a void * to receive the data
    CALLBACK_INFO decoded_frame;
    decoded_frame.changed = false;
    CALLBACK_INFO * decoded_frame_ptr = & decoded_frame;
    void * callback_ptr = (void *) decoded_frame_ptr;

    // Set up the subcarriers for pilot tones, null frequencies, and data
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

	// Create a boolean to see if anything was executed
	bool ran = false;
	bool getInput = true;

	// Create the synchronizer
	ofdmflexframesync sync = ofdmflexframesync_create(num_subcarriers, cyclic_prefix_len, p, myCallBackFunction, callback_ptr);

	// Get input
	bulkio::InFloatPort::dataTransfer * input = dataFloatIn->getPacket(bulkio::Const::BLOCKING);

	while (getInput and input)
	{
		// Have executed at least once
		ran = true;
		// Prepare buffer to feed into sync
		unsigned int size = input->dataBuffer.size()/2;
		std::complex<float> input_buffer[size];

		// Have to reconstruct complex values from the floats
		for (unsigned int i = 0; i < size*2; i+=2) {
			input_buffer[i/2] = std::complex<float>(input->dataBuffer[i], input->dataBuffer[i+1]);
		}

		ofdmflexframesync_execute(sync, input_buffer, size);

		// Check to see if a frame has been captured
		if(decoded_frame_ptr->changed && decoded_frame_ptr->header_valid && decoded_frame_ptr->payload_valid)
		{
			for(unsigned int i = 0; i<decoded_frame_ptr->payload_len; i++)
			{
				std::cout<<decoded_frame_ptr->payload[i]<<"   ";
			}
			std::cout<<std::endl;

			// We are going to use the frame, so nullify the data for future uses
			decoded_frame_ptr->changed = false;

			// Initialize the output data as the header, in case there is a useful header etc. It is always 8 characters
			std::vector<signed char> outputData = decoded_frame_ptr->header;

			// Concatenate the header and payload
			outputData.insert(outputData.end(), decoded_frame_ptr->payload.begin(), decoded_frame_ptr->payload.end());

			// Transmitter sets complex mode. Unset it, as we are sending bytes again
			input->SRI.mode = false;

			// Push out SRI and data
			dataCharOut->pushSRI(input->SRI);
			dataCharOut->pushPacket(outputData, input->T, input->EOS, input->streamID);

			getInput = false;
		}
		// Haven't found a frame yet, get more data
		else
		{
			// Release data
			delete input;

			// Get another packet
			input = dataFloatIn->getPacket(bulkio::Const::BLOCKING);
		}
	}
	if(not ran)
	{
		return NOOP;
	}

	ofdmflexframesync_destroy(sync);
	delete input;

    return NORMAL;
}
