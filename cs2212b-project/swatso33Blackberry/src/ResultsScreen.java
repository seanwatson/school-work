import net.rim.device.api.ui.*;
import net.rim.device.api.ui.container.*;
import net.rim.device.api.ui.component.*;
import net.rim.device.api.ui.decor.Background;
import net.rim.device.api.ui.decor.BackgroundFactory;
import net.rim.device.api.ui.menu.SubMenu;

//Creates a screen to display the results of the game and allow the user to quit or play again
public class ResultsScreen extends MainScreen implements FieldChangeListener{

	private ButtonField newGame, quit;			//Buttons to let the user quit or play again
	private RichTextField results, title;		//Text labels for showing a title and the results
	private boolean player1Result, player2Result;	//Store the results from each players game
	private String skin;						//Stores the current skin
	private Font titleFont, resultsFont;		//Fonts for formating the text labels
	
	public ResultsScreen(){
		
		player1Result = false;		//Initialize the players results
		player2Result = false;
		results = new RichTextField();	//Initialize the results text
		
		try
	    {
			//Create a font for the title
	        FontFamily ff1 = FontFamily.forName("Arial");
	        titleFont = ff1.getFont(Font.BOLD, 60);
	        
	        //Create a font for the results
	        FontFamily ff2 = FontFamily.forName("Arial");
	        resultsFont = ff2.getFont(Font.BOLD, 28);                   
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
		
		//Create the title
		title = new RichTextField("HANGMAN", RichTextField.TEXT_JUSTIFY_HCENTER);
		title.setFont(titleFont);
		
		//Create a button that begins a new game
		newGame = new ButtonField("New Game", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
		newGame.setChangeListener(this);
		newGame.setMargin(50, 0, 10, 0);
		
		//Create a button to quit the game
		quit = new ButtonField("Quit", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
		quit.setChangeListener(this);
		
		//Add all the fields to the screen
		this.add(title);
		this.add(results);
		this.add(newGame);
		this.add(quit);
	}
	
	public void fieldChanged(Field field, int context) {
		
		//Called if the user wants to start a new game
		if (field == newGame){
			//Pop the results screen off
			UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
			
			//Create a new starting screen
			SplashScreen splash = new SplashScreen();
			splash.skinChanger(skin);	//Change its colors to match the current scheme
			UiApplication.getUiApplication().pushScreen(splash);	//Push it to the display
		}
		
		//Close the game if the user hits the quit button
		else if (field == quit){
			System.exit(0);
		}
		
	}
	
	//Method for setting the players results after their game in finished
	public void setPlayerResult(int player, boolean result){
		if (player == 1){
			player1Result = result;
		}
		else{
			player2Result = result;
		}
	}
	
	//Method that creates the proper result text after both games have finished
	public void createResult(){
		if (player1Result && player2Result){
			results.setText("Both Player 1 and Player 2 won!");
		}
		else if (!player1Result && !player2Result){
			results.setText("Both Player 1 and Player 2 lost!");
		}
		else if (player1Result && !player2Result){
			results.setText("Player 1 won!");
		}
		else{
			results.setText("Player 2 won!");
		}
	}
	
	//Changes the color scheme
	public void skinChanger(String scheme){
		
		//Check which color scheme to change to
		if (scheme.compareTo("Fire") == 0){
			skin = "Fire";	//Store the current color scheme
			
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
			
			//Create a new results message in the new color and swap it with the current results message
			RichTextField newResults = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.RED);
					super.paint(graphics);
				}
			};
			newResults.setText(results.getText());
			newResults.setFont(resultsFont);
			this.replace(results, newResults);
			results = newResults;
		}
		else{
			skin = "Water";	//Store the current skin
			
			//Change the background color
			VerticalFieldManager main = (VerticalFieldManager)getMainManager();
			Background bg = BackgroundFactory.createSolidBackground(Color.BLUE);
			main.setBackground(bg);
			
			//Create a new title in the proper color and swap it with the current title
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
			
			//Create a new results message in the right color and swap it with the current results message
			RichTextField newResults = new RichTextField(RichTextField.TEXT_JUSTIFY_HCENTER){
				protected void paint(Graphics graphics){
					graphics.setColor(Color.CYAN);
					super.paint(graphics);
				}
			};
			newResults.setText(results.getText());
			newResults.setFont(resultsFont);
			this.replace(results, newResults);
			results = newResults;
		}	
	}
	
	//Create a menu item to switch to the "fire" scheme
	private MenuItem fire = new MenuItem("Fire", 100, 10){
		public void run(){
			skinChanger("Fire");
		}
	};
	
	//Create a menu item to switch to the "water" scheme
	private MenuItem water = new MenuItem("Water", 200, 20){
		public void run(){
			skinChanger("Water");
		}
	};
	
	//Create a custom menu with an option to change colors
	protected void makeMenu( Menu menu, int instance )
    {
		menu.deleteAll();	//Remove the default menu options
		SubMenu appearance = new SubMenu(null,"Appearance",300,3);	//Create a submenu to hold the color options
        appearance.add(fire);	//Add the color options to the submenu
        appearance.add(water);
        menu.add(appearance);	//Add the submenu to the main menu
        super.makeMenu(menu, instance);	//Create the menu
    };

}
