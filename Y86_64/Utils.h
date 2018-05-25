#ifndef Y86_64_UTILS_H
#define Y86_64_UTILS_H
#include "ISA.h"
#include "CodeGen.h"
void get_reg_name(char name[4], ISA_Register reg);
void get_ins_name(char name[8], ISA_OPCode code);
#endif // !Y86_64_UTILS_H

