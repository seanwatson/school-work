import net.rim.device.api.util.Persistable;

//Class for creating persistent category data
public class CategoryField implements Persistable {

	//Stores the category
	private String category;
	
	//Constructor for persistable category
	public CategoryField(String cat){
		category = cat;
	}
	//Get the category stored
	public String getCategory(){
		return category;
	}
	//Set the category to something new
	public void setCategory(String cat){
		category = cat;
	}
	
}
