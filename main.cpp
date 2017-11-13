#include <iostream>

#include "My_Cloud.h"

int main ()
{
	int ObjectSize;	

	float 
		FilterX1, FilterX2,
		FilterZ1, FilterZ2,
		FilterY1, FilterY2;

	cout<<"Object size: ";
	cin>>ObjectSize;

	cout<<"Filter X first parameter: ";
	cin>>FilterX1;
	cout<<"Filter X second parameter: ";
	cin>>FilterX2;

	cout<<"Filter Y first parameter: ";
	cin>>FilterY1;
	cout<<"Filter Y second parameter: ";
	cin>>FilterY2;

	cout<<"Filter Z first parameter: ";
	cin>>FilterZ1;
	cout<<"Filter Z second parameter: ";
	cin>>FilterZ2;
	
	My_Cloud my_cloud(FilterX1,FilterX2,FilterY1,FilterY2,FilterZ1,FilterZ2,ObjectSize);
	my_cloud.Run();

    return 0;
}