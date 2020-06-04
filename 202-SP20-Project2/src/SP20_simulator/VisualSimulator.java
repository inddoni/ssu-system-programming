package SP20_simulator;

import com.sun.deploy.panel.ControlPanel;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

//exception handling
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;


/**
 * VisualSimulator는 사용자와의 상호작용을 담당한다.<br>
 * 즉, 버튼 클릭등의 이벤트를 전달하고 그에 따른 결과값을 화면에 업데이트 하는 역할을 수행한다.<br>
 * 실제적인 작업은 SicSimulator에서 수행하도록 구현한다.
 */
public class VisualSimulator extends JFrame {

	ResourceManager resourceManager = new ResourceManager();
	InstTable instTable = new InstTable("/inst.txt");
	SicLoader sicLoader = new SicLoader(resourceManager);
	SicSimulator sicSimulator = new SicSimulator(resourceManager, this);
	InstLuncher instLuncher;
	public boolean isInstLoad = false;
	public int totalList;
	public int nowStep;

	public JButton fopenBtn = new JButton("open"); //file open button
	public JButton oneexe = new JButton("execute(1step)");
	public JButton allexe =new JButton("execute(all)");
	public JButton exit =new JButton("exit");
	public JTextField fName = new JTextField(15); //file name user input
	public JTextField pn2Text; //GUI program Name link @pn2Text
	public JTextField pn3Text; //GUI program Start Address link @pn3Text
	public JTextField pn4Text; //GUI program length link @pn4Text
	public JTextField pnAText1; // register
	public JTextField pnAText2;
	public JTextField pnXText1;
	public JTextField pnXText2;
	public JTextField pnLText1;
	public JTextField pnLText2;
	public JTextField pnBText1;
	public JTextField pnBText2;
	public JTextField pnSText1;
	public JTextField pnSText2;
	public JTextField pnTText1;
	public JTextField pnTText2;
	public JTextField pnFText1;
	public JTextField pnPCText1;
	public JTextField pnPCText2;
	public JTextField pnSWText1;
	public JTextField pn7Text; //end record
	public JTextField pn8Text; //memory start address
	public JTextField pn9Text; //target address
	public JTextArea ta; //instructions
	public JTextField pnDivText; //device
	public JTextArea ta2; //log area



	/**
	 * 프로그램 로드 명령을 전달한다.
	 */
	public void load(File program){
		//...
		sicLoader.load(program);
		sicSimulator.load(program);
	};

	/**
	 * 하나의 명령어만 수행할 것을 SicSimulator에 요청한다.
	 */
	public void oneStep(){
		if(isInstLoad == false){
			instLuncher = new InstLuncher(resourceManager, instTable);
			instLuncher.setInstruction();
			isInstLoad = true;
			totalList = instLuncher.instTotal();
			nowStep = 0;
		}
		if(nowStep < instLuncher.instList.size()){
			String inst = instLuncher.getOne(nowStep);
			nowStep++;
			ta.append(inst + "\n");
			ta.setCaretPosition(ta.getDocument().getLength());

			//insturction 수행 요청 => sic Simulator
			sicSimulator.oneStep(inst);
		}

	};

	/**
	 * 남아있는 모든 명령어를 수행할 것을 SicSimulator에 요청한다.
	 */
	public void allStep(){

		if(isInstLoad == false){
			instLuncher = new InstLuncher(resourceManager, instTable);
			instLuncher.setInstruction();
			isInstLoad = true;
			totalList = instLuncher.instTotal();
			oneexe.setEnabled(false);
		}

		if(nowStep < instLuncher.instList.size()){
			ArrayList<String> getAllinst = instLuncher.getAll();
			for(int i = nowStep; i < instLuncher.instList.size(); i++){
				String s = instLuncher.getOne(i);
				ta.append(s + "\n");
				ta.setCaretPosition(ta.getDocument().getLength());
			}
			nowStep = instLuncher.instList.size();
		}

	};

	/**
	 * 화면을 최신값으로 갱신하는 역할을 수행한다.
	 */
	public void update(){
		setinstList();
		setHeader();
		setEnd();
		setAreg();
		setXreg();
		setLreg();
		setBreg();
		setSreg();
		setTreg();
		setTreg();
		setFreg();
		setPCreg();
		setSWreg();
	};


	public VisualSimulator(){
		super("Simulator - MadeByInjeong");
		this.init();
		this.setSize(560,640);
		this.setVisible(true);
	}

	//이벤트 정의!!!!!
	ActionListener listener = new ActionListener() {
		@Override
		public void actionPerformed(ActionEvent e) {
			JFileChooser chooser = new JFileChooser();
			chooser.setFileFilter(new FileNameExtensionFilter("txt", "txt"));
			chooser.setMultiSelectionEnabled(false); //파일 다중 선택 불가
			int ret = chooser.showOpenDialog(null);
			if(ret == JFileChooser.APPROVE_OPTION){
				try{
					String fileName = chooser.getSelectedFile().getName();
					fName.setText(fileName);
					load(chooser.getSelectedFile());
					update();
				} catch(Exception ex){
					ex.printStackTrace();
				}
			}
		}
	};

	ActionListener oneE = new ActionListener() {
		@Override
		public void actionPerformed(ActionEvent e) {
			//System.out.println("one step");
			oneStep();
		}
	};

	ActionListener allE = new ActionListener() {
		@Override
		public void actionPerformed(ActionEvent e) {
			//System.out.println("all");
			allStep();
		}
	};

	ActionListener endlistener = new ActionListener() {
		@Override
		public void actionPerformed(ActionEvent e) {
			System.exit(0);
		}
	};




	public void setTargetAddr(int addr){
		pn9Text.setText(Integer.toString(addr)); //target address
	}
	public void setinstList(String inst){
		//ta.append(inst + "\n");
		//ta.setCaretPosition(ta.getDocument().getLength());
	}
	public void setlogList(String log){
		ta2.append(log + "\n");
		ta2.setCaretPosition(ta.getDocument().getLength());
	}
	public void setDevice(String name){
		pnDivText.setText(name);
	}
	public void setHeader(String n, int addr, int len){
		pn2Text.setText(n); //program name
		pn3Text.setText(Integer.toString(addr)); //GUI program Start Address link @pn3Text
		pn4Text.setText(Integer.toString(len)); //GUI program length link @pn4Text
	}
	public void setEnd(int endrec, int mstart){
		pn7Text.setText(Integer.toString(endrec)); //end record
		pn8Text.setText(Integer.toString(mstart)); //memory start address
	}
	public void setAreg(int num){
		pnAText1.setText(Integer.toString(num));
		pnAText2.setText(String.format("%X", num));
	}
	public void setXreg(int num){
		pnXText1.setText(Integer.toString(num));
		pnXText2.setText(String.format("%X", num));
	}
	public void setLreg(int num){
		pnLText1.setText(Integer.toString(num));
		pnLText2.setText(String.format("%X", num));
	}
	public void setBreg(int num){
		pnBText1.setText(Integer.toString(num));
		pnBText2.setText(String.format("%X", num));
	}
	public void setSreg(int num){
		pnSText1.setText(Integer.toString(num));
		pnSText2.setText(String.format("%X", num));
	}
	public void setTreg(int num){
		pnTText1.setText(Integer.toString(num));
		pnTText2.setText(String.format("%X", num));
	}
	public void setFreg(int num){

		pnFText1.setText(Integer.toString(num));

	}
	public void setPCreg(int num){
		pnPCText1.setText(Integer.toString(num));
		pnPCText2.setText(String.format("%X", num));
	}
	public void setSWreg(int num){
		pnSWText1.setText(Integer.toString(num));
	}



	public void setinstList(){
		//ta.append(resourceManager.getMemory(0,3) + "\n");
		//ta.setCaretPosition(ta.getDocument().getLength());
	}

	public void setHeader(){
		pn2Text.setText(resourceManager.pSectionName.get(0)); //program name
		pn3Text.setText(String.format("%06d",resourceManager.pSectionAddr.get(0))); //GUI program Start Address link @pn3Text
		pn4Text.setText(String.format("%X",resourceManager.pSectionLength.get(0))); //GUI program length link @pn4Text
	}
	public void setEnd(){
		pn7Text.setText(String.format("%06d",resourceManager.endRecord)); //end record
		pn8Text.setText(Integer.toString(0)); //memory start address
	}
	public void setAreg(){
		pnAText1.setText(Integer.toString(resourceManager.register[0]));
		pnAText2.setText(String.format("%X", resourceManager.register[0]));
	}
	public void setXreg(){
		pnXText1.setText(Integer.toString(resourceManager.register[1]));
		pnXText2.setText(String.format("%X", resourceManager.register[1]));
	}
	public void setLreg(){
		pnLText1.setText(Integer.toString(resourceManager.register[2]));
		pnLText2.setText(String.format("%X", resourceManager.register[2]));
	}
	public void setBreg(){
		pnBText1.setText(Integer.toString(resourceManager.register[3]));
		pnBText2.setText(String.format("%X", resourceManager.register[3]));
	}
	public void setSreg(){
		pnSText1.setText(Integer.toString(resourceManager.register[4]));
		pnSText2.setText(String.format("%X", resourceManager.register[4]));
	}
	public void setTreg(){
		pnTText1.setText(Integer.toString(resourceManager.register[5]));
		pnTText2.setText(String.format("%X", resourceManager.register[5]));
	}
	public void setFreg(){
		pnFText1.setText(Double.toString(resourceManager.register_F));
	}
	public void setPCreg(){
		pnPCText1.setText(Integer.toString(resourceManager.register[8]));
		pnPCText2.setText(String.format("%X", resourceManager.register[8]));
	}
	public void setSWreg(){
		pnSWText1.setText(Integer.toString(resourceManager.register[9]));
	}



	public void init(){
		getContentPane().setLayout(new FlowLayout());

		//File Name
		JPanel panel1 = new JPanel();
		panel1.add(new JLabel("FileName : "));
		panel1.add(fName);
		panel1.add(fopenBtn);
		fopenBtn.addActionListener(listener);

		//Header Record
		JPanel panel2 = new JPanel();
		JPanel panel3 = new JPanel();
		JPanel panel4 = new JPanel();
		JPanel panelHeader = new JPanel();
		panel2.setLayout(new BoxLayout(panel2, BoxLayout.X_AXIS));
		panel2.add(new JLabel("Program name : "));
		pn2Text = new JTextField(10);
		pn2Text.setEnabled(false);
		panel2.add(pn2Text);
		panel3.setLayout(new BoxLayout(panel3, BoxLayout.X_AXIS));
		panel3.add(new JLabel("<html>Start Address of<br>Object Program : </html>"));
		pn3Text = new JTextField(10);
		pn3Text.setEnabled(false);
		panel3.add(pn3Text);
		panel4.setLayout(new BoxLayout(panel4, BoxLayout.X_AXIS));
		panel4.add(new JLabel("Length of Program : "));
		pn4Text = new JTextField(10);
		pn4Text.setEnabled(false);
		panel4.add(pn4Text);
		//Header Record Group
		panelHeader.setLayout(new BoxLayout(panelHeader,BoxLayout.Y_AXIS));
		panelHeader.setBorder(new TitledBorder(null, "H(Header Record)", TitledBorder.LEADING,
				TitledBorder.TOP, null, null));
		panelHeader.setBounds(12, 26, 300, 62);
		panelHeader.add(panel2);
		panelHeader.add(panel3);
		panelHeader.add(panel4);


		//Register
		JPanel panel6 = new JPanel();
		JPanel panelA = new JPanel();
		JPanel panelX = new JPanel();
		JPanel panelL = new JPanel();
		JPanel panelB = new JPanel();
		JPanel panelS = new JPanel();
		JPanel panelT = new JPanel();
		JPanel panelF = new JPanel();
		JPanel panelPC = new JPanel();
		JPanel panelSW = new JPanel();
		JPanel panelReg = new JPanel();

		panel6.setLayout(new BoxLayout(panel6, BoxLayout.X_AXIS));
		panel6.add(new JLabel(" "));
		JLabel dec = new JLabel(" Dec  ", SwingConstants.LEFT);
		//dec.setHorizontalAlignment(SwingConstants.LEFT);
		panel6.add(dec);
		JLabel hex = new JLabel("  Hex  ", SwingConstants.LEFT);
		//hex.setHorizontalAlignment(SwingConstants.LEFT);
		panel6.add(hex);

		panelA.setLayout(new BoxLayout(panelA, BoxLayout.X_AXIS));
		pnAText1 = new JTextField(5);
		pnAText1.setEnabled(false);
		pnAText2 = new JTextField(5);
		pnAText2.setEnabled(false);
		panelA.add(new JLabel("A (#0)"));
		panelA.add(pnAText1);
		panelA.add(pnAText2);

		panelX.setLayout(new BoxLayout(panelX, BoxLayout.X_AXIS));
		pnXText1 = new JTextField(5);
		pnXText1.setEnabled(false);
		pnXText2 = new JTextField(5);
		pnXText2.setEnabled(false);
		panelX.add(new JLabel("X (#1)"));
		panelX.add(pnXText1);
		panelX.add(pnXText2);

		panelL.setLayout(new BoxLayout(panelL, BoxLayout.X_AXIS));
		pnLText1 = new JTextField(5);
		pnLText1.setEnabled(false);
		pnLText2 = new JTextField(5);
		pnLText2.setEnabled(false);
		panelL.add(new JLabel("L (#2)"));
		panelL.add(pnLText1);
		panelL.add(pnLText2);

		panelB.setLayout(new BoxLayout(panelB, BoxLayout.X_AXIS));
		pnBText1 = new JTextField(5);
		pnBText1.setEnabled(false);
		pnBText2 = new JTextField(5);
		pnBText2.setEnabled(false);
		panelB.add(new JLabel("B (#3)"));
		panelB.add(pnBText1);
		panelB.add(pnBText2);

		panelS.setLayout(new BoxLayout(panelS, BoxLayout.X_AXIS));
		pnSText1 = new JTextField(5);
		pnSText1.setEnabled(false);
		pnSText2 = new JTextField(5);
		pnSText2.setEnabled(false);
		panelS.add(new JLabel("S (#4)"));
		panelS.add(pnSText1);
		panelS.add(pnSText2);

		panelT.setLayout(new BoxLayout(panelT, BoxLayout.X_AXIS));
		pnTText1 = new JTextField(5);
		pnTText1.setEnabled(false);
		pnTText2 = new JTextField(5);
		pnTText2.setEnabled(false);
		panelT.add(new JLabel("T (#5)"));
		panelT.add(pnTText1);
		panelT.add(pnTText2);

		panelF.setLayout(new BoxLayout(panelF, BoxLayout.X_AXIS));
		pnFText1 = new JTextField(10);
		pnFText1.setEnabled(false);
		panelF.add(new JLabel("F (#6)"));
		panelF.add(pnFText1);


		panelPC.setLayout(new BoxLayout(panelPC, BoxLayout.X_AXIS));
		pnPCText1 = new JTextField(5);
		pnPCText1.setEnabled(false);
		pnPCText2 = new JTextField(5);
		pnPCText2.setEnabled(false);
		panelPC.add(new JLabel("PC (#8)"));
		panelPC.add(pnPCText1);
		panelPC.add(pnPCText2);

		panelSW.setLayout(new BoxLayout(panelSW, BoxLayout.X_AXIS));
		pnSWText1 = new JTextField(10);
		pnSWText1.setEnabled(false);
		panelSW.add(new JLabel("SW (#9)"));
		panelSW.add(pnSWText1);

		panelReg.setLayout(new BoxLayout(panelReg,BoxLayout.Y_AXIS));
		panelReg.setBorder(new TitledBorder(null, "Register", TitledBorder.LEADING,
				TitledBorder.TOP, null, null));
		panelReg.add(panel6);
		panelReg.add(panelA);
		panelReg.add(panelX);
		panelReg.add(panelL);
		panelReg.add(panelB);
		panelReg.add(panelS);
		panelReg.add(panelT);
		panelReg.add(panelF);
		panelReg.add(panelPC);
		panelReg.add(panelSW);

		//EndRecord
		JPanel panelRight = new JPanel();
		JPanel panelEnd = new JPanel();
		JPanel panel7 = new JPanel();
		JPanel panel7_1 = new JPanel();
		JPanel panelLast = new JPanel();
		JPanel panel8 = new JPanel();
		JPanel panel9 = new JPanel();
		JPanel panel10 = new JPanel();
		JPanel panelLastLeft = new JPanel();
		JPanel panelLastRight = new JPanel();
		JPanel panelTextarea = new JPanel();
		JPanel panelDiviceLb = new JPanel();
		JPanel panelDiviceBtn = new JPanel();
		JPanel panelBtnone = new JPanel();
		JPanel panelBtnall = new JPanel();
		JPanel panelBtnend = new JPanel();

		panel7.setLayout(new BoxLayout(panel7, BoxLayout.X_AXIS));
		panel7.add(new JLabel("<html>Address if First instruction<br></html>"));

		panel7_1.setLayout(new BoxLayout(panel7_1, BoxLayout.X_AXIS));
		panel7_1.add(new JLabel("in Object Program :"));
		pn7Text = new JTextField(5);
		pn7Text.setEnabled(false);
		panel7_1.add(pn7Text);

		panel8.setLayout(new BoxLayout(panel8, BoxLayout.X_AXIS));
		panel8.add(new JLabel("<html>Start Address in Memory<br></html>"));
		pn8Text = new JTextField(8);
		pn8Text.setEnabled(false);
		panel8.add(pn8Text);

		panel9.setLayout(new BoxLayout(panel9, BoxLayout.X_AXIS));
		panel9.add(new JLabel("<html>Target Address : <br></html>"));
		pn9Text = new JTextField(8);
		pn9Text.setEnabled(false);
		panel9.add(pn9Text);

		panel10.setLayout(new BoxLayout(panel10, BoxLayout.X_AXIS));
		panel10.add(new JLabel("Instruction : "));
		panelTextarea.setLayout(new BoxLayout(panelTextarea, BoxLayout.X_AXIS));
		ta = new JTextArea("",10, 8);
		JScrollPane scrollPane = new JScrollPane(ta);
		scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
		scrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
		panelTextarea.add(new JScrollPane(ta));
		panelTextarea.add(scrollPane);

		ta.setEnabled(false);

		panelDiviceLb.add(new JLabel("Using Device"));

		pnDivText = new JTextField(8);
		pnDivText.setEnabled(false);
		panelDiviceBtn.add(pnDivText);
		panelBtnone.add(oneexe);
		panelBtnall.add(allexe);
		panelBtnend.add(exit);
		oneexe.addActionListener(oneE);
		allexe.addActionListener(allE);
		exit.addActionListener(endlistener);

		panelEnd.setLayout(new BoxLayout(panelEnd,BoxLayout.Y_AXIS));
		panelEnd.setBorder(new TitledBorder(null, "E(End Record)", TitledBorder.LEADING,
				TitledBorder.TOP, null, null));
		panelEnd.add(panel7);
		panelEnd.add(panel7_1);

		panelLast.setLayout(new BoxLayout(panelLast,BoxLayout.Y_AXIS));
		panelLast.add(panel8);
		panelLast.add(panel9);

		panelLastLeft.setLayout(new BoxLayout(panelLastLeft,BoxLayout.Y_AXIS));
		panelLastLeft.add(panel10);
		panelLastLeft.add(panelTextarea);

		panelLastRight.setLayout(new BoxLayout(panelLastRight,BoxLayout.Y_AXIS));
		panelLastRight.add(panelDiviceLb);
		panelLastRight.add(panelDiviceBtn);
		panelLastRight.add(Box.createVerticalStrut(70));
		panelLastRight.add(panelBtnone);
		panelLastRight.add(panelBtnall);
		panelLastRight.add(panelBtnend);

		panelRight.setLayout(new BoxLayout(panelRight, BoxLayout.Y_AXIS));
		panelRight.add(panelEnd);
		panelRight.add(panelLast);

		JPanel pnrightend = new JPanel();
		pnrightend.setLayout(new BoxLayout(pnrightend, BoxLayout.X_AXIS));
		pnrightend.add(panelLastLeft);
		pnrightend.add(panelLastRight);

		panelRight.add(pnrightend);

		//log part
		JPanel panelLog = new JPanel();
		JPanel panelLglb = new JPanel();
		JPanel panelLgta = new JPanel();

		panelLglb.setLayout(new BoxLayout(panelLglb, BoxLayout.X_AXIS));
		panelLglb.add(new JLabel("Log(about insturction execute) : "));
		panelLgta.setLayout(new BoxLayout(panelLgta, BoxLayout.X_AXIS));
		ta2 = new JTextArea(8, 40);
		ta2.setEnabled(false);
		panelLgta.add(new JScrollPane(ta2));
		panelLog.setLayout(new BoxLayout(panelLog, BoxLayout.Y_AXIS));
		panelLog.add(panelLglb);
		panelLog.add(panelLgta);


		//전체구성

		add(panel1, BorderLayout.PAGE_START);


		JPanel center = new JPanel();
		center.setLayout(new GridLayout(1, 2));
		JPanel section2 = new JPanel();
		section2.setLayout(new BoxLayout(section2, BoxLayout.Y_AXIS));
		section2.add(panelHeader);
		section2.add(panelReg);
		center.add(section2);
		center.add(panelRight);
		add(center, BorderLayout.CENTER);

		JPanel endpart = new JPanel();
		endpart.setLayout(new GridLayout(1, 1));
		endpart.add(panelLog);
		add(endpart, BorderLayout.PAGE_END);
	}



	public static void main(String[] args) {


		ResourceManager resourceManager = new ResourceManager();
		SicLoader loader = new SicLoader(resourceManager);
		VisualSimulator simulator = new VisualSimulator();

	}



}


