/*
 * OFDMReceiverCallback.cpp
 *
 *  Created on: Jan 16, 2015
 *      Author: CyberHawk
 */

#include "OFDMReceiverCallback.h"

int myCallBackFunction(
        		unsigned char *  _header,
                int              _header_valid,
                unsigned char *  _payload,
                unsigned int     _payload_len,
                int              _payload_valid,
                framesyncstats_s _stats,
                void *           _userdata
                )
{
	CALLBACK_INFO * data = (CALLBACK_INFO *) _userdata;
	data->changed = true;
	data->header_valid = _header_valid;
	data->payload_valid = _payload_valid;
	data->payload_len = _payload_len;

	// Create a copy of the header (in a vector) and copy it into the struct
	if(data->header_valid)
	{
		std::vector<signed char> header;
		header.resize(data->payload_len);
		for (unsigned int i = 0; i < 8; i++)
		{
			header[i] = reinterpret_cast<signed char &>(_header[i]);
		}
		data->payload = std::vector<signed char>(header);
	}

	// Create a copy of the payload (in a vector) and copy it into the struct
	if(data->payload_valid)
	{
		std::vector<signed char> payload;
		payload.resize(data->payload_len);
		for (unsigned int i = 0; i < data->payload_len; i++)
		{
			payload[i] = reinterpret_cast<signed char &>(_payload[i]);
		}
		data->payload = std::vector<signed char>(payload);
	}

	return 0;
}
