package SP20_simulator;

import sun.jvm.hotspot.jdi.ArrayReferenceImpl;

import java.io.*;
import java.util.ArrayList;
import java.util.StringTokenizer;

/**
 * SicLoader는 프로그램을 해석해서 메모리에 올리는 역할을 수행한다. 이 과정에서 linker의 역할 또한 수행한다. 
 * <br><br>
 * SicLoader가 수행하는 일을 예를 들면 다음과 같다.<br>
 * - program code를 메모리에 적재시키기<br>
 * - 주어진 공간만큼 메모리에 빈 공간 할당하기<br>
 * - 과정에서 발생하는 symbol, 프로그램 시작주소, control section 등 실행을 위한 정보 생성 및 관리
 */
public class SicLoader {
	ResourceManager rMgr;
	ArrayList<String[]> sectionLine; //각 section 별로 라인 저장

	//constructor
	public SicLoader(ResourceManager resourceManager) {
		// 필요하다면 초기화
		setResourceManager(resourceManager);
		sectionLine = new ArrayList<>();
	}

	/**
	 * Loader와 프로그램을 적재할 메모리를 연결시킨다.
	 * @param resourceManager
	 */
	public void setResourceManager(ResourceManager resourceManager) {
		//resourceManamter 연결
		this.rMgr=resourceManager;
	}


	/**
	 * 2 pass로 동작하는 linking loader의 pass 1 과정
	 *
	 */
	public void loadPass1(){
		//lines 파싱해서 resourcemanager setting
		int sectionCount = -1;
		String remain = "";
		for(String[] sec : sectionLine){ //section 만큼 반복 (총 3번)
			sectionCount++;
			for(String line : sec){
				if(line != null){
					switch (line.charAt(0)){
						case 'H':
							StringTokenizer st = new StringTokenizer(line, "\t");
							String secName = st.nextToken().substring(1);
							String header = st.nextToken();

							rMgr.setProgramName(secName);
							rMgr.setProgramLength(Integer.parseInt(header.substring(6,12), 16));
							rMgr.setpStartAddr(Integer.parseInt(header.substring(0,5), 16));
							rMgr.symtabList.putSymbol(secName, rMgr.getSectionAddr(sectionCount), sectionCount);
							break;
						case 'D':
							int len = line.length()-1; //'D' 글자 빼기(마이너스 1 해줌)
							int num = 1; //substring할 때 사용할 변수
							int loop = len/12;
							for(int i = 0; i < loop; i++){
								int symAddr = 0;
								//param : symbol name, symbol addr, section number
								if(sectionCount >= 1)
									symAddr = rMgr.getSectionAddr(sectionCount);
								symAddr += Integer.parseInt(line.substring(num+6, num+12), 16); //7~13 => 7~12  1~7 => 1~6
								rMgr.symtabList.putSymbol(line.substring(num,num+6), symAddr, sectionCount);
								num += 12;
							}
							break;
						default:
							break;
					}
				}
			}
		}
	}

	public void loadPass2(){

		int sectionCount = -1;
		int memoryLen = 0;
		int sectionMemLen = 0;
		for(String[] sec : sectionLine) { //section 만큼 반복 (총 3번)
			sectionCount++;
			sectionMemLen = memoryLen;
			for (String line : sec) {
				if (line != null) {
					switch (line.charAt(0)) {
						case 'T':
							int startIdx = 9;
							int startAddr = Integer.parseInt(line.substring(1,7), 16)+rMgr.getSectionAddr(sectionCount);
							int linelen = Integer.parseInt(line.substring(7,9), 16);
							memoryLen += linelen;
							for(int i = 0; i < linelen; i++){
								rMgr.setMemory(startAddr, line.substring(startIdx, startIdx+2));
								startIdx += 2;
								startAddr += 1;
							}
							break;
						case 'M':
							int addr = rMgr.getSectionAddr(sectionCount) + Integer.parseInt(line.substring(1,7), 16);
							int mLen = Integer.parseInt(line.substring(7,9));
							char sign = line.charAt(9);
							String sym = line.substring(10);
							sym = String.format("%05x", rMgr.symtabList.search(sym));
							sym = Character.toString(sign) + sym;
							rMgr.setMemory(addr,sym,mLen);
							break;
						case 'E':
							rMgr.setMemoryLoc(rMgr.getSectionAddr(sectionCount)+rMgr.getProgramLength(sectionCount));
							//프로그램 처음위치로 이동해서 실행
							break;
						default:
							break;
					}
				}
			}
		}
	}
	/**
	 * object code를 읽어서 load과정을 수행한다. load한 데이터는 resourceManager가 관리하는 메모리에 올라가도록 한다.
	 * load과정에서 만들어진 symbol table 등 자료구조 역시 resourceManager에 전달한다.
	 * @param objectCode 읽어들인 파일
	 */
	public void load(File objectCode){
		FileReader file = null;
		BufferedReader bufReader = null;

		try{
			file = new FileReader(objectCode);
			bufReader = new BufferedReader(file);
			String[] lines = new String[15];
			String line;
			int lineIdx = 0;
			while((line = bufReader.readLine()) != null) {
				if(line.length() > 0){
					if(line.charAt(0) != 'E'){
						lines[lineIdx] = line;
						lineIdx++;
					}
					else{
						lines[lineIdx] = line;
						lineIdx = 0;
						sectionLine.add(lines);
						lines = new String[15];
					}
				}
			}

			bufReader.close();
			file.close();

		}catch(FileNotFoundException e) {
			System.out.println("File Not Found");
		}catch(IOException e) {
			System.out.println("File IOException");
		}

		loadPass1();
		loadPass2();

	}
	
	

}
