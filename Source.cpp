#include <conio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

string MarkerName(int num)
{
	string str;
	str = "marker.";
	str.append(to_string(num));
	return str;
}

void MarkerOne(int& it)
{
	int num;
	double x, y, z;
	ofstream mark(MarkerName(it));
	ifstream struc("unstruc_surface_in.dat");
	mark << "TITLE=\"3D TRIANGULAR SURFACE DATA\"" << std::endl;
	mark << "VARIABLES= \"X\", \"Y\", \"Z\"" << std::endl;
	mark << "ZONE T=\"UNSTRUC\" N= 606 " << "E= 804 " << "F=FEPOINT " << "ET=TRIANGLE" << std::endl;

	struc.seekg(10);
	while (struc >> num >> x >> y >> z)
	{
		mark << x << " " << y << " " << z << endl;
	}
	mark.close();
	struc.close();
	it++;
}

void MarkerNext(int& it)
{
	int num;
	double x, y, z, dx, dy, dz;
	double dt = 1.0f / 200.0f;
	ofstream mark(MarkerName(it));
	ifstream prevMark(MarkerName(it - 1));
	ifstream fort("fort.41");


	mark << "TITLE=\"3D TRIANGULAR SURFACE DATA\"" << endl;
	mark << "VARIABLES= \"X\", \"Y\", \"Z\"" << endl;
	mark << "ZONE T=\"UNSTRUC\" N= 606 " << "E= 804 " << "F=FEPOINT " << "ET=TRIANGLE" << endl;
	prevMark.seekg(112);
	fort.seekg(16);
	while (prevMark >> num >> x >> y >> z &&
		       fort >> num >> dx >> dy >> dz)
	{
		mark << x + dx * dt << " " << y + dy * dt << " " << z + dx * dt << endl;
		if (num == 580) break;
	}
	mark.close();
	prevMark.close();
	fort.close();
	it++;
}

int main()
{
	int iteration = 1;
	cout << "Working ..." << endl;
	MarkerOne(iteration);
	MarkerNext(iteration);
	cout << "Done!" << endl;
	while (!_kbhit());
	return 0;
}