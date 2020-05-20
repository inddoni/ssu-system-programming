from nltk.tokenize.regexp import RegexpTokenizer
import os
from InstTable import *
from LiteralTable import *
from SymbolTable import *
from TokenTable import *



# 모든 instruction의 정보를 관리하는 클래스. instruction data들을 저장한다
# 또한 instruction 관련 연산, 예를 들면 목록을 구축하는 함수, 관련 정보를 제공하는 함수 등을 제공 한다.
class InstTable:



    # 클래스 초기화(생성자). 파싱을 동시에 처리한다.
    # @param instFile : instuction에 대한 명세가 저장된 파일 이름
    def __init__(self) :
        self.instMap = dict()
        # 파일을 읽어 한 줄씩 저장할 list init
        self.lines = []

    # inst.data 파일을 불러와 저장하는 공간.
    # 명령어의 이름을 집어넣으면 해당하는 Instruction의 정보들을 리턴할 수 있다.
    instMap = {}  # HashMap<String, Instruction>
    lines = []  # ArrayList<String>



    # 입력받은 이름의 파일을 열고 해당 내용을 파싱하여 instMap에 저장한다.
    def openFile(self, fileName) :
        path = os.getcwd()
        f = open(path+fileName, 'r')
        line_num = 0
        line = f.readline()
        while line :
            self.lines.append(line)
            line_num += 1
            line = f.readline()
        f.close()



    def setInstruction(self) :
        #instruction class를 초기화하면서 라인별로 넣어서 parsing
        for line in self.lines :
            readInsts = Instruction() #instruction 인스턴스 생성
            readInsts.parsing(line)
            self.instMap[readInsts.instruction] = readInsts



    def getLines(self) :
        return self.lines


    def printInstMap(self) :
        for key, value in self.instMap.items() :
            print(key + "\t" + value[2] + "\t" + value[1])


    def searchFormat(self, operName) :

        if operName != "" and operName[0] == '+' :
            return 4;
        else :
            if self.instMap.get(operName) :
                return self.instMap[operName][1]
            else :
                return -1


    def searchOpcode(self, operName) :

        if operName != "" :
            if operName[0] == '+' :
                operName = operName[1:]
                return self.instMap[operName][2]; #index = 2 면, opcode

            else :
                if self.instMap.get(operName) :
                    return self.instMap.get(operName).opcode;
                else :
                    return 0;
        else :
            return -1;


    # get, set, search 등의 함수는 자유 구현



'''
 * 명령어 하나하나의 구체적인 정보는 Instruction클래스에 담긴다.
 * instruction과 관련된 정보들을 저장하고 기초적인 연산을 수행한다.
'''
class Instruction :

    # 클래스를 선언하면서 일반문자열을 즉시 구조에 맞게 파싱한다.
    # @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
    def __init__(self) :
        self.instruction = ""
        self.format = 0
        self.opcode = 0
        self.numberOfOperand = 0
        # self.parsing(line)



    # 일반 문자열을 파싱하여 instruction 정보를 파악하고 저장한다.
    # @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
    def parsing(self, line):
        line = line[:-1] #문장 맨 뒤 '\n' 자르기
        # 매개로 들어온 line을 tab 단위로 잘라 tokens list에 저장
        tokenizer = RegexpTokenizer("\t", gaps=True)
        tokens = tokenizer.tokenize(line)

        count = 1
        for token in tokens :
            if count == 1 :
                self.instruction = token
            elif count == 2 :
                self.format = int(token)
            elif count == 3 :
                self.opcode = int(token, 16)
            elif count == 4 :
                self.numberOfOperand = int(token)
            else :
                print("[InstTable.py] parsing() error")

            count += 1
