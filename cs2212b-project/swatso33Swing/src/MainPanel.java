import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Iterator;
import java.util.LinkedList;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

@SuppressWarnings("serial")
public class MainPanel extends JPanel {

	private static JPanel mainPanel;	//For holding all other components
	private static JPanel lettersPanel;	//For holding the letters the player can select
	private static JPanel wordPanel;	//For showing the blanks and uncovered letters
	private static JPanel imageLabel;	//For showing the hang man
	
	private JLabel title, category, correct;	//For storing text to be displayed to the user
	private JLabel[] word;				//For storing the puzzle in blanks and uncovered characters
	
	private JTextField solveField;		//For reading an answer from the player
	
	private JButton solve, ok, cont;	//For allowing the user to solve and continue playing
	private JButton[] letters;			//To allow the user to guess letters
	
	private ImageIcon image;			//Stores the picture of the hang man
	
	private LinkedList<Component> foreground, background, labels;	//For changing skins
	private int wrong, player;			//To store which player the panel is for and how many wrong guesses have been made
	private boolean result;				//True if puzzle was solved, false otherwise
	private String puzzle, skin, cat;	//For storing settings

	//Constructor for creating a panel where the player can guess a puzzle
	public MainPanel(String puzzle, int player, String cat, String skin, JButton cont){
		
		//Initialize the panel settings
		this.cat = cat;
		this.player = player;
		this.cont = cont;
		this.puzzle = puzzle;
		this.skin = skin;
		wrong = 0;
		result = false;
		
		//Create a panel to hold all of the other components
		mainPanel = new JPanel();
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		
		//Initialize the linked lists for changing skins
		foreground = new LinkedList<Component>();
		background = new LinkedList<Component>();
		labels = new LinkedList<Component>();
		
		//Initialize all of the panels components
		initComponents();
		
		//Add all of the components to the main panel
		mainPanel.add(title);
		mainPanel.add(category);
		mainPanel.add(Box.createRigidArea(new Dimension(0,30)));	//Space all of the components
		mainPanel.add(imageLabel);
		mainPanel.add(Box.createRigidArea(new Dimension(0,30)));
		mainPanel.add(wordPanel);
		mainPanel.add(Box.createRigidArea(new Dimension(0,30)));
		mainPanel.add(lettersPanel);
		mainPanel.add(Box.createRigidArea(new Dimension(0,10)));
		mainPanel.add(solve);
		mainPanel.add(Box.createRigidArea(new Dimension(0,30)));
		
		//Change all the components to the proper color scheme
		background.add(mainPanel);
		skinChanger(skin);
		
}
	
	//Initializes all the components that allow the user to play
	private void initComponents(){
		
		//Display a title for the panel
		title = new JLabel("HANGMAN", JLabel.CENTER);
		title.setFont(new Font("Sans-Serif", Font.BOLD, 48));
		title.setAlignmentX(Component.CENTER_ALIGNMENT);
		labels.add(title);
		
		//Display which player is guessing letters and what the category is
		category = new JLabel("Player " + Integer.toString(player) + ", guess letters. Category: " + cat, JLabel.CENTER);
		category.setFont(new Font("Sans-Serif", Font.BOLD, 18));
		category.setAlignmentX(Component.CENTER_ALIGNMENT);
		labels.add(category);
		
		//Display the empty hang man picture at the start of the puzzle
		image = new ImageIcon("start.png", "");
		imageLabel = new JPanel();
		imageLabel.add(new JLabel(image));
		imageLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
		background.add(imageLabel);
		
		//Show the puzzle as all blanks so the player can guess
		wordPanel = new JPanel();
		wordPanel.setLayout(new BoxLayout(wordPanel, BoxLayout.X_AXIS));
		background.add(wordPanel);
		word = new JLabel[puzzle.length()];
		
		//Hide the character if it is a letter, otherwise display it
		for(int i=0; i < puzzle.length(); i++){
			if (puzzle.charAt(i) >= 'A' && puzzle.charAt(i) <= 'Z'){
				word[i] = new JLabel("_");
			}
			else{
				word[i] = new JLabel(Character.toString(puzzle.charAt(i)));
			}
			word[i].setFont(new Font("Sans-Serif", Font.BOLD, 24));
			labels.add(word[i]);
			wordPanel.add(word[i]);									//Add the letter to the word
			wordPanel.add(Box.createRigidArea(new Dimension(3,0)));	//Space the letters
		}
		
		//Initialize the buttons that allow the user to guess letters
		lettersPanel = new JPanel();
		lettersPanel.setLayout(new GridLayout(3,10,10,10));
		initLettersPanel();
		background.add(lettersPanel);
		
		//Displays a button that allows the player to guess the remaining letters and solve the puzzle
		solve = new JButton("Solve");
		solve.setMaximumSize(new Dimension(250,100));
		solve.setAlignmentX(Component.CENTER_ALIGNMENT);
		solve.addActionListener(new SolvePuzzle());
		foreground.add(solve);	
	}

	//Initializes the buttons that allow the user to guess letters
	private void initLettersPanel(){
		
		//Create an array of buttons to hold all of the letters
		letters = new JButton[26];
		
		//Add a button for each letter
		for(int i=0; i < 26; i++){
			letters[i] = new JButton(Character.toString((char)(65 + i)));	//Offsets the index to get the proper ASCII value
			letters[i].addActionListener(new LetterHandler());
			lettersPanel.add(letters[i]);
			foreground.add(letters[i]);
		}
	}
	
	//Called when a user guesses a letter
	private class LetterHandler implements ActionListener{
		public void actionPerformed(ActionEvent e){
			
			//Remove the guesses letter from the remaining letters
			letters[(int)(e.getActionCommand().charAt(0)) - 65].removeActionListener(null);
			letters[(int)(e.getActionCommand().charAt(0)) - 65].setVisible(false);
			
			boolean validLetter = false;	//Set to true if the letter is in the puzzle
			boolean moreLetters = false;	//Set to true if there are still letters that haven't been guessed
			
			//Compare each letter in the puzzle with the letter that was guessed
			for(int i=0; i < puzzle.length(); i++){
				
				//If the letter is in the puzzle display every instance of it
				if (puzzle.charAt(i) == e.getActionCommand().charAt(0)){
					word[i].setText((Character.toString(puzzle.charAt(i))));
					solve.setVisible(true);		//Show the solve button again
					validLetter = true;			//Mark the guess so it isn't considered a wrong guess
				}
				
				//Check if there are any more unguessed letters
				if (word[i].getText() == "_" ){
					moreLetters = true;
				}
			}
			
			//Check if the entire puzzle has been solved
			if (!moreLetters){
				
				//Congratulate the player
				correct = new JLabel("Correct!");
				correct.setFont(new Font("Sans-Serif", Font.BOLD, 18));
				correct.setAlignmentX(CENTER_ALIGNMENT);
				labels.add(correct);
				
				//Add a button to go to the next panel in the game
				ok = cont;
				ok.setAlignmentX(CENTER_ALIGNMENT);
				foreground.add(ok);
				skinChanger(skin);
				
				//Update the panel to show the puzzle was solved and allow the player to go to the next panel
				lettersPanel.setVisible(false);
				solve.setVisible(false);
				mainPanel.add(correct);
				mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
				mainPanel.add(ok);
				mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
				
				result = true;	//Shows the player successfully solved the puzzle
			}
			
			//If the guessed letter is not in the puzzle run a wrong letter routine
			if (!validLetter){
				wrongLetter();
				mainPanel.revalidate();
				mainPanel.repaint();
			}
		}

	}

	//Called when the user attempts to solve the puzzle
	private class SolvePuzzle implements ActionListener{
		public void actionPerformed(ActionEvent arg0) {
			
			//Create a text field for the player to enter their guess
			solveField = new JTextField(20);
			solveField.setMaximumSize(new Dimension(250,60));
			
			//Create a button so they can submit their answer
			ok = new JButton("Submit");
			ok.setMaximumSize(new Dimension(100,50));
			ok.setAlignmentX(Component.CENTER_ALIGNMENT);
			ok.addActionListener(new Submit());
			foreground.add(ok);
			skinChanger(skin);
			
			//Update the panel to show the text field and submit button
			mainPanel.remove(solve);
			lettersPanel.setVisible(false);
			mainPanel.add(Box.createRigidArea(new Dimension(0,-30)));
			mainPanel.add(solveField);
			mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
			mainPanel.add(ok);
			mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
			mainPanel.revalidate();
			mainPanel.repaint();
		}	
	}

	//Called when the user submits an answer to the puzzle
	private class Submit implements ActionListener{
		public void actionPerformed(ActionEvent e){
			
			//Read the players answer
			String answer = solveField.getText();
			
			//Check if it is the right answer
			if (puzzle.compareTo(answer.toUpperCase()) != 0){
				
				//If it is wrong, reset the panel by removing the solve field and replacing the letters
				mainPanel.remove(ok);
				mainPanel.add(Box.createRigidArea(new Dimension(0,-30)));
				mainPanel.remove(solveField);
				mainPanel.add(Box.createRigidArea(new Dimension(0,-30)));
				lettersPanel.setVisible(true);
				mainPanel.add(solve);
				mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
				
				//Count the wrong guess as a wrong letter and refresh the panel
				wrongLetter();
				mainPanel.revalidate();
				mainPanel.repaint();
			}
			else{
				
				//If it is the right answer display all remaining letters
				for(int i=0; i < puzzle.length(); i++){
					word[i].setText((Character.toString(puzzle.charAt(i))));
				}
				
				//Congratulate the player
				correct = new JLabel("Correct!");
				correct.setFont(new Font("Sans-Serif", Font.BOLD, 18));
				correct.setAlignmentX(CENTER_ALIGNMENT);
				labels.add(correct);
				result = true;		//Show that the player got the puzzle right
				
				//Add a button to move to the next part of the game
				mainPanel.remove(ok);
				ok = cont;
				ok.setAlignmentX(CENTER_ALIGNMENT);
				foreground.add(ok);
				skinChanger(skin);
				
				//Update the panel to show that it was the right answer and allow the user to move to the next part of the game
				lettersPanel.setVisible(false);
				
				mainPanel.remove(solveField);
				mainPanel.add(Box.createRigidArea(new Dimension(0,-60)));
				mainPanel.add(correct);
				mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
				mainPanel.add(ok);
				mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
				mainPanel.revalidate();
				mainPanel.repaint();
			}
		}
	}
	
	//Called if the user guesses a wrong letter
	private void wrongLetter(){
		wrong++;	//Increment the number of wrong guesses
		
		//Display the next body part
		image = new ImageIcon(Integer.toString(wrong) + ".png", "");
		imageLabel.removeAll();
		imageLabel.add(new JLabel(image));
		
		//Remove the solve button
		solve.setVisible(false);
		
		//Check if they have lost
		if (wrong == 7){
			
			//Show the remaining letters
			for(int i=0; i < puzzle.length(); i++){
				word[i].setText((Character.toString(puzzle.charAt(i))));
			}
			
			for(int i=0; i<26; i++){
				if (letters[i].getActionListeners().length != 0)
				letters[i].removeActionListener(letters[i].getActionListeners()[0]);
			}
			
			//Add a button to go to the next part of the game
			ok = cont;
			ok.setAlignmentX(CENTER_ALIGNMENT);
			foreground.add(ok);
			skinChanger(skin);
			
			//Update the panel to show the puzzle and the button to continue
			mainPanel.add(Box.createRigidArea(new Dimension(0,-30)));
			mainPanel.add(ok);
			mainPanel.add(Box.createRigidArea(new Dimension(0, 30)));
		}
	}
	
	//Returns the panel so that it can be used by other classes
	//Returns the complete panel so it can be added in other classes
	public JPanel getMainPanel(){
		return mainPanel;
	}
	
	//Returns whether or not the player solved the puzzle
	//Returns true if the player solved the puzzle, false otherwise
	public boolean getResult(){
		return result;
	}
	
	//Changes the color scheme of all components
	//Changes the color of all the components to the proper color scheme
	public void skinChanger(String scheme){
		
		//Create an iterator of all components to be set to foreground colors
		Iterator<Component> foregroundComponents = foreground.iterator();
		JComponent holder;
		
		//Change all foreground components to the proper color
		while(foregroundComponents.hasNext()){
			holder = (JComponent) foregroundComponents.next();
			if (scheme.compareTo("Water") == 0){
				holder.setBackground(Color.cyan);
				skin = "Water";
			}
			else{
				holder.setBackground(Color.orange);
				skin = "Fire";
			}
		}
		
		//Create an iterator of all components to be set to background colors
		Iterator<Component> backgroundComponents = background.iterator();
		
		//Change all the background components to the proper color
		while(backgroundComponents.hasNext()){
			holder = (JComponent) backgroundComponents.next();
			if (scheme.compareTo("Water") == 0){
				holder.setBackground(Color.blue);
			}
			else{
				holder.setBackground(Color.red);
			}
		}
		
		//Create an iterator of all the label components and change their colors
		Iterator<Component> labelComponents = labels.iterator();
		while(labelComponents.hasNext()){
			holder = (JComponent) labelComponents.next();
			if (scheme.compareTo("Water") == 0){
				holder.setForeground(Color.cyan);
			}
			else{
				holder.setForeground(Color.orange);
			}
		}
	}
}
