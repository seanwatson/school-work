
/**
 * Dictionary to store strings and unique codes.
 * Used for file compression.
 */

import java.util.LinkedList;

public class Dictionary implements DictionaryADT{

	//Class data members
	//variables
	private LinkedList<DictEntry>[] table; //hash table with linked lists for separate chaining
	private int numberElements; //counter to keep track of the number of keys in the dictionary
	
	/**
	 * Constructor for initializing a dictionary
	 * @param size Size of the hash table to be constructed
	 */
	@SuppressWarnings("unchecked")
	public Dictionary(int size){
		
		table = (LinkedList<DictEntry>[]) new LinkedList[size]; //create an array of linked lists of the given size
		numberElements=0; //set the number of keys to 0
	}
	
	/**
	 * Insert a new key and code pair to the dictionary
	 */
	public int insert (DictEntry pair) throws DictionaryException{
		
		int index = hash(pair.getKey());	//use the hash function to find where to insert the new key in the table
		
			//Check if there is a collision
			if (table[index] != null){	
				
				//If there is a collision search the linked list at that location for the key
				for (int i=0; i<table[index].size(); i++){ 
					
					if (pair.getKey().compareTo(table[index].get(i).getKey()) == 0){
						
						throw new DictionaryException("Key already exists in table"); //if the key is already in the dictionary throw an exception
					}
				}
				
				//If it isn't in the dictionary already add it to the end of the linked list at the specified location
				table[index].addLast(pair);
				numberElements += 1;	//increment the number of elements
				return 1;
			}
			
			//If there is no collision create a linked list
			else{	
				
				LinkedList<DictEntry> new_list = new LinkedList<DictEntry>();
				new_list.addFirst(pair);	//add the pair to the linked list
				table[index] = new_list;	//add the linked list to the proper location in the table
				numberElements += 1;	//increment the number of keys in the dictionary
				return 0;	
			}
	}
	
	/**
	 * Removes the specified key from the dictionary
	 */
	public void remove(String key) throws DictionaryException{
		
		//Use the hash function to find where the key is stored
		int index=hash(key);
		
			//If there are no keys stored at that location throw an exception
			if (table[index]==null){
				
				throw new DictionaryException("Not Found");
			}
			
			//Check the entire linked list at that location for the key
			for (int i=0; i<table[index].size(); i++){
				
				if (key.compareTo(table[index].get(i).getKey()) == 0){
					
					table[index].remove(i); //if the key is found remove the node in the list that holds it
					numberElements -= 1;	//decrement the number of keys in the dictionary
					return;
				}
			}
			
			//If the key isn't found throw an exception
			throw new DictionaryException("Not Found");
	}
	
	/**
	 * Finds a specified key in the dictionary and returns the element
	 */
	public DictEntry find(String key){
		
		int index=hash(key); //use the hash function to find the proper location in the table
		
		//If there is no keys at that location they key isn't in the table
		if (table[index]==null){
			
			return null;
		}
		
		//If there are keys at that location look through the linked list for the specified key
		for (int i=0; i<table[index].size(); i++){
			
			//Check if the key at the current node matches the key you're looking for
			if ( 0 == key.compareTo(table[index].get(i).getKey())){
				
				return table[index].get(i); //Return the entry if it matches
			}
		}
		
		//If it isn't found return null
		return null;
	}
	
	/**
	 * Returns the number of keys stored in the dictionary
	 */
	public int numElements(){
		return numberElements;
	}
	
	/**
	 * Hash function that returns a location within the table corresponding to the key
	 * @param key Key to be put through hash function
	 * @return	Location in table corresponding to the key
	 */
	private int hash(String key){
		
		int character_sum = (int)key.charAt(0); //Start the sum with the first character
		
		//Sum all the characters in the string according to a polynomial hash code
		for (int i=1; i<key.length(); i++){
			
			character_sum += ((int)key.charAt(i) * (37^i));
		}
		
		//Mod by the length of the table to get an integer within the range of the table's size and return the value
		return (character_sum % table.length);	
	}
}
