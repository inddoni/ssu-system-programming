from InstTable import *
from LiteralTable import *
from SymbolTable import *
from TokenTable import *
"""
 * literal과 관련된 데이터와 연산을 소유한다.
 * section 별로 하나씩 인스턴스를 할당한다.
"""
class LiteralTable:
    # LiteralTable 생성자 - 클래스 초기화
    def __init__(self):
        self.literalList = []
        self.locationList = []
    # 기타 literal, external 선언 및 처리방법을 구현한다.

    literalCount = 0  # literal 넣을때마다 count 해줌 (static)



    """
     * 새로운 Literal을 table에 추가한다.
     * @param literal : 새로 추가되는 literal의 label
     * @param location : 해당 literal이 가지는 주소값
     * 주의 : 만약 중복된 literal이 putLiteral을 통해서 입력된다면 이는 프로그램 코드에 문제가 있음을 나타낸다.
     * 매칭되는 주소값의 변경은 modifyLiteral()을 통해서 이루어져야 한다.
    """

    def putLiteral(self, literal, location):
        # 입력하려는 literal이 있는지 확인 하고 없으면 add
        if self.search(literal) == -1:
            self.literalList.append(literal)
            self.locationList.append(location)
            if literal.find('=C'):
                self.literalCount += 3
            else:
                self.literalCount += 1

    """
     * 기존에 존재하는 literal 값에 대해서 가리키는 주소값을 변경한다.
     * @param literal : 변경을 원하는 literal의 label
     * @param newLocation : 새로 바꾸고자 하는 주소값
    """

    def modifyLiteral(self, literal, newLocation):
        index = self.literalList.index(literal)
        self.locationList[index] = newLocation

    """
     * 인자로 전달된 literal이 어떤 주소를 지칭하는지 알려준다.
     * @param literal : 검색을 원하는 literal의 label
     * @return literal이 가지고 있는 주소값. 해당 literal이 없을 경우 -1 리턴
    """

    def search(self, literal):

        address = -1;
        if literal in self.literalList:
            idx = self.literalList.index(literal)
            address = self.locationList[idx]
            return address;
        else:
            return address;

    # 주소값 넘겨주면 주소값에 해당하는 literal 을 ASCII 코드로 변환 후 반환
    def getlitAscii(self, addr):
        getAscii = ""
        idx = self.locationList.index(addr)
        findLit = self.literalList[idx]
        if findLit[1] == 'C':
            for i in range(0, 3):
                getAscii += '{:X}'.format(hex(int(findLit[i + 3])))
        else:
            findLit = findLit[3:5]
            getAscii += findLit

        return getAscii

    def setLiteralCount(self, num):
        self.literalCount = num