/**
 * This class implements nodes that are used to construct
 * a binary search tree in OrderedDictionary
 * @author Sean Watson
 *
 */
public class OrderedDictNode {

	//Class data members
	//Variables
	private OrderedDictNode parent;		//A node to store this node's parent
	private OrderedDictNode leftChild;	//A node to store this node's left child
	private OrderedDictNode rightChild;	//A node to store this node's right child
	private DictEntry data;				//The data member of the node storing a DictEntry
	
	/**
	 * A constructor for constructing a root node
	 * @param data The data element to be stored in the root node
	 */
	public OrderedDictNode(DictEntry data){
		parent = null;
		leftChild = null;
		rightChild = null;
		this.data = data;
	}
	
	/**
	 * A constructor for constructing nodes other than the root
	 * @param parent The parent of the node being constructed
	 * @param data The data element to be stored in the node
	 */
	public OrderedDictNode(OrderedDictNode parent, DictEntry data){
		this.parent = parent;
		this.data = data;
		leftChild = null;
		rightChild = null;
	}
	
	/**
	 * Returns the data element of the node
	 * @return The DictEntry stored in the node
	 */
	public DictEntry getData(){
		return data;
	}
	
	/**
	 * Returns the parent of the node
	 * @return The parent of the node
	 */
	public OrderedDictNode getParent(){
		return parent;
	}
	
	/**
	 * Returns the left child of the node
	 * @return The left child of the node
	 */
	public OrderedDictNode getLeftChild(){
		return leftChild;
	}
	
	/**
	 * Returns the right child of the node
	 * @return The right child of the node
	 */
	public OrderedDictNode getRightChild(){
		return rightChild;
	}
	
	/**
	 * Used to set the data element of the node to a new DictEntry
	 * @param newData The new DictEntry to store in the node
	 */
	public void setData(DictEntry newData){
		data = newData;
	}
	
	/**
	 * Used to set the parent of the node to a new parent
	 * @param newParent The new parent of the node
	 */
	public void setParent(OrderedDictNode newParent){
		parent = newParent;
	}
	
	/**
	 * Used to set the left child of the node
	 * @param newLeftChild The new left child of the node
	 */
	public void setLeftChild(OrderedDictNode newLeftChild){
		leftChild = newLeftChild;
	}
	
	/**
	 * Used to set the right child of the node
	 * @param newRightChild The new right child of the node
	 */
	public void setRightChild(OrderedDictNode newRightChild){
		rightChild = newRightChild;
	}
	
}
