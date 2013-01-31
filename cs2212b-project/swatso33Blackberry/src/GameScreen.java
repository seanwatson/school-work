import net.rim.device.api.system.Bitmap;
import net.rim.device.api.system.KeyListener;
import net.rim.device.api.ui.*;
import net.rim.device.api.ui.container.*;
import net.rim.device.api.ui.component.*;
import net.rim.device.api.ui.decor.Background;
import net.rim.device.api.ui.decor.BackgroundFactory;
import net.rim.device.api.ui.menu.SubMenu;

public class GameScreen extends MainScreen implements FieldChangeListener{

	private HorizontalFieldManager puzzleManager, letterManager;	//For displaying the puzzle and letters on one line
	private BitmapField hangman;							//Image of the hangman
	private RichTextField title, subtitle, congrats;		//Text labels for a title, prompt/category and to congratulate the user if they get it right
	private RichTextField[] puzzleLetters, letters;			//Text label arrays that store the puzzle's letters and the letters that can be guessed
	private ButtonField solve, next, submit;				//Buttons to attempt a solve, go to the next screen, and submit an answer
	private int player, wrong;								//Store the player guessing letters and the number of wrong guesses they have
	private String puzzle, skin;							//Store the puzzle to be guessed and the current skin
	private Font titleFont, subtitleFont, lettersFont, puzzleFont;	//Fonts for all the text labels
	private KeyListener keyListener;						//Listener for keyboard input when guessing letters
	private EditField answer;								//Text field for the user to guess an answer
	private boolean playerResult;							//Store whether the player won or lost
	
	public GameScreen(int player, String category, String puzzle){
		
		super();
				
		this.player = player;	//Store the player who is guessing letters
		this.puzzle = puzzle;	//Store the puzzle that has to be guessed
		wrong = 0;				//Initialize the wrong guesses to 0

		try
        {
			//Create a font for the title
            FontFamily ff1 = FontFamily.forName("Arial");
            titleFont = ff1.getFont(Font.BOLD, 45);
            
            //Create a font for the subtitle
            FontFamily ff2 = FontFamily.forName("Arial");
            subtitleFont = ff2.getFont(Font.BOLD, 26);
            
            
            //Create a font for the puzzle letters
            FontFamily ff3 = FontFamily.forName("Arial");
            puzzleFont = ff3.getFont(Font.BOLD, 26);
            
            //Create a font for the letters that can be guessed
            FontFamily ff4 = FontFamily.forName("Arial");
            lettersFont = ff4.getFont(Font.BOLD, 22);
                       
        }
        catch(final ClassNotFoundException cnfe)
        {
            UiApplication.getUiApplication().invokeLater(new Runnable()
            {
                public void run()
                {
                    Dialog.alert("FontFamily.forName() threw " + cnfe.toString());
                }
            });              
        }
		
		//Create a title for the screen
		title = new RichTextField("HANGMAN", RichTextField.TEXT_JUSTIFY_HCENTER);
		title.setFont(titleFont);
		
		//Prompt which user is guessing letters and tell them what the category is
		subtitle = new RichTextField("Player " + player + " is guessing letters. The category is " + category + ".", RichTextField.TEXT_JUSTIFY_HCENTER);
		subtitle.setFont(subtitleFont);
		
		//Create an empty hangman
		Bitmap rawImage = Bitmap.getBitmapResource("start.png");
		Bitmap hangmanImage = new Bitmap(200,270);					//Scale the image so it doesn't take up excessive screen space
		rawImage.scaleInto(0, 0, rawImage.getWidth(), rawImage.getHeight(), hangmanImage, 0, 0, hangmanImage.getWidth(), hangmanImage.getHeight(), Bitmap.FILTER_BOX);
		hangman = new BitmapField(hangmanImage, BitmapField.FIELD_HCENTER);
		hangman.setMargin(5, 0, 0, 0);
	
		//Initialize a manager to display the puzzle letters horizontally
		puzzleManager = new HorizontalFieldManager(Field.FIELD_HCENTER | Manager.HORIZONTAL_SCROLL);
		puzzleLetters = new RichTextField[puzzle.length()];
		
		//Initialize each letter in the puzzle
		for(int i=0; i < puzzle.length(); i++){
			
			//If the letter is a character display it as a blank
			if (puzzle.charAt(i) >= 'A' && puzzle.charAt(i) <= 'Z'){
				puzzleLetters[i] = new RichTextField("_", RichTextField.USE_TEXT_WIDTH);
			}
			
			//If it is anything other than a letter display it as is
			else{
				puzzleLetters[i] = new RichTextField(String.valueOf(puzzle.charAt(i)), RichTextField.USE_TEXT_WIDTH);
			}
			puzzleLetters[i].setFont(puzzleFont);
			puzzleLetters[i].setMargin(5, 5, 10, 5);	//Letter spacing
			puzzleManager.add(puzzleLetters[i]);		//Add the letter to the word
		}
		
		//Create a manager to display the letters that can be guessed horizontally
		letters = new RichTextField[26];
		letterManager = new HorizontalFieldManager(Field.FIELD_HCENTER);
		
		//Initialize each letter
		for(int i=0; i < 26; i++){
			letters[i] = new RichTextField(String.valueOf((char)(65 + i)), RichTextField.USE_TEXT_WIDTH);	//Offsets the index to get the proper ASCII value
			letters[i].setFont(lettersFont);
			letters[i].setMargin(5, 10, 10, 5);		//Letter spacing
			letterManager.add(letters[i]);			//Add it to the horizontal manager with the rest of the letters
		}
		
		//Create a button that lets the user solve the puzzle
		solve = new ButtonField("Solve", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
		solve.setChangeListener(this);
		
		//Add all the fields to the screen
		add(title);
		add(subtitle);
		add(hangman);
		add(puzzleManager);
		add(letterManager);
		add(solve);
		
		//Add a key listener to detect when a user guesses a letter
		keyListener = new MyKeyListener();
		addKeyListener(keyListener);
		
	}
	
	public void fieldChanged(Field field, int context) {
		
		//Called when a user attempts to solve the puzzle
		if (field == solve){
			this.removeKeyListener(keyListener);		//Remove the key listener so they can't guess letters while solving
			
			//Create a text field for them to enter an answer and swap it with the remaining letters
			answer = new EditField("Type your answer: ", "");
			this.replace(letterManager, answer);
			skinChanger(skin);		//Make the text field match the current scheme
			
			//Create a button so they can submit their answer
			submit = new ButtonField("Submit", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
			submit.setChangeListener(this);
			this.replace(solve, submit);	//Swap the solve button with the submit button
		}
		
		//Called when the user submits and answer
		else if (field == submit){
			this.addKeyListener(keyListener);	//Add the key listener back so they can guess more letters if they're wrong
			String guess = answer.getText();	//Store their answer
			
			//Put the remaining letters and the solve button back in case they are wrong
			this.replace(answer, letterManager);
			this.replace(submit, solve);
			
			answer = null;	//Make answer null so the skin changer won't attempt to change its colors on a skin change
			
			//Check if they got the right answer
			if (puzzle.compareTo(guess.toUpperCase()) == 0){
				showLetters();		//Show the puzzle
				finishGame(true);	//Finish the game
			}
			else{
				wrongLetter();	//Count a wrong guess the same way as guessing a wrong letter
			}
		}
		
		//Called after a game is finished to go to the next screen
		else if (field == next){
			String nextScreen = next.getLabel();	//Get the purpose of the button
			
			//Check whether it is the next players turn or if both players have played
			if(nextScreen.compareTo("Next Player") == 0){
				
				//Pop the current players game off the screen
				UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
				
				//Pop off the other players game and change its skin to match the current color scheme
				GameScreen nextPlayer = (GameScreen) UiApplication.getUiApplication().getActiveScreen();
				UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
				nextPlayer.skinChanger(skin);
				
				//Create the results screen and set player 2s result
				ResultsScreen results = new ResultsScreen();
				results.setPlayerResult(2, playerResult);
				
				UiApplication.getUiApplication().pushScreen(results);		//Push the result screen
				UiApplication.getUiApplication().pushScreen(nextPlayer);	//Push the next players game on top of the results
			}
			else{
				
				//Pop off the current players game
				UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
				
				//Get the results screen, set player 1s result, create the result message and make sure the color scheme matches
				ResultsScreen results = (ResultsScreen) UiApplication.getUiApplication().getActiveScreen();
				results.setPlayerResult(1, playerResult);
				results.createResult();
				results.skinChanger(skin);
			}
		}
	}
	
	//Key listener to detect guessed letters
	private class MyKeyListener implements KeyListener{

		//Reads characters pressed by the user
		public boolean keyChar(char key, int status, int time) {
			
			//Check if the key pressed is a letter and has not already been guessed
			if (key >= 'a' && key <= 'z' && letters[((int)Character.toUpperCase(key)) - 65].getText().compareTo(" ") != 0){
				boolean validLetter = false;	//Set to true if the letter is in the puzzle
				boolean moreLetters = false;	//Set to true if there are still letters that haven't been guessed
				
				//Remove the letter from the letters that can be guessed
				letters[((int)Character.toUpperCase(key)) - 65].setText(" ");
				
				//Check every letter in the puzzle against the letter that was guessed
				for (int i = 0; i < puzzle.length(); i++){
					
					//Check if the letter matches the current puzzle letter
					if (Character.toUpperCase(key) == puzzle.charAt(i)){
						
						//Create a new label that replaces the blank with the actual letter
						RichTextField showLetter = new RichTextField(String.valueOf(puzzle.charAt(i)), RichTextField.USE_TEXT_WIDTH);
						showLetter.setFont(puzzleFont);
						puzzleManager.replace(puzzleLetters[i], showLetter);
						puzzleLetters[i] = showLetter;						
						
						validLetter = true;	//Mark the guessed letter as a right guess
						allowSolve(true);	//Allow the solve button to be pressed
					}
					
					//Check if there are still blank letters
					else if (puzzleLetters[i].getText().compareTo("_") == 0){
						moreLetters = true;		//Mark true if there are more letters
					}
				}
				skinChanger(skin);		//Update the colors so that any new letters match the current scheme
				
				if (!moreLetters){
					finishGame(true);	//Finish the game if all the letters have been uncovered
				}
				
				if (!validLetter){		//Do all the wrong letter jazz if the letter was not in the puzzle
					wrongLetter();
				}
			}
			
			return false;
		}

		//Not used for this application but needs to be declared to keep key listener happy
		public boolean keyDown(int keycode, int time) {
			return false;
		}
		//Not used for this application but needs to be declared to keep key listener happy
		public boolean keyRepeat(int keycode, int time) {
			return false;
		}
		//Not used for this application but needs to be declared to keep key listener happy
		public boolean keyStatus(int keycode, int time) {
			return false;
		}
		//Not used for this application but needs to be declared to keep key listener happy
		public boolean keyUp(int keycode, int time) {
			return false;
		}
		
	}

	//Called when the user game ends either because of too many wrong guesses or the puzzle is solved
	private void finishGame(boolean win){
		
		this.removeKeyListener(keyListener);	//Remove the key listener so they can't guess more letters after the game is finished
		
		//If they won give them a pat on the back
		if (win){
			congrats = new RichTextField("You got it!", RichTextField.TEXT_JUSTIFY_HCENTER);
			this.replace(letterManager, congrats);
			playerResult = true;		//Save that they won so it can be shown in the results
		}
		
		//If they lost...
		else{
			showLetters();				//Show the rest of the letters 
			playerResult = false;		//Save that they lost so it can be shown in the results
		}
		
		skinChanger(skin);	//Update the colors so the new text on the screen matches the color scheme
		
		//Check if both players have played or if just one has
		if(player == 2){
			//Create a button so the next player can see their game
			next = new ButtonField("Next Player", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
			next.setChangeListener(this);
		}
		else{
			//Create a button that will show the results screen
			next = new ButtonField("See Results", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
			next.setChangeListener(this);
		}
		
		//Swap the button onto the screen
		this.replace(solve, next);
	}
	
	//Called when the player makes an incorrect guess at a letter
	private void wrongLetter(){
		
		wrong++;	//Increment the number of wrong guesses
		
		//Display the next hangman image with one more body part
		Bitmap rawImage = Bitmap.getBitmapResource(String.valueOf(wrong) + ".png");
		Bitmap newHangmanImage = new Bitmap(200,270);								//Scale the image down so it doesn't take up too much screen space
		rawImage.scaleInto(0, 0, rawImage.getWidth(), rawImage.getHeight(), newHangmanImage, 0, 0, newHangmanImage.getWidth(), newHangmanImage.getHeight(), Bitmap.FILTER_BOX);
		BitmapField newHangman = new BitmapField(newHangmanImage, BitmapField.FIELD_HCENTER);
		newHangman.setMargin(5, 0, 0, 0);
		this.replace(hangman, newHangman);	//Swap the new hangman in
		hangman = newHangman;
		
		allowSolve(false);		//Disable the solve button
		
		//Check if they have reached the maximum number of wrong guesses
		if (wrong == 7){
			finishGame(false);		//Finish the game and mark it as a loss :(
		}
		
	}

	//Method to display the remaining hidden letters if the player loses or solves the puzzle before all letters are displayed
	private void showLetters(){
		
		//Check each letter for whether it is a blank still
		for(int i=0; i < puzzle.length(); i++){
			if (puzzleLetters[i].getText().compareTo("_") == 0){
				
				//Replace the blank with the letter it is hiding
				RichTextField showLetter = new RichTextField(String.valueOf(puzzle.charAt(i)), RichTextField.USE_TEXT_WIDTH);
				showLetter.setFont(puzzleFont);
				puzzleManager.replace(puzzleLetters[i], showLetter);
				puzzleLetters[i] = showLetter;
				
			}
		}
		skinChanger(skin); //Update the color of the new text to match the current color scheme
	}
	
	//Method to enable or disable the solve button
	private void allowSolve(boolean allow){
		if (allow){
			if (solve.getChangeListener() == this.getChangeListener()){
				solve.setChangeListener(this);
			}
		}
		else{
			if (solve.getChangeListener() != this.getChangeListener()){
				solve.setChangeListener(null);
			}
		}
	}
	
	//Changes the color scheme of the screen
	public void skinChanger(String scheme){
		
		//Check which scheme to change to
		if (scheme.compareTo("Fire") == 0){
			skin = "Fire";	//Save the new scheme
			
			//Change the background color
			VerticalFieldManager main = (VerticalFieldManager)getMainManager();
			Background bg = BackgroundFactory.createSolidBackground(Color.ORANGE);
			main.setBackground(bg);
			
			//Create a new title in the new color and swap it with the old title
			RichTextField newTitle = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.RED);
					super.paint(graphics);
				}
			};
			newTitle.setText(title.getText());
			newTitle.setFont(titleFont);
			this.replace(title, newTitle);
			title = newTitle;
			
			//Create a new subtitle in the new color and swap it with the old subtitle
			RichTextField newSubtitle = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.RED);
					super.paint(graphics);
				}
			};
			newSubtitle.setText(subtitle.getText());
			newSubtitle.setFont(subtitleFont);
			this.replace(subtitle, newSubtitle);
			subtitle = newSubtitle;
			
			//Create a new congrats message in the new color and swap it with the old one
			if (congrats != null){
				RichTextField newCongrats = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.RED);
						super.paint(graphics);
					}
				};		
				newCongrats.setText(congrats.getText());
				this.replace(congrats, newCongrats);
				congrats = newCongrats;
			}
			
			//Create a new text field for the answer in the new color and swap it with the old one
			if (answer != null){
				EditField newAnswer = new EditField(answer.getLabel(), answer.getText()){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.RED);
						super.paint(graphics);
					}
				};
				this.replace(answer, newAnswer);
				answer = newAnswer;
			}
			
			//For each puzzle letter create a new letter in the new color and swap it with the old letter
			for(int i=0; i<puzzle.length(); i++){
				RichTextField newPuzzleLetter = new RichTextField(puzzleLetters[i].getText(), RichTextField.USE_TEXT_WIDTH){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.RED);
						super.paint(graphics);
					}
				};
				newPuzzleLetter.setFont(puzzleFont);
				newPuzzleLetter.setMargin(5, 5, 10, 5);
				puzzleManager.replace(puzzleLetters[i], newPuzzleLetter);
				puzzleLetters[i] = newPuzzleLetter;
			}
			
			//For each letter left to be guessed create a new letter in the new color and swap it with the old letter
			for(int i=0; i<26; i++){
				RichTextField newLetter = new RichTextField(letters[i].getText(), RichTextField.USE_TEXT_WIDTH){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.RED);
						super.paint(graphics);
					}
				};
				newLetter.setFont(lettersFont);
				newLetter.setMargin(5, 10, 10, 5);
				letterManager.replace(letters[i], newLetter);
				letters[i] = newLetter;
			}
			
		}
		else{
			skin = "Water";
			
			//Change the background color
			VerticalFieldManager main = (VerticalFieldManager)getMainManager();
			Background bg = BackgroundFactory.createSolidBackground(Color.BLUE);
			main.setBackground(bg);
			
			//Create a new title in the new color and swap it with the old one
			RichTextField newTitle = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.CYAN);
					super.paint(graphics);
				}
			};	
			newTitle.setText(title.getText());
			newTitle.setFont(titleFont);
			this.replace(title, newTitle);
			title = newTitle;
			
			//Create a new subtitle in the new color and swap it with the old one
			RichTextField newSubtitle = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.CYAN);
					super.paint(graphics);
				}
			};
			newSubtitle.setText(subtitle.getText());
			newSubtitle.setFont(subtitleFont);
			this.replace(subtitle, newSubtitle);
			subtitle = newSubtitle;
			
			//Create a new congrats message in the new color and swap it with the old one
			if (congrats != null){
				RichTextField newCongrats = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.CYAN);
						super.paint(graphics);
					}
				};
				newCongrats.setText(congrats.getText());
				this.replace(congrats, newCongrats);
				congrats = newCongrats;
			}
			
			//Createa new text field for the answer in the new color and swap it with the old one
			if (answer != null){
				EditField newAnswer = new EditField(answer.getLabel(), answer.getText()){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.CYAN);
						super.paint(graphics);
					}
				};
				this.replace(answer, newAnswer);
				answer = newAnswer;
			}
			
			//For each puzzle letter create a new letter in the new color and swap it with the old one
			for(int i=0; i<puzzle.length(); i++){
				RichTextField newPuzzleLetter = new RichTextField(puzzleLetters[i].getText(), RichTextField.USE_TEXT_WIDTH){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.CYAN);
						super.paint(graphics);
					}
				};
				newPuzzleLetter.setFont(puzzleFont);
				newPuzzleLetter.setMargin(5, 5, 10, 5);
				puzzleManager.replace(puzzleLetters[i], newPuzzleLetter);
				puzzleLetters[i] = newPuzzleLetter;
			}
			
			//For each letter left to be guessed create a new letter in the new color and swap it with the old one
			for(int i=0; i<26; i++){
				RichTextField newLetter = new RichTextField(letters[i].getText(), RichTextField.USE_TEXT_WIDTH){
					protected void paint(Graphics graphics){
						graphics.setColor(Color.CYAN);
						super.paint(graphics);
					}
				};
				newLetter.setFont(lettersFont);
				newLetter.setMargin(5, 10, 10, 5);
				letterManager.replace(letters[i], newLetter);
				letters[i] = newLetter;
			}
			
		}
		
	}
	
	//Create a menu item to switch the color scheme to "fire"
	private MenuItem fire = new MenuItem("Fire", 100, 10){
		public void run(){
			skinChanger("Fire");
		}
	};
	
	//Create a menu item to switch the color scheme to "water"
	private MenuItem water = new MenuItem("Water", 200, 20){
		public void run(){
			skinChanger("Water");
		}
	};
	
	//Create a custom menu with an option for changing the color scheme
	protected void makeMenu( Menu menu, int instance )
    {
		menu.deleteAll();		//Remove the default menu items
		SubMenu appearance = new SubMenu(null,"Appearance",300,3);	//Create a submenu to hold the scheme options
        appearance.add(fire);	//Add the scheme options to the submenu
        appearance.add(water);
        menu.add(appearance);	//Add the submenu to the main menu
        super.makeMenu(menu, instance);		//Create the menu
    };	
}
