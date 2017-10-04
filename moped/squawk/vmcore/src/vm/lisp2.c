/**** Created by Squawk builder from "vmcore/src/vm/lisp2.c.spp.preprocessed" ****/ /*
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

#define bitmap     (com_sun_squawk_Lisp2Bitmap_start)
#define bitmapBase (com_sun_squawk_Lisp2Bitmap_base)
#define bitmapSize (com_sun_squawk_Lisp2Bitmap_size)
#define bitmapEnd  Address_add(com_sun_squawk_Lisp2Bitmap_start, bitmapSize)

#define BIT_INDEX_MASK  (HDR_BITS_PER_WORD - 1)

    /*---------------------------------------------------------------------------*\
     *                                Debug                                      *
    \*---------------------------------------------------------------------------*/

        /**
         * Returns the number of bits set to 1 in the bitmap.
         *
         * @return  the number of bits set to 1 in the bitmap.
         */
INLINE int getBitmapCardinality() {
            int total = 0;
            UWord sizeInWords = bitmapSize / HDR_BYTES_PER_WORD;
            while (sizeInWords != 0) {
                UWord word = getUWord(bitmap, --sizeInWords);
                while (word != 0) {
                    if ((word & 0x1) != 0) {
                        total++;
                    }
                    word = word >> 1;
                }
            }
            return total;
        }

        /**
         * Prints all the addresses that have their corresponding bit set in the bitmap.
         */
#define  printMarkedAddresses() {  \
            Address bm = bitmap; \
            Address base = bitmapBase; \
            Address p = (Address)((UWord)Address_diff(bm, base) << HDR_LOG2_BITS_PER_WORD); \
            UWord sizeInWords = bitmapSize / HDR_BYTES_PER_WORD; \
            Offset offset = 0; \
            fprintf(stderr, "Marked addresses:\n"); \
            while (offset < sizeInWords) { \
                UWord word = getUWord(bitmap, offset++); \
                int bits = HDR_BITS_PER_WORD; \
                while (bits-- != 0) { \
                    if ((word & 0x1) != 0) { \
                        fprintf(stderr, format("  %A\n"), p); \
                    } \
                    word = word >> 1; \
                    p = Address_add(p, HDR_BYTES_PER_WORD); \
                } \
            } \
        }

    /*---------------------------------------------------------------------------*\
     *                   Bit index based bitmap methods                          *
    \*---------------------------------------------------------------------------*/

        /**
         * Given a bit index return index of bitmap word containing the bit.
         *
         * @param n  an index into the bitmap
         * @return the index of the bitmap word containing bit <code>n</code>
         */
#define  getBitmapWordIndex(n_0) (  \
             (n_0) >> HDR_LOG2_BITS_PER_WORD  \
        )

        /**
         * Given a bit index return position of the bit in the bitmap word containing it.
         *
         * @param n  an index into the bitmap
         * @return the position of bit <code>n</code> in the bitmap word containing it
         */
#define  getBitmapBit(n_1) (  \
             (int)((n_1) & BIT_INDEX_MASK)  \
        )

        /**
         * Given a bit index return a mask that can be used to extract the bit from the bitmap word containing it.
         *
         * @param n  an index into the bitmap
         * @return a mask that can be used to extract bit <code>n</code> from the bitmap word containing it
         */
#define  getBitmapMask(n_2) (  \
             (UWord)1 << getBitmapBit((n_2))  \
        )

        /**
         * Gets the address of the word in the bitmap that contains a given bit
         *
         * @param n   the bit index for which the corresponding bitmap word is required
         * @return    the address of the bitmap word that contains the <code>n</code>th bit
         */
#define  getAddressOfBitmapWordForNthBit(n_3) (  \
             Address_add(bitmapBase, getBitmapWordIndex((n_3)) << HDR_LOG2_BYTES_PER_WORD)  \
        )

        /**
         * Determines if a bit index corresponds with a word in the bitmap.
         *
         * @param n  the bit index to test
         * @return true if bit <code>n</code> corresponds with a word in the bitmap
         */
INLINE boolean isNthBitInRange(UWord n_4) {
/*fprintf(stderr, format("isNthBitInRange:\n  base\t= %A\n  bmap\t= %A\n  n\t= %A\n  bmw\t= %A\n"), bitmapBase, bitmap, (n_4), getAddressOfBitmapWordForNthBit((n_4)));*/
            return hieq(getAddressOfBitmapWordForNthBit((n_4)), bitmap) && lo(getAddressOfBitmapWordForNthBit((n_4)), bitmapEnd);
        }

        /**
         * Sets a specified bit in the bitmap.
         *
         * @param n       the index of the bit to be set
         */
#define  setNthBit(n_5) { UWord  n_6 = n_5;  \
            UWord index = getBitmapWordIndex((n_6)); \
            UWord mask = getBitmapMask((n_6)); \
            assume(isNthBitInRange((n_6))); \
            setUWord(bitmapBase, index, getUWord(bitmapBase, index) | mask); \
        }

        /**
         * Clears a specified bit in the bitmap.
         *
         * @param n       the index of the bit to be set
         */
#define  clearNthBit(n_7) { UWord  n_8 = n_7;  \
            UWord index = getBitmapWordIndex((n_8)); \
            UWord mask = ~getBitmapMask((n_8)); \
            assume(isNthBitInRange((n_8))); \
            setUWord(bitmapBase, index, getUWord(bitmapBase, index) & mask); \
        }


        /**
         * Determines if a specified bit in the bitmap is set.
         *
         * @param n       the index of the bit to be tested
         * @return true if bit <code>n</code> is set
         */
INLINE boolean testNthBit(UWord n_9) {
/*fprintf(stderr, format("testNthBit - n = %A bitmap = %A bitmapWordAddress = %A bitmapWord = %A\n"), (n_9), bitmap, Address_add(bitmapBase, getBitmapWordIndex((n_9))), getUWord(bitmapBase, getBitmapWordIndex((n_9))));*/
            return (getUWord(bitmapBase, getBitmapWordIndex((n_9))) & getBitmapMask((n_9))) != 0;
        }

        /**
         * Determines if a specified bit in the bitmap is set and sets it if it isn't.
         *
         * @param n       the index of the bit to be tested
         * @return true if bit <code>n</code> was set before this call
         */
INLINE boolean testAndSetNthBit(UWord n_10) {
            UWord oldval;
            assume(isNthBitInRange((n_10)));
            oldval = getUWord(bitmapBase, getBitmapWordIndex((n_10)));
            if ((oldval & getBitmapMask((n_10))) != 0) {
                return true;
            } else {
                setUWord(bitmapBase, getBitmapWordIndex((n_10)), oldval | getBitmapMask((n_10)));
                return false;
            }
        }

        /*---------------------------------------------------------------------------*\
         *                   Address based bitmap methods                            *
        \*---------------------------------------------------------------------------*/

        /**
         * Converts an address to the index of its corresponsing bit in the bitmap.
         *
         * @param ea  the address to convert
         * @return the index of the bit in the bitmap for <code>ea</code>
         */
#define  asBitIndex(ea_11) (  \
             (((UWord)(ea_11)) >> HDR_LOG2_BYTES_PER_WORD)  \
        )

        /**
         * Gets the address of the word in the bitmap that contains the bit for a given address.
         *
         * @param ea   the address for which the corresponding bitmap word is required
         * @return     the address of the bitmap word that contains the bit for <code>ea</code>
         */
#define  getAddressOfBitmapWordFor(ea_12) (  \
             getAddressOfBitmapWordForNthBit(asBitIndex((ea_12)))  \
        )

        /**
         * Gets the address that is mapped by the first bit in a given bitmap word.
         *
         * @param bitmapWordPtr  the address of a word in the bitmap
         * @return the address corresponding to the first bit in the word at <code>bitmapWordPtr</code>
         */
#define  getAddressForBitmapWord(bitmapWordPtr_13) (  \
             (Address)(((UWord)Address_diff((bitmapWordPtr_13), bitmapBase) << HDR_LOG2_BITS_PER_BYTE) << HDR_LOG2_BYTES_PER_WORD)  \
        )

        /**
         * Determines if the bit in the bitmap for a given address is set.
         *
         * @param ea      the effective address for which the corresponding bit to be tested
         * @return true if the bit for <code>ea</code> is set
         */
#define  testBitFor(ea_14) (  \
             testNthBit(asBitIndex((ea_14)))  \
        )

        /**
         * Sets the appropriate bit in the bitmap for a given address.
         *
         * @param ea      the effective address for which the corresponding bit is to be set
         */

#define  setBitFor(ea_15) { Address  ea_16 = ea_15;  \
 \
 \
 \
/*fprintf(stderr, format("setBitFor - ea = %A *ea = %A\n"), (ea_16), *((UWord*)(ea_16)));*/ \
/*if ((UWord)(ea_16) == BAD_ADDRESS) fprintf(stderr, format("setBitFor - ea = %A *ea = %A\n"), (ea_16), *((UWord*)(ea_16)));*/ \
/*if (*((UWord*)(ea_16)) == 109609828) fatalVMError("bad set bit");*/ \
            setNthBit(asBitIndex((ea_16))); \
            assume(testBitFor((ea_16))); \
/*fprintf(stderr, format("setBitFor - cardinality = %d\n"), getBitmapCardinality());*/ \
/*printMarkedAddresses();*/ \
        }

        /**
         * Clears the appropriate bit in the bitmap for a given address.
         *
         * @param ea      the effective address for which the corresponding bit is to be set
         */
#define  clearBitFor(ea_17) { Address  ea_18 = ea_17;  \
            clearNthBit(asBitIndex((ea_18))); \
        }

        /**
         * Determines if the bit in the bitmap for a given address is set and sets it if it isn't.
         *
         * @param ea      the effective address for which the corresponding bit to be tested
         * @return true if the bit for <code>ea</code> was set before this call
         */
INLINE boolean testAndSetBitFor(Address ea_19) {
/*fprintf(stderr, format("testAndSetBitFor - ea = %A\n"), (ea_19));*/
            return testAndSetNthBit(asBitIndex((ea_19)));
        }

        /**
         * Sets the bits in the bitmap corresponding to the range of memory <code>[start .. end)</code>
         *
         * @param start   the start of the memory range for which the bits are to be set
         * @param end     the end of the memory range for which the bits are to be set
         */
        void setBitsFor(Address start, Address end) {
            const int alignment = HDR_BITS_PER_WORD * HDR_BYTES_PER_WORD;
            Address alignedStart = (Address)roundUp((UWord)start, alignment);
            Address alignedEnd = (Address)roundDown((UWord)end, alignment);

/*fprintf(stderr, format("setBitsFor - start = %A end = %A\n"), start, end);*/
            while (lo(start, end)) {
                setBitFor(start);
                start = Address_add(start, HDR_BYTES_PER_WORD);
            }
        }

        /**
         * Clears the bits in the bitmap corresponding to the range of memory <code>[start .. end)</code>
         *
         * @param start   the start of the memory range for which the bits are to be cleared
         * @param end     the end of the memory range for which the bits are to be cleared
         */
        void clearBitsFor(Address start, Address end) {
            const int alignment = HDR_BITS_PER_WORD * HDR_BYTES_PER_WORD;
            Address alignedStart = (Address)roundUp((UWord)start, alignment);
            Address alignedEnd;

            while (lo(start, alignedStart)) {
                clearBitFor(start);
                start = Address_add(start, HDR_BYTES_PER_WORD);
            }

            // It is always safe to clear past end, so align up
            alignedEnd = (Address)roundUp((UWord)end, alignment);
            zeroWords(getAddressOfBitmapWordFor(alignedStart), getAddressOfBitmapWordFor(alignedEnd));
/*fprintf(stderr, format("clearBitsFor - start = %A end = %A\n"), start, end);*/
        }

        /*---------------------------------------------------------------------------*\
         *                       Initialization/re-initialization                    *
        \*---------------------------------------------------------------------------*/

        /**
         * Initializes or re-initializes the bitmap.
         *
         * @param start              see {@link #getStart}
         * @param size               the size (in bytes) of the bitmap
         * @param objectMemoryStart  the address at which the object memory to be covered by the bitmap starts
         */
        void bitmapInitialize(Address start, int size, Address objectMemoryStart) {
            UWord firstValidBitIndex = asBitIndex(objectMemoryStart);
            UWord bitmapWordIndex = getBitmapWordIndex(firstValidBitIndex);

            bitmapBase = Address_sub(start,  bitmapWordIndex * HDR_BYTES_PER_WORD);
            bitmap = start;
            bitmapSize = size;

            assume(getAddressOfBitmapWordFor(objectMemoryStart) == start);
            assume(getAddressForBitmapWord(start) == objectMemoryStart);

            if (TYPEMAP) {
                /*
                 * Set the type of each word in the bitmap to be UWORD
                 */
                Address p = start;
                size = size / HDR_BYTES_PER_WORD;
                while (size != 0) {
                    setType(p, AddressType_UWORD, HDR_BYTES_PER_WORD);
                    p = Address_add(p, HDR_BYTES_PER_WORD);
                    --size;
                }
            }
        }


        /*---------------------------------------------------------------------------*\
         *                                Iteration                                  *
        \*---------------------------------------------------------------------------*/

        /**
         * Iterates to the next pointer whose bit is set in the bitmap. This updates the values
         * of the global variables used for iteration such that java.lang.Lisp2Bitmap.nextIterationOopAddress
         * will contain the address of the next pointer whose bit is set. If the iteration is
         * finished, then the value of the variable will be 0.
         */
INLINE Address bitmapIterate() {
            Address next = com_sun_squawk_Lisp2Bitmap_Iterator_next; // does caching globals make big diff?
            Address end = com_sun_squawk_Lisp2Bitmap_Iterator_end;
            UWord word = getUWord(bitmapBase, getBitmapWordIndex(asBitIndex(next)));
            UWord mask = getBitmapMask(asBitIndex(next));
            while (true) {
                if (unlikely(next >= end)) { // end of Bitmap range
                    com_sun_squawk_Lisp2Bitmap_Iterator_next = 0;
                    com_sun_squawk_Lisp2Bitmap_Iterator_inUse = false;
                    return 0;
                } else {
                    assume(testBitFor(next) == ((word & mask) != 0));

                    if (unlikely(word & mask)) {    // found bit!
                        com_sun_squawk_Lisp2Bitmap_Iterator_next = Address_add(next, HDR_BYTES_PER_WORD);
                        return next;
                    }

                    next = Address_add(next, HDR_BYTES_PER_WORD);
                    mask = mask << 1;

                    if (unlikely(mask == 0)) { // get next word
                        mask = 1;
                        word = getUWord(bitmapBase, getBitmapWordIndex(asBitIndex(next)));
                        while (word == 0 && next < end) { // skip empty words
                            next = Address_add(next, HDR_BYTES_PER_WORD * HDR_BITS_PER_WORD);
                            word = getUWord(bitmapBase, getBitmapWordIndex(asBitIndex(next)));
                        }
                    } else {
                        // still working on the same word...
                        assume(getBitmapWordIndex(asBitIndex(next)) == getBitmapWordIndex(asBitIndex(Address_sub(next, HDR_BYTES_PER_WORD))));
                    }
                }
            }
        }

NOINLINE static boolean collectGarbage(Address this, Address allocTop, boolean forceFullGC);
NOINLINE static Address copyObjectGraph(Address this, Address object, Address cb, Address allocTop);
