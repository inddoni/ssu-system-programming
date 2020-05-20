from nltk.tokenize.regexp import RegexpTokenizer
from InstTable import *
from LiteralTable import *
from SymbolTable import *
from TokenTable import *

'''
 * 각 라인별로 저장된 코드를 단어 단위로 분할한 후  의미를 해석하는 데에 사용되는 변수와 연산을 정의한다.
 * 의미 해석이 끝나면 pass2에서 object code로 변형되었을 때의 바이트 코드 역시 저장한다.
'''


class Token:
    """
     * 클래스를 초기화 하면서 바로 line의 의미 분석을 수행한다.
     * @param line 문장단위로 저장된 프로그램 코드
    """
    def __init__(self):
        # 의미 분석 단계에서 사용되는 변수들 initialize
        self.location = 0
        self.label = ""
        self.operator = ""
        self.operand = []  # String type list
        # self.operand.append("") # operand[0] 값 "" 으로 초기화
        self.comment = ""
        self.nixbpe = 0

        # object code 생성 단계에서 사용되는 변수들
        self.objectCode = ""
        self.byteSize = 0

    MAX_OPERAND = 3

    # line의 실질적인 분석을 수행하는 함수. Token의 각 변수에 분석한 결과를 저장한다.
    # @param line 문장단위로 저장된 프로그램 코드.
    def parsing(self, line):

        line = line[:-1]  # 문장 맨 뒤 '\n' 자르기

        # 매개로 들어온 line을 tab 단위로 잘라 tokens list에 저장
        tokenizer = RegexpTokenizer("\t", gaps=True)
        tokens = tokenizer.tokenize(line)

        count = 0

        for token in tokens:
            count += 1

            if count == 1:
                self.label = token
            elif count == 2:
                self.operator = token
            elif count == 3:
                opnd = token
                tokenizer = RegexpTokenizer(",", gaps=True)
                opnds = tokenizer.tokenize(opnd)
                i = 0
                for op in opnds:
                    self.operand.append(op)
                    i += 1
            elif count == 4:
                self.comment = token
            else:
                print("[TokenTable.py] parsing() error")

    def setLocation(self, loc):
        self.location = loc

    def setNixbpe(self, nixbpe):
        self.nixbpe = nixbpe

    def setByteSize(self, num):
        self.byteSize = num

    def setObjectCode(self, str):
        self.objectCode = str

    '''
     * n,i,x,b,p,e flag를 설정한다.
     *
     * 사용 예 : setFlag(nFlag, 1);
     *   또는     setFlag(TokenTable.nFlag, 1);
     *
     * @param flag : 원하는 비트 위치
     * @param value : 집어넣고자 하는 값. 1또는 0으로 선언한다.
    '''

    def setFlag(self, flag, value):
        calc = flag * value;
        self.nixbpe += calc;

    '''
     * 원하는 flag들의 값을 얻어올 수 있다. flag의 조합을 통해 동시에 여러개의 플래그를 얻는 것 역시 가능하다
     *
     * 사용 예 : getFlag(nFlag)
     *   또는     getFlag(nFlag|iFlag)
     *
     * @param flags : 값을 확인하고자 하는 비트 위치
     * @return : 비트위치에 들어가 있는 값. 플래그별로 각각 32, 16, 8, 4, 2, 1의 값을 리턴할 것임.
    '''

    def getFlag(self, flags):

        return self.nixbpe;
