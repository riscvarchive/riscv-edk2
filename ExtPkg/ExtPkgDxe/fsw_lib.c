/* $Id: fsw_lib.c 33540 2010-10-28 09:27:05Z vboxsync $ */
/** @file
 * fsw_lib.c - Core file system wrapper library functions.
 */

/*
 * Copyright (C) 2010 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/*-
 * This code is based on:
 *
 * Copyright (c) 2006 Christoph Pfisterer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsw_core.h"

/* Include generated string encoding specific functions */
#include "fsw_strfunc.h"


/**
 * Allocate memory and clear it.
 */

fsw_status_t fsw_alloc_zero(int len, void **ptr_out)
{
    fsw_status_t status;

    status = fsw_alloc(len, ptr_out);
    if (status)
        return status;
    fsw_memzero(*ptr_out, len);
    return FSW_SUCCESS;
}

/**
 * Duplicate a piece of data.
 */

fsw_status_t fsw_memdup(void **dest_out, void *src, int len)
{
    fsw_status_t status;

    status = fsw_alloc(len, dest_out);
    if (status)
        return status;
    fsw_memcpy(*dest_out, src, len);
    return FSW_SUCCESS;
}

/**
 * Get the length of a string. Returns the number of characters in the string.
 */

int fsw_strlen(struct fsw_string *s)
{
    if (s->type == FSW_STRING_TYPE_EMPTY)
        return 0;
    return s->len;
}

static const fsw_u16 fsw_latin_case_fold[] =
{
    /* 0 */ 0xFFFF, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    /* 1 */ 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    /* 2 */ 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    /* 3 */ 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    /* 4 */ 0x0040, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    /* 5 */ 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    /* 6 */ 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    /* 7 */ 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    /* 8 */ 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    /* 9 */ 0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
    /* A */ 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
    /* B */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
    /* C */ 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00E6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
    /* D */ 0x00F0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00F8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00FE, 0x00DF,
    /* E */ 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
    /* F */ 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
};


fsw_u16 fsw_to_lower(fsw_u16 ch)
{
    if (ch < 0x0100)
        return fsw_latin_case_fold[ch];
    return ch;
}

/**
 * Compare two strings for equality. The two strings are compared, taking their
 * encoding into account. If they are considered equal, boolean true is returned.
 * Otherwise, boolean false is returned.
 */

int fsw_streq(struct fsw_string *s1, struct fsw_string *s2)
{
    struct fsw_string temp_s;

    // handle empty strings
    if (s1->type == FSW_STRING_TYPE_EMPTY) {
        temp_s.type = FSW_STRING_TYPE_ISO88591;
        temp_s.size = temp_s.len = 0;
        temp_s.data = NULL;
        return fsw_streq(&temp_s, s2);
    }
    if (s2->type == FSW_STRING_TYPE_EMPTY) {
        temp_s.type = FSW_STRING_TYPE_ISO88591;
        temp_s.size = temp_s.len = 0;
        temp_s.data = NULL;
        return fsw_streq(s1, &temp_s);
    }

    // check length (count of chars)
    if (s1->len != s2->len)
        return 0;
    if (s1->len == 0)   // both strings are empty
        return 1;

    if (s1->type == s2->type) {
        // same type, do a dumb memory compare
        if (s1->size != s2->size)
            return 0;
        return fsw_memeq(s1->data, s2->data, s1->size);
    }

    // dispatch to type-specific functions
    #define STREQ_DISPATCH(type1, type2) \
      if (s1->type == FSW_STRING_TYPE_##type1 && s2->type == FSW_STRING_TYPE_##type2) \
        return fsw_streq_##type1##_##type2(s1->data, s2->data, s1->len); \
      if (s2->type == FSW_STRING_TYPE_##type1 && s1->type == FSW_STRING_TYPE_##type2) \
        return fsw_streq_##type1##_##type2(s2->data, s1->data, s1->len);
    STREQ_DISPATCH(ISO88591, UTF8);
    STREQ_DISPATCH(ISO88591, UTF16);
    STREQ_DISPATCH(ISO88591, UTF16_SWAPPED);
    STREQ_DISPATCH(UTF8, UTF16);
    STREQ_DISPATCH(UTF8, UTF16_SWAPPED);
    STREQ_DISPATCH(UTF16, UTF16_SWAPPED);

    // final fallback
    return 0;
}

/**
 * Compare a string with a C string constant. This sets up a string descriptor
 * for the string constant (second argument) and runs fsw_streq on the two
 * strings. Currently the C string is interpreted as ISO 8859-1.
 * Returns boolean true if the strings are considered equal, boolean false otherwise.
 */

int fsw_streq_cstr(struct fsw_string *s1, const char *s2)
{
    struct fsw_string temp_s;
    int i;

    for (i = 0; s2[i]; i++)
        ;

    temp_s.type = FSW_STRING_TYPE_ISO88591;
    temp_s.size = temp_s.len = i;
    temp_s.data = (char *)s2;

    return fsw_streq(s1, &temp_s);
}

/**
 * Creates a duplicate of a string, converting it to the given encoding during the copy.
 * If the function returns FSW_SUCCESS, the caller must free the string later with
 * fsw_strfree.
 */

fsw_status_t fsw_strdup_coerce(struct fsw_string *dest, int type, struct fsw_string *src)
{
    fsw_status_t    status;

    if (src->type == FSW_STRING_TYPE_EMPTY || src->len == 0) {
        dest->type = type;
        dest->size = dest->len = 0;
        dest->data = NULL;
        return FSW_SUCCESS;
    }

    if (src->type == type) {
        dest->type = type;
        dest->len  = src->len;
        dest->size = src->size;
        status = fsw_alloc(dest->size, &dest->data);
        if (status)
            return status;

        fsw_memcpy(dest->data, src->data, dest->size);
        return FSW_SUCCESS;
    }

    // dispatch to type-specific functions
    #define STRCOERCE_DISPATCH(type1, type2) \
      if (src->type == FSW_STRING_TYPE_##type1 && type == FSW_STRING_TYPE_##type2) \
        return fsw_strcoerce_##type1##_##type2(src->data, src->len, dest);
    STRCOERCE_DISPATCH(UTF8, ISO88591);
    STRCOERCE_DISPATCH(UTF16, ISO88591);
    STRCOERCE_DISPATCH(UTF16_SWAPPED, ISO88591);
    STRCOERCE_DISPATCH(ISO88591, UTF8);
    STRCOERCE_DISPATCH(UTF16, UTF8);
    STRCOERCE_DISPATCH(UTF16_SWAPPED, UTF8);
    STRCOERCE_DISPATCH(ISO88591, UTF16);
    STRCOERCE_DISPATCH(UTF8, UTF16);
    STRCOERCE_DISPATCH(UTF16_SWAPPED, UTF16);

    return FSW_UNSUPPORTED;
}

/**
 * Splits a string at the first occurrence of the separator character.
 * The buffer string is searched for the separator character. If it is found, the
 * element string descriptor is filled to point at the part of the buffer string
 * before the separator. The buffer string itself is adjusted to point at the
 * remaining part of the string (without the separator).
 *
 * If the separator is not found in the buffer string, then element is changed to
 * point at the whole buffer string, and the buffer string itself is changed into
 * an empty string.
 *
 * This function only manipulates the pointers and lengths in the two string descriptors,
 * it does not change the actual string. If the buffer string is dynamically allocated,
 * you must make a copy of it so that you can release it later.
 */

void fsw_strsplit(struct fsw_string *element, struct fsw_string *buffer, char separator)
{
    int i, maxlen;

    if (buffer->type == FSW_STRING_TYPE_EMPTY || buffer->len == 0) {
        element->type = FSW_STRING_TYPE_EMPTY;
        return;
    }

    maxlen = buffer->len;
    *element = *buffer;

    if (buffer->type == FSW_STRING_TYPE_ISO88591) {
        fsw_u8 *p;

        p = (fsw_u8 *)element->data;
        for (i = 0; i < maxlen; i++, p++) {
            if (*p == separator) {
                buffer->data = p + 1;
                buffer->len -= i + 1;
                break;
            }
        }
        element->len = i;
        if (i == maxlen) {
            buffer->data = p;
            buffer->len -= i;
        }

        element->size = element->len;
        buffer->size  = buffer->len;

    } else if (buffer->type == FSW_STRING_TYPE_UTF16) {
        fsw_u16 *p;

        p = (fsw_u16 *)element->data;
        for (i = 0; i < maxlen; i++, p++) {
            if (*p == separator) {
                buffer->data = p + 1;
                buffer->len -= i + 1;
                break;
            }
        }
        element->len = i;
        if (i == maxlen) {
            buffer->data = p;
            buffer->len -= i;
        }

        element->size = element->len * sizeof(fsw_u16);
        buffer->size  = buffer->len  * sizeof(fsw_u16);

    } else {
        // fallback
        buffer->type = FSW_STRING_TYPE_EMPTY;
    }

    // TODO: support UTF8 and UTF16_SWAPPED
}

/**
 * Frees the memory used by a string returned from fsw_strdup_coerce.
 */

void fsw_strfree(struct fsw_string *s)
{
    if (s->type != FSW_STRING_TYPE_EMPTY && s->data)
        fsw_free(s->data);
    s->type = FSW_STRING_TYPE_EMPTY;
}

// EOF
