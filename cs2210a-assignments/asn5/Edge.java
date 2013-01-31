/**
 * Data structure for storing the edges of a graph
 * @author Sean
 *
 */
public class Edge {

	//Class data members
	private Node u, v;			//The endpoints of the edge
	private String type, label;	//A type for wall or hall and a label for discovery
	
	/**
	 * Constructor for the edge class. Sets the endpoints and the type
	 * @param u First endpoint of the edge
	 * @param v Second endpoint of the edge
	 * @param type Type of the edge (hall or wall)
	 */
	public Edge(Node u, Node v, String type){
		this.u = u;
		this.v = v;
		this.type = type;
		label = "";
	}
	
	/**
	 * Returns the edge's first endpoint
	 * @return The first endpoint
	 */
	public Node firstEndpoint(){
		return u;
	}
	
	/**
	 * Returns the edges's second endpoint
	 * @return The second endpoint
	 */
	public Node secondEndpoint(){
		return v;
	}
	
	/**
	 * Returns the edge's type
	 * @return The type of the edge
	 */
	public String getType(){
		return type;
	}
	
	/**
	 * Sets the label of the edge
	 * @param label Value to set label to
	 */
	public void setLabel(String label){
		this.label = label;
	}
	
	/**
	 * Returns the value of the edge's label
	 * @return The label of the edge
	 */
	public String getLabel(){
		return label;
	}
	
}
