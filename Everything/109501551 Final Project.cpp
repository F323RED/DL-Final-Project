/*
Author : F323RED
Date : 2021/1/15
Describe : Finite state machine reduction
*/

#include <iostream>
#include <fstream>

#define BUFFER_SIZE 16
#define INPUT_FILE_NAME "FSM_table.txt"
#define OUTPUT_FILE_NAME "Reducted_FSM_table.txt"

using namespace std;

// Function prototypes

int BinaryDecode(char [], int);
void BinaryEncode(int data, char* arr, int length);

struct StateTableCell
{
	int output = -1;
	char nextState[BUFFER_SIZE] = "";
};

struct StateTable
{
	char currentState[BUFFER_SIZE] = "";
	StateTableCell* cells;
};

int main(int argc, char* argv[])
{
	// Initialize file stream object
    ifstream fsmTable;
	ofstream reductedFsmTable;

	if (argc == 3)
	{
		fsmTable.open(argv[1], ios::in);
		reductedFsmTable.open(argv[2], ios::out);
		cout << "Custom file : " << argv[1] << ", " << argv[2] << "\n";
	}
	else
	{
		fsmTable.open(INPUT_FILE_NAME, ios::in);
		reductedFsmTable.open(OUTPUT_FILE_NAME, ios::out);
		cout << "Default file : " << INPUT_FILE_NAME << ", " << OUTPUT_FILE_NAME << "\n";
	}

	// Reading file
	if (fsmTable.is_open() && reductedFsmTable.is_open())	// If file opened
	{
		cout << "Reading file...\n";

		char str[BUFFER_SIZE];	// Input buffer, BUFFER_SIZE chars width.
		

		// Get number of state tables.
		int numberOfTable;
		
		fsmTable >> str;

		if (strcmp(str, ".t") == 0)	// strcmp returns 0 when two string equal
		{
			fsmTable >> numberOfTable;
			cout << "\nNumber of table : " << numberOfTable << "\n";
		}
		else
		{
			cout << "Tag error.\n";
			return 0;
		}


		// Write reducted table file
		reductedFsmTable << ".t " << numberOfTable << "\n\n";

		// Reading tables, one by one
		for (int i = 0; (i < numberOfTable) && fsmTable.good(); i++)
		{
			int inputSize, outputSize, numberOfState;
			inputSize = outputSize = numberOfState = 0;	// Init
			
			cout << "Table " << i + 1 << "\n";

			// Get parameters

			fsmTable >> str;
			if (strcmp(str, ".i") == 0)	// Get input size
			{
				fsmTable >> inputSize;
				cout << "Input size : " << inputSize << "\n";
			}

			fsmTable >> str;
			if (strcmp(str, ".o") == 0)	// Get output size
			{
				fsmTable >> outputSize;
				cout << "Output size : " << outputSize << "\n";
			}

			fsmTable >> str;
			if (strcmp(str, ".s") == 0)	// Get number of states
			{
				fsmTable >> numberOfState;
				cout << "Number of states : " << numberOfState << "\n";
			}

			if (inputSize == 0 || outputSize == 0 || numberOfState ==0)
			{
				cout << "Format check error.\n";
				cout << "Ending progress.";
				return 0;
			}

			// Init state table

			StateTable *stateTable = new StateTable[numberOfState];	// Dynamic Array
			for (int j = 0; j < numberOfState; j++)
			{
				stateTable[j].cells = new StateTableCell[(int)pow(2, inputSize)];	// 2^n cells
			}


			// Start reading states

			while (fsmTable.good())
			{
				fsmTable >> str;

				// Analize tags
				
				if (strcmp(str, ".e") == 0)	// End of one table
				{
					cout << "End of table " << i+1 << "\n\n";
					break;
				}
				else
				{					
					// Buffers
					char input[BUFFER_SIZE];
					char currentState[BUFFER_SIZE];
					char nextState[BUFFER_SIZE];
					char output[BUFFER_SIZE];

					// Read from file
					strcpy_s(input, str);
					fsmTable >> currentState >> nextState >> output;

					// Print state on screen
					cout << input << " " << currentState << " " << nextState << " " << output << "\n";

					// Log state into state table
					for (int index = 0; index < numberOfState; index++)
					{
						auto& cs = stateTable[index].currentState;	// Shorter name

						if (strcmp(cs, "") == 0 || strcmp(cs, currentState) == 0)
						{
							auto& cell = stateTable[index].cells[BinaryDecode(input, BUFFER_SIZE)];	// Shorter name

							strcpy_s(cs, currentState);
							strcpy_s(cell.nextState, nextState);
							cell.output = BinaryDecode(output, BUFFER_SIZE);

							break;
						}
					}
				}
			}

			// State table finished, move to next step

			// State table reduction
			int numberOfReductedState = numberOfState;

			for (int a = 0; a < numberOfState - 1; a++) {
				auto& temp = stateTable[a];

				if (strcmp(temp.currentState, "") == 0)	// Deleted state
				{
					continue;
				}

				for (int b = a + 1; b < numberOfState; b++) {
					auto& foo = stateTable[b];

					if (strcmp(foo.currentState, "") == 0)	// Deleted state
					{
						continue;
					}

					// Check for equal state
					bool flag = true;

					for (int c = 0; c < pow(2, inputSize); c++)
					{
						if (temp.cells[c].output != foo.cells[c].output) 
						{
							flag = false;
							break;
						}
						
						if (strcmp(temp.cells[c].nextState, foo.cells[c].nextState) != 0) 
						{
							flag = false;
							break;
						}
					}

					// If found equal state
					if (flag) {
						// Replace b with a!
						for (int x = 0; x < numberOfState; x++)
						{
							for (int y = 0; y < pow(2, inputSize); y++)
							{
								if (strcmp(stateTable[x].cells[y].nextState, foo.currentState) == 0)
								{
									strcpy_s(stateTable[x].cells[y].nextState, temp.currentState);
								}
							}
						}
						
						// Then delete b
						strcpy_s(foo.currentState, "");
						numberOfReductedState--;

						// Restart
						a = 0;
						b = a + 1;
					}
				}
			}

			// Test after reduction
			/*
			for (int j = 0; j < numberOfState; j++) {
				auto& row = stateTable[j];
				cout << row.currentState << " : ";

				for (int k = 0; k < pow(2, inputSize); k++) {
					cout << row.cells[k].nextState << "/" << row.cells[k].output << " ";
				}

				cout << "\n";
			}
			*/


			// Output reduced table to file
			reductedFsmTable << ".i " << inputSize << "\n";
			reductedFsmTable << ".o " << outputSize << "\n";
			reductedFsmTable << ".s " << numberOfReductedState << "\n";

			// write state table
			for (int m = 0; m < numberOfState; m++)
			{
				auto& temp = stateTable[m];
				if (strcmp(temp.currentState, "") != 0)
				{
					for (int n = 0; n < pow(2, inputSize); n++)
					{
						char bcdIn[BUFFER_SIZE];
						char bcdOut[BUFFER_SIZE];
						BinaryEncode(n, bcdIn, inputSize);
						BinaryEncode(temp.cells[n].output, bcdOut, outputSize);

						reductedFsmTable << bcdIn << " ";					// Write input
						reductedFsmTable << temp.currentState << " ";		// Write current state
						reductedFsmTable << temp.cells[n].nextState << " ";	// Write next state
						reductedFsmTable << bcdOut << "\n";					// Write output
					}
				}
			}

			reductedFsmTable << ".e " << "\n\n";
		}
	}
	else	// File open failed
	{
		cout << "File opening failed.\n";
		cout << "Ending progress.";
	}

	fsmTable.close();

	return 0;
}


// Functions defination

int BinaryDecode(char bcd[], int length) {
	/*
	Translate string of BCD to integer
	Return -1 if decode failed
	*/

	int temp = 0;

	for (int i = 0; i < length; i++)
	{
		temp <<= 1;

		if (bcd[i] == '0');	// Do nothing
		else if (bcd[i] == '1') 
		{
			temp |= 0x1;	// Insert 1 to LSB
		}
		else if(bcd[i] == '\0')	// End of bcd
		{
			break;
		}
		else 
		{
			return -1;	// char contain non binary code, return error code
		}
	}

	temp >>= 1;
	return temp;
}


void BinaryEncode(int data, char* arr, int length) 
{
	/*
	Translate integer to BCD form string 
	length should be smaller then arr size
	*/

	int max = (length > sizeof(int) * 8) ? (sizeof(int) * 8 - 1) : length;
	int mask = 0x1 << (max - 1);

	int j = 0;
	for (int i = 0; i < max; i++)
	{
		arr[j++] = (((mask & data) != 0x0) ? '1': '0');

		data <<= 1;
	}
	arr[j] = '\0';
}