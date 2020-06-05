package SP20_simulator;

// instruction에 따라 동작을 수행하는 메소드를 정의하는 클래스

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.StringTokenizer;

public class InstLuncher {
    ResourceManager rMgr;
    InstTable instT;
    ArrayList<Instruction> instruction;
    ArrayList<String> instList;

    public InstLuncher(ResourceManager resourceManager, InstTable instTable) {
        this.rMgr = resourceManager;
        this.instT = instTable;
        this.instruction = new ArrayList<Instruction>();
        this.instList = new ArrayList<String>();
    }

    public void setInstruction(){
        int index = 0;
        int count = 0; //instruction instance count
        int passCnt = 0;
        int sectionCount = 0;
        //새로운 instruction 정의
        Instruction[] inst = new Instruction[50];
        boolean test = false;

        for(int SA : rMgr.pSectionAddr){
            if(SA == 4190)
                break;
            System.out.println(SA);
            int limit = rMgr.memoryLoc;
            index = SA;
            passCnt = 0; //다시 0으로 초기화
            sectionCount = 0; //다시 0으로 초기화
            String temp = "";
            while(index < rMgr.memoryLoc){


                //마지막 메모리 처리를 위한 분기문
                if(index + 1 < limit){
                    temp = rMgr.getMemory(index,2);
                    if(temp.length() != 4){
                        if(passCnt < 10){
                            passCnt++;
                            index += 2;
                            continue;
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        inst[count] = new Instruction();
                        instruction.add(inst[count]);
                        temp = Integer.toBinaryString(Integer.parseInt(temp, 16));
                    }
                }
                else { //마지막 메모리 처리 문장
                    temp = rMgr.getMemory(index,1);
                    instList.add(count, temp);
                    index+=1;
                    count++;
                    break;
                }

                //LDA처리
                String front = "";
                String back = "";

                if(temp.length() <= 10){
                    front = "000000";
                    back = temp.substring(0, temp.length()-4);
                    if(back.length() <= 5){;
                        int n = back.length();
                        for(int k = 0; k < 6-n; k++)
                            back = "0" + back;
                    }

                }
                else{
                    front = temp.substring(0, temp.length()-10) + "00";
                    if(front.length() == 4)
                        front = "00"+front;
                    back = temp.substring(temp.length()-10, temp.length()-4);
                }
                int opc = Integer.parseInt(front, 2);



                if(instT.instMap.containsKey(opc) == true) { //instruction 인 경우
                    instruction.get(count).setOpcode(opc); //opcode setting
                    int format = instT.instMap.get(opc).format;
                    instruction.get(count).setSnixbpe(back);
                    instruction.get(count).setNixbpe(back); //nixbpe setting
                    instruction.get(count).setFormat(format); //format setting
                    if (format > 2)
                        instruction.get(count).setNixbpe(""); //nixbpe setting
                    instruction.get(count).setName(instT.instMap.get(opc).instruction); //name setting
                    //buferd-buffer부분 처리
                    if(instruction.get(count).opcode == 0 && instruction.get(count).format == 4)
                        instruction.get(count).format = 3;
                    temp = rMgr.getMemory(index, instruction.get(count).format);
                    instList.add(count, temp);

                    instruction.get(count).instruction = temp;
                    instruction.get(count).pcAddr = index;
                    index += instruction.get(count).format;
                    sectionCount += instruction.get(count).format;

                    if(instruction.get(count).format >= 3)
                        instruction.get(count).setDisp(instList.get(count).substring(3), SA, sectionCount);

                    System.out.println(String.format("%s : disp = %X", temp, instruction.get(count).getDisp()));
                }

                //count 값 증가
                count++;
            }
        }

    }
    public String searchName(String inst){
        int idx = instList.indexOf(inst);
        String name = "";
        try{
            name= instruction.get(idx).name;
        }catch(ArrayIndexOutOfBoundsException e){
            System.out.println("error: " + inst);
        }

        return name;
    }

    public Instruction searchInst(int index){

        Instruction returnInst = null;
        try{
            returnInst = instruction.get(index);
        }catch (ArrayIndexOutOfBoundsException e){
            System.out.println("error");
        }
        return returnInst;
    }

    public ArrayList<String> getAll(){
        return instList;
    }
    public int instTotal(){
        return instList.size();
    }

    public String getOne(int pcaddr){
        String value = "";
        int index = 0;
        for(Instruction i : instruction){
            if(i.pcAddr == pcaddr){
                index = instruction.indexOf(i);
                value = instruction.get(index).instruction;
                return value;
            }
        }
        return value;
    }

    public int getOneIDX(int pcaddr){
        String value = "";
        int index = 0;
        for(Instruction i : instruction){
            if(i.pcAddr == pcaddr){
                index = instruction.indexOf(i);
                value = instruction.get(index).instruction;
                return index;
            }
        }
        return index;
    }
    // instruction 별로 동작을 수행하는 메소드를 정의
    // ex) public void add(){...}
    public void LDA(Instruction instruction){
        //1. 해당 메모리의 값을 가져와서 (상수인지 확인!!)
        int value = 0;
        //상수값 처리를 위한 분기문
        String nixbpe = instruction.Snixbpe;
        String im = nixbpe.substring(0,2);
        if(im.equals("01")){
            //instruction에서 값 가져옴
            value = Integer.parseInt(instruction.instruction.substring(3), 16);
        }
        else{
            //메모리에 접근해서 값 가져옴
            int addr = instruction.getDisp();
            String memV = rMgr.getMemory(addr, 3);
            value = Integer.parseInt(memV, 16);
        }

        //2. A register에 저장
        rMgr.setRegister(0, value);
        rMgr.setRegister(8, instruction.pcAddr+3);
    }

    public void STL(Instruction instruction){
        //1. L register 값을 가져온다.
        int value = rMgr.getRegister(2);
        //2. 메모리 위치 (dis)
        int addr = instruction.getDisp();
        rMgr.setMemoryCal(addr, String.format("%06d", value), 3);

        rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void JSUB(Instruction instruction){
        //1. PC 값을 L register에 저장
        int value = rMgr.getRegister(8);
        rMgr.setRegister(2, value);
        //2. 메모리 값을 PC로 가져 옴
        int addr = Integer.parseInt(instruction.instruction.substring(3), 16);
        rMgr.setRegister(8, addr);
    }
    public void CLEAR(Instruction instruction){
        //해당 레지스터 0으로 초기화
        int regNum = Integer.parseInt(instruction.instruction.substring(2,3));
        rMgr.setRegister(regNum, 0);

        //PCreg값 정해주기
        rMgr.setRegister(8, instruction.pcAddr+2);
    }
    public void LDT(Instruction instruction){
        int value = 0;
        //메모리에서 값 가져오기
        int addr = instruction.getDisp();
        value = Integer.parseInt(rMgr.getMemory(addr,3),16);
        //2. T register에 저장
        rMgr.setRegister(5, value);

        rMgr.setRegister(8, instruction.pcAddr+instruction.instruction.length()/2);
    }
    public void TD(Instruction instruction){
        //해당 메모리에 명시된 테스트 디바이스 이름 가져와서 setting
        int addr = instruction.getDisp();
        String tdName = rMgr.getMemory(addr,1);
        rMgr.testDevice(tdName);

        rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void JEQ(Instruction instruction){
        //1. 같은지 비교를 해서 같으면  (SW register에 flag)
        //2. 메모리에 있는 주소값을 PC에 넣는다.
        if(rMgr.getRegister(9) == 0){
            int addr = instruction.getDisp();
            rMgr.setRegister(8, addr);
        }
        else
            rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void RD(Instruction instruction){
        //1. 메모리에 정의된 디바이스로부터 값을 읽어들여서
        int addr = instruction.getDisp();
        String rdName = rMgr.getMemory(addr,1);
        char[] data = rMgr.readDevice(rdName, 1);
        System.out.println(data);
        //2. A register에 저장한다.
        String origin = String.format("%06x", rMgr.getRegister(0));
        String right = Integer.toHexString(data[0]);
        right = origin.substring(0, 4) + right;

        rMgr.setRegister(0, Integer.parseInt(right,16)); //읽어온 글자의 아스키 코드 넣어줌

        rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void WD(Instruction instruction){
        //1. A-register의 오른쪽 값을 읽어들인다.
        String data = String.format("%06X", rMgr.getRegister(0));
        //16진수로 받아온 것을 10진수로 변환
        int v = Integer.parseInt(data.substring(4), 16);
        //10진수를 char로 변환
        char d = (char)v;


        //2. 메모리에 정의된 장치가져오기
        int addr = instruction.getDisp();
        String wrDevice = rMgr.getMemory(addr,1);
        //3. 장치에 데이터 쓰기
        rMgr.writeDevice(wrDevice, d,3);

        rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void COMPR(Instruction instruction){
        //레지스터 2개를 비교 후 flag 셋팅 (같으면 0, 다르면 1)
        int value1 = rMgr.getRegister(Integer.parseInt(instruction.instruction.substring(2,3)));
        int value2 = rMgr.getRegister(Integer.parseInt(instruction.instruction.substring(3,4)));
        if(value1 == value2)
            rMgr.setRegister(9,0);
        else
            rMgr.setRegister(9,1);

        rMgr.setRegister(8, instruction.pcAddr+2);
    }
    public void STCH(Instruction instruction){
        //1. A-register의 오른쪽 값 가져옴
        String right = String.format("%06x", rMgr.getRegister(0)).substring(4);
        int aReg = Integer.parseInt(right, 16);

        //2. 해당 메모리 + xregister값에 저장 줌값
        int addr = instruction.getDisp() + rMgr.getRegister(1);
        rMgr.setMemoryCal(addr, Integer.toString(aReg), 1);
        rMgr.setRegister(8, instruction.pcAddr+4);
    }
    public void TIXR(Instruction instruction){
        //1. x값 증가해서 다시 x-register에 넣어줌
        int value = rMgr.getRegister(1);
        value += 1;
        rMgr.setRegister(1, value);
        //2. x값과 레지스터 값 비교해서 flag setting
        int regValue = rMgr.getRegister(Integer.parseInt(instruction.instruction.substring(2,3)));
        if(value == regValue)
            rMgr.setRegister(9,0);
        else
            rMgr.setRegister(9,1);
        rMgr.setRegister(8, instruction.pcAddr+2);
    }
    public void COMP(Instruction instruction){
        //1. A register와 메모리 값을 비교한다.
        int aReg = rMgr.getRegister(0);
        //상수값 처리
        //instruction에서 값 가져옴
        int value = Integer.parseInt(instruction.instruction.substring(3), 16);


        if(value == aReg)
            rMgr.setRegister(9, 0);
        else
            rMgr.setRegister(9, 1);

        rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void J(Instruction instruction){
        //1. 메모리에 값을 pc레지스터로 (무조건 점프하는 애)
        int addr = instruction.getDisp();
        rMgr.setRegister(8, addr);
    }
    public void STA(Instruction instruction){
        //1. A register 값을 가져온다.
        int value = rMgr.getRegister(0);
        //2. 메모리 위치 (dis)
        int addr = instruction.getDisp();
        rMgr.setMemoryCal(addr, String.format("%06d", value), 3);

        rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void JLT(Instruction instruction){
        //1. 같은지 비교를 해서 작으면  (SW register에 flag)
        //2. 메모리에 있는 주소값을 PC에 넣는다.
        if(rMgr.getRegister(9) == 1){
            int addr = instruction.getDisp();
            rMgr.setRegister(8, addr);
        }
        else
            rMgr.setRegister(8, instruction.pcAddr+3);
    }
    public void STX(Instruction instruction){
        //1. X register 값을 가져온다.
        int value = rMgr.getRegister(1);
        //2. 메모리 위치 (dis)
        int addr = instruction.getDisp();
        rMgr.setMemoryCal(addr, String.format("%06d", value), 3);

        rMgr.setRegister(8, instruction.pcAddr+4);
    }
    public void RSUB(Instruction instruction){
        //1. L reg 값 가져와서
        //2. PC reg에 넣기
        int value = rMgr.getRegister(2);
        rMgr.setRegister(8, value+4);
    }
    public void LDCH(Instruction instruction){
        //1. 메모리의 값을 가져온다.


        int addr = instruction.getDisp() + rMgr.getRegister(1);
        String s = rMgr.getMemory(addr,1);

        //2. A-register의 오른쪽에 값을 저장한다.
        String origin = String.format("%06x", rMgr.getRegister(0));
        String right = origin.substring(0,4) + s;
        rMgr.setRegister(0, Integer.parseInt(right, 16));


        rMgr.setRegister(8, instruction.pcAddr+4);
    }
}

class Instruction {
    /* bit 조작의 가독성을 위한 선언 */
    public static final int nFlag=32;
    public static final int iFlag=16;
    public static final int xFlag=8;
    public static final int bFlag=4;
    public static final int pFlag=2;
    public static final int eFlag=1;

    public String instruction;
    public int opcode;
    public String Snixbpe;
    public int nixbpe;
    public int disp;
    public int format;
    public String name;
    public int pcAddr;

    public Instruction(){
        instruction = "";
        opcode = 0;
        Snixbpe = "";
        nixbpe = 0;
        disp = 0;
        format = 0;
        name = "";
        pcAddr = 0;
    }
    public void setSnixbpe(String s){
        this.Snixbpe = s;
    }
    public void setDisp(String s, int programAddr, int count){

        int value = Integer.parseInt(s,16);

        if(this.format == 4){
            this.disp = value;
        }
        else{
            if(s.charAt(0) == 'F') //음수처리
                value = value - 4096; // 4096 = FFF + 1을 십진수로 변환한 값
            this.disp = value + programAddr + count;
        }
    }
    public int getDisp(){
        return this.disp;
    }
    public void setOpcode(int code){
        this.opcode = code;
    }
    public void setFormat(int format){
        if(format == 3 && this.Snixbpe.charAt(5) == '1')
            this.format = 4;
        else
            this.format = format;
    }

    public void setName(String name){
        this.name = name;
    }

    public void setNixbpe(String nixbpe) {
        //this.Snixbpe = nixbpe;
        if(nixbpe != ""){
            this.nixbpe += nixbpe.charAt(0) - '0' * nFlag;
            this.nixbpe += nixbpe.charAt(1) - '0' * iFlag;
            this.nixbpe += nixbpe.charAt(2) - '0' * xFlag;
            this.nixbpe += nixbpe.charAt(3) - '0' * bFlag;
            this.nixbpe += nixbpe.charAt(4) - '0' * pFlag;
            this.nixbpe += nixbpe.charAt(5) - '0' * eFlag;
        }
    }

}

/**
 * 명령어 하나하나의 구체적인 정보는 Instruction클래스에 담긴다.
 * instruction과 관련된 정보들을 저장하고 기초적인 연산을 수행한다.
 */
class InstTable {


    ArrayList<String> lines;
    HashMap<Integer, InstSet> instMap;
    /**
     * 클래스 초기화. 파싱을 동시에 처리한다.
     * @param instFile : instuction에 대한 명세가 저장된 파일 이름
     */
    public InstTable(String instFile) {
        instMap = new HashMap<Integer, InstSet>();
        openFile(instFile);
        setInstruction();
    }

    /**
     * 입력받은 이름의 파일을 열고 해당 내용을 파싱하여 instMap에 저장한다.
     */
    public void openFile(String fileName) {
        FileReader file = null;
        BufferedReader bufReader = null;
        lines = new ArrayList<String>();

        try{
            String path = new File("").getAbsolutePath();
            path = path.concat(fileName);
            file = new FileReader(path);
            bufReader = new BufferedReader(file);
            String line = null;

            while((line = bufReader.readLine()) != null) {
                lines.add(line);
            }

            bufReader.close();
            file.close();

        }catch(FileNotFoundException e) {
            System.out.println("File Not Found");
        }catch(IOException e) {
            System.out.println("File IOException");
        }
    }

    public void setInstruction() {
        InstSet[] readInsts = new InstSet[lines.size()];
        int idx = 0;
        for(String line : lines) {
            readInsts[idx] = new InstSet(line);
            instMap.put(readInsts[idx].opcode, readInsts[idx]);
            idx++;
        }
    }

}

class InstSet{

    String instruction;
    int format;
    int opcode;
    int numberOfOperand;

    public InstSet(String line) {
        instruction = "";
        format = 0;
        opcode = 0;
        numberOfOperand = 0;
        parsing(line);
    }
    /**
     * 일반 문자열을 파싱하여 instruction 정보를 파악하고 저장한다.
     * @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
     */
    public void parsing(String line) {

        StringTokenizer st = new StringTokenizer(line, "\t");
        int count = 0;

        while(st.hasMoreTokens()) {
            count++;

            switch(count) {
                case 1:
                    this.instruction = st.nextToken();
                    break;
                case 2:
                    this.format = Integer.parseInt(st.nextToken());
                    break;
                case 3:
                    this.opcode = Integer.parseInt(st.nextToken(), 16);
                    break;
                case 4:
                    this.numberOfOperand = Integer.parseInt(st.nextToken());
                    break;
                default:
                    System.out.println("error");
                    break;
            }
        }
    }
}




