import net.rim.device.api.ui.*;


public class CS2212HangmanBlackberry extends UiApplication{

	public static void main(String[] args) {
		
		//Create a new instance of the game
		CS2212HangmanBlackberry splash = new CS2212HangmanBlackberry();
		splash.enterEventDispatcher();

	}
	
	public CS2212HangmanBlackberry(){
		
		//Push the first screen
		pushScreen(new SplashScreen());
	}

}
