import net.rim.device.api.ui.*;
import net.rim.device.api.ui.container.*;
import net.rim.device.api.ui.component.*;
import net.rim.device.api.ui.decor.Background;
import net.rim.device.api.ui.decor.BackgroundFactory;
import net.rim.device.api.ui.menu.SubMenu;
import net.rim.device.api.system.PersistentObject;
import net.rim.device.api.system.PersistentStore;

//Creates a screen that allows players to select a category and start the game
public class SplashScreen extends MainScreen implements FieldChangeListener{

	private ButtonField begin;							//Button to start the game
	private RadioButtonGroup category;					//Button group for selecting the category
	private RadioButtonField tvShows, movies, phrases;	//Categories to choose from
	private static PersistentObject selectedCategory;	//Stores and reads the category from memory
	private CategoryField lastCategory;					//Object for storing category data
	private String skin;								//Stores the current color scheme
	private RichTextField title, subtitle;				//For displaying the title and prompting to select a category
	private Font titleFont, subtitleFont, categoryFont;	//Fonts for the text
			
	public SplashScreen(){
		super();
		
		//Initialize the persistent object
		selectedCategory = PersistentStore.getPersistentObject(0x12345678);
		
		//Create a title for the screen
		title = new RichTextField("HANGMAN", RichTextField.TEXT_JUSTIFY_HCENTER);
		titleFont = null;
		
		//Create a prompt for the user to select a category
		subtitle = new RichTextField("Choose a category:", RichTextField.TEXT_JUSTIFY_HCENTER);
		subtitleFont = null;
		
		try
        {
			//Initialize the title font
            FontFamily ff1 = FontFamily.forName("Arial");
            titleFont = ff1.getFont(Font.BOLD, 60);
            
            //Initialize the subtitle font
            FontFamily ff2 = FontFamily.forName("Arial");
            subtitleFont = ff2.getFont(Font.BOLD, 32);
            
            //Initialize the category font
            FontFamily ff3 = FontFamily.forName("Arial");
            categoryFont = ff3.getFont(Font.BOLD, 32);
                       
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
		//Set the fonts
		title.setFont(titleFont);
		subtitle.setFont(subtitleFont);
		
		//Create a button group for the categories and create all the categories as radio buttons so the user can select one
		category = new RadioButtonGroup();
		tvShows = new RadioButtonField("TV Shows", category, true, RadioButtonField.FIELD_HCENTER);
		tvShows.setFont(categoryFont);
		movies = new RadioButtonField("Movies", category, false, RadioButtonField.FIELD_HCENTER);
		movies.setFont(categoryFont);
		phrases = new RadioButtonField("Phrases", category, false, RadioButtonField.FIELD_HCENTER);
		phrases.setFont(categoryFont);
		
		//Read the memory to determine the last used category
		synchronized(selectedCategory){
			lastCategory = (CategoryField)selectedCategory.getContents();
			
			//If it is the first time playing set the default to TV Shows
			if (lastCategory == null){
				lastCategory = new CategoryField("TV Shows");
			}
		}
		
		//Set the correct category to selected based on what was read from memory
		if (lastCategory.getCategory().compareTo("TV Shows") == 0){
			category.setSelectedIndex(0);
		}
		else if (lastCategory.getCategory().compareTo("Movies") == 0){
			category.setSelectedIndex(1);
		}
		else{
			category.setSelectedIndex(2);
		}
		
		//Create a button to start the game with the currently selected category
		begin = new ButtonField("Begin", ButtonField.CONSUME_CLICK | ButtonField.FIELD_HCENTER);
		begin.setChangeListener(this);
		begin.setMargin(50,0, 0, 0);
		
		//Add all the fields to the screen
		add(title);
		add(subtitle);
		add(tvShows);
		add(movies);
		add(phrases);
		add(begin);
		
		//Change all the field colors to the default color scheme
		skinChanger("Fire");
	}
	
	//Called when the player begins the game by hitting the "begin" button
	public void fieldChanged(Field field, int context) {
		if (field == begin){
			
			//Pop the splash screen off of the display stack
			UiApplication.getUiApplication().popScreen(UiApplication.getUiApplication().getActiveScreen());
			String cat = "";
			
			//Get the category that the user selected
			if (category.getSelectedIndex() == 0){
				cat = "TV Shows";
			}
			else if (category.getSelectedIndex() == 1){
				cat = "Movies";
			}
			else{
				cat = "Phrases";
			}
			
			//Store the category to memory so it can be recalled next game
			lastCategory.setCategory(cat);
			synchronized(selectedCategory){
				selectedCategory.setContents(lastCategory);
				selectedCategory.commit();
			}
			
			//Create a screen for the first player to enter their puzzle
			EntryScreen player1Entry = new EntryScreen(1,cat);
			player1Entry.skinChanger(skin);						//Change the color scheme to match the current scheme
			UiApplication.getUiApplication().pushScreen(player1Entry);	//Push it to the display stack
		}
		
	}
	
	//Changes the color scheme of the splash screen
	public void skinChanger(String scheme){
		
		//Check which scheme the colors should be set to
		if (scheme.compareTo("Fire") == 0){
			skin = "Fire";	//Store the current scheme so it can be used to change new screens to match it
			
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
			
			//Create a new prompt in the new color and swap it with the old prompt
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
		}
		else{
			skin = "Water";
			
			//Change the background color
			VerticalFieldManager main = (VerticalFieldManager)getMainManager();
			Background bg = BackgroundFactory.createSolidBackground(Color.BLUE);
			main.setBackground(bg);
			
			//Create a new title in the new color and swap it with the old title
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
			
			//Create a new prompt in the new color and swap it with the old prompt
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
		}		
	}
	
	//Menu item that changes the color scheme to "fire"
	private MenuItem fire = new MenuItem("Fire", 100, 10){
		public void run(){
			skinChanger("Fire");
		}
	};
	
	//Menu item that changes the color scheme to "water"
	private MenuItem water = new MenuItem("Water", 200, 20){
		public void run(){
			skinChanger("Water");
		}
	};
	
	//Creates a custom menu with an appearance option to change colors
	protected void makeMenu( Menu menu, int instance )
    {
		menu.deleteAll();			//Remove all default options
		SubMenu appearance = new SubMenu(null,"Appearance",300,3);	//Create submenu appearance
        appearance.add(fire);	//Add the fire and water options to the submenu
        appearance.add(water);
        menu.add(appearance);	//Add the appearance option to the menu
        super.makeMenu(menu, instance);	//Create the menu for the screen
    };
    
}
