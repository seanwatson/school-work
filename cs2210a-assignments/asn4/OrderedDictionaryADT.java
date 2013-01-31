public interface OrderedDictionaryADT {
    /* Ordered Dictionary ADT */

    public String findWord (String word);
                   /* Returns the definition of the given word, or it returns
                      an empty string if the word is not in the dictionary.    */

    public int findType (String word);
                   /* Returns the type of the given word, or it returns
                      -1 if the word is not in the dictionary.    */

    public void insert (String word, String definition, int type) 
	                                               throws DictionaryException;
                  /* Adds the given word, its definition and type into the 
		     dictionary. It throws an OrderedDictionaryException if the 
		     word is already in the dictionary. */

    public void remove (String word) throws DictionaryException;
                 /*  Removes the entry with the given word from the dictionary.
                     It throws an OrderedDictionaryException if the word is not
                     in the dictionary.   */

    public String successor (String word);
                 /* Returns the successor of the given word (the word from the
                    dictionary that lexicographically follows the given one); it
                    returns an empty string if the given word has no successor. */

    public String predecessor (String word);
                 /* Returns the predecessor of the given word (the word from the
                    dictionary that lexicographically precedes the given one); it
                    returns an empty string if the given word has no predecessor. */

}
		


