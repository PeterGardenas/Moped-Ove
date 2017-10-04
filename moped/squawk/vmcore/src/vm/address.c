/**** Created by Squawk builder from "vmcore/src/vm/address.c.spp.preprocessed" ****/ /*
 * Copyright 2004-2008 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 * 
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 16 Network Circle, Menlo
 * Park, CA 94025 or visit www.sun.com if you need additional
 * information or have any questions.
 */

/*
 * This file define types and functions to abstract the size of machine words and addresses.
 * This eases portability between 32 and 64 bit platforms.
 */

typedef void*          Address;
typedef unsigned char* ByteAddress;
#if SQUAWK_64
typedef ujlong UWord;
typedef jlong Offset;
const Address ADDRESS_MAX = (Address)0xFFFFFFFFFFFFFFFFL;
const UWord WORD_MAX = (UWord)0xFFFFFFFFFFFFFFFFL;
const UWord DEADBEEF = 0xDEADBEEFDEADBEEFL;
#else
typedef unsigned int UWord;
typedef int Offset;
const Address ADDRESS_MAX = (Address)0xFFFFFFFF;
const UWord WORD_MAX = (UWord)0xFFFFFFFF;
const UWord DEADBEEF = 0xDEADBEEF;
#endif /* SQUAWK_64 */
typedef UWord* UWordAddress;

        /**
         * Unsigned comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is lower than 'right' in an unsigned comparison
         */
#define  lo(left_489, right_490) (  \
             (UWord)(left_489) < (UWord)(right_490)  \
        )

        /**
         * Unsigned comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is lower than or equal to 'right' in an unsigned comparison
         */
#define  loeq(left_491, right_492) (  \
             (UWord)(left_491) <= (UWord)(right_492)  \
        )

        /**
         * Unsigned comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is higher than 'right' in an unsigned comparison
         */
#define  hi(left_493, right_494) (  \
             (UWord)(left_493) > (UWord)(right_494)  \
        )

        /**
         * Unsigned comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is higher than or equal to 'right' in an unsigned comparison
         */
#define  hieq(left_495, right_496) (  \
             (UWord)(left_495) >= (UWord)(right_496)  \
        )

        /**
         * Signed comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is lower than 'right' in a signed comparison
         */
#define  lt(left_497, right_498) (  \
             (Offset)(left_497) < (Offset)(right_498)  \
        )

        /**
         * Signed comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is lower than or equal to 'right' in a signed comparison
         */
#define  le(left_499, right_500) (  \
             (Offset)(left_499) <= (Offset)(right_500)  \
        )

        /**
         * Signed comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is higher than 'right' in a signed comparison
         */
#define  gt(left_501, right_502) (  \
             (Offset)(left_501) > (Offset)(right_502)  \
        )

        /**
         * Signed comparison between two machine word sized values.
         *
         * @param left  the first value
         * @param right the second value
         * @return true if 'left' is higher than or equal to 'right' in a signed comparison
         */
#define  ge(left_503, right_504) (  \
             (Offset)(left_503) >= (Offset)(right_504)  \
        )


        /**
         * Adds a machine word sized signed offset to an address.
         *
         * @param address  the base address
         * @param offset   the signed offset (in bytes) to add
         * @return the result of adding 'offset' to 'address'
         */
#define  Address_add(address_505, offset_506) (  \
             (Address)((UWord)(address_505) + (Offset)(offset_506))  \
        )

        /**
         * Subtracts a machine word sized signed offset to an address.
         *
         * @param address  the base address
         * @param offset   the signed offset (in bytes) to subtract
         * @return the result of subtracting 'offset' from 'address'
         */
#define  Address_sub(address_507, offset_508) (  \
             (Address)((UWord)(address_507) - (Offset)(offset_508))  \
        )

        /**
         * Computes the signed distance between two addresses.
         *
         * @param address1  the first address
         * @param address2  the second address
         * @return the signed distance (in bytes) between 'address1' and 'address2'
         */
#define  Address_diff(address1_509, address2_510) (  \
             (Offset)(address1_509) - (Offset)(address2_510)  \
        )
