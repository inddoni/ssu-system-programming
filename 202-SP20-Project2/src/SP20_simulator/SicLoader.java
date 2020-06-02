package SP20_simulator;

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
	
	public SicLoader(ResourceManager resourceManager) {
		// 필요하다면 초기화
		setResourceManager(resourceManager);
	}

	/**
	 * Loader와 프로그램을 적재할 메모리를 연결시킨다.
	 * @param resourceManager
	 */
	public void setResourceManager(ResourceManager resourceManager) {
		this.rMgr=resourceManager;
	}
	
	/**
	 * object code를 읽어서 load과정을 수행한다. load한 데이터는 resourceManager가 관리하는 메모리에 올라가도록 한다.
	 * load과정에서 만들어진 symbol table 등 자료구조 역시 resourceManager에 전달한다.
	 * @param objectCode 읽어들인 파일
	 */
	public void load(File objectCode){
		FileReader file = null;
		BufferedReader bufReader = null;
		ArrayList<String> lines = new ArrayList<String>();

		try{
			file = new FileReader(objectCode);
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

		//lines 파싱해서 resourcemanager setting
		int progCount = 0;
		for(String l : lines){

			if(l.length()>0){
				switch ((l.charAt(0))){
					case 'H': //H line, D line, R line
						StringTokenizer st = new StringTokenizer(l, "\t");
						rMgr.setProgramName(st.nextToken(), progCount);
						String header = st.nextToken();
						rMgr.setpStartAddr(Integer.parseInt(header.substring(0,5)), progCount);
						rMgr.setProgranLength(Integer.parseInt(header.substring(6,11)), progCount);
						progCount++;
						break;
					case 'D':
						int len = l.length()-1;
						int num = 1;
						int loop = len/12;
						for(int i = 0; i < loop; i++){
							rMgr.symtabList.putSymbol(l.substring(num,num+5), Integer.parseInt(l.substring(num+6, num+11)));
							num += 12;
						}
						break;
					case 'T':
						System.out.println("T : "+ l);
						break;
					case 'M':
						System.out.println("M : "+ l);
						break;
					case 'E':
						System.out.println("E : "+ l);
						break;
					default:
						System.out.println("error");
						break;

				}
			}
		}
	};
	
	

}
