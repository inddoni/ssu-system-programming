from nltk.tokenize.regexp import RegexpTokenizer
from InstTable import *
from LiteralTable import *
from SymbolTable import *
from TokenTable import *
from Token import *
"""
 * 사용자가 작성한 프로그램 코드를 단어별로 분할 한 후, 의미를 분석하고, 최종 코드로 변환하는 과정을 총괄하는 클래스이다. <br>
 * pass2에서 object code로 변환하는 과정은 혼자 해결할 수 없고 symbolTable과 instTable의 정보가 필요하므로 이를 링크시킨다.<br>
 * section 마다 인스턴스가 하나씩 할당된다.
 *
"""


class TokenTable:
    locCount = 0 #클래스변수선언
    """
     * 초기화하면서 symTable과 literalTable과 instTable을 링크시킨다.
     * @param symTab : 해당 section과 연결되어있는 symbol table
     * @param literalTab : 해당 section과 연결되어있는 literal table
     * @param instTab : instruction 명세가 정의된 instTable
    """
    def __init__(self, symT, literalT, instT):
        # Token을 다룰 때 필요한 테이블들을 링크시킨다.
        self.symTab = symT
        self.literalTab = literalT
        self.instTab = instT
        self.literCheck = []  # String
        TokenTable.locCount = 0 # init
        # 각 line을 의미별로 분할하고 분석하는 공간.
        self.tokenList = []  # tokenList 생성

    """ 전역변수 """
    MAX_OPERAND = 3


    # bit 조작의 가독성을 위한 선언 (static)
    nFlag = 32
    iFlag = 16
    xFlag = 8
    bFlag = 4
    pFlag = 2
    eFlag = 1

    '''
     * 일반 문자열을 받아서 Token단위로 분리시켜 tokenList에 추가한다.
     * @param line : 분리되지 않은 일반 문자열
    '''

    def putToken(self, line):
        tok = Token()
        tok.parsing(line)
        tok.setLocation(TokenTable.locCount)
        self.tokenList.append(tok)

        # 해당 라인의 토큰파싱이 끝나고 Symbol이 있으면 추가해주기
        if tok.label != "." and tok.label != " " and tok.label != "":
            self.symTab.putSymbol(tok.label, tok.location)

        # 해당 라인의 토큰파싱이 끝나고 literal이 있으면 추가해주기
        if len(tok.operand) > 0 and tok.operand[0] != "" and tok.operand[0].find('=') != -1 :
            self.literalTab.putLiteral(tok.operand[0], tok.location)
            self.literCheck.append(tok.operand[0])

        # 해당 라인의 토큰파싱이 끝나고 nixbpe 설정해주기
        if self.instTab.searchOpcode(tok.operator) > 0 or tok.operator == "LDA":
            if self.instTab.searchFormat(tok.operator) < 3:
                tok.nixbpe = 0
            # format이 3이상인 것만 nixbpe 셋팅
            else:
                # n, i flag 설정
                if tok.operand[0][0] == '#':
                    tok.setFlag(self.nFlag, 0)
                    tok.setFlag(self.iFlag, 1)
                elif tok.operand[0][0] == '@':
                    tok.setFlag(self.nFlag, 1)
                    tok.setFlag(self.iFlag, 0)
                else:
                    tok.setFlag(self.nFlag, 1)
                    tok.setFlag(self.iFlag, 1)

                # x flag 설정
                if len(tok.operand) > 1 and tok.operand[1].find("X") != -1:
                    tok.setFlag(self.xFlag, 1)
                else:
                    tok.setFlag(self.xFlag, 0)

                # p flag 설정
                if tok.operand[0][0] != '#' and tok.operator[0] != '+' and tok.operator != "RSUB":
                    tok.setFlag(self.pFlag, 1)
                else:
                    tok.setFlag(self.pFlag, 0)

                # e flag 설정
                if tok.operator[0] == '+':
                    tok.setFlag(self.eFlag, 1)
                else:
                    tok.setFlag(self.eFlag, 0)

            # print(tok.operator + "  " + tok.operand[1] + "  " + Integer.toHexString(tok.nixbpe))

        # 해당 라인의 토큰파싱이 끝나고 operator를 확인해서 locCount 값 계산해주기
        locationlen = self.calcLocation(tok)
        if locationlen > 0:
            TokenTable.locCount += locationlen

    def calcLocation(self, token):
        len = self.instTab.searchFormat(token.operator)
        if len > 0:
            return len
        else:
            if token.operator == "RESW" or token.operator == "WORD":
                len = 3

            elif token.operator == "RESB":
                len = int(token.operand[0])


            elif token.operator == "BYTE":
                len = 1


            elif token.operator == "LTORG":
                len = self.literalTab.literalCount
                self.literalTab.setLiteralCount(0)
                count = 0
                for litCheck in self.literCheck:
                    if litCheck[1:2] == 'C' :
                        len = 3
                    else :
                        len = 1
                    self.literalTab.modifyLiteral(litCheck, TokenTable.locCount + (count * len))
                    count += 1

            elif token.operator == "END":
                len = self.literalTab.literalCount
                self.literalTab.setLiteralCount(0)
                count = 0
                for litCheck in self.literCheck:
                    self.literalTab.modifyLiteral(litCheck, token.location)
                    count += 1

            elif token.operator == "EQU":
                if token.operand[0] == "*":
                    len = 0

                else:

                    tokenizer = RegexpTokenizer("-", gaps=True)
                    tokens = tokenizer.tokenize(token.operand[0])

                    value1 = self.symTab.search(tokens[0])
                    value2 = self.symTab.search(tokens[1])
                    len = value1 - value2
                    self.symTab.modifySymbol(token.label, len)
                    len = 0

            else:
                len = -1

        return len

    def printTokenList(self):
        for t in self.tokenList:
            print(format(int(t.location), 'X') + "\t" + t.label + "\t" + t.operator + "\t" + t.operand[0])

    '''
     * tokenList에서 index에 해당하는 Token을 리턴한다.
     * @param index
     * @return : index번호에 해당하는 코드를 분석한 Token 클래스
    '''

    def getToken(self, index):
        return self.tokenList[index]

    def setBytes(self, index, num):
        self.tokenList[index].setByteSize(num)

    def setObjcode(self, index, objCode):
        self.tokenList[index].setObjectCode(objCode)

    """
     * Pass2 과정에서 사용한다.
     * instruction table, symbol table literal table 등을 참조하여 objectcode를 생성하고, 이를 저장한다.
     * @param index
    """

    def makeObjectCode(self, index):

        resultOb = ""

        if self.instTab.searchFormat(self.getToken(index).operator) == 2:
            self.setBytes(index, 4)
            opcode = self.instTab.searchOpcode(self.getToken(index).operator)
            opcodeStr = "%X" % opcode
            resultOb += opcodeStr

            for i in self.getToken(index).operand :
                if i == "X":
                    resultOb += "1"
                elif i == "A":
                    resultOb += "0"
                elif i == "S":
                    resultOb += "4"
                else:  # T인 경우
                    resultOb += "5"

            if len(resultOb) < self.getToken(index).byteSize:
                resultOb += "0"

            self.setObjcode(index, resultOb)

        elif self.instTab.searchFormat(self.getToken(index).operator) == 3:
            self.setBytes(index, 6)
            disA = 0
            opcode = self.instTab.searchOpcode(self.getToken(index).operator)
            oprand = self.getToken(index).operand[0]

            if oprand[0] == '@':
                oprand = oprand[1:]

            elif oprand[0] == '#':
                disA = int(oprand[1:])

            elif oprand[0] == '=':
                TA = self.literalTab.search(oprand)
                PA = self.getToken(index + 1).location
                disA = TA - PA

            else:
                TA = self.symTab.search(oprand)
                PA = self.getToken(index + 1).location
                disA = TA - PA

            # object code 앞부분 처리
            calcObjCode = opcode * 16 + self.getToken(index).nixbpe
            if len("%X" % calcObjCode) < 3:
                resultOb += "0"
            resultOb += "%X" % calcObjCode

            # object code 뒷부분 처리
            if opcode == 76:  # RSUB 처리
                resultOb += "000"

            elif disA < 0:  # 음수인 경우
                str = hex((disA + (1 << 16)) % (1 << 16))
                str = str[-3:]  # 뒤에서 3글자만 가져오도록 함
                resultOb += str.upper()

            elif len("%X" % disA) < 3:
                str = "0" * (3 - len("%X" % disA))
                resultOb += str
                resultOb += "%X" % disA


            else:
                print("disA error")

            self.setObjcode(index, resultOb)


        elif self.instTab.searchFormat(self.getToken(index).operator) == 4:
            self.setBytes(index, 8)
            disA = "00000"
            opcode = self.instTab.searchOpcode(self.getToken(index).operator)

            # object code 앞부분 처리
            calcObjCode = opcode * 16 + self.getToken(index).nixbpe
            if opcode == 0:  # LDA일 때, 예외처리
                resultOb += "0"
            resultOb += "%X" % calcObjCode

            # object code 뒷부분 처리
            resultOb += disA

            self.setObjcode(index, resultOb)


        elif self.getToken(index).operator == "LTORG" or self.getToken(index).operator == "END":
            resultOb += self.literalTab.getlitAscii(self.getToken(index).location)
            self.setBytes(index, len(resultOb))
            self.setObjcode(index, resultOb)


        elif self.getToken(index).operator == "BYTE":
            resultOb += self.getToken(index).operand[0][2:4]
            self.setBytes(index, 2)
            self.setObjcode(index, resultOb)


        elif self.getToken(index).operator == "WORD":
            resultOb += "000000"
            self.setBytes(index, 6)
            self.setObjcode(index, resultOb)

        # test print
        print('%d\t%s\t%s' % (index, self.getToken(index).operator, self.getToken(index).objectCode))

    """
     * index번호에 해당하는 object code를 리턴한다.
     * @param index
     * @return : object code
    """

    def getObjectCode(self, index):
        return self.tokenList[index].objectCode


