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
                    System.out.println(temp);
                    instruction.get(count).instruction = temp;
                    index += instruction.get(count).format;

                    if(instruction.get(count).format >= 3)
                        instruction.get(count).setDisp(instList.get(count).substring(3));

                }

                //count 값 증가
                count++;
            }
        }

    }

    public ArrayList<String> getAll(){
        return instList;
    }
    public int instTotal(){
        return instList.size();
    }
    public String getOne(int index){
        return instList.get(index);
    }

    // instruction 별로 동작을 수행하는 메소드를 정의
    // ex) public void add(){...}
    public void LDA(Instruction instruction){

    }

    public void STL(Instruction instruction){
        //L register 값을 가져온다.
        int value = rMgr.getRegister(2);
        //메모리 해당부분에 저장한다.
        int addr = instruction.getDisp();
        rMgr.setMemory(addr, Integer.toString(value), 3);

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

    public Instruction(){
        instruction = "";
        opcode = 0;
        Snixbpe = "";
        nixbpe = 0;
        disp = 0;
        format = 0;
        name = "";
    }
    public void setDisp(String s){
        int value = Integer.parseInt(s,16);;
        if(s.charAt(0) == 'F') //음수처리
            value = value - 4096; // 4096 = FFF + 1을 십진수로 변환한 값
        this.disp = value;
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
        this.Snixbpe = nixbpe;
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




