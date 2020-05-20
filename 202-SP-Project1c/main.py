from InstTable import *
from LiteralTable import *
from SymbolTable import *
from TokenTable import *
from Assembler import *



'''
 * 어셈블러의 메인 루틴
'''

assembler = Assembler()
assembler.instTable.openFile("/inst.txt")

assembler.loadInputFile("/input.txt");

assembler.pass1()

assembler.printSymbolTable("symtab_20180445.txt")
assembler.printLiteralTable("literaltab_20180445.txt")
assembler.pass2()
assembler.printObjectCode("output_20180445.txt")

