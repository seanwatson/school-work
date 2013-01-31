import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JRadioButtonMenuItem;


public class CS2212HangmanRegular {

	//Panel components
	private static JFrame mainFrame;
	private static MainPanel mainPanel;
	private static EntryPanel player1Entry, player2Entry;
	private static JPanel resultsPanel;
	
	//Menu Bar components
	private static JMenuBar mainMenuBar;
	private static JMenu options, appearance, category;
	private static JMenuItem quit;
	private static JRadioButtonMenuItem water, fire, tvShow, movies, phrases;
	
	//Button components
	private ButtonGroup schemes, categories;
	private static JButton cont;
	
	//Other components
	private static JLabel result;	//For displaying the winners at the end of the game
	private static String player1Puzzle="", player2Puzzle="", cat, skin;	//For storing settings and puzzles
	private boolean player1Result, player2Result;	//For storing the result of each players puzzle
	
	public CS2212HangmanRegular(){
		
		InitMenuBar();	//Initialize the menu bar and all its components
		
		//Create a button to get player 1s puzzle and change panels to let player 2 enter a puzzle
		cont = new JButton("Ok");
		cont.addActionListener(new GetPuzzle());
		
	}

	@SuppressWarnings("unused")
	public static void main(String[] args){
		
		//Read the settings file for data persistence with the category and skin
		try{
			BufferedReader in = new BufferedReader(new FileReader("settings.txt"));
			try {
				cat = in.readLine();
				skin = in.readLine();
				in.close();
			} catch (IOException e1) {
				System.out.println(e1.getMessage());				
			}
			
			//If it is the first game set the default category and skin and create the settings file
		}catch(FileNotFoundException e){
			try {
				cat = "TV Shows";
				skin = "Fire";
				PrintWriter out = new PrintWriter(new FileWriter("settings.txt"));
				out.println(cat);
				out.print(skin);
				out.close();
			} catch (IOException e1) {
				System.out.println(e1.getMessage());
			}
		}
		
		//Initialize the gui
		CS2212HangmanRegular gui = new CS2212HangmanRegular();
		
		//Create a panel that allows player 1 to enter a puzzle with the proper category
		player1Entry = new EntryPanel(1,cat,cont);
		player1Entry.skinChanger(skin);		//Change the color scheme of the new panel
		
		//Create the frame to contain the panels
		mainFrame = new JFrame("Hangman");		//Set title to Hangman
		mainFrame.addWindowListener(new WindowAdapter(){	//Add the minimize, maximize, close buttons
			public void windowClosing(WindowEvent e){
				System.exit(0);
			}
		});
		
		//Add the first panel and the menu bar
		mainFrame.getContentPane().add(player1Entry.getEntryPanel());
		mainFrame.setJMenuBar(mainMenuBar);
		
		//Display the game
		mainFrame.pack();
		mainFrame.setVisible(true);

	}
	
	//Initializes the menu bar
	private void InitMenuBar(){
		
		//Initialize the menu bar and the menu items
		mainMenuBar = new JMenuBar();
		options = new JMenu("Options");
		appearance = new JMenu("Appearance");
		category = new JMenu("Change Category");
		quit = new JMenuItem("Quit");
		
		//Initialize the category options
		categories = new ButtonGroup();
		tvShow = new JRadioButtonMenuItem("TV Shows");
		movies = new JRadioButtonMenuItem("Movies");
		phrases = new JRadioButtonMenuItem("Phrases");
		
		//Determine what the category is set as and set its radio button to selected
		if (cat.compareTo("TV Shows") == 0){
			tvShow.setSelected(true);
		}
		else if (cat.compareTo("Movies") == 0){
			movies.setSelected(true);
		}
		else{
			phrases.setSelected(true);
		}
		
		//Add the categories into a button group
		categories.add(tvShow);
		categories.add(movies);
		categories.add(phrases);
		
		//Initialize the scheme options
		schemes = new ButtonGroup();
		water = new JRadioButtonMenuItem("Water");
		fire = new JRadioButtonMenuItem("Fire");
		
		//Determine the current skin and set its radio button to selected
		if (skin.compareTo("Fire") == 0){
			fire.setSelected(true);
		}
		else{
			water.setSelected(true);
		}
		
		//Add the scheme options into a button group
		schemes.add(water);
		schemes.add(fire);
		
		//Add listeners for all menu items that perform the appropriate actions
		quit.addActionListener(this.new Quit());
		water.addActionListener(new ChangeSkin());
		fire.addActionListener(new ChangeSkin());
		tvShow.addActionListener(new ChangeCategory());
		movies.addActionListener(new ChangeCategory());
		phrases.addActionListener(new ChangeCategory());
		
		//Add all the menu items to the appropriate menu
		options.add(quit);
		appearance.add(fire);
		appearance.add(water);
		category.add(tvShow);
		category.add(movies);
		category.add(phrases);
			
		//Add all the menus to the menu bar
		mainMenuBar.add(options);
		mainMenuBar.add(appearance);
		mainMenuBar.add(category);
	}
	
	//Get the puzzle entered by one of the players
	private class GetPuzzle implements ActionListener{
		public void actionPerformed(ActionEvent e) {
			
			//Check if it is player 1 or player 2 that has just entered a puzzle
			if (player1Puzzle == ""){
				
				//Check if the puzzle entered is a proper length
				if (player1Entry.getPuzzle().length() >= 1 && player1Entry.getPuzzle().length() <= 100){
					
					//Get the puzzle entered
					player1Puzzle = player1Entry.getPuzzle();
					
					//Create a new panel for player 2 to enter their puzzle
					player2Entry = new EntryPanel(2, cat, cont);
					player2Entry.skinChanger(skin);	//Change the new panel to the proper color scheme
					
					//Remove the option to change categories
					mainMenuBar.remove(category);
					mainMenuBar.revalidate();
					mainMenuBar.repaint();
					
					//Remove player 1s panel and replace it with player 2s
					mainFrame.getContentPane().removeAll();
					mainFrame.getContentPane().add(player2Entry.getEntryPanel());
					mainFrame.revalidate();
				}
			}
			else{
				
				//Check if the puzzle entered is a proper length
				if (player2Entry.getPuzzle().length() >= 1 && player2Entry.getPuzzle().length() <= 100){
					
					//Get the puzzle entered
					player2Puzzle = player2Entry.getPuzzle();
					
					//Change the continue button so that it will switch to second puzzle after the first is solved
					cont = new JButton("Next Player");
					cont.addActionListener(new NextPlayer());
					
					//Create the first puzzle, remove the old panel and replace it with the puzzle
					mainPanel = new MainPanel(player1Puzzle.toUpperCase(), 2, cat, skin, cont);
					mainFrame.getContentPane().removeAll();
					mainFrame.getContentPane().add(mainPanel.getMainPanel());
					mainFrame.pack();
					mainFrame.revalidate();
				}
			}
		}	
	}

	//Switches to the second players puzzle after the first puzzle is finished
	private class NextPlayer implements ActionListener{
		public void actionPerformed(ActionEvent e) {
			
			//Change the continue button so it will show the results panel after the puzzle is finished
			cont = new JButton("See Results");
			cont.addActionListener(new Results());
			
			//Determine whether player 2 solved player 1s puzzle or not
			player1Result = mainPanel.getResult();
			
			//Create player 2s puzzle and replace the panel in the frame with it
			mainPanel = new MainPanel(player2Puzzle.toUpperCase(), 1, cat, skin, cont);
			mainFrame.getContentPane().removeAll();
			mainFrame.getContentPane().add(mainPanel.getMainPanel());
			mainFrame.pack();
			mainFrame.revalidate();
		}
		
	}

	//Displays the results at the end of the game
	private class Results implements ActionListener{
		public void actionPerformed(ActionEvent arg0) {
			
			//Change the continue button so the user can quit after seeing the results
			cont = new JButton("Quit");
			cont.setAlignmentX(Component.CENTER_ALIGNMENT);
			cont.addActionListener(new Quit());
			
			//Determine if player 1 solved player 2s puzzle or not
			player2Result = mainPanel.getResult();

			//Create the appropriate message for the game
			if (player1Result && player2Result){
				result = new JLabel("Both Player 1 and Player 2 won! :D", JLabel.CENTER);
			}
			else if (player1Result && !player2Result){
				result = new JLabel("Player 1 won!", JLabel.CENTER);
			}
			else if (!player1Result && player2Result){
				result = new JLabel("Player 2 won!", JLabel.CENTER);
			}
			else{
				result = new JLabel("Both Player 1 and Player 2 lost! :(", JLabel.CENTER);
			}
			result.setFont(new Font("Sans-Serif", Font.BOLD, 24));
			result.setAlignmentX(Component.CENTER_ALIGNMENT);
									
			//Create a panel that displays the result
			resultsPanel = new JPanel();
			resultsPanel.setLayout(new BoxLayout(resultsPanel,BoxLayout.Y_AXIS));
			resultsSkinChanger();		//Change it to the appropriate color scheme		
			resultsPanel.add(Box.createRigidArea(new Dimension(30,100)));
			resultsPanel.add(result);
			resultsPanel.add(Box.createRigidArea(new Dimension(0,50)));
			resultsPanel.add(cont);
			resultsPanel.add(Box.createRigidArea(new Dimension(0,100)));
			
			//Remove the old panel and replace it with the results
			mainFrame.getContentPane().removeAll();
			mainFrame.getContentPane().add(resultsPanel);
			mainFrame.revalidate();
		}
		
	}

	//Changes the skin of the current panel in the frame
	private class ChangeSkin implements ActionListener{
		public void actionPerformed(ActionEvent e) {
			
			//Get the skin that the user want to change to
			skin = e.getActionCommand();
			
			//Call the appropriate skin changer
			if (mainPanel != null)
				mainPanel.skinChanger(skin);
			if (player1Entry != null)
				player1Entry.skinChanger(skin);
			if (player2Entry != null)
				player2Entry.skinChanger(skin);
			if (resultsPanel != null){
				resultsSkinChanger();
			}
			
			//Write the new skin to the settings file so it will be saved for the next game
			try{
				PrintWriter out = new PrintWriter(new FileWriter("settings.txt"));
				out.println(cat);
				out.println(skin);
				out.close();
			}catch (IOException e2){
				System.out.println(e2.getMessage());
			}
		}	
	}	

	//Changes the category
	private class ChangeCategory implements ActionListener{
		public void actionPerformed(ActionEvent e) {
			
			//Get the category that the user wants to change to
			cat = e.getActionCommand();
			
			//Remover the old panel and replace it with one that has the new category
			mainFrame.getContentPane().remove(player1Entry.getEntryPanel());
			player1Entry = new EntryPanel(1, e.getActionCommand(), cont);
			player1Entry.skinChanger(skin);	//Change the skin to the current skin
			
			//Write the new category to the settings file so it will be saved for the next game
			try{
				PrintWriter out = new PrintWriter(new FileWriter("settings.txt"));
				out.println(cat);
				out.println(skin);
				out.close();
			}catch (IOException e2){
				System.out.println(e2.getMessage());
			}
			
			//Add the panel with the new category and display it
			mainFrame.getContentPane().add(player1Entry.getEntryPanel());
			mainFrame.pack();
			mainFrame.revalidate();
		}
		
	}
	
	//Terminates the program when called
	private class Quit implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			System.exit(0);
		}
	}
	
	//Changes the skin of the results panel
	private void resultsSkinChanger(){
		
		//Check the current skin and change the components to the proper colors
		if (skin.compareTo("Water") == 0){
			cont.setBackground(Color.cyan);
			result.setForeground(Color.cyan);
			resultsPanel.setBackground(Color.blue);
		}
		else{
			cont.setBackground(Color.orange);
			result.setForeground(Color.orange);
			resultsPanel.setBackground(Color.red);
		}
	}
	
}
