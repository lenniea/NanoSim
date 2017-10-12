#ifndef __HEXFILE_H__
#define __HEXFILE_H__

/**
 *  @file   HexFile.h
 *  @brief  Hex library header file
 *  @author	Lennie Araki
 *  @date	15-Oct-2011
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

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  @mainpage
 *
 *  This library is a collection of contants, types and functions for 
 *  manipulating Motorola S-Record and Intel Hex Records.
 *
 *  Modules are hexfile.h intelhex.h srecord.h
 */

#define HEX_FILE_BYTES   256			/*!< Maximum number of bytes in HEX_RECORD */
#define HEX_LINE_BUF     (HEX_FILE_BYTES*2+20) /*!< Size of hex line buffer */


typedef unsigned char HEX_BYTE;			/*!< Hex 8-bit byte */
typedef unsigned long HEX_ADDR;			/*!< Hex 32-bit Address */

/**
 *	Hex record structure.
 *
 *	This structure holds the "union" of either an Intel Hex or Motorola S-Record.
 */
typedef struct hex_record
{
    HEX_ADDR addr;						/*!< 32-bit address (Intel Extended Hex) */
    HEX_BYTE buffer[HEX_FILE_BYTES];	/*!< Data buffer (up to 256 bytes) */
    HEX_ADDR base;						/*!< Base address (Intel Hex) */
    int length;							/*!< Length in bytes */
    int type;							/*!< Record type (Intel or Motorola S-Record */
} HEX_RECORD;


/**
 *	Hex record structure
 *
 *	Holds the "union" of either an Intel Hex or Motorola S-Record
 */
typedef enum hex_result
{
    HEX_END_ERROR = -7,
    HEX_CHECKSUM_ERROR = -6,
    HEX_DATA_ERROR = -5,
    HEX_ADDR_ERROR = -4,
    HEX_LENGTH_ERROR = -3,
    HEX_FORMAT_ERROR = -2,
    HEX_ERROR = -1,
    HEX_DONE = 0,
    HEX_DATA = 1,
    HEX_OTHER = 2,
    HEX_ABORT = 3,
} HEX_RESULT;

/**
 *	This function parses an ASCII character into hex digit.
 *
 *	@param	ch	ASCII character to parse
 *	@return	Hex digit 0-F or HEX_ERROR (-1)
 */
int ParseHexDigit(const char ch);

/**
 *	This function parses an ASCII character string into 8-bit hex byte.
 *
 *	@param	str	ASCII character string to parse
 *	@return	8-bit Hex byte 00-FF or HEX_ERROR (-1)
 */
int ParseHexByte(const char * str);

/**
 *	This function parses an ASCII character string into 16-bit hex word.
 *
 *	@param	str	ASCII character string to parse
 *	@return	16-bit Hex word 0000-FFFFF or HEX_ERROR (-1)
 */
long ParseHexWord(const char* str);

/**
 *	This function parses an ASCII character string into 32-bit hex word.
 *
 *	@param	str	ASCII character string to parse
 *	@return	32-bit Hex long 0000-FFFFF or HEX_ERROR (-1)
 */
long ParseHexLong(const char* str);

#ifdef __cplusplus
}
#endif

#endif /* __HEXFILE_H__ */
