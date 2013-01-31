/**
 * Class that constructs a binary search tree of DictEntry nodes
 * @author Sean Watson
 *
 */
public class OrderedDictionary implements OrderedDictionaryADT{
	
	//Class data members
	//Variables
	private OrderedDictNode root;			//Node to store the root
	private OrderedDictNode currentPos;		//Node to keep track of position in the tree
											//during traversals
	//Constructor initializing an empty tree
	public OrderedDictionary(){
		
		root = null;
		currentPos = root;
		
	}
	
	/**
	 * Returns the definition of the given word, or it returns
     * an empty string if the word is not in the dictionary 
     * 
	 */
	public String findWord (String word){
		
		//If the tree is empty, or the word isn't in the dictionary return empty string
		if (currentPos == null){
			
			return "";
			
		}
		
		//Check if the current node's word matches the word being searched for
		else if (currentPos.getData().word().compareTo(word) == 0){
			
			return currentPos.getData().definition();	//If they match, return the definition
			
		}
		
		//Check if the word should be stored to the left of the current position
		else if (currentPos.getData().word().compareTo(word) > 0){
			
			currentPos = currentPos.getLeftChild();	//Move the current node to the left
			String definition = findWord(word);		//Recursively search for the word
			currentPos = root;						//Reset the current position to the root
			return definition;						//Return the definition
			
		}
		
		//Otherwise search to the right of the current position
		else {
			
			currentPos = currentPos.getRightChild();//Move the current node to the right
			String definition = findWord(word);		//Recursively search for the word
			currentPos = root;						//Reset the current position to root
			return definition;						//Return the definition
			
		}
		 
	}

	/**
	 * Returns the type of the given word, or it returns
	 * -1 if the word is not in the dictionary
	 * 
	 */
	public int findType (String word){
		
		//If the tree is empty or the word is not in the dictionary return -1
		if (currentPos == null){
			
			return -1;
			
		}
		
		//Check if the current node's word matches the word being searched for
		else if (currentPos.getData().word().compareTo(word) == 0){
			
			return currentPos.getData().type();	//Return the current node's type
			
		}
		
		//Check if the word is stored to the left of the current node
		else if (currentPos.getData().word().compareTo(word) > 0){
			
			currentPos = currentPos.getLeftChild();	//Move the current position to the left child
			int type = findType(word);				//Recursively search for the word
			currentPos = root;						//Reset the current position to root
			return type;							//Return the type
			
		}
		
		//Otherwise search to the right of the current node
		else {
			
			currentPos = currentPos.getRightChild();//Move the current position to the right
			int type = findType(word);				//Recursively search for the word
			currentPos = root;						//Reset the current position to the root
			return type;							//Return the type
			
		}
		
	}
	
	/**
	 * Adds the given word, its definition and type into the 
	 * dictionary. It throws an DictionaryException if the 
	 * word is already in the dictionary
	 * 
	 */
	public void insert (String word, String definition, int type) throws DictionaryException{
		
		//Check if the tree is empty
		if (root == null){
			
			//Create a root
			root = new OrderedDictNode(new DictEntry(word, definition, type));
			currentPos = root;
			
		}
		
		//Check if the current node's word matches the word to be added
		else if (currentPos.getData().word().compareTo(word) == 0){
			
			currentPos = root;	//Reset the current position to root
			
			//Indicate that the word is already in the dictionary
			throw new DictionaryException("Word already in Dictionary");
			
		}
		
		//Check if the word should be added to the left of the current node
		else if (currentPos.getData().word().compareTo(word) > 0){
			
			//Check if the left child is a leaf
			if (currentPos.getLeftChild() == null){
				
				//Insert the new word in a new node at the left child
				currentPos.setLeftChild(new OrderedDictNode(currentPos, new DictEntry(word, definition, type)));
			
			}
			
			//Otherwise move the current position to the left and recursively insert
			else{
				
				currentPos = currentPos.getLeftChild();
				insert(word, definition, type);
				currentPos = root;	//Reset the current position to root when finished
				
			}
			
		}
		
		//Otherwise check if the word should be added to the right child
		else{
			
			//Check if the right child is a leaf
			if (currentPos.getRightChild() == null){
				
				//Insert the new word in a new node to the right of the current node
				currentPos.setRightChild(new OrderedDictNode(currentPos, new DictEntry(word, definition, type)));
			
			}
			
			//Otherwise move the current node to the right and recursively insert
			else{
				
				currentPos = currentPos.getRightChild();
				insert(word, definition, type);
				currentPos = root;	//Reset the current position to root when finished
				
			}
			
		}
		
	}
	   
	/**
	 * Removes the entry with the given word from the dictionary.
	 * It throws an OrderedDictionaryException if the word is not
	 * in the dictionary
	 * 
	 */
	public void remove (String word) throws DictionaryException{
		
		//Find the node containing the word to be removed
		OrderedDictNode toRemove = find(word);
		
		//If no such node exists the word isn't in the dictionary
		if (toRemove == null){
			
			throw new DictionaryException("Word not in dictionary");
			
		}
		
		//Otherwise remove the node from the tree
		else{
			
			OrderedDictNode toReplace;
			OrderedDictNode toReplaceChild;
			
			//Check if either child of the node is a leaf
			if (toRemove.getLeftChild() == null || toRemove.getRightChild() == null){
				
				toReplace = toRemove;	//Set the node to replace the node with to itself
				
			}
			
			//Otherwise the node to replace it with will be its successor
			else{
				
				toReplace = successor(toRemove);
				
			}
			
			//If the node to replace it has a left child, store the left child
			if (toReplace.getLeftChild() != null){
				
				toReplaceChild = toReplace.getLeftChild();
				
			}
			
			//Otherwise store the right child
			else{
				
				toReplaceChild = toReplace.getRightChild();
				
			}
			
			//Check if both of the replacement node's children were null
			if (toReplaceChild != null){
				
				//Set toReplaceChild to the parent's parent
				toReplaceChild.setParent(toReplace.getParent());	 
				
			}
			
			//If the replacement node is the root, set the root to the replacement
			if (toReplace.getParent() == null){
				
				root = toReplaceChild;
				
			}
			
			//Otherwise replace the node to be removed with the node to replace it
			else {
				
				//If the replacement node is the left child of its parent
				//replace its parent's left child with it
				if (toReplace == toReplace.getParent().getLeftChild()){
					
					toReplace.getParent().setLeftChild(toReplaceChild);
					
				}
				
				//Otherwise it is its parent's right node so replace
				//its parent's right node with it
				else{
					
					toReplace.getParent().setRightChild(toReplaceChild);
					
				}
				
			}
			
			//Copy the data to the node
			if (toReplace != toRemove){
				
				toRemove.setData(toReplace.getData());
				
			}
			
		}
		
	}

	/**
	 * Returns the successor of the given word (the word from the
	 * dictionary that lexicographically follows the given one); it
	 * returns an empty string if the given word has no successor
	 *
	 */
	public String successor (String word){
		
		//Find the node containing the word, find its successor and store it
		OrderedDictNode succ = successor(find(word));
		
		if (succ == null) return "";		//If there is no successor, return empty string
		else return succ.getData().word();	//Otherwise return the successor's word
		
	}

	/**
	 * Returns the predecessor of the given word (the word from the
	 * dictionary that lexicographically precedes the given one); it
	 * returns an empty string if the given word has no predecessor
	 *
	 */
	public String predecessor (String word){
		
		//Find the node containing the word, find its predecessor and store it
		OrderedDictNode pred = predecessor(find(word));
		
		if (pred == null) return "";		//If there is no predecessor, return empty string
		else return pred.getData().word();	//Otherwise return the predecessor's word
	}

	/**
	 * Finds and returns the node containing the word passed as a parameter
	 * or returns null if there is no node containing that word
	 * 
	 * @param word Word to search for in nodes
	 * @return The node containing the word in its DictEntry element
	 */
	private OrderedDictNode find(String word){
		
		//Begin the search at the root
		OrderedDictNode checkNode = root;
		
		//Continue searching until the node is found or a leaf is reached
		while (checkNode != null && checkNode.getData().word().compareTo(word) != 0){
			
			//Check if the current position should be moved to the left
			if (checkNode.getData().word().compareTo(word) > 0){
				
				checkNode = checkNode.getLeftChild();	//Move the current position to the left
				
			}
			
			//Otherwise move the current position to the right
			else{
				
				checkNode = checkNode.getRightChild();
				
			}
			
		}
		
		return checkNode;	//Return the node found
		
	}
	
	/**
	 * Returns the successor of a node based on it's word
	 * or null if there is no successor
	 * 
	 * @param toFind A node of which to find the successor
	 * @return The node containing the word that succeeds the word of the passed node
	 */
	private OrderedDictNode successor(OrderedDictNode toFind){
		
		//Check if a node was passed
		if (toFind == null) return null;	//Return null if no node passed
		
		//Check if the node has a right child
		if (toFind.getRightChild() != null){
			
			toFind = toFind.getRightChild();		//Move to the right child
			
			//Find that node's left most child
			while (toFind.getLeftChild() != null){
				
				toFind= toFind.getLeftChild();
				
			}
			
			//Return the node that was found
			return toFind;
		}
		
		//Otherwise look for the successor in the node's parents
		else{
			
			//Create a new node to store the current parent
			OrderedDictNode toFindParent = toFind.getParent();
			
			//Continue ascending the tree until the root is reached, or the last node was a left child
			while (toFindParent != null && toFind == toFindParent.getRightChild()){
				
				toFind = toFindParent;						//Move the current node to its parent
				toFindParent = toFindParent.getParent();	//Move the parent to its parent
			}
			
			//Return the node that was found
			return toFindParent;
			
		}
		
	}
	
	/**
	 * Returns the predecessor node based on the word or 
	 * null if it does not have a predecessor
	 * 
	 * @param toFind A node to find the predecessor of
	 * @return The predecessor of the node or null if one does not exist
	 */
	private OrderedDictNode predecessor(OrderedDictNode toFind){
		
		//Check if a node was passed
		if (toFind == null) return null;	//Return null if no node was passed
		
		//Check if the node has a left child
		if (toFind.getLeftChild() != null){
			
			toFind = toFind.getLeftChild();		//Move to its left child
			
			//Find the rightmost child of that node
			while (toFind.getRightChild() != null){
				
				toFind = toFind.getRightChild();
				
			}
			
			//Return the node that was found
			return toFind;
			
		}
		
		//Otherwise look for the predecessor in the node's parents
		else{
			
			//Create a node to store the current node's parent
			OrderedDictNode toFindParent = toFind.getParent();
			
			//Continue ascending the tree until the root is reached or the last node was a right child
			while (toFindParent != null && toFind == toFindParent.getLeftChild()){
				
				toFind = toFindParent;						//Move the current position to its parent
				toFindParent = toFindParent.getParent();	//Move the parent to its parent
				
			}
			
			//Return the node that was found
			return toFindParent;
			
		}
		
	}
	
}
