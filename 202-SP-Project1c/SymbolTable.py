
'''
 * symbol과 관련된 데이터와 연산을 소유한다.
 * section 별로 하나씩 인스턴스를 할당한다.
'''
class SymbolTable:

    # 기타 literal, external 선언 및 처리방법을 구현한다.

    # LiteralTable 생성자 - 클래스 초기화
    def __init__(self):
        self.symbolList = []
        self.locationList = []


    '''
     * 새로운 Symbol을 table에 추가한다.
     * @param symbol : 새로 추가되는 symbol의 label
     * @param location : 해당 symbol이 가지는 주소값
     * 주의 : 만약 중복된 symbol이 putSymbol을 통해서 입력된다면 이는 프로그램 코드에 문제가 있음을 나타낸다.
     * 매칭되는 주소값의 변경은 modifySymbol()을 통해서 이루어져야 한다.
    '''

    def putSymbol(symbol, location):
        if search(symbol) == -1 :
            symbolList.append(symbol)
            locationList.append(location)


    '''
     * 기존에 존재하는 symbol 값에 대해서 가리키는 주소값을 변경한다.
     * @param symbol : 변경을 원하는 symbol의 label
     * @param newLocation : 새로 바꾸고자 하는 주소값
    '''
    def modifySymbol(symbol, newLocation):
        index = symbolList.index(symbol)
        locationList[index] = newLocation


    '''
     * 인자로 전달된 symbol이 어떤 주소를 지칭하는지 알려준다.
     * @param symbol : 검색을 원하는 symbol의 label
     * @return symbol이 가지고 있는 주소값. 해당 symbol이 없을 경우 -1 리턴
    '''
    def search(symbol):
        address = -1
        if symbol in symbolList:
            idx = symbolList.index(symbol)
            address = locationList[idx]
            return address
        else :
            return address


    # test def
    def printSymTab(self):
        count = 0
        for symbol in symbolList :
            print(symbol+"\t"+hex(int(locationList[count])))
            count += 1
