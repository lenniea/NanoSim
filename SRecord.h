#ifndef __SRECORD_H__
#define __SRECORD_H__

/**
 *  @file	SRecord.h
 *  @brief  Motorola S-Record header file
 *  @author	Lennie Araki
 *  @date	15-Oct-2011
 *
 *  This file is a collection of contants, types and functions for 
 *  manipulating Motorola S-Records
 *
 *  Copyright (c) 2011 Lennie Araki
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *
 *  Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "HexFile.h"

/**
 *  This Function parses a line buffer containing a Motorol S-Record into
 *  a HEX_RECORD structure.
 *
 *      Parse Motorola S-Record.  S-Records have two "S" record types
 *  depending on the address size as follows:
 *  <pre>
 *      Ext  Address Bytes  Data,End
 *      ---  -------------  --------
 *      S19   2 (16-bit)     S1,S9
 *      S28   3 (24-bit)     S2,S8
 *      S37   4 (32-bit)     S3,S7
 *
 *  Each S-Record is a line formatted as follows:
 *       Stnnaaaadddddddddddddddddddddddddddddddd ... cc
 *
 *  Where:
 *    S    = 'S' indicates a Motorola S record
 *    t    = Record type, '0' = Header, '1'=data, '9'=end of file.
 *    nn   = Count of number of bytes in record. (in ASCII/HEX)
 *    aaaa = Load address of data record. (in ASCII/HEX)
 *    dd   = Actual data bytes in record. (in ASCII/HEX)
 *    cc   = Checksum of count, address, and data. (in ASCII/HEX)
 *  </pre>
 *  Note 1: Checksum is computed as one's complement of eight bit sum of all
 *          values from 'nn' to end of data.
 *
 *  Note 2: Count 'nn' is three greater than the number of data bytes in the
 *          record.
 *
 *	@param	line containing Motorola S-Record ASCII character string
 *  @param  record Hex record to write to
 *	@return	length in bytes or negative HEX_RESULT code
 */
int ParseMotorolaSRecord(const char * line, HEX_RECORD* record);

#ifdef __cplusplus
}
#endif

#endif /* __HEXFILE_H__ */
