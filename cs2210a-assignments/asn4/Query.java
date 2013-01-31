/**
 * A class to populate a dictionary and query it
 * using various commands.
 */
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.StringTokenizer;


public class Query {
	
	/**
	 * Main method for populating a dictionary and
	 * performing queries on it
	 * @param args An input file used to populate the dictionary
	 */
	public static void main(String[] args){
		
		//Create a dictionary to store all the words and their definitions
		OrderedDictionary dictionary = new OrderedDictionary();
		
		try{
			
			//Create a reader to read in the input file line by line
			BufferedReader in = new BufferedReader(new FileReader(args[0]));
		    String words = in.readLine();	//Read the first word
		    String definition;				//String to store the definition of the current word
		    
		    while (words != null){			//Read words until the end of the file is reached
		    	
		    	words = words.toLowerCase();	//Convert to lower case to make it case insensitive
		    	definition = in.readLine();		//Read the definition of the word
		    	
		    	//Check if the definition is a picture file
		    	if (definition.endsWith(".gif") || definition.endsWith(".jpg")){
		    		
		    		dictionary.insert(words, definition, 3); //Insert the word with picture type
		    	}
		    	
		    	//Check if the definition is an audio file
		    	else if (definition.endsWith(".wav") || definition.endsWith(".mid")){
		    		
		    		dictionary.insert(words, definition, 2); //Insert the word with audio type
		    	}
		    	
		    	//Otherwise insert the word with a plain text definition
		    	else{
		    		
		    		dictionary.insert(words,definition,1);
		    	}
		    	
				words = in.readLine();		//Read the next word in the file
				
		    			
		    }
		    
		//If there are any errors creating the dictionary print the error and terminate    
		}catch (Exception e){
			
			System.out.println(e.getMessage());
			return;
		}
		
		StringReader keyboard = new StringReader();	//Create a string reader to read commands
		String line, command, word ;	//Strings to read the command and separate the argument
		StringTokenizer commands;		//Create a string tokenizer to separate the argument
		boolean end = true;				//Boolean variable to terminate when "end" is entered
		
		while (end){
			
			//Prompt for a command and store the response
			line = keyboard.read("Enter next command: ");
			commands = new StringTokenizer(line);
			
			//Check if a command was given
			if (commands.hasMoreElements()){
				
				command = commands.nextToken();	//Separate the command
				
			}
			
			//If nothing was entered, store empty string so appropriate message will be displayed
			else command = "";
			
			//Check if an argument was given
			if (commands.hasMoreElements()){
				
				word = commands.nextToken();	//Store the argument
				word = word.toLowerCase();		//Convert it to lower case to make it case
												//insensitive
			}
			
			//If no argument given, set it to null so appropriate message will be displayed
			else word = null;
			
			//Check if the command entered was "define"
			if (command.compareTo("define") == 0){
				
				//If no argument was given, print appropriate message
				if (word == null){
					
					System.out.println("Needs argument");
					
				}
				
				//Check if the definition is a picture
				else if (dictionary.findType(word) == 3){
					
					try{
						
						PictureViewer pic = new PictureViewer(); //Create a viewer to display it
						pic.show(dictionary.findWord(word));	 //Display the image
						
					//If there is an error opening the image print an appropriate message
					}catch (MultimediaException e){
						
						System.out.println("Could not open picture file");
						
					}
				}
				
				//Check if the definition is an audio file
				else if (dictionary.findType(word) == 2){
					
					try{
						
						SoundPlayer sound = new SoundPlayer(); //Create a player to play it
						sound.play(dictionary.findWord(word)); //Play the audio file
						
					//If there is an error opening the audio print an appropriate message
					}catch (MultimediaException e){
						
						System.out.println("Could not open sound file");
						
					}
				}
				
				//Check if the word has a text definition
				else if (dictionary.findType(word) == 1){
					
					//Print the definition
					System.out.println(dictionary.findWord(word));
					
				}
				
				//Otherwise the word isn't in the dictionary so print a message
				else{
					
					System.out.println("That word isn't in the dictionary");
					
				}
				
			}
			
			//Check if the command is "next"
			else if (command.compareTo("next") ==  0){
				
				//If no argument was given, display a message
				if (word == null){
					
					System.out.println("Needs argument");
					
				}
				
				//If the word doesn't have a successor, display a message
				else if (dictionary.successor(word) == ""){
					
					//See if it is the last word in the dictionary
					if (dictionary.findWord(word) != ""){
						
						System.out.println("That is the last word in the dictionary");
					
					}
					
					//Otherwise it isn't in the dictionary
					else{
						
						System.out.println("That word isn't in the dictionary");
						
					}
					
				}
				
				//Otherwise display the successor
				else{
					
					System.out.println(dictionary.successor(word));
					
				}
				
			}
			
			//Check if the command entered was "previous"
			else if (command.compareTo("previous") == 0){
				
				//If no argument was given display a message
				if (word == null){
					
					System.out.println("Needs argument");
					
				}
				
				//If the word doens't have a predecessor display a message
				else if (dictionary.predecessor(word) == ""){
					
					//See if it is the first word in the dictionary
					if (dictionary.findWord(word) != ""){
						
						System.out.println("That is the first word in the dictionary");
					
					}
					
					//Otherwise it isn't in the dictionary
					else{
						
						System.out.println("That word isn't in the dictionary");
						
					}
					
				}
				
				//Otherwise display the predecessor
				else{
					
					System.out.println(dictionary.predecessor(word));
					
				}
				
			}
			
			//Check if the command entered was "delete"
			else if (command.compareTo("delete") == 0){
				
				//If no argument was given display a message
				if (word == null){
					
					System.out.println("Needs argument");
					
				}
				
				//Try to remove the word from the dictionary
				else{
					
					try{
						
						dictionary.remove(word);	//Remove the word
						
					//If the word isn't in the dictionary, print a message
					}catch (DictionaryException e){
						
						System.out.println(e.getMessage());
						
					}
					
				}
				
			}
			
			//Check if the command entered was "list"
			else if  (command.compareTo("list") == 0){
				
				//If no argument was given display a message
				if (word == null){
					
					System.out.println("Needs argument");
					
				}
				
				//Find all the words containing the prefix
				else{
					
					try{
						
						//Insert the prefix into the dictionary
						String prefix = word;
						dictionary.insert(prefix, "temp", 1);
						
						//Find the first successor to the prefix
						word = dictionary.successor(prefix);
						
						//Check if the word starts with the prefix
						while (word.startsWith(prefix)){
							
							System.out.print(word + ", ");		//Print the word
							word = dictionary.successor(word);	//Get the next successor
							
						}
						
						//Remove the prefix that was added
						dictionary.remove(prefix);
						System.out.println("");		//Move to the next line
						
					//If there was a problem inserting or removing the prefix display the error
					}catch (DictionaryException e){
						
						System.out.println(e.getMessage());
						
					}
					
				}
				
			}
			
			//Check if the command entered was "end"
			else if (command.compareTo("end") == 0){
				
				end = false;	//Set the end flag to false to break the loop
				
			}
			
			//If an invalid command was entered display a message
			else{
				
				System.out.println("That isn't a command. Try define, delete, list, next, previous, or end.");
				
			}
			
		}
		
		return;		//Terminate the program
		
	}

}
