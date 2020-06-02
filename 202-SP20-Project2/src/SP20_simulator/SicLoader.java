package SP20_simulator;

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
	
	public SicLoader(ResourceManager resourceManager) {
		// �ʿ��ϴٸ� �ʱ�ȭ
		setResourceManager(resourceManager);
	}

	/**
	 * Loader�� ���α׷��� ������ �޸𸮸� �����Ų��.
	 * @param resourceManager
	 */
	public void setResourceManager(ResourceManager resourceManager) {
		this.rMgr=resourceManager;
	}
	
	/**
	 * object code�� �о load������ �����Ѵ�. load�� �����ʹ� resourceManager�� �����ϴ� �޸𸮿� �ö󰡵��� �Ѵ�.
	 * load�������� ������� symbol table �� �ڷᱸ�� ���� resourceManager�� �����Ѵ�.
	 * @param objectCode �о���� ����
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

		//lines �Ľ��ؼ� resourcemanager setting
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
