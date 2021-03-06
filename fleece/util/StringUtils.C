
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

#include <iostream>
#include "StringUtils.h"

TokenList::TokenList(const char* buf) {
   int tokenCount = 0;

   int inToken = 0;
   int i = 0;
   const char* c = buf;

   // Iterate over the string counting the number of non-empty tokens separated
   // by spaces.
   while (*c) {
      if (isspace(*c)) {
         if (inToken) {
            tokenCount++;
            inToken = 0;
         }
      } else {
         inToken = 1;
      }
      c++;
   }

   // If we ended without a space and hit at least one non-space character,
   // then we need to count the last token as well.
   if (inToken) {
      tokenCount++;
   }

   // Now that we know how many tokens we will have, we can properly allocate
   // pointers to each.
   tokens = (char**)malloc(tokenCount * sizeof(char*));
   if (tokens == NULL) {
      throw "ERROR: Unable to allocate token array! Exiting...\n";
   }

   // Now, lets go find all those tokens and make copies that we can keep on
   // the heap.
   c = buf;
   int len = 0;
   while (*c) {
      if (isspace(*c)) {
         if (len > 0) {
            char* curToken = (char*)malloc(len + 1);
            if (curToken == NULL) {
               throw "ERROR: Unable to allocate a token! Exiting...\n";
            }
            bcopy(c - len, curToken, len);
            curToken[len] = 0;
            tokens[i] = curToken;
            len = 0;
            i++;
         }
      } else {
         len++;
      }
      c++;
   }

   if (len > 0) {
      char* curToken = (char*)malloc(len + 1);
      if (curToken == NULL) {
         throw "ERROR: Unable to allocate a token! Exiting...\n";
      }
      bcopy(c - len, curToken, len);
      curToken[len] = 0;
      tokens[tokenCount - 1] = curToken;
   }
   nTokens = tokenCount;

}

TokenList::~TokenList() {
   for (size_t i = 0; i < nTokens; i++) {
      free(tokens[i]);
   }
   free(tokens);
}

unsigned int TokenList::size() {
   return nTokens;
}

unsigned int TokenList::getTotalBytes() {
   unsigned int totalBytes = 1; // Start at 1 to include the null terminator.
   for (unsigned int i = 0; i < nTokens; i++) {
      totalBytes += strlen(tokens[i]) + 1;
   }
   return totalBytes;
}

void TokenList::fillBuf(char* buf, unsigned int len) {
   unsigned int nCopied = 0;
   for (unsigned int i = 0; i < nTokens && nCopied < len; i++) {
      char* c = tokens[i];
      while (*c && nCopied < len) {
         *buf = *c;
         c++;
         buf++;
         nCopied++;
      }
      if (nCopied < len && i + 1 < nTokens) {
         *buf = ' ';
         buf++;
         nCopied++;
      }
   }
   *buf = '\0';
}

bool TokenList::hasToken(char* token) {
    for (size_t i = 0; i < nTokens; i++) {
        if (!strcmp(token, tokens[i])) {
            return true;
        }
    }
    return false;
}

char* TokenList::getToken(unsigned int index) {
   if (index >= nTokens) {
      return NULL;
   }
   return tokens[index];
}

bool TokenList::hasError() {
   for (size_t i = 0; i < nTokens; i++) {
      if (signalsError(tokens[i])) {
         return true;
      }
   }
   return false;
}

/*
 * This function is used to cross-check the error reporting of multiple
 * decoders. The list below is all tokens which suggest that an error has
 * occured during the decoding process. Every entry is checked for every
 * decoder used during reporting.
 */
bool signalsError(const char* token) {
   bool retval =  (
      !strcmp(token, "decoding_error")  ||
      !strcmp(token, "No_Entry")        ||
      !strcmp(token, "[INVALID]")       ||
      !strcmp(token, "[<invalid_reg>]") ||
      !strcmp(token, "<invalid_reg>,")  ||
      !strcmp(token, "<invalid_reg>")   ||
      !strcmp(token, "<INVALID_REG>")   ||
      !strcmp(token, "nop")             ||
      !strcmp(token, "bad")             ||
      !strcmp(token, "?")               ||
      !strcmp(token, "?,")              ||
      !strcmp(token, "undefined")       ||
      !strcmp(token, "undefined,")      ||
      !strcmp(token, "nyi")             ||
      !strcmp(token, "INVALID")         ||
      !strcmp(token, "invalid")         ||
      !strcmp(token, "%?")              ||
      !strcmp(token, "%?,")
   );
   return retval;
}

char negHex(char h) {
   
   if (h >= '0' && h <= '5') {
      return 'f' + '0' - h;
   }

   if (h >= '6' && h <= '9') {
      return '9' + '6' - h;
   }

   if (h >= 'a' && h <= 'f') {
      return 'f' + '0' - h;
   }

   return h;
}

int getStdinBytes(char* buf, unsigned int nBytes) {
   char* end = buf + nBytes;
   int c;
   for (char* cur = buf; cur < end; cur++) {
      c = fgetc(stdin);
      if (c != EOF) {
        *cur = (char)c; 
      } else {
         return -1;
      }
   }
   return 0;
}

bool isHex(char c) {
   return ((c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f'));
}

bool startHex(char* str) {
   return (*(str - 1) == '0' && *str == 'x');
}

void strStripDigits(char* str) {
   bool inDigits = false;
   char* place = str;
   while (*str) {
      if (isdigit(*str)) {
         if (!inDigits) {
            *place = '#';
            place++;
         }
         inDigits = true;
      } else {
         inDigits = false;
         *place = *str;
         place++;
      }
      str++;
   }
   *place = *str;
}

void strStripHex(char* str) {
   str++;
   char* newStr = str;
   bool inHex = false;
   while (*str) {

      if (inHex) {
         if (!isHex(*str)) {
            inHex = false;
         }
      }

      if (!inHex) {
         *newStr = *str;
         newStr++;
         inHex = startHex(str);
      }

      str++;
   }
   *newStr = 0;
}

void TokenList::stripDigits() {
   for (size_t i = 0; i < nTokens; i++) {
      strStripDigits(tokens[i]);
   }
}

void TokenList::stripHex() {
   for (size_t i = 0; i < nTokens; i++) {
      strStripHex(tokens[i]);
   }
}

void printBufferBits(char* buf, int len) {
   int i, j;
   for (i = 0; i < len; i++) {
      for (j = 0; j < 8; j++) {
         if ((buf[i] >> j) & 1) {
            printf("1");
         } else {
            printf("0");
         }
      }
   }
}

unsigned char getBufferBit(char* buf, int bit) {
   return (buf[bit / 8] & (1 << (7 - bit % 8))) >> (7 - bit % 8);
}

void setBufferBit(char* buf, int bit, int val) {
   if (val & 1) {
      buf[bit / 8] |= 1 << (7 - bit % 8);
   } else {
      buf[bit / 8] &= ~(1 << (7 - bit % 8));
   }
}

void flipBufferBit(char* buf, int bit) {
   buf[bit / 8] ^= 1 << (7 - bit % 8);
}

int randomizeBuffer(char* buf, unsigned int len) {
   char* ptr = buf;
   for (ptr = buf; ptr < buf + len; ptr++) {
      *ptr = (char)(rand() & 0xFF);
   }
   return 0;
}

void randomizeBufferBitVector(char* buf, unsigned int* pos, unsigned int len) {
   char* randBuf = (char*)malloc(len);
   randomizeBuffer(randBuf, len);
   setBufferBitVector(buf, pos, randBuf, len);
   free(randBuf);
}

void setBufferBitVector(char* buf, 
                        unsigned int* pos, 
                        char* value, 
                        unsigned int len) {
   for (unsigned int i = 0; i < len; i++) {
      setBufferBit(buf, pos[i], getBufferBit(value, i));
   }
}

void prepend0x(std::string& str) {
   static std::string prep = std::string("0x");
   int start = -1;
   bool inHex = false;
   bool inAlnum = false;
   bool mult = false;
   for (size_t i = 0; i < str.length(); i++) {
      if (isxdigit(str.at(i))) {
         if (!inAlnum && !inHex && !mult) {
            start = i;
            inHex = true;
         }
      } else {
         inAlnum = true;
         if (str.at(i) == '*') {
            inAlnum = false;
            mult = true;
         } else if (!isalnum(str.at(i))) {
            inAlnum = false;
            mult = false;
            if (inHex) {
               str = str.insert(start, prep);
               i += prep.length();
            }
         }
         inHex = false;
      }
   }

   if (inHex) {
      str = str.insert(start, prep);
   }
}

void removeOperand(std::string& str, const std::string& op, const std::string& operand) {
   if (str.find(op) != std::string::npos) {
      size_t pos = str.find(operand);
      if (pos != std::string::npos) {
         str.erase(pos, operand.length());
         if (pos < str.length() && str.at(pos) == ',') {
            str.erase(pos, 2);
         }
      }
   }
}

void removeAtSubStr(std::string& str, const std::string& substr, int len) {
   size_t index = str.find(substr);
   if (index != std::string::npos) {
      str = str.erase(index, len);
   }
}

void replaceStr(char* buf, int bufLen, const char* oldStr, const char* newStr) {
   char* tmp = (char*)malloc(bufLen);
   assert(tmp != NULL);

   size_t oldLen = strlen(oldStr);
   
   char* place = tmp;
   char* cur = buf;

   while (*cur) {
      if (!strncmp(cur, oldStr, oldLen)) {
         size_t newLen = strlen(newStr);
         strncpy(place, newStr, strlen(newStr));
         place += newLen;
         cur += oldLen;
      } else {
         *place = *cur;
         place++;
         cur++;
      }
   }
   *place = *cur;

   strncpy(buf, tmp, bufLen);
   free(tmp);
}

void removeFirst(char* buf, int bufLen, const char* str) {
   char* cur = buf;
   char* place = buf;

   size_t len = strlen(str);
   bool found = false;

   while (*cur) {
      if (!found && !strncmp(cur, str, len)) {
         cur += strlen(str);
      }
      *place = *cur;
      place++;
      cur++;
   }
   *place = *cur;
}

void removeTrailing(char* buf, int bufLen, const char* str) {
   char* cur = buf;
   
   // Advance to the end of the buffer.
   while (*cur) {
      cur++;
   }

   // Count back equal to the number of characters in the string
   const char* s = str;
   while (*s && cur > buf) {
      cur--;
      s++;
   }

   // Check if we have reached what we are looking for, and remove it by
   // placing a null byte if so.
   if (!strcmp(cur, str)) {
      *cur = 0;
   }
}

int getCharHexVal(char c) {
   if (c >= '0' && c <= '9') {
      return c - '0';
   }
   
   if (c >= 'A' && c <= 'F') {
      return 10 + c - 'A';
   }

   return 10 + c - 'a';
}

char valToHex(int val) {
   
   if (val > 9) {
      return val - 10 + 'a';
   }

   return val + '0';
}

int shiftHex(char* hex, int shift, char* buf, int bufLen) {

   if (shift < 0 || hex == NULL || *hex == 0 || bufLen < 2) {
      if (buf != NULL && bufLen > 0) {
         *buf = 0;
      }
      printf("Bad inputs to shift hex!\n");
      return -1;
   }

   int charShift = shift % 4;
   int mod = 1;
   
   for (int i = 0; i < charShift; i++) {
      mod *= 2;
   }

   mod = 16 / mod;
   
   char last = '0';
   char* cur = buf;
   
   while (*hex && isxdigit(*hex) && cur < buf + bufLen - 1) {
      char tmp = *hex;
      int lowerVal = getCharHexVal(*hex) >> (4 - charShift);
      int upperVal = (getCharHexVal(last) % mod) << charShift;

      *cur = valToHex(lowerVal + upperVal);
      last = tmp;
      if (cur != buf || *cur != '0') {
         cur++;
      }
      hex++;
   }

   if (cur < buf + bufLen - 1) {
      *cur = valToHex(((getCharHexVal(last) % mod) << charShift));
      cur++;
   }

   for (int i = 0; i < shift / 4 && cur < buf + bufLen - 1; i++) {
      *cur = '0';
      cur++;
   }
   *cur = 0;

   return 0;
}

void removeCharacter(char* buf, int bufLen, char c) {
   char* place = buf;
   char* cur = buf;
   
   while (*cur) {
      if (*cur != c) {
         *place = *cur;
         place++;
      }
      cur++;
   }
   *place = 0;
}

int getMinBits(long l) {
   
   int n = 64;
   long tmp = l;
   unsigned long matchVal = 0;
   if (l < 0) {
      matchVal = 0xFF00000000000000;
   }


   for (int i = 0; i < 8; i++) {
      if ((tmp & 0xFF00000000000000) == matchVal) {
         n -= 8;
      } else {
         return n;
      }
      tmp = tmp << 8;

   }
   
   return n;
}

