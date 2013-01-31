import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

@SuppressWarnings("serial")
public class EntryPanel extends JPanel{
	
	private static JPanel entryPanel;
	private static JLabel prompt, title; //To prompt user and show them the category
	private JButton cont;	//A button used to remove the panel and add the next one
	private JTextField puzzle;	//A place for a player to enter their puzzle

	public EntryPanel(int playerNumber, String category, JButton cont){
		
		//Get the button that will send control back to the creating class
		this.cont = cont;
		cont.setAlignmentX(CENTER_ALIGNMENT);
		
		//Create a panel for the user to enter a puzzle
		entryPanel = new JPanel();
		entryPanel.setLayout(new BoxLayout(entryPanel, BoxLayout.Y_AXIS));
		
		//Create a title for the game and add it to the panel
		title = new JLabel("HANGMAN", JLabel.CENTER);
		title.setFont(new Font("Sans-Serif", Font.BOLD, 48));
		title.setAlignmentX(Component.CENTER_ALIGNMENT);
		entryPanel.add(title);
		
		//Prompt the proper user for a puzzle in the proper category
		prompt = new JLabel("Player " + Integer.toString(playerNumber) + " please enter a puzzle. The category is " + category +".");
		prompt.setAlignmentX(CENTER_ALIGNMENT);
		prompt.setFont(new Font("Sans-Serif", Font.BOLD, 18));
		entryPanel.add(prompt);
		entryPanel.add(Box.createRigidArea(new Dimension(0,20)));	//Add some spacing
		
		//Create a text field for user input and add it to the panel
		puzzle = new JTextField(10);
		puzzle.setMaximumSize(new Dimension(250,20));
		puzzle.setAlignmentX(CENTER_ALIGNMENT);
		entryPanel.add(puzzle);
		entryPanel.add(Box.createRigidArea(new Dimension(0,30)));	//Add some spacing
		
		//Add the button that will submit the puzzle
		entryPanel.add(this.cont);
		entryPanel.add(Box.createRigidArea(new Dimension(0,30)));	//Add some spacing
	}
	
	//Returns the panel created in the constructor so it can be added in other classes
	public JPanel getEntryPanel(){
		return entryPanel;
	}
	
	//Returns the text field to get the players puzzle input
	public String getPuzzle(){
		return puzzle.getText();
	}
	
	//Changes the skin of all the components
	public void skinChanger(String scheme){
		
		//Determine what the skin is and change the colors accordingly
		if (scheme.compareTo("Water") == 0){
			title.setForeground(Color.cyan);
			prompt.setForeground(Color.cyan);
			entryPanel.setBackground(Color.blue);
			cont.setBackground(Color.cyan);
		}
		else{
			prompt.setForeground(Color.orange);
			title.setForeground(Color.orange);
			entryPanel.setBackground(Color.red);
			cont.setBackground(Color.orange);
		}
		
	}
	
}
