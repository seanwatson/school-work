import java.io.*;

public class MyOutput
{
   // class data members
   // constants
   final static int BYTE_SIZE = 8;
   final static int EXCESS = 4;    // 12 - ByteSize
   final static int MASK1 = 255;   // One byte
   final static int MASK2 = 15;    // 2^EXCESS - 1

   // variables
   static int leftOver;      // code bits yet to be output
   static boolean bitsLeftOver;

   public MyOutput(){}
   
   /* =========================================================== */
   public  void output(int pcode, BufferedOutputStream out) throws IOException
   /* =========================================================== */
   /** Prints code "pcode" into the output file referenced by "out" */
   {
      int firstByte, secondByte;

      if (bitsLeftOver)
      {// half byte remains from before
         firstByte = pcode & MASK1; // right BYTE_SIZE bits
         secondByte = (leftOver << EXCESS) + (pcode >> BYTE_SIZE);
         out.write(secondByte);
         out.write(firstByte);
         bitsLeftOver = false;
      }
      else
      {// no bits remain from before
         leftOver = pcode & MASK2; // right EXCESS bits
         secondByte = pcode >> EXCESS;
         out.write(secondByte);
         bitsLeftOver = true;
      }
   }

   /* ============================================= */
   public  void flush(BufferedOutputStream out)  throws IOException
   /* ============================================= */
     /** Prints any codes that have not been set to the output file */
   {
     if (bitsLeftOver) {
            bitsLeftOver = false;
            out.write(leftOver << EXCESS);
     }
  }
}

   
 
