/*
 * OFDMReceiverCallback.h
 *
 *  Created on: Jan 16, 2015
 *      Author: CyberHawk
 */

#ifndef OFDMRECEIVERCALLBACK_H_
#define OFDMRECEIVERCALLBACK_H_

#include "OFDMReceiver_base.h"
#include <liquid/liquid.h>

int myCallBackFunction(
        		unsigned char *  _header,
                int              _header_valid,
                unsigned char *  _payload,
                unsigned int     _payload_len,
                int              _payload_valid,
                framesyncstats_s _stats,
                void *           _userdata
                );

struct CALLBACK_INFO{
	bool changed;
	int header_valid;
	int payload_valid;
	unsigned int payload_len;
	std::vector<signed char> header;
	std::vector<signed char> payload;
};


#endif /* OFDMRECEIVERCALLBACK_H_ */
