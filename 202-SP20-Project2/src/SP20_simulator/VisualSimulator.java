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


/**
 * VisualSimulator는 사용자와의 상호작용을 담당한다.<br>
 * 즉, 버튼 클릭등의 이벤트를 전달하고 그에 따른 결과값을 화면에 업데이트 하는 역할을 수행한다.<br>
 * 실제적인 작업은 SicSimulator에서 수행하도록 구현한다.
 */
public class VisualSimulator extends JFrame {
	ResourceManager resourceManager = new ResourceManager();
	SicLoader sicLoader = new SicLoader(resourceManager);
	SicSimulator sicSimulator = new SicSimulator(resourceManager);
	public JButton fopenBtn = new JButton("open"); //file open button
	public JTextField fName = new JTextField(15); //file name user input
	/**
	 * 프로그램 로드 명령을 전달한다.
	 */
	public void load(File program){
		//...

		sicLoader.load(program);
		//sicSimulator.load(program);
	};

	/**
	 * 하나의 명령어만 수행할 것을 SicSimulator에 요청한다.
	 */
	public void oneStep(){
		
	};

	/**
	 * 남아있는 모든 명령어를 수행할 것을 SicSimulator에 요청한다.
	 */
	public void allStep(){
		
	};
	
	/**
	 * 화면을 최신값으로 갱신하는 역할을 수행한다.
	 */
	public void update(){
		
	};

	public void initalize(){

	}

	public VisualSimulator(){
		super("Simulator - MadeByInjeong");
		this.init();
		this.start();
		this.setSize(560,640);
		this.setVisible(true);
	}

	public void init(){
		getContentPane().setLayout(new FlowLayout());

		//File Name
		JPanel panel1 = new JPanel();
		panel1.add(new JLabel("FileName : "));
		panel1.add(fName);
		panel1.add(fopenBtn);

		//Header Record
		JPanel panel2 = new JPanel();
		JPanel panel3 = new JPanel();
		JPanel panel4 = new JPanel();
		JPanel panelHeader = new JPanel();
		panel2.setLayout(new BoxLayout(panel2, BoxLayout.X_AXIS));
		panel2.add(new JLabel("Program name : "));
		JTextField pn2Text = new JTextField(10);
		pn2Text.setEnabled(false);
		panel2.add(pn2Text);
		panel3.setLayout(new BoxLayout(panel3, BoxLayout.X_AXIS));
		panel3.add(new JLabel("<html>Start Address of<br>Object Program : </html>"));
		JTextField pn3Text = new JTextField(10);
		pn3Text.setEnabled(false);
		panel3.add(pn3Text);
		panel4.setLayout(new BoxLayout(panel4, BoxLayout.X_AXIS));
		panel4.add(new JLabel("Length of Program : "));
		JTextField pn4Text = new JTextField(10);
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
		JTextField pnAText1 = new JTextField(5);
		pnAText1.setEnabled(false);
		JTextField pnAText2 = new JTextField(5);
		pnAText2.setEnabled(false);
		panelA.add(new JLabel("A (#0)"));
		panelA.add(pnAText1);
		panelA.add(pnAText2);

		panelX.setLayout(new BoxLayout(panelX, BoxLayout.X_AXIS));
		JTextField pnXText1 = new JTextField(5);
		pnXText1.setEnabled(false);
		JTextField pnXText2 = new JTextField(5);
		pnXText2.setEnabled(false);
		panelX.add(new JLabel("X (#1)"));
		panelX.add(pnXText1);
		panelX.add(pnXText2);

		panelL.setLayout(new BoxLayout(panelL, BoxLayout.X_AXIS));
		JTextField pnLText1 = new JTextField(5);
		pnLText1.setEnabled(false);
		JTextField pnLText2 = new JTextField(5);
		pnLText2.setEnabled(false);
		panelL.add(new JLabel("L (#2)"));
		panelL.add(pnLText1);
		panelL.add(pnLText2);

		panelB.setLayout(new BoxLayout(panelB, BoxLayout.X_AXIS));
		JTextField pnBText1 = new JTextField(5);
		pnBText1.setEnabled(false);
		JTextField pnBText2 = new JTextField(5);
		pnBText2.setEnabled(false);
		panelB.add(new JLabel("B (#3)"));
		panelB.add(pnBText1);
		panelB.add(pnBText2);

		panelS.setLayout(new BoxLayout(panelS, BoxLayout.X_AXIS));
		JTextField pnSText1 = new JTextField(5);
		pnSText1.setEnabled(false);
		JTextField pnSText2 = new JTextField(5);
		pnSText2.setEnabled(false);
		panelS.add(new JLabel("S (#4)"));
		panelS.add(pnSText1);
		panelS.add(pnSText2);

		panelT.setLayout(new BoxLayout(panelT, BoxLayout.X_AXIS));
		JTextField pnTText1 = new JTextField(5);
		pnTText1.setEnabled(false);
		JTextField pnTText2 = new JTextField(5);
		pnTText2.setEnabled(false);
		panelT.add(new JLabel("T (#5)"));
		panelT.add(pnTText1);
		panelT.add(pnTText2);

		panelF.setLayout(new BoxLayout(panelF, BoxLayout.X_AXIS));
		JTextField pnFText1 = new JTextField(10);
		pnFText1.setEnabled(false);
		panelF.add(new JLabel("F (#6)"));
		panelF.add(pnFText1);


		panelPC.setLayout(new BoxLayout(panelPC, BoxLayout.X_AXIS));
		JTextField pnPCText1 = new JTextField(5);
		pnPCText1.setEnabled(false);
		JTextField pnPCText2 = new JTextField(5);
		pnPCText2.setEnabled(false);
		panelPC.add(new JLabel("PC (#8)"));
		panelPC.add(pnPCText1);
		panelPC.add(pnPCText2);

		panelSW.setLayout(new BoxLayout(panelSW, BoxLayout.X_AXIS));
		JTextField pnSWText1 = new JTextField(10);
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
		JTextField pn7Text = new JTextField(5);
		pn7Text.setEnabled(false);
		panel7_1.add(pn7Text);

		panel8.setLayout(new BoxLayout(panel8, BoxLayout.X_AXIS));
		panel8.add(new JLabel("<html>Start Address in Memory<br></html>"));
		JTextField pn8Text = new JTextField(8);
		pn8Text.setEnabled(false);
		panel8.add(pn8Text);

		panel9.setLayout(new BoxLayout(panel9, BoxLayout.X_AXIS));
		panel9.add(new JLabel("<html>Target Address : <br></html>"));
		JTextField pn9Text = new JTextField(8);
		pn9Text.setEnabled(false);
		panel9.add(pn9Text);

		panel10.setLayout(new BoxLayout(panel10, BoxLayout.X_AXIS));
		panel10.add(new JLabel("Instruction : "));
		panelTextarea.setLayout(new BoxLayout(panelTextarea, BoxLayout.X_AXIS));
		JTextArea ta = new JTextArea("",10, 8);
		panelTextarea.add(new JScrollPane(ta));
		ta.setEnabled(false);

		panelDiviceLb.add(new JLabel("Using Device"));

		JTextField pnDivText = new JTextField(8);
		pnDivText.setEnabled(false);
		panelDiviceBtn.add(pnDivText);
		panelBtnone.add(new JButton("execute(1step)"));
		panelBtnall.add(new JButton("execute(all)"));
		panelBtnend.add(new JButton("exit"));

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
		JTextArea ta2 = new JTextArea(8, 40);
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


	public void start(){ //event 처리
		fopenBtn.addActionListener(new ActionListener() {
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
						//System.out.println("load : " + chooser.getSelectedFile());
						initalize();
						load(chooser.getSelectedFile());
						update();
					} catch(Exception ex){
						ex.printStackTrace();
					}
				}
			}

		});
	}


	public static void main(String[] args) {

		ResourceManager resourceManager = new ResourceManager();
		SicLoader loader = new SicLoader(resourceManager);
		InstTable instTable = new InstTable("/inst.txt");
		VisualSimulator simulator = new VisualSimulator();

	}



}


