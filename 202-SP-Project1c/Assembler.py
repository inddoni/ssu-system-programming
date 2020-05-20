from InstTable import *
from LiteralTable import *
from SymbolTable import *
from TokenTable import *

'''
 * Assembler :
 * 이 프로그램은 SIC/XE 머신을 위한 Assembler 프로그램의 메인 루틴이다.
 * 프로그램의 수행 작업은 다음과 같다.
 * 1) 처음 시작하면 Instruction 명세를 읽어들여서 assembler를 세팅한다.
 * 2) 사용자가 작성한 input 파일을 읽어들인 후 저장한다.
 * 3) input 파일의 문장들을 단어별로 분할하고 의미를 파악해서 정리한다. (pass1)
 * 4) 분석된 내용을 바탕으로 컴퓨터가 사용할 수 있는 object code를 생성한다. (pass2)
 *
 *
 * 작성중의 유의사항 :
 *  1) 새로운 클래스, 새로운 변수, 새로운 함수 선언은 얼마든지 허용됨. 단,  기존의 변수와 함수들을 삭제하거나 완전히 대체하는 것은 안된다.
 *  2) 마찬가지로 작성된 코드를 삭제하지 않으면 필요에 따라 예외처리, 인터페이스 또는 상속 사용 또한 허용됨.
 *  3) 모든 void 타입의 리턴값은 유저의 필요에 따라 다른 리턴 타입으로 변경 가능.
 *  4) 파일, 또는 콘솔창에 한글을 출력시키지 말 것. (채점상의 이유. 주석에 포함된 한글은 상관 없음)
 *
 *
 *  + 제공하는 프로그램 구조의 개선방법을 제안하고 싶은 분들은 보고서의 결론 뒷부분에 첨부 바랍니다. 내용에 따라 가산점이 있을 수 있습니다.
 */
'''


class Assembler:
    '''
     * 클래스 초기화. instruction Table을 초기화와 동시에 세팅한다.
     * @param instFile : instruction 명세를 작성한 파일 이름.
    '''

    def __init__(self):
        # instruction 명세를 저장한 공간
        self.instTable = InstTable()

        # 읽어들인 input 파일의 내용을 한 줄 씩 저장하는 공간.
        self.inputTable = InstTable()
        self.lineList = []

        # 프로그램의 section별로 symbol table을 저장하는 공간
        self.symtabList = []

        # 프로그램의 section별로 literal table을 저장하는 공간
        self.literaltabList = []

        # 프로그램의 section별로 프로그램을 저장하는 공간
        self.TokenList = []

        # Token, 또는 지시어에 따라 만들어진 오브젝트 코드들을 출력 형태로 저장하는 공간.
        self.codeList = []

    '''
     * inputFile을 읽어들여서 lineList에 저장한다.
     * @param inputFile : input 파일 이름.
    '''

    def loadInputFile(self, inputFile):
        self.inputTable.openFile(inputFile)
        self.lineList = self.inputTable.getLines()

    '''
     * pass1 과정을 수행한다.
     *   1) 프로그램 소스를 스캔하여 토큰단위로 분리한 뒤 토큰테이블 생성
     *   2) label을 symbolTable에 정리
     *
     *    주의사항 : SymbolTable과 LiteralTable과 TokenTable은 프로그램의 section별로 하나씩 선언되어야 한다.
    '''

    def pass1(self):

        section = -1;

        for line in self.lineList:
            if line.find('START') != -1 or line.find('CSECT') != -1 :
                section += 1;
                self.symtabList.append(SymbolTable())
                self.literaltabList.append(LiteralTable())
                tokT = TokenTable(self.symtabList[section], self.literaltabList[section], self.instTable)
                self.TokenList.append(tokT)

            self.TokenList[section].putToken(line)

    """
     * 작성된 SymbolTable들을 출력형태에 맞게 출력한다.
     * @param fileName : 저장되는 파일 이름
    """

    def printSymbolTable(self, fileName):

        file = open(fileName, mode='w', encoding='utf-8')

        for st in self.symtabList:
            count = 0
            for symbol in st.symbolList:
                if len(symbol) != 0 :
                    loc = st.locationList[count]
                    str = "%s\t%X\n" % (symbol, loc)
                    file.write(str)
                    count += 1

        file.write("\n")

    '''
     * 작성된 LiteralTable들을 출력형태에 맞게 출력한다.
     * @param fileName : 저장되는 파일 이름
    '''

    def printLiteralTable(self, fileName):

        file = open(fileName, mode='w', encoding='utf-8')

        for lt in self.literaltabList:
            count = 0
            for literal in lt.literalList:
                if len(literal) != 0 :
                    loc = lt.locationList[count]
                    literal = literal[3:-1]
                    str = "%s\t%X\n" % (literal, loc)
                    file.write(str)
                    count += 1

    '''
     * pass2 과정을 수행한다.
     *   1) 분석된 내용을 바탕으로 object code를 생성하여 codeList에 저장.
    '''

    def pass2(self):
        for tt in self.TokenList:
            count = 0
            for t in tt.tokenList:
                tt.makeObjectCode(count)
                count += 1

    '''
     * 작성된 codeList를 출력형태에 맞게 출력한다.
     * @param fileName : 저장되는 파일 이름
    '''

    def printObjectCode(self, fileName):
        endCount = 1

        for tokT in self.TokenList:
            count = 0
            str = "H%s\t000000%06X" % (tokT.getToken(count).label, tokT.locCount)
            self.codeList.append(str)
            lineLength = 0  # T line에서 쓸 것
            tempContent = ""  # T line에서 쓸 것
            tempName = ""
            REFlist = []
            Mlist = []
            check = True



            for t in tokT.tokenList:
                # D Line
                if tokT.getToken(count).operator != "" and tokT.getToken(count).operator == "EXTDEF":
                    temp = "D"
                    for oprand in tokT.getToken(count).operand:
                        temp += oprand
                        temp += "%06X" % tokT.symTab.search(oprand)

                    self.codeList.append(temp)


                # R Line
                elif tokT.getToken(count).operator != "" and tokT.getToken(count).operator == "EXTREF":
                    temp = "R"
                    for oprand in tokT.getToken(count).operand:
                        if oprand != "":
                            REFlist.append(oprand)
                            temp += oprand

                    self.codeList.append(temp)


                # T Line

                elif tokT.getToken(count).operator != "" and tokT.getObjectCode(count) != "" or tempContent != "":

                    # M line prepare
                    if len(tokT.getToken(count).operand) != 0 :
                        exceptStr = tokT.getToken(count).operand[0].split('-')  # befend-buffer 처리
                        if tokT.getToken(count).operand[0] in REFlist:
                            line = "M%06X05+%s" % (tokT.getToken(count).location + 1, tokT.getToken(count).operand[0])
                            Mlist.append(line)

                        elif exceptStr[0] in REFlist:
                            line1 = "M%06X06+%s" % (tokT.getToken(count).location, exceptStr[0])
                            line2 = "M%06X06-%s" % (tokT.getToken(count).location, exceptStr[1])
                            Mlist.append(line1)
                            Mlist.append(line2)



                    if tokT.getObjectCode(count) == "":
                        temp = "%s%02X%s" % (tempName, len(tempContent) // 2, tempContent)
                        self.codeList.append(temp)
                        tempContent = ""  # init


                    elif lineLength < 57:
                        if tempContent == "":
                            tempName = "T"
                            tempName = "%s%06X" % (tempName, tokT.getToken(count).location)
                            tempContent = tokT.getObjectCode(count)


                        else:
                            tempContent += tokT.getObjectCode(count)
                            if len(tokT.tokenList) == count + 1:
                                if tempContent != "":
                                    temp = "%s%02X%s" % (tempName, len(tempContent) // 2, tempContent)
                                    self.codeList.append(temp)
                                    tempContent = ""
                                    lineLength = 0

                        lineLength += tokT.getToken(count).byteSize

                    else:
                        temp = "%s%02X%s" % (tempName, len(tempContent) // 2, tempContent)
                        self.codeList.append(temp)
                        lineLength = 0
                        tempContent = ""

                        # 라인 꽉 차서 못들어 간 것 초기화 후에 다시 넣어주기
                        if tempContent == "":
                            tempName = "T"
                            tempName = "%s%06X" % (tempName, tokT.getToken(count).location)
                            tempContent = tokT.getObjectCode(count)

                        else:
                            tempContent += tokT.getObjectCode(count)
                        lineLength += tokT.getToken(count).byteSize

                count += 1

            # M line
            for mLine in Mlist:
                self.codeList.append(mLine)

            # E line
            if endCount == 1:
                self.codeList.append("E000000")
                endCount = 0

            else:
                self.codeList.append("E")

            self.codeList.append("\n")

        file = open(fileName, mode='w', encoding='utf-8')

        for s in self.codeList:
            file.write(s + "\n")
