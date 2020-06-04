package SP20_simulator;

import javax.swing.*;
import java.io.File;

/**
 * �ùķ����ͷμ��� �۾��� ����Ѵ�. VisualSimulator���� ������� ��û�� ������ �̿� ����
 * ResourceManager�� �����Ͽ� �۾��� �����Ѵ�.  
 * 
 * �ۼ����� ���ǻ��� : <br>
 *  1) ���ο� Ŭ����, ���ο� ����, ���ο� �Լ� ������ �󸶵��� ����. ��, ������ ������ �Լ����� �����ϰų� ������ ��ü�ϴ� ���� ������ ��.<br>
 *  2) �ʿ信 ���� ����ó��, �������̽� �Ǵ� ��� ��� ���� ����.<br>
 *  3) ��� void Ÿ���� ���ϰ��� ������ �ʿ信 ���� �ٸ� ���� Ÿ������ ���� ����.<br>
 *  4) ����, �Ǵ� �ܼ�â�� �ѱ��� ��½�Ű�� �� ��. (ä������ ����. �ּ��� ���Ե� �ѱ��� ��� ����)<br>
 * 
 * <br><br>
 *  + �����ϴ� ���α׷� ������ ��������� �����ϰ� ���� �е��� ������ ��� �޺κп� ÷�� �ٶ��ϴ�. ���뿡 ���� �������� ���� �� �ֽ��ϴ�.
 */
public class SicSimulator {
	ResourceManager rMgr;
    VisualSimulator vSim;
	public SicSimulator(ResourceManager resourceManager, VisualSimulator visualSimulator) {
		// �ʿ��ϴٸ� �ʱ�ȭ ���� �߰�
		this.rMgr = resourceManager;
		this.vSim = visualSimulator;
	}

	/**
	 * ��������, �޸� �ʱ�ȭ �� ���α׷� load�� ���õ� �۾� ����.
	 * ��, object code�� �޸� ���� �� �ؼ��� SicLoader���� �����ϵ��� �Ѵ�. 
	 */
	public void load(File program) {
		/* �޸� �ʱ�ȭ, �������� �ʱ�ȭ ��*/
		//�������� �ʱ�ȭ
		rMgr.initializeResource();
	}

	/**
	 * 1���� instruction�� ����� ����� ���δ�. 
	 */
	public void oneStep(String inst) {


        //1. PC register setting
		//rMgr.setRegister(8, inst.length()/2);

		//2. log textarea setting
		addLog(inst);
		//3. instruction �ؼ�
		String name = vSim.instLuncher.searchName(inst);
		Instruction instruction = vSim.instLuncher.searchInst(inst);
		if(name.equals("STL"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("JSUB"))
			vSim.instLuncher.JSUB(instruction);
		else if(name.equals("LDA"))
			vSim.instLuncher.LDA(instruction);
		else if(name.equals("COMP"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("JEQ"))
			vSim.instLuncher.JEQ(instruction);
		else if(name.equals("J"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("STA"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("CLEAR"))
			vSim.instLuncher.CLEAR(instruction);
		else if(name.equals("LDT"))
			vSim.instLuncher.LDT(instruction);
		else if(name.equals("TD"))
			vSim.instLuncher.TD(instruction);
		else if(name.equals("RD"))
			vSim.instLuncher.RD(instruction);
		else if(name.equals("COMPR"))
			vSim.instLuncher.COMPR(instruction);
		else if(name.equals("STCH"))
			vSim.instLuncher.STCH(instruction);
		else if(name.equals("TIXR"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("JLT"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("STX"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("RSUB"))
			vSim.instLuncher.STL(instruction);
		else if(name.equals("LDCH"))
			vSim.instLuncher.STL(instruction);
		else
			System.out.println("undefine instruction");

		// �� �������� �� �ʿ��� �������� ���� �����ȴ�.

	}


	/**
	 * ���� ��� instruction�� ����� ����� ���δ�.
	 */
	public void allStep() {

	}
	
	/**
	 * �� �ܰ踦 ������ �� ���� ���õ� ����� ���⵵�� �Ѵ�.
	 */
	public void addLog(String inst) {
		if(inst.length() != 0){
			String log = vSim.instLuncher.searchName(inst);
			vSim.setlogList(log);
		}
	}	
}
