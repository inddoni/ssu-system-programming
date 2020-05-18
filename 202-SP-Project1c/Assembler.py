import InstTable
import LiteralTable
import SymbolTable
import TokenTable


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


class Assembler :
    '''
     * 클래스 초기화. instruction Table을 초기화와 동시에 세팅한다.
     *
     * @param instFile : instruction 명세를 작성한 파일 이름.
    '''
    def __init__(instFile) :
        # instruction 명세를 저장한 공간
        self.instTable = InstTable()
        self.instTable.openFile(instFile)
        self.instTable.setInstruction()

        # 읽어들인 input 파일의 내용을 한 줄 씩 저장하는 공간.
        self.inputTable = null
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
    def loadInputFile(self, inputFile) :
        self.inputTable = InstTable(inputFile)
        self.inputTable.openFile(inputFile)
        self.lineList = inputTable.getLines()



    '''
     * pass1 과정을 수행한다.
     *   1) 프로그램 소스를 스캔하여 토큰단위로 분리한 뒤 토큰테이블 생성
     *   2) label을 symbolTable에 정리
     *
     *    주의사항 : SymbolTable과 LiteralTable과 TokenTable은 프로그램의 section별로 하나씩 선언되어야 한다.
    '''
    def pass1(self) :
        SymbolTable[] symbolTables = new SymbolTable[3];
        LiteralTable[] literalTables = new LiteralTable[3];
        TokenTable[] tokenTables = new TokenTable[3];
        section = -1;

        for line in lineList :
            if line.find('START') or line.find('CSECT') :
                section += 1;
                symbolTables[section] = new SymbolTable();
                literalTables[section] = new LiteralTable();
                tokenTables[section] = new TokenTable(symbolTables[section], literalTables[section], instTable);
            }
            tokenTables[section].putToken(line);

        }

        for(SymbolTable st : symbolTables)
            symtabList.add(st);

        for(LiteralTable lt : literalTables)
            literaltabList.add(lt);

        for(TokenTable tt : tokenTables)
            TokenList.add(tt);

        //tokenTables[0].printTokenList();
        //tokenTables[1].printTokenList();
        //tokenTables[2].printTokenList();
    }

    '''
     * 작성된 SymbolTable들을 출력형태에 맞게 출력한다.
     * @param fileName : 저장되는 파일 이름
    '''
    private void printSymbolTable(String fileName) {
        File file = new File(fileName);
        FileWriter writer = null;
        try{
            writer = new FileWriter(file, false);
            for(SymbolTable st : symtabList){
                int count = 0;
                for(String symbol : st.symbolList) {
                    writer.write(symbol + "\t" + Integer.toHexString(st.locationList.get(count)) + "\n");
                    count++;
                }
                writer.write("\n");
            }
            writer.close();
        }catch(IOException e){
            e.printStackTrace();
        }
    }

    '''
     * 작성된 LiteralTable들을 출력형태에 맞게 출력한다.
     * @param fileName : 저장되는 파일 이름
    '''
    private void printLiteralTable(String fileName) {
        File file = new File(fileName);
        FileWriter writer = null;
        try{
            writer = new FileWriter(file, false);
            for(LiteralTable lt : literaltabList){
                int count = 0;
                for(String literal : lt.literalList) {
                    int last = literal.length() -1;
                    writer.write(literal.substring(3, last) + "\t" + Integer.toHexString(lt.locationList.get(count)) + "\n");
                    count++;
                }
            }
            writer.close();
        }catch(IOException e){
            e.printStackTrace();
        }
    }

    '''
     * pass2 과정을 수행한다.
     *   1) 분석된 내용을 바탕으로 object code를 생성하여 codeList에 저장.
    '''
    private void pass2() {
        for(TokenTable tt : TokenList){
            int count = 0;
            for(Token t : tt.tokenList){
                tt.makeObjectCode(count);
                count++;
            }
        }
    }

    '''
     * 작성된 codeList를 출력형태에 맞게 출력한다.
     * @param fileName : 저장되는 파일 이름
    '''
    private void printObjectCode(String fileName) {
        int endCount = 1;

        for(TokenTable tokT : TokenList){
            int count = 0;
            codeList.add(String.format("H%s\t000000%06X",tokT.getToken(count).label,tokT.locCount));
            int lineLength = 0; //T line에서 쓸 것
            String tempContent = ""; //T line에서 쓸 것
            String tempName = "";
            ArrayList<String> REFlist = new ArrayList<String>();
            ArrayList<String> Mlist = new ArrayList<String>();

            for(Token t : tokT.tokenList){
                //D Line
                if(!tokT.getToken(count).operator.equals("") && tokT.getToken(count).operator.equals("EXTDEF")){
                    String temp = "D";
                    for(String oprand : tokT.getToken(count).operand){
                        temp += oprand;
                        temp += String.format("%06X", tokT.symTab.search(oprand));
                    }
                    codeList.add(temp);
                }

                //R Line
                else if(!tokT.getToken(count).operator.equals("") && tokT.getToken(count).operator.equals("EXTREF")){
                    String temp = "R";
                    for(String oprand : tokT.getToken(count).operand){
                        if(oprand != null) {
                            REFlist.add(oprand);
                            temp += oprand;
                        }
                    }
                    codeList.add(temp);
                }

                //T Line
                else if(!tokT.getToken(count).operator.equals("") && tokT.getObjectCode(count) != null || !tempContent.equals("")){

                    //M line prepare
                    String[] except = tokT.getToken(count).operand[0].split("-"); //befend-buffer 처리
                    if(REFlist.contains(tokT.getToken(count).operand[0])){
                        String line = String.format("M%06X05+%s", tokT.getToken(count).location+1,tokT.getToken(count).operand[0]);
                        Mlist.add(line);
                    }
                    else if(REFlist.contains(except[0])){
                        String line1 = String.format("M%06X06+%s", tokT.getToken(count).location,except[0]);
                        String line2 = String.format("M%06X06-%s", tokT.getToken(count).location,except[1]);
                        Mlist.add(line1);
                        Mlist.add(line2);
                    }


                    if(tokT.getObjectCode(count) == null){
                        String temp = String.format("%s%02X%s", tempName, tempContent.length()/2, tempContent);
                        codeList.add(temp);
                        tempContent = "";
                    }

                    else if (lineLength < 57){
                        if(tempContent.equals("")){
                            tempName = "T";
                            tempName = String.format("%s%06X", tempName, tokT.getToken(count).location);
                            tempContent = tokT.getObjectCode(count);
                        }

                        else {
                            tempContent += tokT.getObjectCode(count);
                            if(tokT.tokenList.size() == count+1){
                                if(!tempContent.equals("")){
                                    String temp = String.format("%s%02X%s", tempName, tempContent.length()/2, tempContent);
                                    codeList.add(temp);
                                    tempContent = "";
                                    lineLength = 0;
                                }

                            }
                        }

                        lineLength += tokT.getToken(count).byteSize;
                    }
                    else {
                        String temp = String.format("%s%02X%s", tempName, tempContent.length()/2, tempContent);
                        codeList.add(temp);
                        lineLength = 0;
                        tempContent = "";

                        //라인 꽉 차서 못들어 간 것 초기화 후에 다시 넣어주기
                        if(tempContent.equals("")){
                            tempName = "T";
                            tempName = String.format("%s%06X", tempName, tokT.getToken(count).location);
                            tempContent = tokT.getObjectCode(count);
                        }
                        else
                            tempContent += tokT.getObjectCode(count);
                        lineLength += tokT.getToken(count).byteSize;
                    }

                }

                count++;
            }

            //M line
            for(String mLine : Mlist){
                codeList.add(mLine);
            }

            //E line
            if(endCount == 1){
                codeList.add("E000000");
                endCount = 0;
            }
            else
                codeList.add("E");

            codeList.add("\n");
        }

        File file = new File(fileName);
        FileWriter writer = null;
        try{
            writer = new FileWriter(file, false);
            for(String s : codeList){

                writer.write(s + "\n");

            }
            writer.close();
        }catch(IOException e){
            e.printStackTrace();
        }

    }

}


'''
 * 어셈블러의 메인 루틴
'''

Assembler assembler = new Assembler("/inst.txt");
assembler.loadInputFile("/input.txt");

assembler.pass1();

assembler.printSymbolTable("symtab_20180445.txt");
assembler.printLiteralTable("literaltab_20180445.txt");
assembler.pass2();
assembler.printObjectCode("output_20180445.txt");

