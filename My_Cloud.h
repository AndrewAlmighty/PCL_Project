#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/passthrough.h>

#pragma once
class My_Cloud
{
public:
	My_Cloud(float,float,float,float,float,float,int);
	~My_Cloud(void);

	void Run(); //Tutaj jest petla glowna

private:
	void Filter();	//filtrowanie glowne
	void FilterLegs();	//filtrowanie nog
	void IsObjectInScene();	//sprawdzanie czy obiekt jest w scenie
	bool LoadFile();	//wczytujemy pliki
	void CalculateVolumes();	//obliczamy objetosci
	void IsObjectGesture();	//Sprawdzamy czy obiekt wykonuje gest
	void setPointsBorder();	//ustawienie PointsBorder na wys. 2/3 tulowia -> gdy podnosza sie rece

	bool CheckPointsBorder();	//Sprawdzenie czy rece dalej sa w gorze

	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr 
		external_cloud, //chmura zewnetrzna, do stwierdzenia ze obiekt opuscil scene
		cloud;	//chmura glowna
	
	pcl::visualization::CloudViewer viewer;	

	//Zmienne potrzebne do filtrowania
	float 
		filterX1, filterX2, 
		filterY1, filterY2, 
		filterZ1, filterZ2;

	int 
		ObjectSizeBorder, FileNumber, //Minimalna ilosc punktow do stwierdzenia ze obiekt jest w scenie; Numer pliku
		average_volume_index, NumberOfFrames, // Index potrzebny do liczenia sr. obj.; Ustala, ile kadrow poswiecic na ustalenie sredniej objetosci
		not_gesture_index, gesture_index, // Warunek sie musi spelnic od 3 razy, aby stwierdzic wykonywanie lub brak wykonywania gestu
		PointsBorder; //Kolejna granica, ilosc punktow po kolejnym filtrowaniu, gdy stwierdzimy wykonywanie gestu. 

	bool ObjectisinScene, IsGesture;	//glowne flagi
	float average_volume, volume;	//Do obliczania zmian objetosci
};

