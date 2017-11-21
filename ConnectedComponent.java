import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class ConnectedComponent {
   private int numRows;
   private int numCols;
   private int minVal;
   private int maxVal;
   private int newMin;
   private int newMax;
   private int newLabel;
   private int trueLabel;
   private int[][] zeroFramedAry;
   private int[][] CClabel;
   private int[] NeighborAry;
   private int[] EQAry;
   private int[][] Property;
   private String inFile;
   private String outFile1;
   private String outFile2;
   private String outFile3;

   public ConnectedComponent(String[] args) throws IOException {
      // initialize file name;
      inFile = args[0];
      outFile1 = args[1];
      outFile2 = args[2];
      outFile3 = args[3];
      msg("Input image file name is : " + inFile);
      msg("Output file name is : " + outFile1 + "/" + outFile2 + "/" + outFile3);

      // loading the image data
      msg("Start loading image...");
      Scanner sc;
      sc = new Scanner(new FileInputStream(inFile));
      if (!sc.hasNext()) {
         System.out.println("Error, Empty input file.");
         sc.close();
         return;
      } else {
         numRows = sc.nextInt();
         numCols = sc.nextInt();
         minVal = sc.nextInt();
         newMin = minVal;
         maxVal = sc.nextInt();
         newMax = maxVal;
      }
      sc.close();

      // initialize Variables and Arrays
      newLabel = 0;
      zeroFramedAry = new int[numRows + 2][numCols + 2];
      CClabel = new int[numRows + 2][numCols + 2];
      for (int i = 0; i < numRows + 2; i++) {
         for (int j = 0; j < numCols + 2; j++) {
            zeroFramedAry[i][j] = 0;
            CClabel[i][j] = 0;
         }
      }
      NeighborAry = new int[4];
      EQAry = new int[numRows * numCols / 2];
      for (int x = 0; x < numRows * numCols / 2; x++) {
         EQAry[x] = x;
      }

      // Start Loading
      loadImage();

      // Create output file1
      FileWriter fw = new FileWriter(outFile1);
      fw.write(outFile1 + "\r\n");
      fw.close();
   }

   public void loadNeighbors(int i, int j, int pass) {
      if (pass == 1) {
         NeighborAry[0] = CClabel[i - 1][j]; // Top
         NeighborAry[1] = CClabel[i][j - 1]; // Left
      } else if (pass == 2) {
         NeighborAry[0] = CClabel[i][j + 1]; // Right
         NeighborAry[1] = CClabel[i + 1][j]; // Bottom
         NeighborAry[2] = CClabel[i][j]; // Self
      }
   }

   public void ConnectCC_Pass1() throws IOException {
      for (int i = 1; i < numRows + 1; i++) {
         for (int j = 1; j < numCols + 1; j++) {
            if (zeroFramedAry[i][j] > 0) {
               loadNeighbors(i, j, 1);
               if (NeighborAry[0] == 0 && NeighborAry[1] == 0) {
                  // top = 0 and left = 0;
                  newLabel++;
                  CClabel[i][j] = newLabel;
               } else if ((NeighborAry[0] > 0 && NeighborAry[1] == 0)) {
                  // top > 0 and left = 0;
                  CClabel[i][j] = NeighborAry[0];
               } else if ((NeighborAry[0] == 0 && NeighborAry[1] > 0)) {
                  // top = 0 and left > 0;
                  CClabel[i][j] = NeighborAry[1];
               } else {
                  // top > 0 and left > 0;
                  if (NeighborAry[0] == NeighborAry[1])
                     CClabel[i][j] = NeighborAry[0];
                  else {
                     if (NeighborAry[0] < NeighborAry[1]) {
                        // top < left
                        CClabel[i][j] = NeighborAry[0];
                        updateEQAry(NeighborAry[1], NeighborAry[0]);// update
                                                                    // EQAry
                     } else {
                        // top > left
                        CClabel[i][j] = NeighborAry[1];
                        updateEQAry(NeighborAry[0], NeighborAry[1]);// update
                                                                    // EQAry
                     }
                  }
               }
            }
         }
      }

      // Output the result to file
      msg("C.C Pass-1 PrettyPrity");
      prettyPrint(outFile1);
      printEQAry(outFile1);

   }

   public void ConnectCC_Pass2() throws IOException {
      for (int i = numRows; i > 0; i--) {
         for (int j = numCols; j > 0; j--) {
            if (zeroFramedAry[i][j] > 0) {
               loadNeighbors(i, j, 2);
               if (NeighborAry[0] != 0 && NeighborAry[1] != 0) {
                  // Right and Botten both none Zero.
                  if (NeighborAry[0] < NeighborAry[1]) {
                     // Right < Botten
                     updateEQAry(NeighborAry[1], NeighborAry[0]);// update EQAry
                     if (NeighborAry[0] < CClabel[i][j]) {
                        // Right < itself
                        updateEQAry(CClabel[i][j], NeighborAry[0]);// update
                                                                   // EQAry
                        CClabel[i][j] = NeighborAry[0];
                     } else if (NeighborAry[0] > CClabel[i][j]) {
                        // Right > itself
                        updateEQAry(NeighborAry[0], CClabel[i][j]);// update
                                                                   // EQAry
                        updateEQAry(NeighborAry[1], CClabel[i][j]);// update
                                                                   // EQAry
                     }
                  } else if (NeighborAry[0] > NeighborAry[1]) {
                     // Right > Botten
                     updateEQAry(NeighborAry[0], NeighborAry[1]);// update EQAry
                     if (NeighborAry[1] < CClabel[i][j]) {
                        // Botten < itself
                        updateEQAry(CClabel[i][j], NeighborAry[1]);// update
                                                                   // EQAry
                        CClabel[i][j] = NeighborAry[1];
                     } else if (NeighborAry[1] > CClabel[i][j]) {
                        // Botten > itself
                        updateEQAry(NeighborAry[0], CClabel[i][j]);// update
                                                                   // EQAry
                        updateEQAry(NeighborAry[1], CClabel[i][j]);// update
                                                                   // EQAry
                     }
                  } else {
                     // Right = Botten
                     if (CClabel[i][j] < NeighborAry[0]) {
                        // itself < Right and Botten
                        updateEQAry(NeighborAry[0], CClabel[i][j]);// update
                                                                   // EQAry
                        updateEQAry(NeighborAry[1], CClabel[i][j]);// update
                                                                   // EQAry
                     } else if (CClabel[i][j] > NeighborAry[0]) {
                        // itself > Right and Botten
                        updateEQAry(CClabel[i][j], NeighborAry[0]);// update
                                                                   // EQAry
                        CClabel[i][j] = NeighborAry[0];
                     }
                  }

               } else {
                  if (NeighborAry[0] == 0 && NeighborAry[1] != 0) {
                     // Right = 0 and Botten none Zero
                     if (NeighborAry[1] < CClabel[i][j]) {
                        // Botten < itSelf
                        updateEQAry(CClabel[i][j], NeighborAry[1]);// update
                                                                   // EQAry
                        CClabel[i][j] = NeighborAry[1];
                     } else if (NeighborAry[1] > CClabel[i][j]) {
                        // Botten > itSelf
                        updateEQAry(NeighborAry[1], CClabel[i][j]);// update
                                                                   // EQAry
                     }
                  }
                  if (NeighborAry[1] == 0 && NeighborAry[0] != 0) {
                     // Right none Zero and Botten = 0
                     if (NeighborAry[0] < CClabel[i][j]) {
                        // Right < itSelf
                        updateEQAry(CClabel[i][j], NeighborAry[0]);// update
                                                                   // EQAry
                        CClabel[i][j] = NeighborAry[0];
                     } else if (NeighborAry[0] > CClabel[i][j]) {
                        // Right > itSelf
                        updateEQAry(NeighborAry[0], CClabel[i][j]);// update
                                                                   // EQAry
                     }
                  }
               }
            }
         }
      }

      // Output the result to file
      msg("C.C Pass-2 PrettyPrity");
      prettyPrint(outFile1);
      printEQAry(outFile1);
   }

   public void ConnectCC_Pass3() throws IOException {
      for (int i = 1; i < numRows + 1; i++) {
         for (int j = 1; j < numCols + 1; j++) {
            if (zeroFramedAry[i][j] > 0) {
               CClabel[i][j] = EQAry[CClabel[i][j]];
               Property[CClabel[i][j]][0]++;
               // compare to minVal
               if (Property[CClabel[i][j]][1] == 0)
                  Property[CClabel[i][j]][1] = i;
               else if (Property[CClabel[i][j]][1] > i)
                  Property[CClabel[i][j]][1] = i;
               if (Property[CClabel[i][j]][2] == 0)
                  Property[CClabel[i][j]][2] = j;
               else if (Property[CClabel[i][j]][2] > j)
                  Property[CClabel[i][j]][2] = j;
               // compare to maxVal
               if (Property[CClabel[i][j]][3] == 0)
                  Property[CClabel[i][j]][3] = i;
               else if (Property[CClabel[i][j]][3] < i)
                  Property[CClabel[i][j]][3] = i;
               if (Property[CClabel[i][j]][4] == 0)
                  Property[CClabel[i][j]][4] = j;
               else if (Property[CClabel[i][j]][4] < j)
                  Property[CClabel[i][j]][4] = j;
            }
         }
      }
      newMin = 0;
      newMax = trueLabel;
      // Output the result to file
      msg("C.C Pass-3 PrettyPrity");
      prettyPrint(outFile1);
      printEQAry(outFile1);

      printImage(outFile2);

   }

   public void updateEQAry(int idx, int val) {
      if (EQAry[idx] > val)
         EQAry[idx] = val; // update EQAry
   }

   public void manageEQAry() throws IOException {
      trueLabel = 0;
      for (int idx = 1; idx <= newLabel; idx++) {
         if (EQAry[idx] == idx) {
            trueLabel++;
            EQAry[idx] = trueLabel;
         } else {
            EQAry[idx] = EQAry[EQAry[idx]];
         }
      }

      // initialize Property Array;
      Property = new int[trueLabel + 1][6];
      for (int i = 0; i < trueLabel + 1; i++) {
         for (int j = 0; j < 6; j++) {
            Property[i][j] = 0;
         }
      }

      printEQAry(outFile1);
   }

   public void printCCProperty() throws IOException {
      FileWriter fw = new FileWriter(outFile3);
      fw.write(numRows + " " + numCols + " " + 0 + " " + trueLabel + "\r\n");
      System.out.println(numRows + " " + numCols + " " + 0 + " " + trueLabel);
      fw.write(trueLabel + "\r\n");
      System.out.println(trueLabel);
      for (int i = 1; i < trueLabel + 1; i++) {
         fw.write(i + "\r\n");
         fw.write(Property[i][0] + "\r\n");
         fw.write(Property[i][1] + " " + Property[i][2] + "\r\n");
         fw.write(Property[i][3] + " " + Property[i][4] + "\r\n");
         System.out.println("C.C. " + i + " has " + Property[i][0] + " pixels [(" + Property[i][1] + ", "
               + Property[i][2] + "), (" + Property[i][3] + ", " + Property[i][4] + ")]");
      }
      fw.close();
   }

   public void prettyPrint(String output) throws IOException {
      FileWriter fw = new FileWriter(output, true);
      fw.write(numRows + " " + numCols + " " + 0 + " " + newLabel + "\r\n");
      System.out.println("#Rows:" + numRows + " #Cols:" + numCols + " minVal:" + 0 + " maxVal:" + newLabel);
      for (int i = 1; i < numRows + 1; i++) {
         for (int j = 1; j < numCols + 1; j++) {
            if (CClabel[i][j] > 0) {
               if (CClabel[i][j] < 10) {
                  System.out.print(CClabel[i][j] + "  ");
                  fw.write(CClabel[i][j] + "  ");
               } else {
                  System.out.print(CClabel[i][j] + " ");
                  fw.write(CClabel[i][j] + " ");
               }
               ;
            } else {
               System.out.print("   ");
               fw.write("   ");

            }
         }
         System.out.println("");
         fw.write("\r\n");
      }
      fw.close();
   }

   public void printEQAry(String output) throws IOException {
      FileWriter fw = new FileWriter(output, true);
      fw.write("\r\n");
      System.out.println("");
      fw.write("Index[val] ==> " + "\r\n");
      System.out.print("Index[val] ==> ");
      for (int i = 1; i <= newLabel; i++) {
         System.out.print(i + "[" + EQAry[i] + "] ; ");
         fw.write(i + "[" + EQAry[i] + "] ; ");
         if (i % 10 == 0) {
            System.out.println("");
            fw.write("\r\n");
         }
      }
      System.out.println("");
      fw.write("\r\n");
      fw.close();
   }

   private void printImage(String output) throws IOException {
      FileWriter fw = new FileWriter(output);
      msg("Printing New Image...");
      msg("Image size is " + numRows + "x" + numCols + ", " + "minVal:" + minVal + " maxVal:" + maxVal);
      fw.write(numRows + " " + numCols + " " + newMin + " " + newMax + "\r\n");
      for (int i = 0; i < numRows + 2; i++) {
         for (int j = 1; j < numCols + 2; j++) {
            if (CClabel[i][j] < 10) {
               System.out.print(CClabel[i][j] + "  ");
               fw.write(CClabel[i][j] + "  ");
            } else {
               System.out.print(CClabel[i][j] + " ");
               fw.write(CClabel[i][j] + " ");
            }
         }
         System.out.println("");
         fw.write("\r\n");
      }
      fw.close();
   }

   private void loadImage() throws FileNotFoundException {
      // Read the original image
      Scanner sc;
      sc = new Scanner(new FileInputStream(inFile));
      if (!sc.hasNext()) {
         System.out.println("Error, Empty input file.");
         sc.close();
         return;
      } else {
         String temp = sc.nextLine();
         msg(temp);
         int gVal = 0;
         for (int i = 1; i < numRows + 1; i++) {
            for (int j = 1; j < numCols + 1; j++) {
               if (sc.hasNext()) {
                  gVal = sc.nextInt();
               } else {
                  break;
               }
               zeroFramedAry[i][j] = gVal;
            }
         }
      }
      sc.close();
      msg("Finish Reading Original Image...");
   }

   public static void msg(String m) {
      System.out.println("[" + m + "]");
   }

   public static void main(String[] args) {
      try {
         if (args.length != 4) {
            System.out
                  .println("Run as : java ConnectedComponent [inputFile1] [outputFile1] [outputFile2] [outputFile3]");
            return;
         }
         ConnectedComponent CC = new ConnectedComponent(args);
         CC.ConnectCC_Pass1();
         CC.ConnectCC_Pass2();
         CC.manageEQAry();
         CC.ConnectCC_Pass3();
         CC.printCCProperty();

      } catch (IOException e) {
         e.getMessage();
         System.out.println(e.getMessage() + "     ~~~ Try again ~~~");
      }
   }

}
