
/**
 * A data structure for storing and matching strings to a given integer code
 * @author Sean
 *
 */
public class DictEntry {

	//Class data members
	//variables
	private String entry_key; //string to hold the key
	private int entry_code;	//integer to hold the corresponding code

	/**
	 * A constructor which returns a new DictEntry with the specified key and
	 * code.
	 * 
	 * @param key The entry's key
	 * @param code The code corresponding to the key
	 */
	public DictEntry(String key, int code) {
		entry_key = key;
		entry_code = code;
	}
	
	/**
	 * Returns the value of the key.
	 * 
	 * @return The key
	 */
	
	public String getKey(){
		return entry_key;
	}
	
	/**
	 * Returns the code
	 * @return The code
	 */
	public int code(){
		return entry_code;
	}
}
