package SP20_simulator;

import sun.jvm.hotspot.jdi.ArrayReferenceImpl;

import java.io.*;
import java.util.ArrayList;
import java.util.StringTokenizer;

/**
 * SicLoader�� ���α׷��� �ؼ��ؼ� �޸𸮿� �ø��� ������ �����Ѵ�. �� �������� linker�� ���� ���� �����Ѵ�. 
 * <br><br>
 * SicLoader�� �����ϴ� ���� ���� ��� ������ ����.<br>
 * - program code�� �޸𸮿� �����Ű��<br>
 * - �־��� ������ŭ �޸𸮿� �� ���� �Ҵ��ϱ�<br>
 * - �������� �߻��ϴ� symbol, ���α׷� �����ּ�, control section �� ������ ���� ���� ���� �� ����
 */
public class SicLoader {
	ResourceManager rMgr;
	ArrayList<String[]> sectionLine; //�� section ���� ���� ����

	//constructor
	public SicLoader(ResourceManager resourceManager) {
		// �ʿ��ϴٸ� �ʱ�ȭ
		setResourceManager(resourceManager);
		sectionLine = new ArrayList<>();
	}

	/**
	 * Loader�� ���α׷��� ������ �޸𸮸� �����Ų��.
	 * @param resourceManager
	 */
	public void setResourceManager(ResourceManager resourceManager) {
		//resourceManamter ����
		this.rMgr=resourceManager;
	}


	/**
	 * 2 pass�� �����ϴ� linking loader�� pass 1 ����
	 *
	 */
	public void loadPass1(){
		//lines �Ľ��ؼ� resourcemanager setting
		int sectionCount = -1;
		String remain = "";
		for(String[] sec : sectionLine){ //section ��ŭ �ݺ� (�� 3��)
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
							int len = line.length()-1; //'D' ���� ����(���̳ʽ� 1 ����)
							int num = 1; //substring�� �� ����� ����
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
		for(String[] sec : sectionLine) { //section ��ŭ �ݺ� (�� 3��)
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
							//���α׷� ó����ġ�� �̵��ؼ� ����
							break;
						default:
							break;
					}
				}
			}
		}
	}
	/**
	 * object code�� �о load������ �����Ѵ�. load�� �����ʹ� resourceManager�� �����ϴ� �޸𸮿� �ö󰡵��� �Ѵ�.
	 * load�������� ������� symbol table �� �ڷᱸ�� ���� resourceManager�� �����Ѵ�.
	 * @param objectCode �о���� ����
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
