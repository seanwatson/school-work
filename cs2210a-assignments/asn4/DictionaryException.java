/**
 * Used to throw exceptions caused by the insert and remove methods
 * of the OrderedDictionary class
 * @author Sean Watson
 *
 */
@SuppressWarnings("serial")
public class DictionaryException extends Exception {
	
	public DictionaryException(String msg){
		super(msg);
	}

}
