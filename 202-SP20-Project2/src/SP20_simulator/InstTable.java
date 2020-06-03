package SP20_simulator;

import java.util.HashMap;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.StringTokenizer;
import java.io.File;

public class InstTable {
    /**
     * inst.data 파일을 불러와 저장하는 공간.
     *  명령어의 이름을 집어넣으면 해당하는 Instruction의 정보들을 리턴할 수 있다.
     */
    HashMap<String, Instruction> instMap;
    ArrayList<String> lines;
    /**
     * 클래스 초기화. 파싱을 동시에 처리한다.
     * @param instFile : instuction에 대한 명세가 저장된 파일 이름
     */
    public InstTable(String instFile) {
        instMap = new HashMap<String, Instruction>();
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
        Instruction[] readInsts = new Instruction[lines.size()];
        int idx = 0;
        for(String line : lines) {
            readInsts[idx] = new Instruction(line);
            instMap.put(readInsts[idx].instruction, readInsts[idx]);
            idx++;
        }

    }

    public int searchFormat(String operName){
        int result = 0;
        if(!operName.equals("") && operName.charAt(0) == '+')
            return 4;
        else {
            if(instMap.containsKey(operName))
                return instMap.get(operName).format;
            else
                return -1;
        }
    }

    public int searchOpcode(String operName){

        if(!operName.equals("")){
            if(operName.charAt(0) == '+'){
                operName = operName.substring(1,operName.length());
                return instMap.get(operName).opcode;
            }
            else{
                if(instMap.containsKey(operName))
                    return instMap.get(operName).opcode;
                else
                    return 0;
            }
        }
        else
            return -1;
    }

}

class Instruction {
    /*
     * 각자의 inst.data 파일에 맞게 저장하는 변수를 선언한다.
     */
    String instruction;
    int format;
    int opcode;
    int numberOfOperand;

    /* bit 조작의 가독성을 위한 선언 */
    public static final int nFlag=32;
    public static final int iFlag=16;
    public static final int xFlag=8;
    public static final int bFlag=4;
    public static final int pFlag=2;
    public static final int eFlag=1;

    /**
     * 클래스를 선언하면서 일반문자열을 즉시 구조에 맞게 파싱한다.
     * @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
     */
    public Instruction(String line) {
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
                    System.out.println("[InstTable.java] parsing() error");
                    break;
            }
        }
    }

    public int getFlag(){
        return 0;
    }

}
