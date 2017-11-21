#include <iostream> 
#include <fstream> 
#include <string>
#include<iostream>

using namespace std;
/*
--- Please use C++ 11 when you Compile in Venus  ---g++ -std=gnu++11 Histogram.cpp -o histogram.out
*/

class ConnectedComponent4{
private:
	int numRows;
	int numCols;
	int minVal;
	int maxVal;
	int newMin;
	int newMax;
	int newLable; // initallize to 0.
	int trueLabel; // real label;
	int **zeroFramedAry; // 2D Array, size = numRows+2 by numCols+2.
	int **CClabel;// 2D Array, size = numRows+2 by numCols+2.
	int NeighborAry[4];	//4 neighbors
	int *EQAry; // size = (numRows by numCols)/2; initallize to EQAry[i]=i;
	int **Property; // Roms = components+1; Cols have label, numPixels, minRow, minCol, maxRow, maxCol 6 properties;

	string inFile;
	string outFile1;
	string outFile2;
	string outFile3;

public:
	ConnectedComponent4(char **File) {
		//initialize file name;
		inFile = File[1];
		outFile1 = File[2];
		outFile2 = File[3];
		outFile3 = File[4];
		cout << "Input image file name is :" << inFile << endl;
		cout << "Output file name is :" << outFile1  << " / " << outFile2 << " / " << outFile3 << endl;
		// loading the image data
		cout << "Start Loading Image..." << endl;
		HeadLoading();
		//initialize Variables and Arrays
		newLable = 0;
		zeroFramedAry = new int*[numRows + 2];
		CClabel = new int*[numRows + 2];
		for (int i = 0; i < numRows + 2; i++) {
			zeroFramedAry[i] = new int[numCols + 2];
			CClabel[i] = new int[numCols + 2];
			for (int j = 0; j < numCols + 2; j++) {
				zeroFramedAry[i][j] = 0;
				CClabel[i][j] = 0;
			}
		}
		NeighborAry[4] = { 0 };
		EQAry = new int[numRows*numCols/2];
		for (int x = 0; x < numRows*numCols / 2; x++) {
			EQAry[x] = x;
		}
		//Start loading
		loadImage();

		//Open output file1
		fstream foutput;
		foutput.open(outFile1, ios::out);
		foutput << "outFile1.txt" << endl;
		foutput.close();
	}
	~ConnectedComponent4() {
		for (int i = 0; i < numRows + 2; i++) {
			delete[] zeroFramedAry[i];
		}
		for (int j = 0; j < trueLabel; j++) {
			delete[] Property[j];
		}
		delete[] zeroFramedAry;
		delete[] EQAry;
		delete[] Property;
	}

	void loadImage() {
		//Read the original image
		fstream finput;
		finput.open(inFile);
		if (!finput.is_open()) {
			throw "Error ! Open Input File Failed, try again...";
		}
		if (!finput.eof() && finput.peek() != EOF) {
			string temp;
			getline(finput, temp); // skip the image head
			int gVal = 0;
			for (int i = 1; i < numRows + 1; i++) {
				for (int j = 1; j < numCols + 1; j++) {
					if (!finput.eof() && finput.peek() != EOF) {
						finput >> gVal;
					}
					else { break; }
					zeroFramedAry[i][j] = gVal;
				}
			}
		}
		else {
			finput.close();
			throw "Error ! Input File is empty, try again...";
		}
		finput.close();
		cout << "Finish Loading Original Image." << endl;
	}

	void loadNeighbors(int i,int j,int Pass) {
		if (Pass == 1) {
			NeighborAry[0] = CClabel[i - 1][j]; //Top
			NeighborAry[1] = CClabel[i][j - 1];	//Left
		}
		else if (Pass == 2) {
			NeighborAry[0] = CClabel[i][j + 1]; //Right
			NeighborAry[1] = CClabel[i + 1][j]; //Bottom
			NeighborAry[2] = CClabel[i][j];	//Self
		}
		else {
			//
		}
	}

	void ConnectCC_Pass1() {
		for (int i = 1; i < numRows + 1; i++) {
			for (int j = 1; j < numCols + 1; j++) {
				if (zeroFramedAry[i][j] > 0) {
					loadNeighbors(i, j, 1);
					if (NeighborAry[0] == 0 && NeighborAry[1] == 0) {
						// top = 0 and left = 0;
						newLable++;
						CClabel[i][j] = newLable;
					}
					else if ((NeighborAry[0] > 0 && NeighborAry[1] == 0)) {
						// top > 0 and left = 0;
						CClabel[i][j] = NeighborAry[0];
					}
					else if ((NeighborAry[0] == 0 && NeighborAry[1] > 0)) {
						// top = 0 and left > 0;
						CClabel[i][j] = NeighborAry[1];
					}
					else {
						// top > 0 and left > 0;
						if (NeighborAry[0] == NeighborAry[1]) CClabel[i][j] = NeighborAry[0];
						else {
							if (NeighborAry[0] < NeighborAry[1]) {
								// top < left
								CClabel[i][j] = NeighborAry[0];
								updateEQAry(NeighborAry[1], NeighborAry[0]);//update EQAry								
							}
							else {
								// top > left
								CClabel[i][j] = NeighborAry[1];
								updateEQAry(NeighborAry[0], NeighborAry[1]);//update EQAry
							}
						}
					}
				}
			}
		}

		//Output the result to file
		cout << "C.C Pass-1 PrettyPrity" << endl;
		PrettyPrity(outFile1);
		printEQAry(outFile1);
	}
	void ConnectCC_Pass2() {
		for (int i = numRows; i > 0 ; i--) {
			for (int j = numCols; j > 0; j--) {
				if (zeroFramedAry[i][j]>0) {
					loadNeighbors(i, j, 2);
					if (NeighborAry[0] != 0 && NeighborAry[1] != 0) {
						//Right and Botten both none Zero.
						if (NeighborAry[0] < NeighborAry[1]){
							//Right < Botten
							updateEQAry(NeighborAry[1], NeighborAry[0]);//update EQAry
							if (NeighborAry[0] < CClabel[i][j]) {
								//Right < itself
								updateEQAry(CClabel[i][j], NeighborAry[0]);//update EQAry
								CClabel[i][j] = NeighborAry[0];
							}
							else if (NeighborAry[0] > CClabel[i][j]){
								//Right > itself
								updateEQAry(NeighborAry[0], CClabel[i][j]);//update EQAry
								updateEQAry(NeighborAry[1], CClabel[i][j]);//update EQAry
							}
						}
						else if (NeighborAry[0] > NeighborAry[1]) {
							//Right > Botten
							updateEQAry(NeighborAry[0], NeighborAry[1]);//update EQAry
							if (NeighborAry[1] < CClabel[i][j]) {
								//Botten < itself
								updateEQAry(CClabel[i][j], NeighborAry[1]);//update EQAry
								CClabel[i][j] = NeighborAry[1];
							}
							else if (NeighborAry[1] > CClabel[i][j]) {
								//Botten > itself
								updateEQAry(NeighborAry[0], CClabel[i][j]);//update EQAry
								updateEQAry(NeighborAry[1], CClabel[i][j]);//update EQAry
							}
						}
						else {
							//Right = Botten
							if (CClabel[i][j] < NeighborAry[0]) {
								// itself < Right and Botten
								updateEQAry(NeighborAry[0], CClabel[i][j]);//update EQAry
								updateEQAry(NeighborAry[1], CClabel[i][j]);//update EQAry
							}
							else if (CClabel[i][j] > NeighborAry[0]) {
								// itself > Right and Botten
								updateEQAry(CClabel[i][j], NeighborAry[0]);//update EQAry
								CClabel[i][j] = NeighborAry[0];
							}
						}

					}
					else {
						if (NeighborAry[0] == 0 && NeighborAry[1] != 0) {
							//Right = 0 and Botten none Zero
							if (NeighborAry[1] < CClabel[i][j]) {
								//Botten < itSelf
								updateEQAry(CClabel[i][j], NeighborAry[1]);//update EQAry
								CClabel[i][j] = NeighborAry[1];
							}
							else if (NeighborAry[1] > CClabel[i][j]) {
								//Botten > itSelf
								updateEQAry(NeighborAry[1], CClabel[i][j]);//update EQAry
							}
						}
						if (NeighborAry[1] == 0 && NeighborAry[0] != 0) {
							//Right none Zero and Botten = 0
							if (NeighborAry[0] < CClabel[i][j]) {
								//Right < itSelf
								updateEQAry(CClabel[i][j], NeighborAry[0]);//update EQAry
								CClabel[i][j] = NeighborAry[0];
							}
							else if (NeighborAry[0] > CClabel[i][j]) {
								//Right > itSelf
								updateEQAry(NeighborAry[0], CClabel[i][j]);//update EQAry
							}
						}
					}
				}
			}
		}

		//Output the result to file
		cout << "C.C Pass-2 PrettyPrity" << endl;
		PrettyPrity(outFile1);
		printEQAry(outFile1);
	}
	void ConnectCC_Pass3() {

		for (int i = 1; i < numRows + 1; i++) {
			for (int j = 1; j < numCols + 1; j++) {
				if (zeroFramedAry[i][j] > 0) {
					CClabel[i][j] = EQAry[CClabel[i][j]];
					Property[CClabel[i][j]][0]++;
					//compare to minVal
					if (Property[CClabel[i][j]][1] == 0) Property[CClabel[i][j]][1] = i;
					else if (Property[CClabel[i][j]][1] > i) Property[CClabel[i][j]][1] = i;
					if (Property[CClabel[i][j]][2] == 0) Property[CClabel[i][j]][2] = j;
					else if (Property[CClabel[i][j]][2] > j) Property[CClabel[i][j]][2] = j;
					//compare to maxVal
					if (Property[CClabel[i][j]][3] == 0) Property[CClabel[i][j]][3] = i;
					else if (Property[CClabel[i][j]][3] < i) Property[CClabel[i][j]][3] = i;
					if (Property[CClabel[i][j]][4] == 0) Property[CClabel[i][j]][4] = j;
					else if (Property[CClabel[i][j]][4] < j) Property[CClabel[i][j]][4] = j;
				}
			}
		}
		//Output the result to file
		cout << "C.C Pass-3 PrettyPrity" << endl;
		PrettyPrity(outFile1);
		printEQAry(outFile1);

		printImage(outFile2);


	}

	void updateEQAry(int idx, int val) {
		if (EQAry[idx] > val) EQAry[idx] = val;	//update EQAry
	}
	
	void manageEQary() {
		trueLabel = 0;
		for (int idx = 1; idx <= newLable; idx++) {
			if (EQAry[idx] == idx) {
				trueLabel++;
				EQAry[idx] = trueLabel;
			}
			else {
				EQAry[idx] = EQAry[EQAry[idx]];
			}
		}
		//cout << "True Label : " << trueLabel << endl;


		//initialize Property Array; 
		Property = new int*[trueLabel + 1];
		for (int i = 0; i < trueLabel + 1; i++) {
			Property[i] = new int[6];
			for (int j = 0; j < 6; j++) {
				Property[i][j] = 0;
			}
		}
		
		printEQAry(outFile1);
	}

	void printEQAry(string output) {
		fstream foutput;
		foutput.open(output, ios::out | ios::app);
		foutput << endl << "Index[val] ==> ";
		cout << endl << "Index[val] ==> ";
		for (int i = 1; i <= newLable; i++) {
			cout << i << "[" << EQAry[i] << "] ; ";
			foutput << i << "[" << EQAry[i] << "] ; ";
			if (i % 10 == 0) {
				cout << endl;
				foutput << endl;
			}
		}
		cout << endl;
		foutput << endl;
		foutput.close();
	}

	void printCCProperty() {
		fstream foutput;
		foutput.open(outFile3, ios::out);
		foutput << numRows << " " << numCols << " " << 0 << " " << trueLabel << endl;
		foutput << trueLabel << endl;
		for (int i = 1; i < trueLabel + 1; i++) {
			cout << "C.C. " << i << " has " << Property[i][0] << " pixels [(" << Property[i][1] << ", " << Property[i][2] << "), (" << Property[i][3] << ", " << Property[i][4] << ")]" << endl;
			foutput << i << endl;
			foutput << Property[i][0] << endl;
			foutput << Property[i][1] << " " << Property[i][2] << endl;
			foutput << Property[i][3] << " " << Property[i][4] << endl;
		}
		foutput.close();
	}

	void PrettyPrity(string output) {
		fstream foutput;
		foutput.open(output, ios::out | ios::app);

		cout << "#Rows:" << numRows << " #Cols:" << numCols << " minVal:" << minVal << " maxVal:" << maxVal << endl;
		foutput << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
		for (int i = 1; i < numRows + 1; i++) {
			for (int j = 1; j < numCols + 1; j++) {
				if (CClabel[i][j] > 0) {
					if (CClabel[i][j] < 10) { 
						cout << CClabel[i][j] << "  ";
						foutput << CClabel[i][j] << "  ";
					}
					else { 
						cout << CClabel[i][j] << " ";
						foutput << CClabel[i][j] << " ";
					};
				}
				else {
					cout << "   ";
					foutput << "   ";
				}
			}
			cout << endl;
			foutput << endl;
		}
		foutput.close();
	}

	void HeadLoading() {
		fstream finput;
		finput.open(inFile);
		if (!finput.is_open()) {
			throw "Error ! Open Input File Failed, try again...";
		}
		if (!finput.eof() && finput.peek() != EOF) {
			finput >> numRows;
			finput >> numCols;
			finput >> minVal;
			newMin = minVal;
			finput >> maxVal;
			newMax = maxVal;
		}
		else {
			finput.close();
			throw "Error ! Input File is empty, try again...";
		}
		finput.close();
	}

	void printImage(string output) {
		cout << "Printing New Image..." << endl;
		fstream foutput;
		foutput.open(output, ios::out);
		foutput << numRows << " " << numCols << " " << 0 << " " << trueLabel << endl;
		cout << "Image size is " << numRows << "x" << numCols << ", " << "C.C is " << 0 << "-" << newLable << ";" << endl;

		for (int i = 1; i < numRows + 1; i++) {
			for (int j = 1; j < numCols + 1; j++) {
				if (CClabel[i][j] < 10) {
					cout << CClabel[i][j] << "  ";
					foutput << CClabel[i][j] << "  ";
				}
				else {
					cout << CClabel[i][j] << " ";
					foutput << CClabel[i][j] << " ";
				}
			}
			cout << endl;
			foutput << endl;
		}
		foutput.close();
	}

};


int main(int argc, char *argv[])
{
	try {
		if (argc != 5) {
			cout << "Parameters missing... ( Run as : 4ConnectedComponent <input file> <output file 1> <output file 2> <output file 3> ).";
			return 0;
		}
		ConnectedComponent4 CC(argv);

		CC.ConnectCC_Pass1();
		CC.ConnectCC_Pass2();
		CC.manageEQary();
		CC.ConnectCC_Pass3();
		CC.printCCProperty();

	}
	catch (const char* msg) {
		cerr << msg << endl;
		return 0;
	}
	return 0;
};