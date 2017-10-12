#ifndef __INTELHEX_H__
#define __INTELHEX_H__

/**
 *  @file	IntelHex.h
 *  @brief  Intel Hex Record header file
 *  @author	Lennie Araki
 *  @date	15-Oct-2011
 *
 *  This library is a collection of contants, types and functions for 
 *  manipulating Intel Hex Records.
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
 *  This function parses a line buffer containing an Intel (Extended) Hex 
 *  Record into a HEX_RECORD structure.
 *
 *  Intel Hex records begin with a colon (:). There are two types of records,
 *  a variable length data and a fixed length EOF record.
 *
 *  <pre>
 *  Each S-Record is a line formatted as follows:
 *      :nnaaaattdddddddddddddddddddddddddddddd ... cc
 *
 *  Where:
 *    :    = Indicates an Intel an Hex record
 *    nn   = Count of number of bytes in record. (in ASCII/HEX)
 *    aaaa = Load address of data record. (in ASCII/HEX)
 *    t    = Record type (00=Data, 01=End of file)
 *    dd   = Actual data bytes in record (in ASCII/HEX)
 *    cc   = Checksum of address, type and data (not count). (in ASCII/HEX)
 *  </pre>
 *
 *  Note 1: Checksum is computed as two's complement of eight bit sum of
 *          all values from 'nn' to end of data (ie. to just before checksum).
 *  Note 2: End of file record contains count of 00.
 *
 *	@param	line containing Intel Hex Record ASCII character string
 *  @param  record Hex record to write to
 *	@return	length in bytes or negative HEX_RESULT code
 */
int ParseIntelHex(const char * line, HEX_RECORD* record);

/**
 *  This function writes a series of Intel Hex Records (up to 256 bytes) to the specified file.
 *
 *	@param	fout FILE to write Intel Hex format record to
 *	@param	buf  buffer containing raw binary data bytes
 *	@param	offset 32-bit address offset
 *	@param	count count of bytes to write
 *	@return	length in bytes or negative HEX_RESULT code
 */
int WriteIntelHex(FILE* fout, void* buf, HEX_ADDR offset, int count);

/**
 *  This function writes an Intel Hex End Record to the specified file.
 *
 *	@param	fout FILE to write Intel Hex format record to
 *	@return	non-negative for sucess or negative HEX_RESULT code
 */
int WriteIntelEnd(FILE* fout);

#ifdef __cplusplus
}
#endif

#endif /* __INTELHEX_H__ */
