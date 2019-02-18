#include <cassert>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


struct Vec3
{
	Vec3 operator+( const Vec3& _other )const
	{
		return { x + _other.x, y + _other.y, z + _other.z };
	}
	Vec3 operator*( const double _scalar )const
	{
		return{ x * _scalar, y * _scalar, z * _scalar };
	}
	double x = 0., y = 0., z = 0.;
};

std::ostream& operator<<( std::ostream& stream, const Vec3& v )
{
	stream << v.x << " " << v.y << " " << v.z << '\n';
}
std::istream& operator>>( std::istream& stream, Vec3& v )
{
	stream >> v.x >> v.y >> v.z;
}
std::string MakeHeader()
{
	return {
		"TITLE=\"3D TRIANGULAR SURFACE DATA\"\n"
		"VARIABLES= \"X\", \"Y\", \"Z\"\n"
		"ZONE T=\"UNSTRUC\" N= 606 E= 804 F=FEPOINT ET=TRIANGLE\n"
	};
}
std::vector<std::string> ExtractLines( std::string filename, int header_size, int line_count = -1 )
{
	std::ifstream file( filename );
	assert( file.is_open() );
	file.seekg( header_size );

	std::vector<std::string> lines;
	std::string line;

	if(line_count == -1)
	{
		while( std::getline( file, line ) )
		{
			if( !line.empty() )
				lines.push_back( std::move( line ) );
		}
		lines.shrink_to_fit();
	}
	else
	{
		lines.reserve( line_count );
		int count = 0;
		while( std::getline( file, line ) && count < line_count )
		{
			if( !line.empty() )
				lines.push_back( std::move( line ) );

			count++;
		}
	}

	file.close();

	return lines;
}
std::string MarkerName( int num )
{
	return ( "marker." + std::to_string( num ) );
}

void MarkerOne( int& it )
{
	const auto lines = ExtractLines( "unstruc_surface_in.dat", 10 );

	std::ofstream mark( MarkerName( it ) );
	mark << MakeHeader();

	for( const auto& line : lines )
	{
		int num = 0;
		Vec3 v;
		std::stringstream ss;
		ss << line;
		ss >> num >> v;

		mark << v;
	}
	mark.close();
	it++;
}

void MarkerNext( int& it )
{
	std::ofstream mark( MarkerName( it ) );
	mark << MakeHeader();

	const auto prevMarkLines = ExtractLines( MarkerName( it - 1 ), 114, 580 );
	const auto fortLines = ExtractLines( "fort.41", 16, 580 );
	
	const double dt = 1.0f / 200.0f;
	for( int i = 0; i < prevMarkLines.size(); ++i )
	{
		std::stringstream prev_ss;
		prev_ss << prevMarkLines[ i ];

		Vec3 pos;
		prev_ss >> pos;
		
		std::stringstream fort_ss;
		fort_ss << fortLines[ i ];

		Vec3 vel;
		fort_ss >> vel;
		
		mark << ( pos + ( vel * dt ) );
	}

	mark.close();

	it++;
}

int main()
{
	int iteration = 1;
	std::cout << "Working ..." << '\n';
	MarkerOne( iteration );
	MarkerNext( iteration );
	std::cout << "Done!" << '\n';
	while( !_kbhit() );
	return 0;
}
