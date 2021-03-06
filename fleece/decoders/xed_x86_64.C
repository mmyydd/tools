
/*
 * See fleece/COPYRIGHT for copyright information.
 *
 * This file is a part of Fleece.
 *
 * Fleece is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *  
 * This software is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software; if not, see www.gnu.org/licenses
*/

#ifdef __cplusplus
extern "C" {
#include "xed-interface.h"
}
#else
#include "xed-interface.h"
#endif

#include <iomanip>
#include "Alias.h"
#include "Normalization.h"
#include "StringUtils.h"

#define XED_MACHINE_MODE XED_MACHINE_MODE_LONG_64
#define XED_ADDRESS_WIDTH XED_ADDRESS_WIDTH_64b

void aliasSizes(const char* base) {
   int baseLen = strlen(base);
   char* withSize = (char*)malloc(baseLen + 2);
   strcpy(withSize, base);
   withSize[baseLen + 1] = 0;

   
   withSize[baseLen] = 'w';
   Alias::addAlias(withSize, base);

   withSize[baseLen] = 'l';
   Alias::addAlias(withSize, base);
   
   withSize[baseLen] = 'b';
   Alias::addAlias(withSize, base);

   withSize[baseLen] = 'q';
   Alias::addAlias(withSize, base);

   free(withSize);
}

int xedInit(void) {
   xed_tables_init(); 

   Alias::addAlias("%mmx0", "%mm0");
   Alias::addAlias("%mmx1", "%mm1");
   Alias::addAlias("%mmx2", "%mm2");
   Alias::addAlias("%mmx3", "%mm3");
   Alias::addAlias("%mmx4", "%mm4");
   Alias::addAlias("%mmx5", "%mm5");
   Alias::addAlias("%mmx6", "%mm6");
   Alias::addAlias("%mmx7", "%mm7");

   Alias::addAlias("%mmx0,", "%mm0,");
   Alias::addAlias("%mmx1,", "%mm1,");
   Alias::addAlias("%mmx2,", "%mm2,");
   Alias::addAlias("%mmx3,", "%mm3,");
   Alias::addAlias("%mmx4,", "%mm4,");
   Alias::addAlias("%mmx5,", "%mm5,");
   Alias::addAlias("%mmx6,", "%mm6,");
   Alias::addAlias("%mmx7,", "%mm7,");
   
   Alias::addAlias("%st0", "%st");
   Alias::addAlias("%st0", "%st(0)");
   Alias::addAlias("%st1", "%st(1)");
   Alias::addAlias("%st2", "%st(2)");
   Alias::addAlias("%st3", "%st(3)");
   Alias::addAlias("%st4", "%st(4)");
   Alias::addAlias("%st5", "%st(5)");
   Alias::addAlias("%st6", "%st(6)");
   Alias::addAlias("%st7", "%st(7)");

   Alias::addAlias("%st0,", "%st,");
   Alias::addAlias("%st0,", "%st(0),");
   Alias::addAlias("%st1,", "%st(1),");
   Alias::addAlias("%st2,", "%st(2),");
   Alias::addAlias("%st3,", "%st(3),");
   Alias::addAlias("%st4,", "%st(4),");
   Alias::addAlias("%st5,", "%st(5),");
   Alias::addAlias("%st6,", "%st(6),");
   Alias::addAlias("%st7,", "%st(7),");

   aliasSizes("or");
   aliasSizes("adc");
   aliasSizes("sub");
   aliasSizes("and");
   aliasSizes("xor");
   aliasSizes("lea");
   aliasSizes("lar");
   aliasSizes("mov");
   aliasSizes("add");
   aliasSizes("sbb");
   aliasSizes("cmp");
   aliasSizes("str");
   aliasSizes("pop");
   aliasSizes("push");
   aliasSizes("shrd");
   aliasSizes("xchg");
   aliasSizes("imul");
   aliasSizes("setb");
   aliasSizes("sets");
   aliasSizes("pand");
   aliasSizes("test");
   aliasSizes("seto");
   aliasSizes("cmovl");
   aliasSizes("cmovo");
   aliasSizes("cmovp");
   aliasSizes("fimul");
   aliasSizes("paddw");
   aliasSizes("pslld");
   aliasSizes("psllq");
   aliasSizes("pandn");
   aliasSizes("psadbw");
   aliasSizes("paddsw");
   aliasSizes("pmulhw");
   aliasSizes("pminsw");
   aliasSizes("pmaxsw");
   aliasSizes("pmulhuw");
   aliasSizes("pcmpgtd");
   aliasSizes("pcmpgtb");
   aliasSizes("ucomiss");
   aliasSizes("punpckldq");
   aliasSizes("packssdwq");
   aliasSizes("punpckhwd");

   return 0;
}

void xed_x86_64_norm(char* buf, int bufLen) {

   cleanSpaces(buf, bufLen);
   toLowerCase(buf, bufLen);
   spaceAfterCommas(buf, bufLen);
   trimHexZeroes(buf, bufLen);
   trimHexFs(buf, bufLen);

   std::string str(buf);
  
   removeOperand(str, "fadd", ", %st0");
   removeOperand(str, "faddb", ", %st0");
   removeOperand(str, "faddw", ", %st0");
   removeOperand(str, "faddl", ", %st0");
   removeOperand(str, "faddq", ", %st0");

   removeOperand(str, "fldl", ", %st0");
   removeOperand(str, "fldq", ", %st0");
   removeOperand(str, "fld", ", %st0");
   removeOperand(str, "fbld", ", %st0");
   removeOperand(str, "fst", "%st0, ");
   removeOperand(str, "fstp", "%st0, ");
   removeOperand(str, "fstpl", "%st0, ");
   removeOperand(str, "fbstp", "%st0, ");
   removeOperand(str, "fstpq", "%st0, ");
   removeOperand(str, "fcmovu", ", %st0");
   removeOperand(str, "fcmovnu", ", %st0");
   removeOperand(str, "fildw", ", %st0");
   removeOperand(str, "fildq", ", %st0");
   removeOperand(str, "fistw", "%st0, ");
   removeOperand(str, "fistpl", "%st0, ");
   removeOperand(str, "fistpq", "%st0, ");
   removeOperand(str, "fistpw", "%st0, ");
   removeOperand(str, "fisttpw", "%st0, ");
   removeOperand(str, "fisubw", ", %st0");
   removeOperand(str, "fisubl", ", %st0");
   removeOperand(str, "fsubq", ", %st0");
   removeOperand(str, "fsubrq", ", %st0");
   removeOperand(str, "fsubl", ", %st0");

   removeOperand(str, "fmull", ", %st0");
   removeOperand(str, "fucom", ", %st0");
   removeOperand(str, "fcom", ", %st0");
   removeOperand(str, "fcomp", ", %st0");
   removeOperand(str, "fcoml", ", %st0");
   removeOperand(str, "fcompl", ", %st0");
   removeOperand(str, "fidivw", ", %st0");
   removeOperand(str, "fdivl", ", %st0");
   removeOperand(str, "fdivrl", ", %st0");
   removeOperand(str, "fsubrl", ", %st0");
   removeOperand(str, "fisubrw", ", %st0");
   removeOperand(str, "fisubrl", ", %st0");
   removeOperand(str, "fidivrw", ", %st0");
   removeOperand(str, "fidivrl", ", %st0");
   removeOperand(str, "ficomw", ", %st0");
   removeOperand(str, "ficompw", ", %st0");
   removeOperand(str, "ficomu", ", %st0");
   removeOperand(str, "ficoml", ", %st0");
   removeOperand(str, "ficompl", ", %st0");
   removeOperand(str, "ficompw", ", %st0");
   removeOperand(str, "fimulw", ", %st0");
   removeOperand(str, "fimull", ", %st0");
   removeOperand(str, "fiaddl", ", %st0");

   removeOperand(str, "fcoml", "%st0, ");
   removeOperand(str, "ficoml", "%st0, ");
   removeOperand(str, "fmull", "%st0, ");
   removeOperand(str, "fimull", "%st0, ");
   removeOperand(str, "fiaddl", "%st0, ");
   removeOperand(str, "fistl", ", %st0");
   removeOperand(str, "fstl", ", %st0");
   removeOperand(str, "fstpl", ", %st0");
   removeOperand(str, "fldl", "%st0, ");
   removeOperand(str, "fbldl", "%st0, ");
   removeOperand(str, "fildl", "%st0, ");
   removeOperand(str, "fsubrl", "%st0, ");
   removeOperand(str, "fsubrwl", ", %st0");
   removeOperand(str, "fisubl", "%st0, ");
   removeOperand(str, "fbstpl", "%st0, ");
   removeOperand(str, "fisttpl", "%st0, ");
   removeOperand(str, "fsqrtl", " %st0");
   
   strncpy(buf, str.c_str(), bufLen);
   if (buf[str.length() - 1] == ' ') {
      buf[str.length() - 1] = 0;
   }
}

int xed_x86_64_decode(char* inst, int nBytes, char* buf, int bufLen) {
   xed_machine_mode_enum_t mmode = XED_MACHINE_MODE;
   xed_address_width_enum_t stack_addr_width = XED_ADDRESS_WIDTH;

   xed_error_enum_t err;
   xed_decoded_inst_t decoded_inst;

   xed_decoded_inst_zero(&decoded_inst);
   xed_decoded_inst_set_mode(&decoded_inst, mmode, stack_addr_width);
   err = xed_decode(&decoded_inst, (xed_uint8_t*)inst, nBytes);
   if (err != XED_ERROR_NONE) {
      return -1;
   }
   if (!xed_format_context(XED_SYNTAX_ATT, 
          &decoded_inst, buf, bufLen, 0, 0, 0)) {
      return -1;
   }
   return 0;
}

