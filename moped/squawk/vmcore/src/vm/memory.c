/**** Created by Squawk builder from "vmcore/src/vm/memory.c.spp.preprocessed" ****/ /*
 * Copyright 2004-2010 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2011 Oracle Corporation. All Rights Reserved.
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
 * Please contact Oracle Corporation, 500 Oracle Parkway, Redwood
 * Shores, CA 94065 or visit www.oracle.com if you need additional
 * information or have any questions.
 */

    /*-----------------------------------------------------------------------*\
     *                                     DEFNS                             *
    \*-----------------------------------------------------------------------*/


#if 0

#define INLINE_UNALIGNED INLINE
#else
#define INLINE_UNALIGNED 
#endif /* PLATFORM_UNALIGNED_LOADS */

#ifndef PLATFORM_UNALIGNED_64_LOADS
#define PLATFORM_UNALIGNED_64_LOADS PLATFORM_UNALIGNED_LOADS
#endif

    /*-----------------------------------------------------------------------*\
     *                           Memory access verification                  *
    \*-----------------------------------------------------------------------*/

/**
 * Comment the following line for %50 slower but better type checking
 */
#define FASTER_SET_TYPES

/**
 * Comment the following line for assume code that is 20% faster.
 */


/**
 * Address to look for in setType()
 */
#define BADSETTYPE 0

/**
 * Fast object allocation
 */
#define FASTALLOC true

#if SETASSUMES
#define setAssume(x) assume(x)
int     getByte(Address oop, int offset);
int     getUShort(Address oop, int offset);
UWord   getUWord(Address oop, int offset);
jlong   getLong(Address oop, int offset);
jlong   getLongAtWord(Address oop, int offset);
Address getObject(Address oop, int offset);
#else
#define setAssume(x) /**/
#endif

#ifndef C_PARMS_LEFT_TO_RIGHT
#define C_PARMS_LEFT_TO_RIGHT false
#endif

#ifndef C_PARMS_RIGHT_TO_LEFT
#define C_PARMS_RIGHT_TO_LEFT false
#endif

INLINE boolean setArrayLength(Address _oop, int _size);

        /*-----------------------------------------------------------------------*\
         *                               Assertions                               *
        \*-----------------------------------------------------------------------*/

INLINE int inRAM(Address ea_20) {
            Address addr = (ea_20);
    return addr >= com_sun_squawk_GC_ramStart && addr < com_sun_squawk_GC_ramEnd;
        }

INLINE int inROM(Address ea_21) {
            Address addr = (ea_21);
    return addr >= com_sun_squawk_VM_romStart && addr < com_sun_squawk_VM_romEnd;
        }

#ifdef FLASH_MEMORY
#define  inCode(ea_22) (  \
         inROM((ea_22))  \
        )
#else
INLINE int inCode(Address ea_23) {
            Address addr = (ea_23);
    return addr >= com_sun_squawk_VM_romStart && addr < com_sun_squawk_GC_ramEnd;
        }
#endif

        /*-----------------------------------------------------------------------*\
         *                            Type map checking                          *
        \*-----------------------------------------------------------------------*/

#if TYPEMAP
        /**
         * Gets the ASCII character representing a given type.
         *
         * @param type  the type to represent
         * @return the ASCII representation of 'type'
         */
INLINE char getTypeMnemonic(char type_24) {
            return AddressType_Mnemonics[(type_24) & AddressType_TYPE_MASK];
        }

        /**
         * Determines if a given address is within the range of type mapped memory.
         *
         * @param ea   the address to test
         * @return true if 'ea' is within the range of type mapped memory
         */
INLINE boolean isInTypedMemoryRange(Address ea_25) {
            return (hieq((ea_25), memory) && lo((ea_25), memoryEnd));
        }

        /**
         * Gets the address at which the type for a given address is recorded.
         *
         * @param ea   the address for which the type is being queried
         * @return the address at which the type for 'ea' is recorded
         */
INLINE char *getTypePointer(Address ea_26) {
            /*if (!isInTypedMemoryRange((ea_26))) {
                fprintf(stderr, format("access outside of 'memory' chunk: %A\n"), (ea_26));
                return;
             }
             */

            return (char *)(ea_26) + memorySize;
        }

        /**
         * Records the type of the value written to a given address.
         *
         * @param ea   the address written to
         * @param type the type of the value written to 'ea'
         * @param size the length in bytes of the field
         */
INLINE void setType(Address ea_28, char type_30, int size_32) {
            if (isInTypedMemoryRange((ea_28))) {
                char *ptr = getTypePointer((ea_28));
                switch ((size_32)) {
                    case 1:                                                                            break;
                    case 2: *( (unsigned short *)ptr)    = (unsigned short)AddressType_UNDEFINED_WORD; break;
                    case 4: *( (unsigned int   *)ptr)    = (unsigned int)  AddressType_UNDEFINED_WORD; break;
                    case 8: *( (unsigned int   *)ptr)    = (unsigned int)  AddressType_UNDEFINED_WORD;
                            *(((unsigned int   *)ptr)+1) = (unsigned int)  AddressType_UNDEFINED_WORD; break;
                    default: fatalVMError("unknown size in setType()");
                }
                *ptr = (type_30);

                if (BADSETTYPE && (ea_28) == (Address)BADSETTYPE) {
                    openTraceFile();
                    fprintf(
                            traceFile,
                            format("setType @ %A is %c,  [ea - rom = %A]\n"),
                            (ea_28),
                            getTypeMnemonic((type_30)),
                            Address_diff((ea_28), com_sun_squawk_VM_romStart)
                           );
                    printStackTrace("setType");
                }
            }
        }

        /**
         * Verifies that the type of the value at a given address matches a given type.
         *
         * @param ea   the address to test
         * @param type the type to match
         */
        void checkTypeError(Address ea, char recordedType, char type) {
            fprintf(
                    stderr,
                    format("checkType @ %A is %c, not %c  [ea - rom = %A]\n"),
                    ea,
                    getTypeMnemonic(recordedType),
                    getTypeMnemonic(type),
                    Address_diff(ea, com_sun_squawk_VM_romStart)
                   );
            fatalVMError("memory access type check failed");
        }

        /**
         * Verifies that the type of the value at a given address matches a given type.
         *
         * @param ea   the address to test
         * @param type the type to match
         */
INLINE void checkType2(Address ea_34, char recordedType_36, char type_38) {
            char recordedType = (char)((recordedType_36) & AddressType_TYPE_MASK);
            if (recordedType != AddressType_ANY && recordedType != (type_38)) {
                checkTypeError((ea_34), recordedType, (type_38));
            }
        }

        /**
         * Verifies that the type of the value at a given address matches a given type.
         *
         * @param ea   the address to test
         * @param type the type to match
         * @param size the length in bytes of the field
         */
INLINE Address checkType(Address ea_39, char type_40, int size_41) {
            if (isInTypedMemoryRange((ea_39))) {
                /* AddressType_ANY always matches */
                if ((type_40) != AddressType_ANY) {
                    char *a = (char *)(ea_39);
                    char *p = getTypePointer((ea_39));
                    char fillType = ((type_40) == AddressType_BYTECODE) ? AddressType_BYTECODE : AddressType_UNDEFINED;
#ifdef FASTER_SET_TYPES
                    checkType2(a++, *p, (type_40));
#else
                    switch ((size_41)) {
                        case 8: {
                            checkType2(a++, *p++, (type_40));
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            break;
                        }
                        case 4: {
                            checkType2(a++, *p++, (type_40));
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            checkType2(a++, *p++, fillType);
                            break;
                        }
                        case 2: {
                            checkType2(a++, *p++, (type_40));
                            checkType2(a++, *p++, fillType);
                            break;
                        }
                        case 1: {
                            checkType2(a++, *p, (type_40));
                            break;
                        }
                        default: shouldNotReachHere();
                    }
#endif
                }
            }
            return (ea_39);
        }

        /**
         * Gets the type recorded for a given address.
         *
         * @param  the address to test
         */
#define  getType(ea_42) (  \
             *getTypePointer((ea_42))  \
        )

        /**
         * Gets the type of the value that is written to memory by the current memory mutating instruction.
         * This method assumes that the current value of 'ip' is one byte past the current
         * instruction (i.e. it points to the opcode of the next instruction).
         */
#define  getMutationType() (  \
             (char)((*getTypePointer(ip - 1) >> AddressType_MUTATION_TYPE_SHIFT) & AddressType_TYPE_MASK)  \
        )

        /**
         * Sets the type recorded for each address in a range of word-aligned memory to be AddressType_ANY, the default for every type.
         *
         * @param start   the start address of the memory range
         * @param end     the end address of the memory range
         */
        void zeroTypes(Address start, Address end) {
            /* memset is not used as this can only be called on the service thread. */
            UWordAddress s = (UWordAddress)(getTypePointer(start));
            UWordAddress e = (UWordAddress)(getTypePointer(end));
            assume(isWordAligned((UWord)s));
            assume(isWordAligned((UWord)e));
            while (s < e) {
                *s++ = AddressType_ANY_WORD;
            }
        }

        /**
         * Block copies the types recorded for a range of memory to another range of memory.
         *
         * @param src    the start address of the source range
         * @param dst    the start address of the destination range
         * @param length the length (in bytes) of the range
         */
        void copyTypes(Address src, Address dst, int length) {
            /* memmove is not used as this can only be called on the service thread. */
            assume(length >= 0);
/*fprintf(stderr, format("copyTypes: src=%A, dst=%A, length=%d\n"), src, dst, length);*/
            if (lo(src, dst)) {
                char *s = getTypePointer(src) + length;
                char *d = getTypePointer(dst) + length;
                char *end = getTypePointer(src);
                while (s != end) {
                    *--d = *--s;
                }
            } else if (hi(src, dst)) {
                char *s = getTypePointer(src);
                char *d = getTypePointer(dst);
                char *end = s + length;
                while (s != end) {
                    *d++ = *s++;
                }
            }
        }

#else

/**
 * These macros disable the type checking for a production build.
 * A macro replacement for 'getType()' is intentionally omitted.
 */
#define setType(ea, type, size)
#define checkType(ea, type, size)     ea
#define setTypeRange(ea, length, type)
#define zeroTypes(start, end)
#define copyTypes(src, dst, length)
#define getMutationType() 0
        char getType(Address ea) {
            fatalVMError("getType() called without TYPEMAP");
            return 0;
        }
#endif /* TYPEMAP */

    /*-----------------------------------------------------------------------*\
     *                              Memory addressing                        *
    \*-----------------------------------------------------------------------*/







#ifdef BAD_ADDRESS
void checkOneAddress(Address ea, int size, Address addr);
#else
#define checkOneAddress(ea, size, addr)
#endif /* BAD_ADDRESS */

#ifdef com_sun_squawk_CheneyCollector
#define cheneyCheck(ea) assume(cheneyStartMemoryProtect == 0 || \
                   lo(ea, cheneyStartMemoryProtect) || \
                   hieq(ea, cheneyEndMemoryProtect))
#else
#define cheneyCheck(ea)
#endif /* com_sun_squawk_CheneyCollector */

        /**
         * Performs a number of checks on a given part of memory immediately after
         * it was written to.
         *
         * @param ea   the address of the last write to memory
         * @param size the number of bytes written
         */
#define  checkPostWrite(ea_43, size_45) { Address  ea_44 = ea_43;  int  size_46 = size_45;  \
            checkOneAddress((ea_44), (size_46), (Address)BAD_ADDRESS); \
            cheneyCheck((ea_44)); \
        }

        /**
         * Given a base address and offset to a byte value, returns the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value specified by 'oop' and 'offset'
         */
#define  getByteTyped(base_47, offset_48, type_49) (  \
             *((signed char *)checkType(&((signed char *)(base_47))[(offset_48)], (type_49), 1))  \
        )

        /**
         * Given a base address and offset to a byte value, returns the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value specified by 'oop' and 'offset'
         */
#define  getUByteTyped(base_50, offset_51, type_52) (  \
             *((unsigned char *)checkType(&((unsigned char *)(base_50))[(offset_51)], (type_52), 1))  \
        )

        /**
         * Given a base address and offset to a byte value, sets the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' at which to write
         * @param type   the type of the value about to be written to the effective address
         * @param value  the value
         */
#define  setByteTyped(base_53, offset_55, type_57, value_59) { Address  base_54 = base_53;  Offset  offset_56 = offset_55;  char  type_58 = type_57;  signed char  value_60 = value_59;  \
            signed char *ea = &((signed char *)(base_54))[(offset_56)]; \
            setType(ea, (type_58), 1); \
            *ea = (value_60); \
            checkPostWrite(ea, 1); \
        }

        /**
         * Given a base address and offset to a short value, returns the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in 16 bit words) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value
         */
#define  getShortTyped(base_61, offset_62, type_63) (  \
             *((short *)checkType(&((short *)(base_61))[(offset_62)], (type_63), 2))  \
        )

        /**
         * Given a base address and offset to a short value, returns the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in 16 bit words) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value
         */
#define  getUShortTyped(base_64, offset_65, type_66) (  \
             *((unsigned short *)checkType(&((unsigned short *)(base_64))[(offset_65)], (type_66), 2))  \
        )

        /**
         * Given a base address and offset to a short value, sets the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in 16 bit words) from 'base' at which to write
         * @param type   the type of the value about to be written to the effective address
         * @param value  the value
         */
#define  setShortTyped(base_67, offset_69, type_71, value_73) { Address  base_68 = base_67;  Offset  offset_70 = offset_69;  char  type_72 = type_71;  short  value_74 = value_73;  \
            short *ea = &((short *)(base_68))[(offset_70)]; \
            setType(ea, (type_72), 2); \
            *ea = (value_74); \
            checkPostWrite(ea, 2); \
        }

        /**
         * Given a base address and offset to an integer value, returns the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in 32 bit words) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value specified by 'oop' and 'offset'
         */
#define  getIntTyped(base_75, offset_76, type_77) (  \
             *((int *)checkType(&((int *)(base_75))[(offset_76)], (type_77), 4))  \
        )

        /**
         * Given a base address and offset to an integer value, sets the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in 32 bit words) from 'base' at which to write
         * @param type   the type of the value about to be written to the effective address
         * @param value  the value
         */
#define  setIntTyped(base_78, offset_80, type_82, value_84) { Address  base_79 = base_78;  Offset  offset_81 = offset_80;  char  type_83 = type_82;  int  value_85 = value_84;  \
            int *ea = &((int *)(base_79))[(offset_81)]; \
            setType(ea, (type_83), 4); \
            *ea = (value_85); \
            checkPostWrite(ea, 4); \
        }

        /**
         * Given a base address and offset to a 64 bit value, returns the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value
         */
#define  getLongAtWordTyped(base_86, offset_87, type_88) (  \
             *((jlong *)checkType(&((UWordAddress)(base_86))[(offset_87)], (type_88), 8))  \
        )

        /**
         * Given a base address and offset to a 64 bit value, sets the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' at which to write
         * @param type   the type of the value about to be written to the effective address
         * @param value  the value
         */
#define  setLongAtWordTyped(base_89, offset_91, type_93, value_95) { Address  base_90 = base_89;  Offset  offset_92 = offset_91;  char  type_94 = type_93;  jlong  value_96 = value_95;  \
            jlong *ea = (jlong *)&((UWordAddress)(base_90))[(offset_92)]; \
            setType(ea, (type_94), 8); \
            *ea = (value_96); \
            checkPostWrite(ea, 8); \
        }

        /**
         * Given a base address and offset to a 64 bit value, return the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in 64 bit words) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value
         */
#define  getLongTyped(base_97, offset_98, type_99) (  \
             *((jlong *)checkType(&((jlong *)(base_97))[(offset_98)], (type_99), 8))  \
        )

        /**
         * Given a base address and offset to a 64 bit value, set the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in 64 bit words) from 'base' at which to write
         * @param type   the type of the value about to be written to the effective address
         * @param value  the value
         */
#define  setLongTyped(base_100, offset_102, type_104, value_106) { Address  base_101 = base_100;  Offset  offset_103 = offset_102;  char  type_105 = type_104;  jlong  value_107 = value_106;  \
            jlong *ea = (jlong *)&((jlong *)(base_101))[(offset_103)]; \
            setType(ea, (type_105), 8); \
            *ea = (value_107); \
            checkPostWrite(ea, 8); \
        }

        /**
         * Given a base address and offset to a UWord value, return the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' at which to write
         * @param type   the expected type of the value about to be read from the effective address
         * @return       the value
         */
#if SQUAWK_64
#define  getUWordTyped(base_108, offset_109, type_110) (  \
             (UWord)getLongTyped((base_108), (offset_109), (type_110))  \
        )
#else
#define  getUWordTyped(base_111, offset_112, type_113) (  \
             (UWord)getIntTyped((base_111), (offset_112), (type_113))  \
        )
#endif

        /**
         * Given a base address and offset to a UWord value, set the corresponding value.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' at which to write
         * @param type   the type of the value about to be written to the effective address
         * @param value  the value
         */
#if SQUAWK_64
#define  setUWordTyped(base_114, offset_116, type_118, value_120) { Address  base_115 = base_114;  Offset  offset_117 = offset_116;  char  type_119 = type_118;  UWord  value_121 = value_120;  \
            setLongTyped((base_115), (offset_117), (type_119), (UWord)(value_121)); \
        }
#else
#define  setUWordTyped(base_122, offset_124, type_126, value_128) { Address  base_123 = base_122;  Offset  offset_125 = offset_124;  char  type_127 = type_126;  UWord  value_129 = value_128;  \
            setIntTyped((base_123), (offset_125), (type_127), (UWord)(value_129)); \
        }
#endif


        /*-----------------------------------------------------------------------*\
         *                           Memory access interface                     *
        \*-----------------------------------------------------------------------*/

        /**
         * Sets an 8 bit value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' at which to write
         * @param value the value to write
         */
#define  setByte(base_130, offset_132, value_134) { Address  base_131 = base_130;  Offset  offset_133 = offset_132;  int  value_135 = value_134;  \
            setByteTyped((base_131), (offset_133), AddressType_BYTE, (signed char)(value_135)); \
            setAssume(((value_135) & 0xFF) == (getByte((base_131), (offset_133)) & 0xFF)); \
        }

        /**
         * Sets a 16 bit value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in 16 bit words) from 'base' at which to write
         * @param value  the value to write
         */
#define  setShort(base_136, offset_138, value_140) { Address  base_137 = base_136;  Offset  offset_139 = offset_138;  int  value_141 = value_140;  \
            setShortTyped((base_137), (offset_139), AddressType_SHORT, (short)(value_141)); \
            setAssume(((value_141) & 0xFFFF) == getUShort((base_137), (offset_139))); \
        }

        /**
         * Sets a 32 bit value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in 32 bit words) from 'base' at which to write
         * @param value  the value to write
         */
#define  setInt(base_142, offset_144, value_146) { Address  base_143 = base_142;  Offset  offset_145 = offset_144;  int  value_147 = value_146;  \
            setIntTyped((base_143), (offset_145), AddressType_INT, (value_147)); \
            setAssume((value_147) == getInt((base_143), (offset_145))); \
        }

        /**
         * Sets a UWord value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from oop at which to write
         * @param value  the value to write
         */
#define  setUWord(base_148, offset_150, value_152) { Address  base_149 = base_148;  Offset  offset_151 = offset_150;  UWord  value_153 = value_152;  \
            setUWordTyped((base_149), (offset_151), AddressType_UWORD, (value_153)); \
            setAssume((value_153) == getUWord((base_149), (offset_151))); \
        }

        /**
         * Sets a pointer value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from oop at which to write
         * @param value  the value to write
         */
#define  setObject(base_154, offset_156, value_158) { Address  base_155 = base_154;  Offset  offset_157 = offset_156;  Address  value_159 = value_158;  \
            setUWordTyped((base_155), (offset_157), AddressType_REF, (UWord)(value_159)); \
            setAssume((value_159) == getObject((base_155), (offset_157))); \
        }

        /**
         * Sets a pointer value in memory and updates write barrier bit for the pointer if
         * a write barrier is being maintained.
         *
         * @param base   the base address
         * @param offset the offset to a field in the object
         */
#define  setObjectAndUpdateWriteBarrier(base_160, offset_162, value_164) { Address  base_161 = base_160;  Offset  offset_163 = offset_162;  Address  value_165 = value_164;  \
            setObject((base_161), (offset_163), (value_165)); \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
 \
        }

        /**
         * Sets a 64 bit value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in 64 bit words) from 'base' at which to write
         * @param value  the value to write
         */
#define  setLong(base_166, offset_168, value_170) { Address  base_167 = base_166;  Offset  offset_169 = offset_168;  jlong  value_171 = value_170;  \
            setLongTyped((base_167), (offset_169), AddressType_LONG, (value_171)); \
            setAssume((value_171) == getLong((base_167), (offset_169))); \
        }

        /**
         * Sets a 64 bit value in memory at a UWord offset.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' at which to write
         * @param value  the value to write
         */
#define  setLongAtWord(base_172, offset_174, value_176) { Address  base_173 = base_172;  Offset  offset_175 = offset_174;  jlong  value_177 = value_176;  \
            if (SQUAWK_64 || PLATFORM_UNALIGNED_64_LOADS) { \
                setLongAtWordTyped((base_173), (offset_175), AddressType_LONG, (value_177)); \
            } else { \
                const int highOffset = (PLATFORM_BIG_ENDIAN) ? (offset_175)     : (offset_175) + 1; \
                const int lowOffset  = (PLATFORM_BIG_ENDIAN) ? (offset_175) + 1 : (offset_175); \
                setIntTyped((base_173), highOffset, AddressType_LONG,  (int)((value_177) >> 32)); \
                setIntTyped((base_173), lowOffset,  AddressType_LONG2, (int) (value_177)); \
            } \
            setAssume((value_177) == getLongAtWord((base_173), (offset_175))); \
        }

        /**
         * Sets a 16 bit value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in 16 bit words) from 'base' at which to write
         * @param value  the value to write
         */
INLINE_UNALIGNED void setUnalignedShort(Address base, Offset offset, int value) {
            signed char *ea = &((signed char *)base)[offset];

            if (PLATFORM_UNALIGNED_LOADS) {
                 setShortTyped(ea, 0, AddressType_SHORT, (short)value);
            } else {
                int b0 =  value       & 0xFF;
                int b1 = (value >> 8) & 0xFF;

                if (PLATFORM_BIG_ENDIAN) {
                    setByteTyped(ea, 0, AddressType_ANY, (char)b1);
                    setByteTyped(ea, 1, AddressType_ANY, (char)b0);
                } else {
                    setByteTyped(ea, 0, AddressType_ANY, (char)b0);
                    setByteTyped(ea, 1, AddressType_ANY, (char)b1);
                }
            }
            setAssume((value & 0xFFFF) == getUnalignedShort(base, offset));
        }

        /**
         * Sets a 32 bit value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in 32 bit words) from 'base' at which to write
         * @param value  the value to write
         */
INLINE_UNALIGNED void setUnalignedInt(Address base, Offset offset, int value) {
            signed char *ea = &((signed char *)base)[offset];
            if (PLATFORM_UNALIGNED_LOADS) {
                 setIntTyped(ea, 0, AddressType_INT, value);
            } else {
                int b0 =  value        & 0xFF;
                int b1 = (value >> 8)  & 0xFF;
                int b2 = (value >> 16) & 0xFF;
                int b3 = (value >> 24) & 0xFF;
                if (PLATFORM_BIG_ENDIAN) {
                    setByteTyped(ea, 0, AddressType_ANY, (char)b3);
                    setByteTyped(ea, 1, AddressType_ANY, (char)b2);
                    setByteTyped(ea, 2, AddressType_ANY, (char)b1);
                    setByteTyped(ea, 3, AddressType_ANY, (char)b0);
                } else {
                    setByteTyped(ea, 0, AddressType_ANY, (char)b0);
                    setByteTyped(ea, 1, AddressType_ANY, (char)b1);
                    setByteTyped(ea, 2, AddressType_ANY, (char)b2);
                    setByteTyped(ea, 3, AddressType_ANY, (char)b3);
                }
            }
            setAssume(value == getUnalignedInt(base, offset));
        }

        /**
         * Sets a 64 bit value in memory.
         *
         * @param base   the base address
         * @param offset the offset (in 64 bit words) from 'base' at which to write
         * @param value  the value to write
         */
INLINE_UNALIGNED void setUnalignedLong(Address base, Offset offset, jlong value) {
            signed char *ea = &((signed char *)base)[offset];
            if (PLATFORM_UNALIGNED_64_LOADS || (!PLATFORM_UNALIGNED_LOADS && isAligned((UWord)ea, 8))) {
                setLongTyped(ea, 0, AddressType_LONG, value);
            } else if (PLATFORM_UNALIGNED_LOADS) {
                /* optimize for 32-bit registers */
                unsigned int hi = value >> 32;
                unsigned int lo = value & 0xFFFFFFFF;
                if (PLATFORM_BIG_ENDIAN) {
                    setIntTyped(ea, 0, AddressType_LONG, hi);
                    setIntTyped(ea, 1, AddressType_LONG, lo);
                } else {
                    setIntTyped(ea, 0, AddressType_LONG, lo);
                    setIntTyped(ea, 1, AddressType_LONG, hi);
                }
            } else {
                /* optimize for 32-bit registers */
                unsigned int hi = value >> 32;
                unsigned int lo = value & 0xFFFFFFFF;
                unsigned int b0 =  hi        & 0xFF;
                unsigned int b1 = (hi >> 8)  & 0xFF;
                unsigned int b2 = (hi >> 16) & 0xFF;
                unsigned int b3 = (hi >> 24) & 0xFF;
                unsigned int b4 = (lo >> 0) & 0xFF;
                unsigned int b5 = (lo >> 8) & 0xFF;
                unsigned int b6 = (lo >> 16) & 0xFF;
                unsigned int b7 = (lo >> 24) & 0xFF;
                if (PLATFORM_BIG_ENDIAN) {
                    setByteTyped(ea, 0, AddressType_ANY, (char)b7);
                    setByteTyped(ea, 1, AddressType_ANY, (char)b6);
                    setByteTyped(ea, 2, AddressType_ANY, (char)b5);
                    setByteTyped(ea, 3, AddressType_ANY, (char)b4);
                    setByteTyped(ea, 4, AddressType_ANY, (char)b3);
                    setByteTyped(ea, 5, AddressType_ANY, (char)b2);
                    setByteTyped(ea, 6, AddressType_ANY, (char)b1);
                    setByteTyped(ea, 7, AddressType_ANY, (char)b0);
                } else {
                    setByteTyped(ea, 0, AddressType_ANY, (char)b0);
                    setByteTyped(ea, 1, AddressType_ANY, (char)b1);
                    setByteTyped(ea, 2, AddressType_ANY, (char)b2);
                    setByteTyped(ea, 3, AddressType_ANY, (char)b3);
                    setByteTyped(ea, 4, AddressType_ANY, (char)b4);
                    setByteTyped(ea, 5, AddressType_ANY, (char)b5);
                    setByteTyped(ea, 6, AddressType_ANY, (char)b6);
                    setByteTyped(ea, 7, AddressType_ANY, (char)b7);
                }
            }
            setAssume(value == getUnalignedLong(base, offset));
        }

        /**
         * Gets a signed 8 bit value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' from which to load
         * @return the value
         */
#define  getByte(base_178, offset_179) (  \
             getByteTyped((base_178), (offset_179), AddressType_BYTE)  \
        )

        /**
         * Gets an unsigned 8 bit value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' from which to load
         * @return the value
         */
#define  getUByte(base_180, offset_181) (  \
             getUByteTyped((base_180), (offset_181), AddressType_BYTE)  \
        )

        /**
         * Gets a signed 16 bit value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in 16 bit words) from 'base' from which to load
         * @return the value
         */
#define  getShort(base_182, offset_183) (  \
             getShortTyped((base_182), (offset_183), AddressType_SHORT)  \
        )

        /**
         * Gets an unsigned 16 bit value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in 16 bit words) from 'base' from which to load
         * @return the value
         */
#define  getUShort(base_184, offset_185) (  \
             getUShortTyped((base_184), (offset_185), AddressType_SHORT)  \
        )

        /**
         * Gets a signed 32 bit value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in 32 bit words) from 'base' from which to load
         * @return the value
         */
#define  getInt(base_186, offset_187) (  \
             getIntTyped((base_186), (offset_187), AddressType_INT)  \
        )

        /**
         * Gets a UWord value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' from which to load
         * @return the value
         */
#define  getUWord(base_188, offset_189) (  \
             getUWordTyped((base_188), (offset_189), AddressType_UWORD)  \
        )

        /**
         * Gets a pointer from memory.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' from which to load
         * @return the value
         */
#define  getObject(base_190, offset_191) (  \
             (Address)getUWordTyped((base_190), (offset_191), AddressType_REF)  \
        )

        /**
         * Gets a 64 bit value from memory using a 64 bit word offset.
         *
         * @param base   the base address
         * @param offset the offset (in 64 bit words) from 'base' from which to load
         * @return the value
         */
#define  getLong(base_192, offset_193) (  \
             getLongTyped((base_192), (offset_193), AddressType_LONG)  \
        )

        /**
         * Gets a 64 bit value from memory using a UWord offset.
         *
         * @param base   the base address
         * @param offset the offset (in UWords) from 'base' from which to load
         * @return the value
         */
INLINE jlong getLongAtWord(Address base_194, Offset offset_195) {
            if (SQUAWK_64 || PLATFORM_UNALIGNED_64_LOADS) {
                return getLongAtWordTyped((base_194), (offset_195), AddressType_LONG);
            } else {
                const int highOffset = (PLATFORM_BIG_ENDIAN) ? (offset_195)     : (offset_195) + 1;
                const int lowOffset  = (PLATFORM_BIG_ENDIAN) ? (offset_195) + 1 : (offset_195);
                const unsigned int high = getIntTyped((base_194), highOffset, AddressType_LONG);
                const unsigned int low  = getIntTyped((base_194), lowOffset,  AddressType_LONG2);

                /*Some strange MSC 6 bug prevents the following line from working:
                  return (jlong)(((jlong)high) << 32 | (((jlong)low) & 0xFFFFFFFF)); */

                /* But, for some reason, the following two lines do: */
const jlong res = makeLong(high, low);
                return res;
            }
        }

        /**
         * Gets a signed 16 bit value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' from which to load
         * @return the value
         */
INLINE_UNALIGNED int getUnalignedShort(Address base, Offset offset) {
            signed char *ea = &((signed char *)base)[offset];
            if (PLATFORM_UNALIGNED_LOADS) {
                 return getShortTyped(ea, 0, AddressType_ANY);
            } else {
                if (PLATFORM_BIG_ENDIAN) {
                    int b1 = getByteTyped(ea, 0, AddressType_ANY);
                    int b2 = getUByteTyped(ea, 1, AddressType_ANY);
                    return (b1 << 8) | b2;
                } else {
                    int b1 = getUByteTyped(ea, 0, AddressType_ANY);
                    int b2 = getByteTyped(ea,  1, AddressType_ANY);
                    return (b2 << 8) | b1;
                }
            }
        }

        /**
         * Gets a signed 32 bit value from memory.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' from which to load
         * @return the value
         */
INLINE_UNALIGNED int getUnalignedInt(Address base, Offset offset) {
            signed char *ea = &((signed char *)base)[offset];
            if (PLATFORM_UNALIGNED_LOADS) {
                return getIntTyped(ea, 0, AddressType_ANY);
            } else {
                int b1 = getUByteTyped(ea, 0, AddressType_ANY);
                int b2 = getUByteTyped(ea, 1, AddressType_ANY);
                int b3 = getUByteTyped(ea, 2, AddressType_ANY);
                int b4 = getUByteTyped(ea, 3, AddressType_ANY);
                if (PLATFORM_BIG_ENDIAN) {
                    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
                } else {
                    return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
                }
            }
        }

        /**
         * Gets a 64 bit value from memory using a byte offset.
         *
         * @param base   the base address
         * @param offset the offset (in bytes) from 'base' from which to load
         * @return the value
         */
INLINE_UNALIGNED jlong getUnalignedLong(Address base, Offset offset) {
            signed char *ea = &((signed char *)base)[offset];
            if (PLATFORM_UNALIGNED_64_LOADS || (!PLATFORM_UNALIGNED_LOADS && isAligned((UWord)ea, 8))) {
                return getLongTyped(ea, 0, AddressType_ANY);
            } else if (PLATFORM_UNALIGNED_LOADS) {
                /* optimize for 32-bit registers */
                unsigned int first = getIntTyped((Address)ea, 0, AddressType_ANY);
                unsigned int second = getIntTyped((Address)ea, 1, AddressType_ANY);
                if (PLATFORM_BIG_ENDIAN) {
                      return ((jlong)first << 32) | second;
                } else {
                      return ((jlong)second << 32) | first;
                }
            } else {
                unsigned int b1 = getUByteTyped(ea, 0, AddressType_ANY);
                unsigned int b2 = getUByteTyped(ea, 1, AddressType_ANY);
                unsigned int b3 = getUByteTyped(ea, 2, AddressType_ANY);
                unsigned int b4 = getUByteTyped(ea, 3, AddressType_ANY);
                unsigned int b5 = getUByteTyped(ea, 4, AddressType_ANY);
                unsigned int b6 = getUByteTyped(ea, 5, AddressType_ANY);
                unsigned int b7 = getUByteTyped(ea, 6, AddressType_ANY);
                unsigned int b8 = getUByteTyped(ea, 7, AddressType_ANY);
                if (PLATFORM_BIG_ENDIAN) {
                    unsigned int lo =  (b5 << 24) | (b6 << 16) | (b7 << 8) | b8;
                    unsigned int hi =  (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
                    return ((jlong)hi << 32) | lo;
                } else {
                    unsigned int lo =  (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
                    unsigned int hi =  (b8 << 24) | (b7 << 16) | (b6 << 8) | b5;
                    return ((jlong)hi << 32) | lo;
                }
            }
        }

        /*-----------------------------------------------------------------------*\
         *                           Endianess swapping                          *
        \*-----------------------------------------------------------------------*/

        /**
         * Swaps the endianess of a 2 byte value.
         *
         * @param address   the address of the value
         */
#define  swap2(address_196) { Address  address_197 = address_196;  \
            char type = (TYPEMAP ? getType((address_197)) : AddressType_UNDEFINED); \
            if (!(PLATFORM_UNALIGNED_LOADS || isAligned((UWord)(address_197), 2))) { \
                int b0 = getUByteTyped((address_197), 0, AddressType_ANY); \
                int b1 = getUByteTyped((address_197), 1, AddressType_ANY); \
                setByteTyped((address_197), 0, AddressType_ANY, (char)b1); \
                setByteTyped((address_197), 1, AddressType_ANY, (char)b0); \
            } else { \
                int value = getUShortTyped((address_197), 0, AddressType_ANY); \
                int b0 =  value       & 0xFF; \
                int b1 = (value >> 8) & 0xFF; \
                value = (b0 << 8) | b1; \
                setShortTyped((address_197), 0, type, (short)value); \
            } \
        }

        INLINE unsigned int SWAP4_VAL(unsigned int val) {
            return ((val & 0x000000FF) << 24) | ((val & 0x0000FF00) << 8) | ((val & 0x00FF0000) >> 8) | ((val & 0xFF000000) >> 24);
        }

        /**
         * Swaps the endianess of a 4 byte value.
         *
         * @param address   the address of the value
         */
        void swap4(Address address) {
            char type = (TYPEMAP ? getType(address) : AddressType_UNDEFINED);
            if (!(PLATFORM_UNALIGNED_LOADS || isAligned((UWord)address, 4))) {
                int b0 = getUByteTyped(address, 0, AddressType_ANY);
                int b1 = getUByteTyped(address, 1, AddressType_ANY);
                int b2 = getUByteTyped(address, 2, AddressType_ANY);
                int b3 = getUByteTyped(address, 3, AddressType_ANY);
                setByteTyped(address, 0, AddressType_ANY, (char)b3);
                setByteTyped(address, 1, AddressType_ANY, (char)b2);
                setByteTyped(address, 2, AddressType_ANY, (char)b1);
                setByteTyped(address, 3, AddressType_ANY, (char)b0);
            } else {
                setIntTyped(address, 0, type, SWAP4_VAL(getIntTyped(address, 0, AddressType_ANY)));
            }
        }

        /**
         * Swaps the endianess of a 8 byte value.
         *
         * @param address   the address of the value
         */
        void swap8(Address address) {
            char type = (TYPEMAP ? getType(address) : AddressType_UNDEFINED);
            if (!(PLATFORM_UNALIGNED_LOADS || isAligned((UWord)address, 4))) {
                int b0 = getUByteTyped(address, 0, AddressType_ANY);
                int b1 = getUByteTyped(address, 1, AddressType_ANY);
                int b2 = getUByteTyped(address, 2, AddressType_ANY);
                int b3 = getUByteTyped(address, 3, AddressType_ANY);
                int b4 = getUByteTyped(address, 4, AddressType_ANY);
                int b5 = getUByteTyped(address, 5, AddressType_ANY);
                int b6 = getUByteTyped(address, 6, AddressType_ANY);
                int b7 = getUByteTyped(address, 7, AddressType_ANY);
                setByteTyped(address, 0, AddressType_ANY, (char)b7);
                setByteTyped(address, 1, AddressType_ANY, (char)b6);
                setByteTyped(address, 2, AddressType_ANY, (char)b5);
                setByteTyped(address, 3, AddressType_ANY, (char)b4);
                setByteTyped(address, 4, AddressType_ANY, (char)b3);
                setByteTyped(address, 5, AddressType_ANY, (char)b2);
                setByteTyped(address, 6, AddressType_ANY, (char)b1);
                setByteTyped(address, 7, AddressType_ANY, (char)b0);
            } else {
                /* optimize for 32-bit registers */
                unsigned int hi = getIntTyped(address, 0, AddressType_ANY);
                unsigned int lo = getIntTyped(address, 1, AddressType_ANY);
                setIntTyped(address, 0, type, SWAP4_VAL(lo));
                setIntTyped(address, 1, type, SWAP4_VAL(hi));
            }
        }

        /**
         * Swaps the endianess of a value.
         *
         * @param address   the address of the value
         * @param dataSize  the size (in bytes) of the value
         */
#define  swap(address_198, dataSize_200) { Address  address_199 = address_198;  int  dataSize_201 = dataSize_200;  \
            /*fprintf(stderr, format("swap(%A, %d)\n"), (address_199), (dataSize_201));*/ \
            switch ((dataSize_201)) { \
                case 1:               break; \
                case 2: swap2((address_199)); break; \
                case 4: swap4((address_199)); break; \
                case 8: swap8((address_199)); break; \
                default: \
                    fprintf(stderr, "dataSize=%d\n", (dataSize_201)); \
                    shouldNotReachHere(); \
            } \
        }

        /**
         * Swaps the endianess of a word sized value.
         *
         * @param address   the address of the value
         */
#define  swapWord(address_202) { Address  address_203 = address_202;  \
            swap((address_203), HDR_BYTES_PER_WORD); \
        }

        /*-----------------------------------------------------------------------*\
         *                             Memory management                         *
        \*-----------------------------------------------------------------------*/

        /**
         * Zeros a range of words.
         *
         * @param start the start address
         * @param end the end address
         */
#define  zeroWords(start_204, end_206) { UWordAddress  start_205 = start_204;  UWordAddress  end_207 = end_206;  \
            assume(isWordAligned((UWord)(start_205))); \
            assume(isWordAligned((UWord)(end_207))); \
            zeroTypes((start_205), (end_207)); \
            while ((start_205) < (end_207)) { \
                *(start_205) = 0; \
                (start_205)++; \
            } \
        }

#define  traceAllocation(oop_208, size_210) { Address  oop_209 = oop_208;  int  size_211 = size_210;  \
            fprintf(stderr, "%s allocating object: size=%d, alloc free=%d, total free=%d, ptr=%d\n", \
                    (((oop_209) != 0) ? "succeeded" : "failed"), \
                    (size_211), \
                    Address_diff(com_sun_squawk_GC_allocEnd, com_sun_squawk_GC_allocTop), \
                    Address_diff(com_sun_squawk_GC_heapEnd, com_sun_squawk_GC_allocTop), \
                    (Offset)(oop_209)); \
        }

        /**
         * Allocate a chunk of zeroed memory from RAM.
         *
         * @param   size        the length in bytes of the object and its header (i.e. the total number of bytes to be allocated).
         * @param   arrayLength the number of elements in the array being allocated or -1 if a non-array object is being allocated
         * @return a pointer to a well-formed object or null if the allocation failed
         */
        Address allocate(int size, Address klass, int arrayLength) {
            Address block = com_sun_squawk_GC_allocTop;
            Offset available = Address_diff(com_sun_squawk_GC_allocEnd, block);
            Address oop;
            assume(size >= 0);
            if (unlikely(lt(available, size))) {
                if (available < 0) {
                    /* The last allocation overflowed the allocEnd boundary */
                    /*traceAllocation(null, size);*/
                    return null;
                } else {
                    /*
                     * If the object being allocated does not fit in the remaining allocation space
                     * (e.g. Lisp2 young generation) but there is enough total memory available,
                     * then allow the allocation to succeed. Without this, allocation of objects larger than
                     * the allocation space would never succeed.
                     */
                    available = Address_diff(com_sun_squawk_GC_heapEnd, block);
                    if (lt(available, size)) {
                        /*traceAllocation(null, size);*/
                        return null;
                    }
                }
            }

            if (arrayLength == -1) {
                oop = Address_add(block, HDR_basicHeaderSize);
                setObject(oop, HDR_klass, klass);
            } else {
                oop = Address_add(block, HDR_arrayHeaderSize);
                setObject(oop, HDR_klass, klass);
                if (!setArrayLength(oop, arrayLength)) {
                    return 0;
                }
            }
            com_sun_squawk_GC_allocTop = Address_add(block, size);
            zeroWords(oop, com_sun_squawk_GC_allocTop);
            com_sun_squawk_GC_newCount++;
            /*traceAllocation(oop, size);*/
            return oop;
        }

        /**
         * Allocate a chunk of zeroed memory from RAM with hosted.
         *
         * @param   size        the length in bytes of the object and its header (i.e. the total number of
         *                      bytes to be allocated).
         * @param   arrayLength the number of elements in the array being allocated or -1 if a non-array
         *                      object is being allocated
         * @return a pointer to a well-formed object or null if the allocation failed
         */
INLINE Address allocateFast(int size_212, Address klass_213, int arrayLength_214) {
            if (
                com_sun_squawk_GC_excessiveGC != false       ||
                com_sun_squawk_GC_allocationEnabled == false ||
                (com_sun_squawk_GC_GC_TRACING_SUPPORTED && (com_sun_squawk_GC_traceFlags & com_sun_squawk_GC_TRACE_ALLOCATION) != 0)
               ) {
                return null; /* Force call to Java code */
            }
            return allocate((size_212), (klass_213), (arrayLength_214));
        }

        /**
         * Static version of {@link #getDataSize()} so that garbage collector can
         * invoke this method on a possibly forwarded Klass object.
         */
INLINE int getDataSize(Address klass_215) {
            switch (com_sun_squawk_Klass_id((klass_215))) {
                case CID_BOOLEAN:
                case CID_BYTECODE:
                case CID_BYTE: {
                    return 1;
                }
                case CID_CHAR:
                case CID_SHORT: {
                    return 2;
                }
                case CID_DOUBLE:
                case CID_LONG: {
                    return 8;
                }
                case CID_FLOAT:
                case CID_INT: {
                    return 4;
                }
                default: {
                    return HDR_BYTES_PER_WORD;
                }
            }
        }

        /**
         * Copies bytes using memmove.
         */
        void copyBytes(Address src, int srcPos, Address dst, int dstPos, int length, boolean nvmDst) {
            /*fprintf(stderr, format("copying  %d bytes from %A at offset %d to %A at offset %d nvmDst=%d\n"), length, src, srcPos, dst, dstPos, nvmDst);*/
            if (nvmDst) {
                sysToggleMemoryProtection(com_sun_squawk_GC_nvmStart, com_sun_squawk_GC_nvmEnd, false);
            }
            memmove(Address_add(dst, dstPos), Address_add(src, srcPos), length);
            checkPostWrite(Address_add(dst, dstPos), length);
            if (nvmDst) {
                sysToggleMemoryProtection(com_sun_squawk_GC_nvmStart, com_sun_squawk_GC_nvmEnd, true);
            }
        }

#ifdef BAD_ADDRESS
        /**
         * Checks to see if a specific address was written to and print it if it was.
         *
         * @param ea    the address of the last write to memory
         * @param size  the number of bytes written
         * @param addr  the address to check for
         */
        void checkOneAddress(Address ea, int size, Address addr) {
            ByteAddress start  = (ByteAddress)ea;
            ByteAddress end    = start + size;
            ByteAddress target = (ByteAddress)addr;
            if (target >= start && target < end) {
                UWord value = ((UWord *)target)[0];
                fprintf(stderr, format("*******************  [%A] = %A [bcount=%L]\n"), target, value, getBranchCount());
#ifdef BAD_VALUE
                if (value == BAD_VALUE) {
                    fprintf(stderr, format("Stopping because bad value %A written in the range [%A .. %A)\n"), value, start, end);
                    stopVM(-1);
                }
#endif
            }
        }
#endif /* BAD_ADDRESS */
