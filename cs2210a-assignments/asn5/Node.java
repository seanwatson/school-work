/**
 * Data structure for storing nodes of a graph
 * @author Sean
 *
 */
public class Node {
	
	//Class data members
	private int name;		//Name of the node to distinguish it from other nodes
	private boolean mark;	//A marker to represent if the node has been searched
	
	/**
	 * Constructor for the node class. Initializes with a name and sets mark to false
	 * @param name The name of the node being created
	 */
	public Node (int name){
		this.name = name;
		mark = false;
	}
	
	/**
	 * Sets the value of the node's marker
	 * @param mark Value to set the marker to
	 */
	public void setMark(boolean mark){
		this.mark = mark;
	}
	
	/**
	 * Returns the value of the node's marker
	 * @return Returns mark's value
	 */
	public boolean getMark(){
		return mark;
	}
	
	/**
	 * Returns the value of the node's name
	 * @return The name of the node
	 */
	public int getName(){
		return name;
	}

}
