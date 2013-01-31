import net.rim.device.api.ui.*;
import net.rim.device.api.ui.container.*;
import net.rim.device.api.ui.component.*;
import net.rim.device.api.ui.decor.Background;
import net.rim.device.api.ui.decor.BackgroundFactory;
import net.rim.device.api.ui.menu.SubMenu;

//Creates a screen for a player to enter a puzzle
public class EntryScreen extends MainScreen implements FieldChangeListener{
	
	private ButtonField ok;			//For submitting the puzzle
	private EditField puzzle;		//A text field for entering the puzzle
	private int player;				//Stores the player that is entering a puzzle
	private String category, skin;	//Stores the games category and the current color scheme
	private Font titleFont, puzzleFont;	//Fonts for the title and text field
	private RichTextField title;	//Creates a title for the screen
	
	public EntryScreen(int player, String category){
		super();
		
		this.player = player;		//Store the player this screen is for
		this.category = category;	//Store the category for the game
		
		try
	    {
			//Create the titles font
	        FontFamily ff1 = FontFamily.forName("Arial");
	        titleFont = ff1.getFont(Font.BOLD, 60);
	        
	        //Create the text fields font
	        FontFamily ff2 = FontFamily.forName("Arial");
	        puzzleFont = ff2.getFont(Font.BOLD, 28);                   
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
		
		//Create the screens title
		title = new RichTextField("HANGMAN", RichTextField.TEXT_JUSTIFY_HCENTER);
		title.setFont(titleFont);
		
		//Create a text field for the user to enter their puzzle
		puzzle = new EditField("Player " + Integer.toString(player) + " enter your puzzle. The category is " + category + ":\n", "");
		puzzle.setFont(puzzleFont);
		
		//Create a button for the user to submit their puzzle
		ok = new ButtonField("OK", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
		ok.setChangeListener(this);
		ok.setMargin(50, 0,0, 0);
		
		//Add the fields to the screen
		add(title);
		add(puzzle);
		add(ok);
		
	}

	//Called for the user to submit their puzzles
	public void fieldChanged(Field field, int context) {
		
		if (field == ok){
			
			//Check that the puzzle is a valid length
			if (puzzle.getText().length() > 0 && puzzle.getText().length() < 101){
				
				//Check which player the screen is for
				if (player == 1){
					
					//Get the puzzle they entered and remove their screen
					String player1Puzzle = puzzle.getText();
					UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
					
					//Create a new game screen with their game and push it to the display stack
					GameScreen player1Game = new GameScreen(2,category, player1Puzzle.toUpperCase());
					UiApplication.getUiApplication().pushScreen(player1Game);
					
					//Create a screen for player 2 to enter their puzzle
					EntryScreen player2Entry = new EntryScreen(2,category);
					player2Entry.skinChanger(skin);						//Change its colors to match the current skin
					UiApplication.getUiApplication().pushScreen(player2Entry);	//Push it to the screen
				}
				else{
					
					//Get the puzzle they entered
					String player2Puzzle = puzzle.getText();
					UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
					
					//Take player 1s game off of the display stack to we can put player 2s game underneath it
					GameScreen player1Game = (GameScreen)UiApplication.getUiApplication().getActiveScreen();
					UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
					player1Game.skinChanger(skin);		//Change its colors to match the current scheme
					
					//Create a game with player 2s puzzle
					GameScreen player2Game = new GameScreen(1,category, player2Puzzle.toUpperCase());
					UiApplication.getUiApplication().pushScreen(player2Game);	//Push player 2s game to the screen
					UiApplication.getUiApplication().pushScreen(player1Game);	//Push player 1s game on top of player 2s
					
				}
			}
		}
	}
	
	//Changes the color scheme
	public void skinChanger(String scheme){
		
		//Check the color scheme to change to
		if (scheme.compareTo("Fire") == 0){
			skin = "Fire";	//Store the current scheme
			
			//Change the background color
			VerticalFieldManager main = (VerticalFieldManager)getMainManager();
			Background bg = BackgroundFactory.createSolidBackground(Color.ORANGE);
			main.setBackground(bg);
			
			//Create a new title in the new color and swap it with the current title
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
			
			//Create a new text field in the new color and swap it with the current text field
			EditField newPuzzle = new EditField(puzzle.getLabel(), puzzle.getText()){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.RED);
					super.paint(graphics);
				}
			};
			newPuzzle.setFont(puzzleFont);
			this.replace(puzzle, newPuzzle);
			puzzle = newPuzzle;
			
		}
		else{
			skin = "Water";
			
			//Change the background color
			VerticalFieldManager main = (VerticalFieldManager)getMainManager();
			Background bg = BackgroundFactory.createSolidBackground(Color.BLUE);
			main.setBackground(bg);
			
			
			//Create a new title in the new color and swap it with the current title
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
			
			//Create a new text field in the new color and swap it with the current text field
			EditField newPuzzle = new EditField(puzzle.getLabel(), puzzle.getText()){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.CYAN);
					super.paint(graphics);
				}
			};
			newPuzzle.setFont(puzzleFont);
			this.replace(puzzle, newPuzzle);
			puzzle = newPuzzle;
		}
		
	}
	
	//Menu item for changing the color scheme to "fire"
	private MenuItem fire = new MenuItem("Fire", 100, 10){
		public void run(){
			skinChanger("Fire");
		}
	};
	
	//Menu item for changing the color schem to "water"
	private MenuItem water = new MenuItem("Water", 200, 20){
		public void run(){
			skinChanger("Water");
		}
	};
	
	//Creates a custom menu with an appearance option for changing the colors
	protected void makeMenu( Menu menu, int instance )
    {
		menu.deleteAll();		//Remove all the default menu items
		SubMenu appearance = new SubMenu(null,"Appearance",300,3);	//Create a submenu to store the scheme options
        appearance.add(fire);		//Add the scheme options to the submenu
        appearance.add(water);
        menu.add(appearance);		//Add the submenu as an option in the main menu
        super.makeMenu(menu, instance);	//Create the menu
    };	

}
