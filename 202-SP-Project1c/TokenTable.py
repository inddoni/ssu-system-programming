from nltk.tokenize.regexp import RegexpTokenizer

"""
 * 사용자가 작성한 프로그램 코드를 단어별로 분할 한 후, 의미를 분석하고, 최종 코드로 변환하는 과정을 총괄하는 클래스이다. <br>
 * pass2에서 object code로 변환하는 과정은 혼자 해결할 수 없고 symbolTable과 instTable의 정보가 필요하므로 이를 링크시킨다.<br>
 * section 마다 인스턴스가 하나씩 할당된다.
 *
"""
class TokenTable :
    MAX_OPERAND=3
    locCount = 0 #location counter 전역 변수 선언


    # bit 조작의 가독성을 위한 선언 (static)
    nFlag = 32
    iFlag = 16
    xFlag = 8
    bFlag = 4
    pFlag = 2
    eFlag = 1


    '''
     * 초기화하면서 symTable과 literalTable과 instTable을 링크시킨다.
     * @param symTab : 해당 section과 연결되어있는 symbol table
     * @param literalTab : 해당 section과 연결되어있는 literal table
     * @param instTab : instruction 명세가 정의된 instTable
    '''
    def __init__(self, symTab, literalTab, instTab):
        # Token을 다룰 때 필요한 테이블들을 링크시킨다.
        self.symTab = symTab
        self.literalTab = literalTab;
        self.instTab = instTab;
        self.literCheck = []     # String

        # 각 line을 의미별로 분할하고 분석하는 공간.
        self.tokenList = []      # tokenList 생성

        # 주소값계산
        self.locCount = 0


    '''
     * 일반 문자열을 받아서 Token단위로 분리시켜 tokenList에 추가한다.
     * @param line : 분리되지 않은 일반 문자열
    '''
    def putToken(line) :
        tok = Token()
        tok.parsing(line)
        tok.setLocation(locCount);
        tokenList.add(tok);
        #해당 라인의 토큰파싱이 끝나고 Symbol이 있으면 추가해주기
        if(!tok.label.equals(".") && !tok.label.equals(" ") && !tok.label.equals(null)){
            symTab.putSymbol(tok.label, tok.location);
        }
        #해당 라인의 토큰파싱이 끝나고 literal이 있으면 추가해주기
        if(!tok.operand[0].equals("") && tok.operand[0].contains("=")){
            literalTab.putLiteral(tok.operand[0], tok.location);
            literCheck.add(tok.operand[0]);
        }
        #해당 라인의 토큰파싱이 끝나고 nixbpe 설정해주기
        if(instTab.searchOpcode(tok.operator) > 0 || tok.operator.equals("LDA")){
            if(instTab.searchFormat(tok.operator) < 3)
                tok.nixbpe = 0;
            #format이 3이상인 것만 nixbpe 셋팅
            else{
                #n, i flag 설정
                if(tok.operand[0].charAt(0) == '#'){
                    tok.setFlag(nFlag, 0);
                    tok.setFlag(iFlag, 1);
                }
                else if(tok.operand[0].charAt(0) == '@'){
                    tok.setFlag(nFlag, 1);
                    tok.setFlag(iFlag, 0);
                }
                else {
                    tok.setFlag(nFlag, 1);
                    tok.setFlag(iFlag, 1);
                }
                #x flag 설정
                if(tok.operand[1]!=null && tok.operand[1].contains("X"))
                    tok.setFlag(xFlag, 1);
                else
                    tok.setFlag(xFlag,0);

                #p flag 설정
                if(tok.operand[0].charAt(0) != '#' && tok.operator.charAt(0) != '+' && !tok.operator.equals("RSUB"))
                    tok.setFlag(pFlag,1);
                else
                    tok.setFlag(pFlag, 0);

                #e flag 설정
                if(tok.operator.charAt(0) == '+')
                    tok.setFlag(eFlag,1);
                else
                    tok.setFlag(eFlag,0);
            }
            #System.out.println(tok.operator + "  " + tok.operand[1] + "  " + Integer.toHexString(tok.nixbpe));
        }
        #해당 라인의 토큰파싱이 끝나고 operator를 확인해서 locCount 값 계산해주기
        int locationlen = calcLocation(tok);
        if(locationlen > 0)
            locCount += locationlen;

    }

    public int calcLocation(Token token){
        int len = instTab.searchFormat(token.operator);
        if(len > 0)
            return len;
        else{
            if(token.operator.equals("RESW") || token.operator.equals("WORD")){
                len = 3;
            }
            else if(token.operator.equals("RESB")){
                len = Integer.parseInt(token.operand[0]);
            }
            else if(token.operator.equals("BYTE")){
                len = 1;
            }
            else if(token.operator.equals("LTORG")){
                len = literalTab.literalCount;
                literalTab.literalCount = 0;
                int count = 0;
                for(String litCheck : literCheck){
                    literalTab.modifyLiteral(litCheck, locCount+(count*len));
                    count++;
                }
            }
            else if(token.operator.equals("END")){
                len = literalTab.literalCount;
                literalTab.literalCount = 0;
                int count = 0;
                for(String litCheck : literCheck){
                    literalTab.modifyLiteral(litCheck, token.location);
                    count++;
                }
            }
            else if(token.operator.equals("EQU")){
                if(token.operand[0].equals("*")){
                    len = 0;
                }
                else{
                    StringTokenizer st = new StringTokenizer(token.operand[0],"-");
                    int value1 = symTab.search(st.nextToken());
                    int value2 = symTab.search(st.nextToken());
                    len = value1 - value2;
                    symTab.modifySymbol(token.label, len);
                    len = 0;
                }
            }
            else{
                len = -1;
            }
            return len;
        }
    }

    public void printTokenList(){
        for(Token t : tokenList){
            System.out.println(Integer.toHexString(t.location) + "\t" + t.label + "\t" + t.operator + "\t" + t.operand[0]);
        }
    }

    '''
     * tokenList에서 index에 해당하는 Token을 리턴한다.
     * @param index
     * @return : index번호에 해당하는 코드를 분석한 Token 클래스
    '''
    public Token getToken(int index) {
        return tokenList.get(index);
    }

    public void setBytes(int index, int num){
        tokenList.get(index).byteSize = num;
    }

    public void setObjcode(int index, String objCode){
        tokenList.get(index).objectCode = objCode;
    }

    /**
     * Pass2 과정에서 사용한다.
     * instruction table, symbol table literal table 등을 참조하여 objectcode를 생성하고, 이를 저장한다.
     * @param index
     */
    public void makeObjectCode(int index){

        String resultOb = "";
        if(instTab.searchFormat(getToken(index).operator) == 2){
            setBytes(index, 4);
            int opcode = instTab.searchOpcode(getToken(index).operator);
            String opcodeStr = Integer.toHexString(opcode).toUpperCase();
            resultOb += opcodeStr;
            int i = 0;
            while(getToken(index).operand[i] != null){
                if(getToken(index).operand[i].equals("X"))
                    resultOb += "1";
                else if(getToken(index).operand[i].equals("A"))
                    resultOb += "0";
                else if(getToken(index).operand[i].equals("S"))
                    resultOb += "4";
                else //T인 경우
                    resultOb += "5";
                i++;
            }
            if(resultOb.length() < getToken(index).byteSize)
                resultOb += "0";
            setObjcode(index, resultOb);
        }
        else if(instTab.searchFormat(getToken(index).operator) == 3) {
            setBytes(index, 6);
            int disA = 0;
            int opcode = instTab.searchOpcode(getToken(index).operator);
            String oprand = getToken(index).operand[0];

            if(oprand.charAt(0)=='@')
                oprand = oprand.substring(1,oprand.length());

            if(oprand.charAt(0)=='#'){
                disA = Integer.parseInt(oprand.substring(1,oprand.length()));
            }
            else if(oprand.charAt(0) == '='){
                int TA = this.literalTab.search(oprand);
                int PA = getToken(index+1).location;
                disA = TA - PA;
            }
            else{

                int TA = this.symTab.search(oprand);
                int PA = getToken(index+1).location;
                disA = TA - PA;
            }

            # object code 앞부분 처리
            int calcObjCode = opcode * 16 + getToken(index).nixbpe;
            if(Integer.toHexString(calcObjCode).length() < 3)
                resultOb += "0";
            resultOb += Integer.toHexString(calcObjCode).toUpperCase();

            # object code 뒷부분 처리
            if(opcode == 76) //RSUB 처리
                resultOb += "000";

            else if(Integer.toHexString(disA).length() < 3){
                for(int i = 0; i < 3-Integer.toHexString(disA).length(); i++)
                    resultOb += "0";
                resultOb += Integer.toHexString(disA).toUpperCase();
            }
            else if (disA < 0){
                String str = Integer.toHexString(disA).toUpperCase();
                str = str.substring(str.length()-3, str.length());
                resultOb += str;
            }
            else {
                System.out.println("disA error");
            }
            setObjcode(index, resultOb);
        }

        else if(instTab.searchFormat(getToken(index).operator) == 4) {
            setBytes(index, 8);
            String disA="00000";
            int opcode = instTab.searchOpcode(getToken(index).operator);

            # object code 앞부분 처리
            int calcObjCode = opcode * 16 + getToken(index).nixbpe;
            if(opcode == 0) //LDA일 때, 예외처리
                resultOb += "0";
            resultOb += Integer.toHexString(calcObjCode).toUpperCase();

            # object code 뒷부분 처리
            resultOb += disA;

            setObjcode(index, resultOb);
        }

        else if(getToken(index).operator.equals("LTORG") || getToken(index).operator.equals("END")){
            resultOb += literalTab.getlitAscii(getToken(index).location);
            setBytes(index, resultOb.length());
            setObjcode(index, resultOb);
        }

        else if(getToken(index).operator.equals("BYTE")){
            resultOb += getToken(index).operand[0].substring(2, 4);
            setBytes(index, 2);
            setObjcode(index, resultOb);
        }

        else if(getToken(index).operator.equals("WORD")){
            resultOb += "000000";
            setBytes(index, 6);
            setObjcode(index, resultOb);
        }

        #test print
        #System.out.println(index + "   " + getToken(index).operator + "   " + getToken(index).objectCode);
    }

    '''
     * index번호에 해당하는 object code를 리턴한다.
     * @param index
     * @return : object code
    '''
    public String getObjectCode(int index) {
        return tokenList.get(index).objectCode;
    }

}

'''
 * 각 라인별로 저장된 코드를 단어 단위로 분할한 후  의미를 해석하는 데에 사용되는 변수와 연산을 정의한다.
 * 의미 해석이 끝나면 pass2에서 object code로 변형되었을 때의 바이트 코드 역시 저장한다.
'''
class Token :
    MAX_OPERAND = 3






    '''
     * 클래스를 초기화 하면서 바로 line의 의미 분석을 수행한다.
     * @param line 문장단위로 저장된 프로그램 코드
    '''

    def __init__(self):
        # 의미 분석 단계에서 사용되는 변수들 initialize
        self.location = 0
        self.label = ""
        self.operator = ""
        self.operand = [] #String type list
        self.operand[0] = ""
        self.comment = ""
        self.nixbpe = 0

        # object code 생성 단계에서 사용되는 변수들
        self.objectCode = ""
        self.byteSize = 0




    # line의 실질적인 분석을 수행하는 함수. Token의 각 변수에 분석한 결과를 저장한다.
    # @param line 문장단위로 저장된 프로그램 코드.
    def parsing(self, line):
        # 매개로 들어온 line을 tab 단위로 잘라 tokens list에 저장
        tokenizer = ("\t")
        tokens = tokenizer.tokenize(line)

        count = 0

        for token in tokens :
            count += 1

            if count == 1 :
                self.label = token
            elif count == 2 :
                self.operator = token
            elif count == 3 :
                opnd = token
                tokenizer = (",")
                opnds = tokenizer.tokenize(opnd)
                i = 0
                for op in opnds :
                    self.operand[i] = op
                    i += 1
            elif count == 4 :
                self.comment = token
            else :
                 print("[TokenTable.py] parsing() error")

    def setLocation(self, loc) :
        self.location = loc


    def setNixbpe(self, nixbpe) :
        self.nixbpe = nixbpe


    '''
     * n,i,x,b,p,e flag를 설정한다.
     *
     * 사용 예 : setFlag(nFlag, 1);
     *   또는     setFlag(TokenTable.nFlag, 1);
     *
     * @param flag : 원하는 비트 위치
     * @param value : 집어넣고자 하는 값. 1또는 0으로 선언한다.
    '''
    def setFlag(self, flag, value) :
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
    def getFlag(self, flags) :
        
        return self.nixbpe;


