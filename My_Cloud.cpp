#include "My_Cloud.h"

#include <iostream>
#include <sstream>
#include <string>

My_Cloud::My_Cloud(float x1, float x2, float y1, float y2, float z1, float z2, int objectSize) : cloud(new pcl::PointCloud<pcl::PointXYZRGBA>), viewer("Cloud Viewer"), external_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>)
{
	filterX1=x1;
	filterX2=x2; 
	filterY1=y1;
	filterY2=y2;
	filterZ1=z1;
	filterZ2=z2;

	ObjectSizeBorder=objectSize;
	ObjectisinScene=false;
	IsGesture=false;
	FileNumber=1;	
	average_volume_index=0; 
	average_volume=0;
	NumberOfFrames=10; 
	not_gesture_index=0;
	gesture_index=0;
	PointsBorder=0;
}

My_Cloud::~My_Cloud(void)
{
	std::cout<<endl;
	system("PAUSE");
}

void My_Cloud::Run()
{	
	bool NoMoreFiles=false; //Petla sie wykonuje dopóki nie zabraknie plikow		

	while(NoMoreFiles!=true)
	{	
		NoMoreFiles=LoadFile(); //Wczytujemy plik
		Filter(); //Filtrujemy to co nas nie interesuje
		IsObjectInScene();	//Sprawdzamy czy obiekt znajduje sie w specjalnie wyznaczonym szescianie

		if(ObjectisinScene)	//Jesli jest
		{
			viewer.showCloud(cloud); //Pokazujemy go
			FilterLegs(); //Usuwamy nogi
			CalculateVolumes();	//Obliczamy objetosci					
			
			if(average_volume_index>=NumberOfFrames+1) //Gdy ustalimy juz srednia objetosc, sprawdzamy czy wykonywany jest gest
				IsObjectGesture(); //Sprawdzamy czy wykonujemy gest
		}

		if(viewer.wasStopped()==true) //gdy zamkniemy viewera
			break;	
	}	
}

void My_Cloud::Filter()
{
	pcl::PassThrough<pcl::PointXYZRGBA> filter_z;
	pcl::PassThrough<pcl::PointXYZRGBA> filter_x;
	pcl::PassThrough<pcl::PointXYZRGBA> filter_y;
	pcl::PassThrough<pcl::PointXYZRGBA> external_filter_z;
	pcl::PassThrough<pcl::PointXYZRGBA> external_filter_x;
	pcl::PassThrough<pcl::PointXYZRGBA> external_filter_y;

	//wewnêtrzna	
	filter_z.setInputCloud (cloud);
	filter_z.setFilterFieldName ("z");
	filter_z.setFilterLimits (filterZ1, filterZ2); 
	filter_z.filter (*cloud);	
	
	filter_x.setInputCloud (cloud);
	filter_x.setFilterFieldName ("x");
	filter_x.setFilterLimits (filterX1 , filterX2);
	filter_x.filter (*cloud);	

	filter_y.setInputCloud (cloud);
	filter_y.setFilterFieldName ("y"); 
	filter_y.setFilterLimits (filterY1 , filterY2);
	filter_y.filter (*cloud);

	//zewnêtrzna
	float
		external_filterZ1=filterZ1-0.15,
		external_filterZ2=filterZ2+0.15,
		external_filterX1=filterX1-0.15,
		external_filterX2=filterX2+0.15,
		external_filterY1=filterY1-0.15,
		external_filterY2=filterY2+0.15;
	
	external_filter_z.setInputCloud (external_cloud);
	external_filter_z.setFilterFieldName ("z");
	external_filter_z.setFilterLimits (external_filterZ1, external_filterZ2); 
	external_filter_z.filter (*external_cloud);	
	
	external_filter_x.setInputCloud (external_cloud);
	external_filter_x.setFilterFieldName ("x");
	external_filter_x.setFilterLimits (external_filterX1, external_filterX2);
	external_filter_x.filter (*external_cloud);	
	
	external_filter_y.setInputCloud (external_cloud);
	external_filter_y.setFilterFieldName ("y");
	external_filter_y.setFilterLimits (external_filterY1, external_filterY2);
	external_filter_y.filter (*external_cloud);
	
}

void My_Cloud::FilterLegs()
{
	//Do znalezienia pkt polozonego najwyzej i najnizej
	pcl::PointXYZRGBA min_pt, max_pt; 
	pcl::getMinMax3D(*cloud,min_pt,max_pt);

	//Interesuje mnie 2/3 ciala obiektu, od bioder do gory
	float height, max_y=0;
	height=max_pt.y-min_pt.y;
	height=(66*height)/100;

	max_y=height+min_pt.y;	//max jest na dole, do minimum dodaje obliczone h i mam 2nd parametr do filtra

	pcl::PassThrough<pcl::PointXYZRGBA> filter_y;
	filter_y.setInputCloud (cloud);
	filter_y.setFilterFieldName ("y"); 
	filter_y.setFilterLimits (filterY1 , max_y);
	filter_y.filter (*cloud);
}

void My_Cloud::IsObjectInScene()
{
	if(ObjectisinScene==false && ObjectSizeBorder <= cloud->points.size())	//obiekt jest w scenie gdy go flaga == false a ilosc punktow przekracza granice
	{
		cout<<endl<<"Object is in the Scene!"<<endl;
		ObjectisinScene=true;
	}

	if(ObjectisinScene==true && ObjectSizeBorder > external_cloud->points.size()) //obiektu nie ma gdy flaga == true a ilosc punktow chmury zewn jest mniejsza niz graniczna
	{
		cout<<"Object is not in the Scene!"<<endl;
		ObjectisinScene=false;
	}
}

bool My_Cloud::LoadFile()
{
	std::ostringstream Number;
	std::string FileName;
	Number<<FileNumber;
	FileName="Frame ("+Number.str()+").pcd"; // pliki nazywaka sie Frame (x).pcd; x-> FileNumber
	FileNumber++;
	
	if(pcl::io::loadPCDFile(FileName, *external_cloud) ==-1 || pcl::io::loadPCDFile(FileName, *cloud) ==-1)
		return true;

	else
		return false;
}

void My_Cloud::CalculateVolumes()
{
	pcl::PointXYZRGBA min_pt, max_pt; 
	pcl::getMinMax3D(*cloud,min_pt,max_pt);

	float width,height,deph;

	width=max_pt.x-min_pt.x;	
	height=max_pt.y-min_pt.y;
	deph=max_pt.z-min_pt.z;
	volume=width*height*deph;	

	if(average_volume_index<NumberOfFrames) //Sumujemy pierwsze x objetosci
	{
		average_volume+=(width*height*deph);
		average_volume_index++;
	}

	else if(average_volume_index==NumberOfFrames) //obliczamy srednia
	{
			average_volume/=NumberOfFrames;
			average_volume_index++;
	}
}

//cout<<relativeVolume<<"|"<<volume<<"|"<<average_volume<<endl;	//Wyswietla wyniki

void My_Cloud::IsObjectGesture()
{
	if(ObjectisinScene==false)	//Jezeli nie wykryto obiektu, to nie ma gestu
	{
		IsGesture=false;
		return;
	}	

	float relativeVolume = volume/average_volume;	//obliczamy objetosc wzgledna

	if(!IsGesture && relativeVolume >= 1.3) //Pierwszy warunek do stwierdzenia, ze jest wykonywany gest
	{
		not_gesture_index=0;
		gesture_index++;	

		if(gesture_index>=3) //Drugi warunek do spelnienia
		{
			IsGesture=true;
			cout<<"Object is making gesture!"<<endl;	
			setPointsBorder();
		}
	}

	else 
		gesture_index=0;

	if(IsGesture && relativeVolume < 1.1 && CheckPointsBorder())	//pierwszy warunek do stwierdzenia ze nie jest wykonywany gest
	{
		gesture_index=0;
		not_gesture_index++;		

		if(not_gesture_index>=3)	//drugi warunek
		{
			IsGesture=false;
			cout<<"Object is not making gesture!"<<endl;			
		}
	}

	else
		not_gesture_index=0;
}

void My_Cloud::setPointsBorder()	//Wlasciwie to samo co w filtrowaniu nog	
{
	pcl::PointXYZRGBA min_pt, max_pt; 
	pcl::getMinMax3D(*cloud,min_pt,max_pt);

	float height, max_y=0;
	height=max_pt.y-min_pt.y;
	height=(66*height)/100;
	max_y=height+min_pt.y;

	pcl::PassThrough<pcl::PointXYZRGBA> filter_y;
	filter_y.setInputCloud (cloud);
	filter_y.setFilterFieldName ("y"); 
	filter_y.setFilterLimits (filterY1 , max_y);
	filter_y.filter (*cloud);
	
	PointsBorder=cloud->points.size();	// + ustalenie granicy
}

bool My_Cloud::CheckPointsBorder()	//Sprawdzamy czy punktow jest mniej od PointsBorder
{
	pcl::PointXYZRGBA min_pt, max_pt; 
	pcl::getMinMax3D(*cloud,min_pt,max_pt);

	float height, max_y=0;
	height=max_pt.y-min_pt.y;
	height=(66*height)/100;
	max_y=height+min_pt.y;

	pcl::PassThrough<pcl::PointXYZRGBA> filter_y;
	filter_y.setInputCloud (cloud);
	filter_y.setFilterFieldName ("y"); 
	filter_y.setFilterLimits (filterY1 , max_y);
	filter_y.filter (*cloud);

	if(PointsBorder<cloud->points.size())
		return false;

	else return true;
}