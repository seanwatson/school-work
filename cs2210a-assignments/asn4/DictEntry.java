/**
 * A data structure for storing words along with their definition and
 * file type.
 * 
 * @author Sean Watson
 *
 */
public class DictEntry {
	
	//Class data members
	//Variables
	private String word;		//String to hold the key word
	private String definition;	//String to hold the definition of the word
	private int type;			//Integer representing the file type of definition
	
	/**
	 * A constructor to initialize a DictEntry with a word, definition
	 * and type
	 * 
	 * @param word The word described by the definition
	 * @param definition The definition describing the word
	 * @param type A type representing the file type of the definition
	 */
	public DictEntry(String word, String definition, int type){
		
		this.word = word;
		this.definition = definition;
		this.type = type;
		
	}
	
	/**
	 * Returns the value of the DictEntry's word
	 * @return The word stored in the DictEntry
	 */
	public String word(){
		
		return word;
		
	}
	
	/**
	 * Returns the value of the DictEntry's definition
	 * @return The definition of the DictEntry's word
	 */
	public String definition(){
		
		return definition;
		
	}
	
	/**
	 * Returns the value of the DictEntry's file type
	 * @return The file type of the definition
	 */
	public int type(){
		
		return type;
		
	}

}
