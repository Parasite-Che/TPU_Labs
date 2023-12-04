#include <iostream>
#include <cassert>
#include <chrono>

#define ROWS 2
#define COLUMN 10

using namespace std;
using namespace std::chrono;


void CreatingAndChecking(uint64_t* size, bool* isEnoughMem);
int8_t** Change(int8_t** m, unsigned char num_mass, unsigned char num_of_change, int8_t num);
int8_t** ChangeWithPointer(int8_t** m, unsigned char num_mass, unsigned char num_of_change, int8_t num);
int8_t** InitializeDoubleMas(int8_t rows, int8_t columns);
int8_t* InitializeMas(int8_t size);
void ShowMas(int8_t* mas, int8_t size);
void DeleteDoubleMas(int8_t** mases, int8_t rows, int8_t columns);

int main() //mas = new int[2 ^ 1073741824]; // 2 ^ (2 ^ 30 - 1)
{
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));
	uint64_t* len = new uint64_t;
	bool* isEnoughMem = new bool;
	
	
	*len = 3000000000;
	*isEnoughMem = true;
	

	/*
	while (*isEnoughMem)
	{
		*len += 2000000;
		CreatingAndChecking(len, isEnoughMem);
	}
	*/

	int8_t** mases = InitializeDoubleMas(ROWS, COLUMN);
	cout << "Два изначальных массива" << endl;
	ShowMas(mases[0], COLUMN);
	ShowMas(mases[1], COLUMN);
	
	int8_t** mases2 = Change(mases, 0, 20, 100);
	
	int8_t num = 10;

	//assert(num >= 0 && num < COLUMN);
	//cout << (int)mases2[1][num] << endl;

	cout << endl << "Выход за границы первого массива, меняют случайную переменную во втором" << endl;
	ShowMas(mases2[0], COLUMN);
	ShowMas(mases2[1], COLUMN);

	DeleteDoubleMas(mases2, ROWS, COLUMN);
	mases2 = ChangeWithPointer(mases, 0, 20, 100);
	cout << endl << "Выход за границы первого массива, меняют случайную переменную во втором (указатели)" << endl;
	ShowMas(mases2[0], COLUMN);
	ShowMas(mases2[1], COLUMN);

	DeleteDoubleMas(mases, ROWS, COLUMN);
	DeleteDoubleMas(mases2, ROWS, COLUMN);

	int* Massive = new int[1000000];

	char c = ' ';

	while (c != 'q') {
		cin >> c;
		cout << endl;
	}
	c = ' ';
	delete[] Massive;

	while (c != 'q') {
		cin >> c;
		cout << endl;
	}


	size_t steps = 32 * 1024 * 1024; // Arbitrary number of steps
	unsigned int lengthMod = 256;

	for (size_t i = 0; lengthMod < steps; i++)
	{
		auto start = high_resolution_clock::now();
		lengthMod = 256 * pow(2, i);
		int32_t* arr = new int32_t[lengthMod]; // (x & lengthMod) is equal to (x % arr.Length)
		auto end = high_resolution_clock::now();
		delete[] arr;
		auto duration = duration_cast<microseconds>(end - start);
		cout << "size : " << (lengthMod * 32) / 8192 << "kb ::" << " time : " << duration.count() << "ms" << endl;
	}



	c = ' ';
	while (c != 'q') {
		cin >> c;
		cout << endl;
	}

	return 0;
}

void CreatingAndChecking(uint64_t* size, bool* isEnoughMem)
{
	try
	{
		int* Massive = new int[*size];
		cout << *size << endl;
		delete[] Massive;
	}
	catch (...)
	{
		cout << *size << " элементов :: " << (*size * 32) / (8 * 1024 * 1024) << " Mb" << endl;
		*isEnoughMem = false;
	}
}

int8_t** Change(int8_t** m, unsigned char num_mass, unsigned char num_of_change, int8_t num) {
	int8_t** mass = InitializeDoubleMas(ROWS, COLUMN);
	for (size_t i = 0; i < ROWS; i++) {
		for (size_t j = 0; j < COLUMN; j++) {
			mass[i][j] = m[i][j];
		}
	}
	if ((num_of_change >= 0 && num_of_change < COLUMN) && (num_mass >= 0 && num_mass < ROWS)) {
		mass[num_mass][num_of_change] = num;
	}
	else {
		mass[num_mass ^ 1][rand() % COLUMN] = (int8_t)0xFF;
	}
	return mass;
}

int8_t** ChangeWithPointer(int8_t** m, unsigned char num_mass, unsigned char num_of_change, int8_t num) {
	int8_t** mass = InitializeDoubleMas(ROWS, COLUMN);
	for (size_t i = 0; i < ROWS; i++) {
		for (size_t j = 0; j < COLUMN; j++) {
			mass[i][j] = m[i][j];
		}
	}
	if ((num_of_change >= 0 && num_of_change < COLUMN) && (num_mass >= 0 && num_mass < ROWS)) {
		*(*(mass + num_mass) + num_of_change) = num;
	}
	else {
		*(*(mass + (num_mass ^ 1)) + (rand() % 10)) = (int8_t)0xFF;
	}
	return mass;
}

int8_t** InitializeDoubleMas(int8_t rows, int8_t columns) {
	int8_t** mases = new int8_t * [rows];
	for (size_t i = 0; i < rows; i++) {
		mases[i] = new int8_t[columns];
		for (size_t j = 0; j < columns; j++) {
			mases[i][j] = rand() % 255;
		}
	}
	return mases;
}

void DeleteDoubleMas(int8_t** mases, int8_t rows, int8_t columns) {
	for (size_t i = 0; i < rows; i++) {
		delete[] mases[i];
	}
	delete[] mases;
}

int8_t* InitializeMas(int8_t size) {
	int8_t* mases = new int8_t [size];
	for (size_t j = 0; j < size; j++) {
		mases[j] = rand() % 255;
	}
	return mases;
}

void ShowMas(int8_t* mas, int8_t size) {
	for (size_t i = 0; i < size; i++) {
		if (i != (size - 1)) {
			cout << (int)mas[i] << ", ";
		}
		else {
			cout << (int)mas[i] << endl;
		}
	}
}
