/**
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

#include <assert.h>

#include "Decoder.h"
#include "MappedInst.h"

Decoder* dec_xed_x86_64;
Decoder* dec_dyninst_x86_64;
Decoder* dec_dyninst_aarch64;
Decoder* dec_gnu_x86_64;
Decoder* dec_gnu_aarch64;
Decoder* dec_llvm_x86_64;
Decoder* dec_llvm_aarch64;
Decoder* dec_capstone_x86_64;
Decoder* dec_capstone_aarch64;
Decoder* dec_null_x86_64;
Decoder* dec_null_aarch64;

Decoder::Decoder(
        int (*decodeFunc)(char*, int, char*, int),
        int (*initFunc)(void),
        void (*normFunction)(char*, int),
        const char* name,
        const char* arch) {

   func = decodeFunc;
   normFunc = normFunction;
   
   // Execute any initialization required for this decoder.
   if (initFunc != NULL) {
      assert((*initFunc)() != -1);
   }
   
   this->arch = arch;
   this->name = name;

   totalNormTime = 0;
   totalDecodeTime = 0;
   totalDecodedInsns = 0;
}

void Decoder::initAllDecoders()
{
    dec_xed_x86_64 = new Decoder(&xed_x86_64_decode, &xedInit, 
            &xed_x86_64_norm, "xed", "x86_64");
    dec_dyninst_x86_64 = new Decoder(&dyninst_x86_64_decode, NULL, 
            &dyninst_x86_64_norm, "dyninst", "x86_64");
    dec_dyninst_aarch64 = new Decoder(&dyninst_aarch64_decode, 
            &dyninst_aarch64_init, &dyninst_aarch64_norm, "dyninst", "aarch64");
    dec_gnu_x86_64 = new Decoder(&gnu_x86_64_decode, NULL, 
            &gnu_x86_64_norm, "gnu", "x86_64");
    dec_gnu_aarch64 = new Decoder(&gnu_aarch64_decode, NULL, 
            &gnu_aarch64_norm, "gnu", "aarch64");
    dec_llvm_x86_64 = new Decoder(&llvm_x86_64_decode, &LLVMInit, 
            &llvm_x86_64_norm, "llvm", "x86_64");
    dec_llvm_aarch64 = new Decoder(&llvm_aarch64_decode, &LLVMInit, 
            &llvm_aarch64_norm, "llvm", "aarch64");
    dec_capstone_x86_64 = new Decoder(&capstone_x86_64_decode, NULL, 
            &capstone_x86_64_norm, "capstone", "x86_64");
    dec_capstone_aarch64 = new Decoder(&capstone_aarch64_decode, NULL, 
            &capstone_aarch64_norm, "capstone", "aarch64");
    dec_null_x86_64 = new Decoder(&null_x86_64_decode, NULL, 
            &null_x86_64_norm, "null", "x86_64");
    dec_null_aarch64 = new Decoder(&null_aarch64_decode, NULL, 
            &null_aarch64_norm, "null", "aarch64");
}

void Decoder::destroyAllDecoders()
{
    delete dec_xed_x86_64;
    delete dec_dyninst_x86_64;
    delete dec_dyninst_aarch64;
    delete dec_gnu_x86_64;
    delete dec_gnu_aarch64;
    delete dec_llvm_x86_64;
    delete dec_llvm_aarch64;
    delete dec_capstone_x86_64;
    delete dec_capstone_aarch64;
    delete dec_null_x86_64;
    delete dec_null_aarch64;
}

std::vector<Decoder> Decoder::getAllDecoders() {
   std::vector<Decoder> dec;
   dec.push_back(*dec_llvm_x86_64);
   dec.push_back(*dec_llvm_aarch64);
   dec.push_back(*dec_gnu_x86_64);
   dec.push_back(*dec_gnu_aarch64);
   dec.push_back(*dec_dyninst_x86_64);
   dec.push_back(*dec_dyninst_aarch64);
   dec.push_back(*dec_xed_x86_64);
   dec.push_back(*dec_capstone_x86_64);
   dec.push_back(*dec_capstone_aarch64);
   dec.push_back(*dec_null_aarch64);
   dec.push_back(*dec_null_x86_64);
   return dec;
}

void Decoder::printAllNames(void) {
   std::vector<Decoder> allDecoders = getAllDecoders();
   for (size_t i = 0; i < allDecoders.size(); i++) {
      Decoder d = allDecoders[i];
      std::cout << "\t" << d.arch << ":\t" << d.name << "\n";
   }
}

void Decoder::normalize(char* buf, int bufLen) {

   struct timespec startTime;
   struct timespec endTime;

   clock_gettime(CLOCK_MONOTONIC, &startTime);

   normFunc(buf, bufLen);
   clock_gettime(CLOCK_MONOTONIC, &endTime);

   totalNormTime += 1000000000 * (endTime.tv_sec  - startTime.tv_sec ) +
                                 (endTime.tv_nsec - startTime.tv_nsec);
}

int Decoder::decode(char* inst, int nBytes, char* buf, int bufLen) {

   totalDecodedInsns++;

   struct timespec startTime;
   struct timespec endTime;

   clock_gettime(CLOCK_MONOTONIC, &startTime);
   int rc = func(inst, nBytes, buf, bufLen);
   clock_gettime(CLOCK_MONOTONIC, &endTime);

   totalDecodeTime += 1000000000 * (endTime.tv_sec  - startTime.tv_sec ) +
                                   (endTime.tv_nsec - startTime.tv_nsec);
   
   return rc;
}

const char* Decoder::getArch(void) {
   return arch;
}

const char* Decoder::getName(void) {
   return name;
}

int Decoder::getNumBytesUsed(char* inst, int nBytes) {
   MappedInst* mInst = new MappedInst(inst, nBytes, this, false);
   int nUsed = mInst->getNumUsedBytes();
   delete mInst;
   return nUsed;
}

unsigned long Decoder::getTotalDecodeTime() {
   return totalDecodeTime;
}

unsigned long Decoder::getTotalNormalizeTime() {
   return totalNormTime;
}

unsigned long Decoder::getTotalDecodedInsns() {
   return totalDecodedInsns;
}

std::vector<Decoder> Decoder::getDecoders(char* arch, char* decNames) {

    assert(arch);

    /* Get a list of all of the availble decoders */
    std::vector<Decoder> curDecoders = getAllDecoders();

    /* Remove all decoders that don't apply to our architecture */
    auto decode_iter = curDecoders.begin();
    while(decode_iter != curDecoders.end()) {
        Decoder d = *decode_iter;
        if (strcmp(d.getArch(), arch)) {
            decode_iter = curDecoders.erase(decode_iter);
        } else {
            ++decode_iter;
        }
    }

    /* If specific decoders weren't specified, we're done */
    if (decNames == NULL) {
        exit(1);
        return curDecoders;
    }

    /* Turn the comma seperated list into a vector of names */
    std::vector<char*> names;

    size_t decCopyLen = strlen(decNames);
    char* decCopyBase = new char[decCopyLen + 1];
    char* decCopyOrig = decCopyBase;
    assert(decCopyBase);
    memset(decCopyBase, 0, decCopyLen + 1);
    strncpy(decCopyBase, decNames, decCopyLen); 

    char* decCopyStart = decCopyBase;
    for(;*decCopyBase;decCopyBase++)
    {
        if(*decCopyBase == ',')
        {
            *decCopyBase = 0;
            names.push_back(decCopyStart);
            decCopyStart = decCopyBase + 1;
        }
    }

    /* The last arg will not have a comma to terminate */
    names.push_back(decCopyStart);

    /** 
     * names now contains the names of the decoders we need. Now
     * we will remove all of the other decoders the user doesn't
     * want to use.
     */

    decode_iter = curDecoders.begin();
    while(decode_iter != curDecoders.end())
    {
        Decoder d = *decode_iter;

        bool matches = false;
        auto name_iter = names.begin();
        while(name_iter != names.end())
        {
            if(!strcmp(d.getName(), *name_iter))
            {
                matches = true;
                break;
            }

            ++name_iter;
        }

        /* Should this decoder be in the list? */
        if(!matches)
        {
            decode_iter = curDecoders.erase(decode_iter);
        } else {
            ++decode_iter;
        }
    }

    /* Clean up the copy that we made */
    delete decCopyOrig;

    /* Return the filtered list of decoders */
    return curDecoders;
}
