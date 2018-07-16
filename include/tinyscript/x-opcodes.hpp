//
//  opcodes.hpp
//  mvm
//
//  Created by Amy Parent on 03/07/2018.
//  Copyright © 2018 Amy Parent. All rights reserved.
//

OPCODE(halt,0,0)

OPCODE(load_c,1,1)
OPCODE(load_yes,1,0)
OPCODE(load_no,1,0)
OPCODE(load,1,1)
OPCODE(store,0,1)

OPCODE(fmin,0,0)
OPCODE(fadd,-1,0)
OPCODE(fsub,-1,0)
OPCODE(fmul,-1,0)
OPCODE(fdiv,-1,0)

OPCODE(imin,0,0)
OPCODE(iadd,-1,0)
OPCODE(isub,-1,0)
OPCODE(imul,-1,0)
OPCODE(idiv,-1,0)

OPCODE(i2f,0,0)
OPCODE(f2i,0,0)

OPCODE(sadd,-1,0)

OPCODE(log_and,-1,0)
OPCODE(log_or,-1,0)
OPCODE(test_flt,-1,0)
OPCODE(test_flteq,-1,0)
OPCODE(test_fgt,-1,0)
OPCODE(test_fgteq,-1,0)
OPCODE(test_feq,-1,0)
OPCODE(test_ilt,-1,0)
OPCODE(test_ilteq,-1,0)
OPCODE(test_igt,-1,0)
OPCODE(test_igteq,-1,0)
OPCODE(test_ieq,-1,0)

OPCODE(jmp,0,2)
OPCODE(rjmp,0,2)
OPCODE(jnz,0,2)
OPCODE(rjnz,0,2)

OPCODE(retain,0,0)
OPCODE(release,0,0)

OPCODE(call_f,-1,1)
OPCODE(yield,0,0)
OPCODE(yield_v,0,0)
OPCODE(fail,0,1)

OPCODE(nop,0,0)
