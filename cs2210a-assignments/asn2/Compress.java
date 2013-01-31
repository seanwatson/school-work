
/**
 * Compression Algorithm
 */
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;


public class Compress {

	/**
	 * Main method for compressing a file
	 * @param args Input file to be compressed
	 */
	public static void main(String[] args){

		//Make sure the program was given an input file
		if (args.length < 1){
			
			//If no input given, state usage and terminate program
			System.out.println("Usage: java Compress <filename>");
			return;
		}
		
		//Create a dictionary to store strings from the input file
		/*Size 9887 is a good balance between the number of collisions
		and memory required to store the dictionary*/ 
		Dictionary dict = new Dictionary(9887);
		
		try{
			
			//Initialize the dictionary with the character values from 1-256
			for (int i=0; i<256; i++){
				
				String new_key = new String();	//create a string to hold the key to be added
				new_key += (char) i;			//add the current character to the string 
				dict.insert(new DictEntry( new_key , i)); //create a DictEntry with the current key and add it to the dictionary
			}
			
			//If there is any error initializing the dictionary, tell the user and terminate the program
		}catch (DictionaryException e){
			
			System.out.println(e.getMessage());
			System.out.println("Failed to initalize dictionary");
			return;
		}
		
		//Declare input and output steams
		BufferedInputStream input;
		BufferedOutputStream output_buffer;
		
		try {
			
			//Open the input stream
			input = new BufferedInputStream(new FileInputStream(args[0]));
			
			//If there is an error opening the input file tell the user and terminate the program
		} catch (FileNotFoundException e) {
			
			System.out.println("Could not find input file");
			return;
		}
		
		try {
			
			//Open the output stream
			output_buffer = new BufferedOutputStream(new FileOutputStream(args[0] + ".zzz"));
			
			//If there is an error opening the output file tell the user and terminate the program
		} catch (FileNotFoundException e) {
			
			System.out.println("Could not create output file");
			return;
		}
		
		//Initialize a string to store the bytes from the input file
		String input_chars = new String();
		
		try {
			
			//Read each byte in the input file, cast it to a character and add it to the string
			while (input.available() > 0){
				input_chars+= (char) input.read();
			}
			
			//If there is an error reading the bytes from the file tell the user and terminate the program
		} catch (IOException e) {
			
			System.out.println("Error initializing character array");
			return;
		}
		
		//Initialize a counter to keep track of the position in the string of input characters currently being processed
		int char_position = 0;
		
		//Compression algorithm
		while (char_position < input_chars.length()){ //process every character in the input string
			
				String check_key = new String();	//string to store the key to look for in the dictionary
				String output_key = new String();	//string to store the key to be output
				check_key += input_chars.charAt(char_position);	//add the first character that hasn't been processed yet
				
				//Look for the key in the dictionary
				while (dict.find(check_key) != null){
					
					output_key = check_key;	//save the key that was just found so it can be output later
					
					if (char_position < input_chars.length()-1){ //check if the whole input string has been processed
						
						check_key += input_chars.charAt(++char_position); //if not add the next character in the string to be checked
					}
					else{ //if the whole input string has been processed
						
						char_position++; //increase the char_position to break the outer while loop after outputting the last code
						break;	//leave the inner while loop
					}
				}
				
				try {
					
					//Write the largest code found to the output file
					MyOutput output = new MyOutput();
					output.output(dict.find(output_key).code(), output_buffer);
					
					//If there is an error writing to the output tell the user and terminate the program
				} catch (IOException e1) {
					
					System.out.println("Error writing to output file");
					return;
				}
				
				//Add the string that wasn't found in the dictionary to the dictionary
				//if it is not already full and the whole input string has not already been processed
				if (dict.numElements() < 4096 && char_position < input_chars.length()){
					
					try {
						
						dict.insert(new DictEntry(check_key, dict.numElements()));	//insert the key that wasn't found and assign the next available code
					
					//If there is an error tell the user, but do not terminate because the compression will still work but with a lower compression ratio
					} catch (DictionaryException e) {
						
						System.out.println("Error adding to dictionary");
					}
				}
		}
		
		try {
			
			input.close();	//close the input file
			MyOutput output = new MyOutput();
			output.flush(output_buffer); //write any bytes that haven't been output yet
			output_buffer.close(); //close the output file
			
			//If there is any error closing the files tell the user
		} catch (IOException e1) {
			
			System.out.println("Error closing files");
		}
		return;
	}
}
