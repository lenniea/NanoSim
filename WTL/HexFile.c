/**
 *  @file	HexFile.c
 *  @brief  Hex library implementation file
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

#include "HexFile.h"
#include <stdlib.h>

/*
 *  ===== ParseHexDigit =====
 *  Parse a string of hex digits ('0123456789ABCDEF') into
 *  a 8/16-bit value.
 *
 *  Returns HEX_ERROR (-1) for error else valid number
 */
int ParseHexDigit(const char ch)
{
    int digit;

    if (ch >= '0' && ch <= '9')
        digit = (ch - '0');
    else if (ch >= 'a' && ch <= 'f')
        digit = (ch - 'a') + 10;
    else if (ch >= 'A' && ch <= 'F')
        digit = (ch - 'A') + 10;
    else
        digit = HEX_ERROR;
    return digit;
}

/*
 *  ===== ParseHexByte =====
 *  Parse a string of hex digits ('0123456789ABCDEF') into a 8-bit byte.
 *
 *  Returns HEX_ERROR (-1) for error else valid byte
 */
int ParseHexByte(const char *line)
{
    int lo,hi;

    hi = ParseHexDigit(line[0]);
    if (hi < 0)
        return hi;
    lo = ParseHexDigit(line[1]);
    if (lo < 0)
        return lo;
    return (hi << 4) | lo;
}

/*
 *  ===== ParseHexWord =====
 *  Parse a string of 4 hex digits ('0123456789ABCDEF') into a 16-bit word
 *
 *  Returns HEX_ERROR (-1) for error else valid unsigned 16-bit word
 */
long ParseHexWord(const char *line)
{
    int hi = ParseHexByte(line);
    int lo = ParseHexByte(line + 2);
    if (lo < 0 || hi < 0)
        return -1;
    return (hi << 8) | lo;
}

/*
 *  ===== ParseHexLong =====
 *  Parse a string of hex digits ('0123456789ABCDEF') into a 32-bit long
 *
 *  Returns HEX_ERROR (-1) for error else valid 32-bit word
 */
long ParseHexLong(const char *line)
{
    int hex = 0;
    char ch = *line;

    while (ch != '\0')
    {
        int digit = ParseHexDigit(ch);
        if (digit < 0)
            return digit;
        hex = (hex << 4) + digit;
        ch = *++line;
    }
    return hex;
}
