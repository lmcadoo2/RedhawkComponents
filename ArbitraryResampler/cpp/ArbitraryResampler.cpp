/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "ArbitraryResampler.h"

PREPARE_LOGGING(ArbitraryResampler_i)

ArbitraryResampler_i::ArbitraryResampler_i(const char *uuid, const char *label) :
    ArbitraryResampler_base(uuid, label)
{
}

ArbitraryResampler_i::~ArbitraryResampler_i()
{
	for(std::map<std::string, resamp_rrrf>::iterator i = resamplers_real.begin(); i != resamplers_real.end(); i++)
	{
		resamp_rrrf_destroy(i->second);
	}
	for(std::map<std::string, resamp_crcf>::iterator i = resamplers_complex.begin(); i != resamplers_complex.end(); i++)
	{
		resamp_crcf_destroy(i->second);
	}
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
        (ArbitraryResampler_base).
    
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
        addPropertyChangeListener(<property name>, this, &ArbitraryResampler_i::<callback method>)
        in the constructor.

        Callback methods should take two arguments, both const pointers to the value
        type (e.g., "const float *"), and return void.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            
        //Add to ArbitraryResampler.cpp
        ArbitraryResampler_i::ArbitraryResampler_i(const char *uuid, const char *label) :
            ArbitraryResampler_base(uuid, label)
        {
            addPropertyChangeListener("scaleValue", this, &ArbitraryResampler_i::scaleChanged);
        }

        void ArbitraryResampler_i::scaleChanged(const float *oldValue, const float *newValue)
        {
            std::cout << "scaleValue changed from" << *oldValue << " to " << *newValue
                      << std::endl;
        }
            
        //Add to ArbitraryResampler.h
        void scaleChanged(const float* oldValue, const float* newValue);
        

************************************************************************************************/
int ArbitraryResampler_i::serviceFunction()
{
	//Perhaps add a lock?

	//These can later be changed to the value of a property. As soon as this function begins, grab the value,
	//	allowing a user to change it, but not affect the resampling of this particular set of data

	float resample_rate = resampling_rate;	// Resample rate
	unsigned int delay = 13;				// For the filter delay. Whatever that is
	float bandwidth = 0.25f;				// Obviously the bandwidth of the filter
	float As = 60.0f;						// Stop-band attenuation
	unsigned int npfd = 32;					// Number of filters in bank (polyphase)

	//Unlock would be here

	//Get the data from the port
	bulkio::InFloatPort::dataTransfer *input = dataFloatIn->getPacket(bulkio::Const::BLOCKING);

	//If no data, exit
	if(not input)
	{
		return NOOP;
	}

	// Basic computations
	unsigned int num_input_samples = input->dataBuffer.size();
	unsigned int length_input_buffer = num_input_samples;
	unsigned int length_output_buffer = (unsigned int) ceilf(1.1 * length_input_buffer * resample_rate) + 4 ;  // Easily more than we need

	// Check to see what kind of data we are dealing with
	bool complex_data = input->SRI.mode;

	//Get StreamID
	std::string stream_ID(input->SRI.streamID.in());

	// Since the number of data points output is undetermined, keep a running index
	unsigned int index = 0;
	// Passed to the resampler so that we can know how many points were output
	unsigned int num_resampled_points;

	std::vector<float> output;

	if(complex_data)
	{
		std::complex<float> in_samples[length_input_buffer];
		std::complex<float> out_samples[length_output_buffer];

		// Try and get the resampler with the StreamID
		std::map<std::string, resamp_crcf>::iterator i = resamplers_complex.find(stream_ID);

		if (i == resamplers_complex.end())
		{
			// No resampler found with this StreamID. Must create one, and insert it
			resamp_crcf resampler = resamp_crcf_create(resample_rate, delay, bandwidth, As, npfd);
			i = resamplers_complex.insert(resamplers_complex.end(), std::pair<std::string, resamp_crcf>(stream_ID, resampler));
		}

		// Now the iterator is pointing to an (ID, resampler) pair
		resamp_crcf resampler = (i->second);

		std::vector< std::complex<float> > * intermediate_complex_data_ptr = (std::vector< std::complex<float> > * ) &(input->dataBuffer);
		std::vector< std::complex<float> > intermediate_complex_data = *(intermediate_complex_data_ptr);

		for (unsigned int i = 0; i < length_input_buffer; i++)
		{
			in_samples[i] = 0.0f;
			if (i < num_input_samples)
			{
				in_samples[i] = intermediate_complex_data[i];
			}
		}

		for (unsigned int i = 0; i < length_input_buffer; i++)
		{
			resamp_crcf_execute(resampler, in_samples[i], &out_samples[index], &num_resampled_points);
			index += num_resampled_points;
		}

		intermediate_complex_data = (std::vector< std::complex<float> >(out_samples, out_samples + index));
		intermediate_complex_data_ptr = &(intermediate_complex_data);

		output = *( (std::vector<float> *) intermediate_complex_data_ptr);

		if(input->EOS)
		{
			resamp_crcf_destroy(resampler);
			resamplers_complex.erase(i);
		}
	}
	else
	{
		float in_samples[length_input_buffer];
		float out_samples[length_output_buffer];


		// Try and get the resampler with the StreamID
		std::map<std::string, resamp_rrrf>::iterator i = resamplers_real.find(stream_ID);

		if(i == resamplers_real.end())
		{
			// No resampler found with this StreamID. Must create one, and insert it
			resamp_rrrf resampler = resamp_rrrf_create(resample_rate, delay, bandwidth, As, npfd);
			i = resamplers_real.insert(resamplers_real.end(), std::pair<std::string, resamp_rrrf>(stream_ID, resampler));
		}

		// Now the iterator is pointing to an (ID, resampler) pair
		resamp_rrrf resampler = (i->second);

		for(unsigned int i = 0; i < length_input_buffer; i++)
		{
			in_samples[i] = 0.0f;
			if(i < num_input_samples)
			{
				in_samples[i] = input->dataBuffer[i];
			}
		}

		for(unsigned int i = 0; i < length_input_buffer; i++)
		{
			resamp_rrrf_execute(resampler, in_samples[i], &out_samples[index], &num_resampled_points);
			index += num_resampled_points;
		}

		output = std::vector<float>(out_samples, out_samples + index);

		if(input->EOS)
		{
			resamp_rrrf_destroy(resampler);
			resamplers_real.erase(i);
		}
	}

	// NOTE: You must make at least one valid pushSRI call
	if (input->sriChanged)
	{
		float inputRate = 1.0/input->SRI.xdelta;
		float outputRate = inputRate*resample_rate;
		float xdelta = 1.0/outputRate;

		input->SRI.xdelta = xdelta;

		dataFloatOut->pushSRI(input->SRI);

		input->T.toff = (input->T.toff/inputRate + delay)*outputRate;
	}
	dataFloatOut->pushPacket(output, input->T, input->EOS, input->streamID);

	delete input; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK


    return NORMAL;
}
