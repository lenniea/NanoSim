#include "hexfile.h"
#include <stdlib.h>

/**
 *  @file	SRecord.c
 *  @brief  Motorola S-Record implementation file
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

/*
 *  ===== ParseMotorolaSRecord =====
 */
static int ParseSRecordLine(int addrbytes, const char * line, HEX_RECORD* hexrec)
{
    int i, count, chksum;

    /* Read byte count (2 digits) */
    int result = ParseHexByte(line);
    if (result < 5)
        return HEX_LENGTH_ERROR;
    line += 2;
    chksum = result;
    hexrec->length = count = result - 5;

    /* Read address (2-4 bytes) */
    hexrec->addr = 0;
    for (i = 0; i < addrbytes; ++i)
    {
        result = ParseHexByte(line);
        if (result < 0)
            return HEX_ADDR_ERROR;
        line += 2;
        hexrec->addr = (hexrec->addr << 8) | result;
        chksum += result;
    }

    /* Read data bytes */
    for (i = 0; i < count; ++i)
    {
        result = ParseHexByte(line);
        if (result < 0)
            return HEX_DATA_ERROR;
        line += 2;
        hexrec->buffer[i] = (unsigned char) result;
        chksum += result;
    }

    /* Verify checksum */
    chksum = ~chksum & 255;
    result = ParseHexByte(line);
    if (result < 0 || result != chksum)
        return HEX_CHECKSUM_ERROR;

    return HEX_DATA;
}

int ParseMotorolaSRecord(const char * line, HEX_RECORD* hexrec)
{
    char ch = *line++;
    int type;
    int i;

    if (ch != 'S')
        return HEX_FORMAT_ERROR;

    ch = *line++;
    type = ch - '0';
    switch (ch) {
        case '1':
        case '2':
        case '3':
            i = ParseSRecordLine(type + 1, line, hexrec);
            break;

        case '7':
        case '8':
        case '9':
            if (hexrec->type != 10 - type)
                return HEX_END_ERROR;

            i = ParseSRecordLine(11 - type, line, hexrec);
            if (i >= 0)
                i = HEX_DONE;
            break;
        default:
            i = HEX_FORMAT_ERROR;
    }
    hexrec->type = type;
    return i;
}
