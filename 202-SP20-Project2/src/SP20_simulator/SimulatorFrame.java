package SP20_simulator;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class SimulatorFrame extends JFrame implements ActionListener {
	private JFileChooser jfc = new JFileChooser();
	private JButton jbt_open = new JButton("Open");
	private JLabel jlb = new JLabel(" ");

	//생성자
	public SimulatorFrame() {
		super("Simulator - MadeByInjeong");
		this.init();
		this.start();
		this.setSize(600,800);
		this.setVisible(true);
	}

	public void init(){
		getContentPane().setLayout(new FlowLayout());
		add(new JLabel("FileName : "));
		add(new JTextField(15));
		add(new JButton("Open"));
		add(jlb);
	}

	public void start(){
		jbt_open.addActionListener(this);

		jfc.setFileFilter(new FileNameExtensionFilter("txt", "txt"));
		// 파일 필터
		jfc.setMultiSelectionEnabled(false);//다중 선택 불가
	}

	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub
		if(arg0.getSource() == jbt_open){
			if(jfc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION){
				// showopendialog 열기 창을 열고 확인 버튼을 눌렀는지 확인
				jlb.setText("열기 경로 : " + jfc.getSelectedFile().toString());
			}
		}
	}
	public static void main(String[] args) {
		SimulatorFrame frame = new SimulatorFrame();
	}
}
