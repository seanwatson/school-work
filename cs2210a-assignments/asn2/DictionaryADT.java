public interface DictionaryADT {

    public int insert (DictEntry pair) throws DictionaryException;

    public void remove (String key) throws DictionaryException;

    public DictEntry find (String key);

    public int numElements();

}


