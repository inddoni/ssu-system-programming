package SP20_simulator;

import java.io.*;
import java.util.*;


/**
 * ResourceManager�� ��ǻ���� ���� ���ҽ����� �����ϰ� �����ϴ� Ŭ�����̴�.
 * ũ�� �װ����� ���� �ڿ� ������ �����ϰ�, �̸� ������ �� �ִ� �Լ����� �����Ѵ�.<br><br>
 * 
 * 1) ������� ���� �ܺ� ��ġ �Ǵ� device<br>
 * 2) ���α׷� �ε� �� ������ ���� �޸� ����. ���⼭�� 64KB�� �ִ밪���� ��´�.<br>
 * 3) ������ �����ϴµ� ����ϴ� �������� ����.<br>
 * 4) SYMTAB �� simulator�� ���� �������� ���Ǵ� �����͵��� ���� ������. 
 * <br><br>
 * 2���� simulator������ ����Ǵ� ���α׷��� ���� �޸𸮰����� �ݸ�,
 * 4���� simulator�� ������ ���� �޸� �����̶�� ������ ���̰� �ִ�.
 */
public class ResourceManager{
	/**
	 * ����̽��� ���� ����� ��ġ���� �ǹ� ������ ���⼭�� ���Ϸ� ����̽��� ��ü�Ѵ�.<br>
	 * ��, 'F1'�̶�� ����̽��� 'F1'�̶�� �̸��� ������ �ǹ��Ѵ�. <br>
	 * deviceManager�� ����̽��� �̸��� �Է¹޾��� �� �ش� �̸��� ���� ����� ���� Ŭ������ �����ϴ� ������ �Ѵ�.
	 * ���� ���, 'A1'�̶�� ����̽����� ������ read���� ������ ���, hashMap�� <"A1", scanner(A1)> ���� �������μ� �̸� ������ �� �ִ�.
	 * <br><br>
	 * ������ ���·� ����ϴ� �� ���� ����Ѵ�.<br>
	 * ���� ��� key������ String��� Integer�� ����� �� �ִ�.
	 * ���� ������� ���� ����ϴ� stream ���� �������� ����, �����Ѵ�.
	 * <br><br>
	 * �̰͵� �����ϸ� �˾Ƽ� �����ؼ� ����ص� �������ϴ�.
	 */
	HashMap<String,Object> deviceManager = new HashMap<String,Object>();
	String[] memory = new String[65536]; // String���� �����ؼ� ����Ͽ��� ������.
	int[] register = new int[10];
	double register_F;
	String nowDev = "";

	static int endRecord = 0;
	ArrayList<String> pSectionName; //�� section�� �̸�
	ArrayList<Integer> pSectionLength; //�� section�� ��
	ArrayList<Integer> pSectionAddr;//�� section�� ���� �ּ�
	static int memoryLoc = 0; //memory�� index
	static int totalProgramAddr = 0; //��ü ���α׷��� ���� �ּ�
	SymbolTable symtabList;
	// �̿ܿ��� �ʿ��� ���� �����ؼ� ����� ��.

	public ResourceManager(){
		pSectionName = new ArrayList<String>();
		pSectionLength = new ArrayList<Integer>();
		pSectionAddr = new ArrayList<Integer>();
		symtabList = new SymbolTable();
	}

	/**
	 * �޸�, �������͵� ���� ���ҽ����� �ʱ�ȭ�Ѵ�.
	 */
	public void initializeResource(){
		//init start adderss in memory
		totalProgramAddr = 0;
		//init 10 register
		for(int i = 0; i < register.length; i++){
			register[i] = 0;
		}
		register_F = 0.0;
		//Arrays.fill(memory,"");

	}
	
	/**
	 * deviceManager�� �����ϰ� �ִ� ���� ����� stream���� ���� �����Ű�� ����.
	 * ���α׷��� �����ϰų� ������ ���� �� ȣ���Ѵ�.
	 */
	public void closeDevice() {
		for(String s : deviceManager.keySet()){
			Scanner sc = (Scanner)deviceManager.get(s);
			sc.close();
		}
	}
	
	/**
	 * ����̽��� ����� �� �ִ� ��Ȳ���� üũ. TD��ɾ ������� �� ȣ��Ǵ� �Լ�.
	 * ����� stream�� ���� deviceManager�� ���� ������Ų��.
	 * @param devName Ȯ���ϰ��� �ϴ� ����̽��� ��ȣ,�Ǵ� �̸�
	 */
	public void testDevice(String devName) {
		try{
			File file = new File(devName+".txt");
			nowDev = devName;
			Scanner scanner = new Scanner(file);
			deviceManager.put(devName,scanner);
			setRegister(9, 1); //���� flag ����
		} catch(FileNotFoundException e){
			setRegister(9, 0); //������ ���� flag
			e.printStackTrace();
		} catch (Exception e){
			setRegister(9, 0); //������ ���� flag
			e.printStackTrace();
		}

	}

	/**
	 * ����̽��κ��� ���ϴ� ������ŭ�� ���ڸ� �о���δ�. RD��ɾ ������� �� ȣ��Ǵ� �Լ�.
	 * @param devName ����̽��� �̸�
	 * @param num �������� ������ ����
	 * @return ������ ������
	 */
	public char[] readDevice(String devName, int num){
		char[] data = new char[100];
		try{
			Scanner sc = (Scanner)deviceManager.get(devName);
			nowDev = devName;
			String line = "";
			if(sc.hasNext()){
				line = sc.nextLine();
				for(int i = 0; i < num; i++){
					data[i] += line.charAt(i);
				}
			}

			//file �� ������ŭ ����
			String remain = "";
			Boolean re = false;
			for(int i = 0; i < line.length(); i++){
				re = true;
				if(data[i] == line.charAt(i)){
					continue;
				}
				remain += line.substring(i,i+1);
			}

			if(re){
				try{
					String fName = devName+".txt";
					File file = new File(fName);
					FileWriter writer = new FileWriter(file, false);
					writer.write(remain);
					writer.close();
				}catch (FileNotFoundException e){
					e.printStackTrace();
				}
			}
		}catch (Exception e){
			e.printStackTrace();
		}
		return data;
	}

	/**
	 * ����̽��� ���ϴ� ���� ��ŭ�� ���ڸ� ����Ѵ�. WD��ɾ ������� �� ȣ��Ǵ� �Լ�.
	 * @param devName ����̽��� �̸�
	 * @param data ������ ������
	 * @param num ������ ������ ����
	 */
	public void writeDevice(String devName, char data, int num){
		try{
			Scanner sc = (Scanner)deviceManager.get(devName);
			String fName = devName+".txt";
			File file = new File(fName);
			FileWriter writer = new FileWriter(file, true);

			nowDev = devName;

			writer.write(data);

			writer.close();
		}catch (Exception e){
			e.printStackTrace();
		}
	}
	
	/**
	 * �޸��� Ư�� ��ġ���� ���ϴ� ������ŭ�� ���ڸ� �����´�.
	 * @param location �޸� ���� ��ġ �ε���
	 * @param num ������ ����
	 * @return �������� ������
	 */
	public String getMemory(int location, int num){
		String getM = "";
		for(int i = 0; i < num; i++){
			getM += memory[location+i];
		}
		return getM;
	}

	/**
	 * �޸��� Ư�� ��ġ�� ���ϴ� ������ŭ�� �����͸� �����Ѵ�. 
	 * @param locate ���� ��ġ �ε���
	 * @param data �����Ϸ��� ������
	 * @param num �����ϴ� �������� ����
	 */
	public void setMemory(int locate, String data, int num){
		char sign = data.charAt(0);
		data = data.substring(1);

		String temp = "";
		for(int i = 0; i < 3; i++)
			temp += memory[locate+i];

		if (sign == '+'){
			if(num == 5)
				temp = temp.substring(0,1) + data;
			else
				temp = "0"+data;
		}
		else{
			int value1 = Integer.parseInt(temp, 16);
			int value2 = Integer.parseInt(data, 16);
			int result = value1 - value2;
			temp = String.format("%06x", result);
		}

		//set replace memory
		for(int i = 0; i < 3; i++)
			memory[locate+i] = temp.substring(i*2,(i*2)+2);

	}

	/** ==> ����Ҷ� �� �޸� ���� �Լ�
	 * �޸��� Ư�� ��ġ�� ���ϴ� ������ŭ�� �����͸� �����Ѵ�.
	 * @param locate ���� ��ġ �ε���
	 * @param data �����Ϸ��� ������
	 * @param num �����ϴ� �������� ����
	 */
	public void setMemoryCal(int locate, String data, int num){

		int value = Integer.parseInt(data);//���� ������ ���� 10����
		String store = "";
		if(num == 3)
			store=String.format("%06x", value);
		else if(num==1)
			store=String.format("%02x", value);
		//set replace memory
		for(int i = 0; i < num; i++)
			memory[locate+i] = store.substring(i*2,(i*2)+2);

	}

	public void setMemory(int locate, String data){
		memory[locate] = data;

	}

	public void setMemoryLoc(int memoryLoc) {
		this.memoryLoc = memoryLoc;
	}

	/**
	 * ��ȣ�� �ش��ϴ� �������Ͱ� ���� ��� �ִ� ���� �����Ѵ�. �������Ͱ� ��� �ִ� ���� ���ڿ��� �ƴԿ� �����Ѵ�.
	 * @param regNum �������� �з���ȣ
	 * @return �������Ͱ� ������ ��
	 */
	public int getRegister(int regNum){
		return register[regNum];
	}

	/**
	 * ��ȣ�� �ش��ϴ� �������Ϳ� ���ο� ���� �Է��Ѵ�. �������Ͱ� ��� �ִ� ���� ���ڿ��� �ƴԿ� �����Ѵ�.
	 * @param regNum ���������� �з���ȣ
	 * @param value �������Ϳ� ����ִ� ��
	 */
	public void setRegister(int regNum, int value){
//		if(regNum == 8) //PCregister => ������
//			register[regNum] += value;
//		else
			register[regNum] = value;
	}

	/**
	 * �ַ� �������Ϳ� �޸𸮰��� ������ ��ȯ���� ���ȴ�. int���� char[]���·� �����Ѵ�.
	 * @param data
	 * @return
	 */
	public char[] intToChar(int data){
		return null;
	}

	/**
	 * �ַ� �������Ϳ� �޸𸮰��� ������ ��ȯ���� ���ȴ�. char[]���� int���·� �����Ѵ�.
	 * @param data
	 * @return
	 */
	public int byteToInt(byte[] data){
		return 0;
	}

	public void setProgramName(String s){
		pSectionName.add(s);
	}

	public void setpStartAddr(int i){
		if(pSectionAddr.size() > 0){
			int addr = i + pSectionAddr.get(pSectionAddr.size()-1) + pSectionLength.get(pSectionAddr.size()-1);
			pSectionAddr.add(addr);
		}
		else {
			pSectionAddr.add(i);
		}

		totalProgramAddr = getSectionAddr(pSectionAddr.size()-1) + getProgramLength(pSectionAddr.size()-1);
	}

	public int getSectionAddr(int index){
		return pSectionAddr.get(index);
	}

	public void setProgramLength(int i){
		pSectionLength.add(i);
	}
	public int getProgramLength(int index){
		return pSectionLength.get(index);
	}
}