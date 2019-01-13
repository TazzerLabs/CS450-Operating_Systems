/*
 * CS 450
 * Problem Set 4 #1 & 2
 * Alissa Greaney and Johnathan Soto
 */

#include <iostream>
#include <fstream>
#include <bitset>
#include <sstream>
#include <string>
#include <math.h>
using namespace std;

/*
 * Default is 64 bits - trims to the correct bit size
 */
string trimBin(string s, int size)
{
	string n = s.substr(64 - size, size);
	return n;
}

/*
 *Converts decimal to binary - trims to correct bit size
 */
string decToBin(int n, int size)
{
	string bin = bitset<64>(n).to_string();	
	bin = trimBin(bin, size);
	return bin;
}

/*
 * Converts hex to binary - trims to correct bit size
 */
string hexToBin(string s, int size)
{
	//hex to binary 
	stringstream ss;
	ss << hex << s;
	unsigned b;
	ss >> b;
	string bin1 = bitset<64>(b).to_string();
	bin1 = trimBin(bin1, size);
	return bin1;
}

/*
 * Converts binary to hex
 */
string binToHex(string s)
{
	//binary to hex
	bitset<64> bs(s);
	unsigned h = bs.to_ulong();
	stringstream ss;
	ss << hex << h;
	string a = ss.str();
	for(int i = 0; i < a.length(); i++)
	{
		a[i] = toupper(a[i]);
	}
	return a;
	//cout << ss.str() << endl;
}

int main(int argc, char*argv[])
{
	int virtAdd, phyAdd, page, value;
	string table[64][4]; //default 
	string filename;
	ifstream in;
	filename = argv[1];
	in.open(filename);
	in >> virtAdd;
	in >> phyAdd;
	in >> page;

	int offset = log2(page);
	int ppnSize = phyAdd - offset;

	//read in table values 
	//valid - permission - ppn - use bit
	int i = 0;
	while(in >> value)
	{
		for(int j = 0; j < 4; j++)
		{
			if(j == 2)
			{//ppn - convert to binary 
				table[i][j] = decToBin(value, ppnSize);
			}
			else{
				table[i][j] = to_string(value);
			}
			//cout << table[i][j] << " ";
			if(j != 3)
				in >> value;
		}
		i++;
		//cout << endl;
	}
	in.close();
	
	
	//cout << "Offset " << offset << endl;

	cout << "Enter a hex value (q to quit) : ";
	string hexVal;
	cin >> hexVal;
	while(hexVal != "q")
	{
		string hexBin = hexToBin(hexVal, virtAdd);
		string toIndex = hexBin.substr(0, virtAdd - offset);
		string offsetBits = hexBin.substr(virtAdd - offset, offset);
		bitset<64> bits(toIndex);
		int idx = bits.to_ulong();
		string pa = table[idx][2] + offsetBits;
		
		cout << "Physical Address: ";
		if(table[idx][0] == "0" && table[idx][1] == "1")
		{
			cout << "DISK" << endl;
		}
		else if(table[idx][1] == "0")
		{
			cout << "SEGFAULT" << endl;
		}
		else
		{
			cout << binToHex(pa) << endl;
		}
		cout << "Enter a hex value (q to quit) : ";
		cin >> hexVal;
	}
	return 0;
}
