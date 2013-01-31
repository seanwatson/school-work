/**
 * A class for creating graphs consisting of nodes and edges
 */

import java.util.Iterator;
import java.util.Stack;


public class Graph {

	//Class data members
	private Node nodes[];	//Array storing all the nodes in the graph
	private Edge edges[][];	//Adjacency matrix to store all edges between nodes
	
	/**
	 * Constructor for a graph. Initializes n nodes and the adjacency matrix
	 * @param n The number of nodes in the graph
	 */
	public Graph(int n){
		nodes = new Node[n];
		edges = new Edge[n][n];
		
		for (int i=0; i < n; i++){
			nodes[i] = new Node(i);
		}
		
	}
	
	/**
	 * Adds to the graph an edge connecting the given vertices.
	 * The type of the edge is as indicated. The label of the
	 * edge is set to the empty String. Throws a GraphException
	 * if either node does not exist or if the edge is already 
	 * in the graph
	 * @param nodeu First endpoint of the edge to be created
	 * @param nodev Second endpoint of the edge to be created
	 * @param type	Type of the edge to be created
	 * @throws GraphException Throws an exception if either of the nodes do not exist
	 */
	public void insertEdge(Node nodeu, Node nodev, String type) throws GraphException{
		//Make sure the nodes are within the graph
		if (nodeu.getName()>=0 && nodev.getName()>=0 && nodeu.getName()<nodes.length && nodev.getName()<nodes.length){
			
			//Check that the edge doesn't already exist
			if (edges[nodeu.getName()][nodev.getName()] == null && edges[nodev.getName()][nodeu.getName()] == null){
				
				//Insert the edge in node u and node v's rows of the adjacency matrix
				edges[nodeu.getName()][nodev.getName()] = new Edge(nodeu, nodev, type);
				edges[nodev.getName()][nodeu.getName()] = new Edge(nodev, nodeu, type);
			}
			else
				throw new GraphException("Edge already exists");
		}
		else
			throw new GraphException("Node does not exist");
	}

	/** 
	 * Returns the node with the specified name. Throws a 
	 * GraphException if the node does not exist
	 */
	public Node getNode(int u) throws GraphException{
		if (u>=0 && u<nodes.length)
			return nodes[u];
		else
			throw new GraphException("Invalid node index");
	}

	/**
	 * Returns a java iterator storing all the edges incident on the 
	 * specified node. It returns null if the node does not have any
	 * edges incident on it. Throws a GraphException if the node does
	 * not exits.
	 * @param u	Node to be returned
	 * @return The node at index u
	 * @throws GraphException Throws exception if the node does not exist
	 */
	public Iterator<Edge> incidentEdges(Node u) throws GraphException{
		
		//Check if node exists
		if (u.getName()>=0 && u.getName()<nodes.length){
		
			//Create stack for storing incident edges
			Stack<Edge> incident_edges = new Stack<Edge>();
			
			//Search the row of the adjacency matrix corresponding to node u
			for (int i=0; i<nodes.length; i++){
				
				//If an edge exists push it to the stack
				if (edges[u.getName()][i] != null){
					incident_edges.push(edges[u.getName()][i]);
				}
			}
			
			//Return null if no edges were found
			if (incident_edges.isEmpty())
				return null;
			
			//Otherwise return an iterator with all the edges from the stack
			return incident_edges.iterator();
		}
		else
			throw new GraphException("Invalid node index");
	}

	/**
	 * Returns the edge connecting the given vertices. Throws
	 * a GraphException if there is no edge connecting the given
	 * vertices or if u or v do not exist.
	 * @param u First endpoint of the edge
	 * @param v Second endpoint of the edge
	 * @return	The edge connecting the two nodes
	 * @throws GraphException Throws exception if there is no edge or nodes dont exist
	 */
	public Edge getEdge(Node u, Node v) throws GraphException{
		
		//Check if nodes exist
		if (u.getName()>=0 && v.getName()>=0 && u.getName()<nodes.length && v.getName()<nodes.length){
			
			//Check if edge exists
			if (edges[u.getName()][v.getName()] == null)
				throw new GraphException("No edge between specified nodes");
			else
				return edges[u.getName()][v.getName()];
		}
		else
			throw new GraphException("Invalid node index");
	}

	/**
	 * Returns true if u and v are adjacent, false otherwise.
	 * Throws a Graph exception if either vertex does not exist
	 * @param u First endpoint
	 * @param v Second endpoint
	 * @return True if the nodes are adjacent, false otherwise
	 * @throws GraphException Throws exception if the nodes do not exist
	 */
	public boolean areAdjacent(Node u, Node v) throws GraphException{
		
		//Check if nodes exist
		if (u.getName()>=0 && v.getName()>=0 && u.getName()<nodes.length && v.getName()<nodes.length)
			return edges[u.getName()][v.getName()] != null;
		else
			throw new GraphException("Invalid node index");
	}
}
