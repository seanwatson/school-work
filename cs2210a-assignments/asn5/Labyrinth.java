/**
 * Implements a labyrinth based on an input file using a graph with nodes and edges
 */

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.Iterator;
import java.util.Stack;


public class Labyrinth {
	
	//Class data members
	private Graph lab;	//A graph representing the labyrinth
	private int bombs;	//The number of bombs available for use
	private Node start, end;	//The start and end of the labyrinth
	private Stack<Node> stack;	//A stack for storing a path from start to end
	
	/**
	 * Constructor for the labyrinth. Reads an input file and inserts edges into
	 * the graph to represent all halls and walls of the labyrinth. Also sets the
	 * start and end nodes of the labyrinth and the number of bombs available.
	 * Throws a LabyrinthException if any errors in reading or initialization are encountered
	 * @param inputFile File to read labyrinth information from
	 * @throws LabyrinthException Throws exception if any errors are caught.
	 */
	public Labyrinth(String inputFile) throws LabyrinthException{
		
		try {
			
			//Initialize the stack
			stack = new Stack<Node>();
			
			//Initialize a reader to read the input file
			BufferedReader input = new BufferedReader(new FileReader(inputFile));
			String line = input.readLine();	//Read the first line but do nothing with it
			
			//Read the width from the input file
			line = input.readLine();
			int width = Integer.parseInt(line);
			
			//Read the length from the input file
			line = input.readLine();
			int length = Integer.parseInt(line);
			
			//Initialize the graph with the number of nodes in the labyrinth
			lab = new Graph(width * length);
			
			//Read the number of bombs from the input file
			line = input.readLine();
			bombs = Integer.parseInt(line);
			
			line = input.readLine();
			
			//Read the entire length of the labyrinth
			for(int i = 0; i < (length*2)-1; i++){
				
				//Read the entire width of the labyrinth
				for(int j = 0; j < (width*2)-1; j++){
					
					//If it is an even numbered row
					if (i%2 == 0){
						
						//and an even numbered column, the char represents a node.
						if (j%2 == 0){
							
							//so check if that node is the start or the end.
							if(line.charAt(j) == 's'){
								start = lab.getNode((i/2*width) + (j/2));
							}
							else if(line.charAt(j) == 'e'){
								end = lab.getNode((i/2*width) + (j/2));
							}
						}
						
						//and an odd numbered column, the char represents a hall or wall
						else{
							
							//so check if it is a hall or wall and insert the proper edge to represent it
							if(line.charAt(j) == '-'){
								lab.insertEdge(lab.getNode((i/2*width) + ((j-1)/2)), lab.getNode((i/2*width) + ((j+1)/2)), "hall");
							}
							else if(line.charAt(j) == 'h'){
								lab.insertEdge(lab.getNode((i/2*width) + ((j-1)/2)), lab.getNode((i/2*width) + ((j+1)/2)), "wall");
							}
						}
					}
					
					//If it is an odd numbered row
					else{
						
						//and an even numbered column, the char represents a hall or a wall
						if (j%2 == 0){
							
							//so check if it is a hall or a wall and insert the proper edge to represent it
							if (line.charAt(j) == '|'){
								lab.insertEdge(lab.getNode(((i-1)/2*width) + (j/2)), lab.getNode(((i+1)/2*width) + (j/2)), "hall");
							}
							else if (line.charAt(j) == 'v'){
								lab.insertEdge(lab.getNode(((i-1)/2*width) + (j/2)), lab.getNode(((i+1)/2*width) + (j/2)), "wall");
							}
						}
					}
				}
				
				//After each row has been processed, read the next row
				line = input.readLine();
			}
			
		//Catch any errors and throw a LabyrinthException
		} catch (Exception e) {	
			throw new LabyrinthException("Problem with input file");
		}
	}

	/**
	 * Returns the graph representing the labyrinth
	 * @return The graph representing the labyrinth
	 * @throws LabyrinthException Throws an exception if the graph isn't initialized
	 */
	public Graph getGraph() throws LabyrinthException{
		if (lab != null)
			return lab;
		else
			throw new LabyrinthException("Graph not initalized");
	}
	
	/**
	 * Solves the labyrinth by finding a path from the start node
	 * to the end node using less than or equal to the allowed number
	 * of bombs
	 * @return An iterator storing the nodes along the path from the start to the end node
	 */
	public Iterator<Node> solve(){
		
		//Start looking for a path from the start node
		path(start);
		
		//Return null if no path was found
		if (stack.empty())
			return null;
		return stack.iterator();
	}
	
	/**
	 * A depth first search algorithm for finding the path from one node to the end node
	 * @param u The node to start the DFS from
	 * @return True if a path was found, false otherwise
	 */
	private boolean path(Node u){
		
		//Push the current node on to the path stack
		stack.push(u);
		
		//If the current node is the end, a path has been found
		if (u == end){
			return true;
		}
		
		else{
			
			//Otherwise mark the node as having been visited
			u.setMark(true);
			try {
				
				Iterator<Edge> incident_edges = lab.incidentEdges(u);
				
				//Search along every edge incident on u
				while(incident_edges.hasNext()){
					Edge discovery = incident_edges.next();
					
					//Check if the edge is the path that was taken to get to the current node
					if (discovery.getLabel()!="discovery"){
						
						//Check if the edge is a hall or a wall
						if (discovery.getType() == "hall"){
							
							//Check if the other endpoint of the edge has already been visited
							if (!discovery.secondEndpoint().getMark()){
								
								//Label both edges in the matrix as discovery and recursively search the
								//second endpoint
								discovery.setLabel("discovery");
								lab.getEdge(discovery.secondEndpoint(), u).setLabel("discovery");
								if (path(discovery.secondEndpoint()))
									return true;
							}	
						}
						else{
							
							//If it is a wall, check if there is still bombs left
							if (bombs > 0){
								
								//Check if the second endpoint has already been visited
								if (!discovery.secondEndpoint().getMark()){
									
									//Label both edges in the matrix, subtract one bomb for going through the wall
									//and recursively search the second endpoint
									discovery.setLabel("discovery");
									lab.getEdge(discovery.secondEndpoint(), u).setLabel("discovery");
									bombs--;
									if(path(discovery.secondEndpoint()))
										return true;
								}	
							}
						}
					}
				}
				
				//If a dead end is reached, undo all of the labeling and marking
				Iterator<Edge> find_predecessor = lab.incidentEdges(u);
				
				//Find the path that was taken to get to the current node
				while(find_predecessor.hasNext()){
					Edge predecessor = find_predecessor.next();
					if (predecessor.getLabel() == "discovery"){
						
						//Set both edges in the matrix back to null
						predecessor.setLabel(null);
						lab.getEdge(predecessor.secondEndpoint(), u).setLabel(null);	

						//If it was a wall, get a bomb back
						if (predecessor.getType() == "wall"){
							bombs++;
						}
						break;
					}
				}
				
				//Reset the current node's mark to false
				u.setMark(false);
				stack.pop();	//Remove it from the path stack
				return false;	//Return to the previous node
				
			//Catch any errors and print the message
			} catch (GraphException e) {
				System.out.println(e.getMessage());
			}
			
		}
		return false;
	}
	
}
