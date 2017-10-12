/**
 *  @file	IntelHex.c
 *  @brief  Intel Hex Record implementation file
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

#include "IntelHex.h"
#include <stdlib.h>

/*
 *  ===== ParseIntelHex =====
 */
int ParseIntelHex(const char * line, HEX_RECORD* hexrec)
{
    char ch = *line++;
    int i, type, count, chksum;
    int result;
    int offset;

    if (ch != ':')
        return HEX_FORMAT_ERROR;

    /* Read byte count (2 digits) */
    count = ParseHexByte(line);
    if (count < 0)
        return HEX_LENGTH_ERROR;
    line += 2;
    hexrec->length = chksum = count;

    /* Read offset (2 bytes) */
    offset = 0;
    for (i = 0; i < 2; ++i)
    {
        result = ParseHexByte(line);
        if (result < 0)
            return HEX_ADDR_ERROR;
        line += 2;
        offset = (offset << 8) | result;
        chksum += result;
    }

    /* Read type */
    type = ParseHexByte(line);
    if (type < 0)
        return HEX_LENGTH_ERROR;
    line += 2;
    chksum += type;
    hexrec->type = type;

    switch (type)
    {
    case 0:
    case 1:
        hexrec->addr = hexrec->base + offset;
        break;
    case 2:
        hexrec->base = (HEX_ADDR) offset << 4;
        break;
    case 4:
        /* extended hex record: read data bytes as offset */
        for (i = 0; i < count; ++i)
        {
            result = ParseHexByte(line);
            if (result < 0)
                return HEX_DATA_ERROR;
            line += 2;
            offset = (offset << 8) | result;
            chksum += result;
        }
        count = 0;
        hexrec->base = (HEX_ADDR) offset << 16;
    default:
        ;
    }

    /* Read data bytes */
    for (i = 0; i < count; ++i)
    {
        result = ParseHexByte(line);
        if (result < 0)
            return HEX_DATA_ERROR;
        line += 2;
        hexrec->buffer[i] = (HEX_BYTE) result;
        chksum += result;
    }

    /* Verify checksum */
    chksum = (0 - chksum) & 255;
    result = ParseHexByte(line);
    if (result < 0 || result != chksum)
        return HEX_CHECKSUM_ERROR;

    if (type == 0)
    {
        /* Handle wrap around of base address */
        if ((hexrec->addr & 0xFFFF) + hexrec->length > 0x10000)
            hexrec->base += 0x10000;
        return HEX_DATA;
    }
    if (type == 1)
        return HEX_DONE;

    return HEX_OTHER;
}

/*
 *  ===== WriteIntelHex =====
 *      Write Intel Hex record from memory into file.
 *      Returns positive for success else negative error code
 */
int WriteIntelHex(FILE* fout, void* buf, HEX_ADDR offset, int count)
{
    int i, result;
    int chksum;
    int lo = offset & 255;
    int hi = (offset >> 8) & 255;

    if (offset > 0xFFFF)
    {
        unsigned int block = offset >> 16;
        chksum = 0xFA - (block & 255) - (block >> 8);
        result = fprintf(fout, ":02000004%04X%02X\n", offset >> 16, chksum & 255);
    }

    /* Output length and offset */
    chksum = count;
    result = fprintf(fout, ":%02X%02X%02X%02X", count, hi, lo, 0);
    if (result < 0)
        return result;
    
    chksum += hi + lo;
    for (i = 0; i < count; ++i)
    {
        HEX_BYTE b = ((HEX_BYTE*) buf)[i];
        result = fprintf(fout, "%02X", b);
        if (result < 0)
            return result;
        chksum += b;
    }
    return fprintf(fout, "%02X\n", (256 - chksum) & 255);
}

/*
 *  ===== WriteIntelEnd =====
 *      Write Intel End record from memory into file.
 *      Returns positive for success else negative error code
 */
int WriteIntelEnd(FILE* fout)
{
    return fprintf(fout, ":00000001FF\n");
}
