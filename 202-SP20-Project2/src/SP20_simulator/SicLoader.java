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
		System.out.println("welcome to pass 2 load");
		int sectionCount = -1;
		String remain = "";
		for(String[] sec : sectionLine) { //section ��ŭ �ݺ� (�� 3��)
			sectionCount++;
			for (String line : sec) {
				if (line != null) {
					switch (line.charAt(0)) {
						case 'T':
							int startIdx = 9;
							int linelen = (Integer.parseInt(line.substring(7,9), 16))*2;
							int cnt = 0;
							int store = 0;
							//���� ���� ���� ���� �ִ��� Ȯ��
							if(remain.length() > 0){
								int prelen = 8-remain.length(); //line ���ۿ��� �� ��ŭ�� �̸� �� ���̴�.
								String temp = remain + line.substring(startIdx, startIdx+prelen);
								rMgr.setMemory(temp);
								startIdx += prelen;
								cnt = (linelen-prelen) / 8;
								store = (linelen-prelen) % 8;
								remain = "";
							}
							else {
								cnt = linelen / 8;
								store = linelen % 8;
							}

							for(int i = 0; i < cnt; i++){
								rMgr.setMemory(line.substring(startIdx, startIdx+8));
								startIdx += 8;
							}
							if(store > 0){
								remain = line.substring(startIdx,startIdx+store);
							}
							break;
						case 'M':
							//T ���� ó�� ���� ������ �ڵ� �޸𸮿� �߰�
							if(remain.length() > 0 && sectionCount == 2){
								rMgr.setMemory(remain);
								remain = "";
							}
							//substring : memory address, modify length, sign, symbolname
							int addr = Integer.parseInt(line.substring(1,7));
							int mLen = Integer.parseInt(line.substring(7,9));
							char sign = line.charAt(9);
							String sym = line.substring(10);

							//memory modify
							int mLoc = addr / 4;
							int mSubLoc = (addr % 4) * 2;
							if(mLen == 5)
								mSubLoc += 1;
							String s = rMgr.getMemory(mLoc, mSubLoc);
							int result = Integer.parseInt(s.substring(0,mLen));
							result += rMgr.symtabList.search(sym);
							String re = String.format("%05d", result);
							rMgr.setMemory(mLoc, re, mSubLoc);
							break;
						case 'E':
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
