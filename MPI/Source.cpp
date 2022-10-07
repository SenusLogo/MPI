#include <iostream>
#include <Windows.h>
#include "mpi.h"

using namespace std;

#define ZERO_PROCCESSOR 0

int main(int* argc, char** argv)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	int thread, thread_size;
	double* number;

	const char operation[4] = { '+', '-', '/', '*' };

	MPI_Init(argc, &argv);

	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread_size != 4)
	{
		cout << "Кол-во процессоров не верно" << endl;
		return 1;
	}

	number = new double[2]; // new memory for each processor

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter number: ";
		for (int i(0); i < 2; i++)
			cin >> number[i]; // enter 2 number

		cout << "a = " << number[0] << "\nb = " << number[1] << endl;

		for (int thread_lvl(1); thread_lvl < thread_size; thread_lvl++)
			MPI_Send(number, 2, MPI_DOUBLE, thread_lvl, 0, MPI_COMM_WORLD);

		cout << "Send successful." << endl;

	} else MPI_Recv(number, 2, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	cout << "a " << operation[thread] << " b = " << (thread == 0 ? number[0] + number[1] : thread == 1 ? number[0] - number[1] : thread == 2 ? number[0] / number[1] : number[0] * number[1]);

	MPI_Finalize();

	return 1;
}