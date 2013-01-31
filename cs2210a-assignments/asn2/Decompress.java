
/**  Decompression Algorithm */

import java.io.*;

public class Decompress
{

   // class data members
   // constants
   final static int MAX_CODES = 4096;  // 2^12
   final static int BYTE_SIZE = 8;
   final static int EXCESS = 4;    // 12 - ByteSize
   final static int ALPHA = 256;   // 2^ByteSize
   final static int MASK = 15;     // 2^EXCESS - 1
    //final static int ASCII_CODES = 128;
   final static int ASCII_CODES = 256;

   // variables
   static int [] s;                // used to reconstruct text
   static int size;                // size of reconstructed text
   static int leftOver;            // input bits yet to be output
   static boolean bitsLeftOver;
   static BufferedInputStream in;
   static BufferedOutputStream out;
   static Element [] h;            // dictionary

   
   /** create input and output streams */
   private static  boolean setFiles(String [] argv)
   // **************************************************
   {
      String inputFile, name = "", outputFile;

      try {
	  // see if file name provided
	  if (argv.length >= 1)
	      name = argv[0];
	  else {// input file name not provided, ask for it
	      System.out.println("File to decompress (omit the extension .zzz):");
	      BufferedReader keyboard =  new BufferedReader
		                         (new InputStreamReader(System.in));
	      name = keyboard.readLine();
	  }

	  // open input and output streams
	  inputFile = name + ".zzz";
	  outputFile = name + ".unc";

	  in = new BufferedInputStream(new FileInputStream(inputFile));
	  out = new BufferedOutputStream(new FileOutputStream(outputFile));
	  return true;
      }
      catch (IOException e) {
	  System.out.println("Cannot open input or output files: "+name);
	  return false;
      }
   }


   /** Decompressor */
   private static void decompress() throws IOException
   // **************************************************
   {
      int codesUsed = ASCII_CODES; // codes used so far
      s = new int [MAX_CODES];
      h = new Element [MAX_CODES];

      Element p = new Element();
   
      // input and decompress
      int pcode = getCode(),  // previous code
          ccode;              // current code
      
      if (pcode >= 0)
      {// input file is not empty
         s[0] = pcode;     // byte for pcode 
         out.write(s[0]); 
         size = 0; // s[size] is first character of
                   // last string output
                   
         do
         {// get another code
            ccode = getCode();
            if (ccode < 0) break;  // no more codes
            if (ccode < codesUsed)
            {// ccode is defined
               output(ccode);
	      
               if (codesUsed < MAX_CODES)              
                  // create new code
		 h[codesUsed++] = new Element(pcode, s[size]);
            }
            else
            {// special case, undefined code
	       if (codesUsed < MAX_CODES) {
                  h[codesUsed++] = new Element(pcode, s[size]);
                  output(ccode);
	       }
            }
            pcode = ccode;
        }  while(true);
      }
      out.close();
      in.close();
   }
   
   /** @return next code from compressed file
     * @return -1 if there is no next code */
   private static int getCode() throws IOException
   // **************************************************
   {

      int c = in.read();

      if (c == -1) return -1;  // no more codes
   
      // see if any leftover bits from before
      // if yes, concatenate with leftover bits
      int code;
      if (bitsLeftOver)
         code = (leftOver << BYTE_SIZE) + c;
      else
      {// no leftover bits, need more bits to complete code
         int d = in.read();  // another byte

         code = (c << EXCESS) + (d >> EXCESS);
         leftOver = d & MASK;  // save unused bits
      }
      bitsLeftOver = !bitsLeftOver;
      return code;
   }
      
   /** output the byte sequence that corresponds to code */
   private static void output(int code) throws IOException
   // **************************************************
   {

      size = -1;
      while (code >= ASCII_CODES)
      {// suffix is in the dictionary
	s[++size] = h[code].suffix;
	code = h[code].prefix;

      }
      s[++size] = code;  // code < ALPHA
   
      // decompressed string is s[size] ... s[0]
      for (int i = size; i >= 0; i--)
         out.write(s[i]);
   }
   
   public static void main(String [] argv)
   // ********************************************************
   {
       try {
           if (setFiles(argv)) decompress();
       }
       catch(Exception e) {
	   System.out.println("Error decompressing file:"+e.getMessage());
       }

   }


   // top-level nested class
   private static class Element
   // **************************************************
   {
      // data members
      public int prefix;
      public int suffix;

      // constructor
      public Element( int thePrefix, int theSuffix)
      {
         prefix = thePrefix;
         suffix = theSuffix;
      }

     public Element() {}
   }
   
}
