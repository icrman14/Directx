#include <cassert>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


std::string MarkerName(int num)
{
	std::string str;
	str = "marker.";
	str.append(std::to_string(num));
	return str;
}

void getPosition(std::vector<double> &x, std::vector<double> &y, std::vector<double> &z)
{
	std::ifstream file("unstruc_surface_in.dat");
	assert(file.is_open());
	file.seekg(10);
	int num;
	double xp;
	double yp;
	double zp;
	int counter = 0;
	file.precision(8);
	while (file >> num >> xp >> yp >> zp)
	{
		x.push_back(xp);
		y.push_back(yp);
		z.push_back(zp);
		counter++;
		if (counter == 606) break;
	}

	file.close();
}

void getVelocities(std::vector<double> &x, std::vector<double> &y, std::vector<double> &z, int &iter)
{
	std::ifstream file("fort.41");
	assert(file.is_open());
	file.seekg(16 + 606*iter);
	int num;
	double xp;
	double yp;
	double zp;
	int counter = 0;
	file.precision(8);
	while (file >> num >> xp >> yp >> zp)
	{
		x.push_back(xp);
		y.push_back(yp);
		z.push_back(zp);
		counter++;
		if (counter == 606) break;
	}
	iter++;
	file.close();
}

void newMarker(std::vector<double> &x, std::vector<double> &y, std::vector<double> &z,
	std::vector<double> vx, std::vector<double> vy, std::vector<double> vz, int iter)  
{
	std::ofstream mark(MarkerName(iter));
	assert(mark.is_open());
	mark.precision(8);
	mark << "TITLE=\"3D TRIANGULAR SURFACE DATA\"" << std::endl;
	mark << "VARIABLES= \"X\", \"Y\", \"Z\"" << std::endl;
	mark << "ZONE T=\"UNSTRUC\" N= 606 E= 808 F=FEPOINT ET=TRIANGLE" << std::endl;
	double dt = 1.0 / 200.0;
	for (int i = 0; i < 606; i++)
	{
		double newx = x.at(i) + vx.at(i) * dt;
		double newy = y.at(i) + vy.at(i) * dt;
		double newz = z.at(i) + vz.at(i) * dt;

		mark << newx << " " << newy << " " << newz << std::endl;

		x.at(i) = newx;
		y.at(i) = newy;
		z.at(i) = newz;
	}
	int dnp = 202;
	for (int i = 1; i <= dnp + 1; i++)
	{
		mark << i << " " << i + 1 << " " << dnp + i << std::endl;
		mark << i + 1 << " " << dnp + i + 1 << " " << dnp + i << std::endl;
		mark << dnp + i << " " << dnp + i + 1 << " " << 2 * dnp + i << std::endl;
		mark << dnp + 1 + i << " " << 2 * dnp + i + 1 << " " << 2 * dnp + i << std::endl;
		if (i + 2 > dnp)
		{
			mark << "1" << " " << dnp << " " << dnp * 2 << std::endl; // 1, 202, 404
			mark << "1" << " " << dnp + 1 << " " << dnp * 2 << std::endl; // 1, 203, 404
			mark << dnp + 1 << " " << dnp * 2 << " " << dnp * 3 << std::endl; // 203, 404, 606
			mark << dnp + 1 << " " << dnp * 2 + 1 << " " << dnp * 3 << std::endl; // 203 405 606
			break;
		}
	}
	mark.close();
}
int main()
{
	std::vector<double> xpos;
	std::vector<double> ypos;
	std::vector<double> zpos;

	std::vector<double> dx;
	std::vector<double> dy;
	std::vector<double> dz;
	int time = 0;
	std::cout << "Working ..." << std::endl;
	//getPosition(xpos, ypos, zpos);
	//getVelocities(dx, dy, dz, time);
	//getVelocities(dx, dy, dz, time);
	for (int i = 0; i < 200; i++) {
		getPosition(xpos, ypos, zpos);
		getVelocities(dx, dy, dz, time);
		newMarker(xpos, ypos, zpos, dx, dy, dz, time);
	}
	std::cout << "Done!" << std::endl;
	while (!_kbhit());

	return 0;
}