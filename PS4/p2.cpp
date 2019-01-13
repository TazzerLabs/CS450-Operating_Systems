/*
 * CS 450
 * Problem Set 4 #2
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
	string vtable[64][4]; //default virtual table size
	string ptable[64][3]; //default physical table size
	string filename;
	ifstream in;
	filename = argv[1];
	in.open(filename);
	in >> virtAdd;
	in >> phyAdd;
	in >> page;

	int offset = log2(page);
	int ppnSize = phyAdd - offset;

	//read in vtable values 
	int psize = 0; //physical table size
	int vsize = 0; //virtual table size
	while(in >> value)
	{
		//valid - permission - ppn - use bit
		for(int j = 0; j < 4; j++)
		{
			if(j == 2)
			{//ppn - convert to binary 
				vtable[vsize][j] = decToBin(value, ppnSize);
			}
			else{
				vtable[vsize][j] = to_string(value);
			}
			if(j != 3)
				in >> value;
		}
		vsize++;
	}

	in.close();
	//ptable - use - physical - virtual
	for(int m = 0; m < vsize; m++)
	{
		if(vtable[m][0] == "1")
		{
			ptable[psize][0] = vtable[m][3];
			ptable[psize][1] = binToHex(vtable[m][2]);
			ptable[psize][2] = binToHex(decToBin(m,ppnSize));
			psize++;
		}
	}
	
	

	cout << "Enter a hex value (q to quit) : ";
	string hexVal;
	cin >> hexVal;
	int clockidx = 0;
	while(hexVal != "q")
	{
		string hexBin = hexToBin(hexVal, virtAdd);
		string toIndex = hexBin.substr(0, virtAdd - offset);
		string offsetBits = hexBin.substr(virtAdd - offset, offset);
		bitset<64> bits(toIndex);
		int idx = bits.to_ulong();
		string pa = vtable[idx][2] + offsetBits;
		
		cout << "Physical Address: ";
		//UPDATE PTABLE WITH USE
		if(vtable[idx][0] == "0" && vtable[idx][1] == "1")
		{
			cout << "PAGE FAULT" << endl;
			bool replacing = true;
			while(replacing)
			{
				if(clockidx == psize)
					clockidx = 0;
				if(ptable[clockidx][0] == "1")
				{
					ptable[clockidx][0] = "0";
				}
				else
				{
					//do the replacing and address translation
					string vaddress = hexToBin(ptable[clockidx][2], virtAdd);

					bitset<64> vbits(vaddress);
					int vidx = vbits.to_ulong();
					vtable[vidx][0] = "0";
					vtable[vidx][2] = "x";
					
					vtable[idx][0] = "1";
					vtable[idx][1] = "1";
					vtable[idx][2] = hexToBin(ptable[clockidx][1], ppnSize);
					vtable[idx][3] = "1";

					ptable[clockidx][2] = toIndex;

					replacing = false;
				}
				clockidx++;
			}
			string pa = vtable[idx][2] + offsetBits;
			cout << "0x" << binToHex(pa) << endl;
		}
		else if(vtable[idx][1] == "0")
		{
			cout << "SEGFAULT" << endl;
		}
		else
		{
			cout << "0x" << binToHex(pa) << endl;
			for(int x = 0; x < psize; x++)
			{
				if(ptable[x][1] == vtable[idx][2])
				{
					ptable[x][0] = "1";
				}
			}
		}
		cout << "Enter a hex value (q to quit) : ";
		cin >> hexVal;
	}
	return 0;
}
