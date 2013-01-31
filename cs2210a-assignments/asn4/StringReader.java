import java.io.*;
public class StringReader {
  
    private BufferedReader input;  // Keyboard input

 /* -------------------------- */
    public StringReader() {
 /* -------------------------- */
	input = new BufferedReader(new InputStreamReader(System.in));
    }

 /* --------------------------------- */
    public String read(String label) {
 /* --------------------------------- */
 /* Print the given label and read a String from the keyboard */

        try {
          System.out.println(label);
          return(input.readLine());
        }
        catch (IOException e) {
          System.out.println("Error: can't read keyboard input");
          System.exit(0);
        }
         return "end";
    }
}
