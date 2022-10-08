#include <iostream>
#include <Windows.h>
#include <time.h>
#include "mpi.h"

using namespace std;

#define ZERO_PROCCESSOR 0
#define LAST_PROCESSOR(a) a - 1 // rofl

void problem_1(int* argc, char** argv)
{
	int thread, thread_size;

	MPI_Init(argc, &argv);

	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	cout << "Hello world from proccessor " << thread << " of " << thread_size << endl;

	MPI_Finalize();

	return;
}

void problem_2(int* argc, char** argv)
{
	int thread, thread_size;

	MPI_Init(argc, &argv);

	double A[2];

	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread_size != 4)
	{
		cout << "so many processors! need 4." << endl;
		return;
	}

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter 2 number: "; cin >> A[0] >> A[1];
		
		for (int i(1); i < thread_size; i++)
			MPI_Send(A, 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
	}
	
	if(thread != ZERO_PROCCESSOR)
		MPI_Recv(A, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

	switch (thread)
	{
	case 0:
		cout << "a + b = " << A[0] + A[1] << endl;
		break;
	case 1:
		cout << "a - b = " << A[0] - A[1] << endl;
		break;
	case 2:
		cout << "a * b = " << A[0] * A[1] << endl;
		break;
	case 3:
		cout << "a / b = " << A[0] / A[1] << endl;
		break;
	}
	

	MPI_Finalize();

	return;
}

void problem_3(int* argc, char** argv)
{
	int thread, thread_size;
	char hello[13] = "Hello world!";

	MPI_Init(argc, &argv);

	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		MPI_Send(hello, 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
	}
	else if (thread != ZERO_PROCCESSOR && thread != LAST_PROCESSOR(thread_size))
	{
		MPI_Recv(&hello, 1, MPI_CHAR, thread - 1, 0, MPI_COMM_WORLD, &status);
		MPI_Send(&hello, 1, MPI_CHAR, thread + 1, 0, MPI_COMM_WORLD);
	}
	else
		MPI_Recv(&hello, 1, MPI_CHAR, thread - 1, 0, MPI_COMM_WORLD, &status);	

	cout << hello << " from processor " << thread << endl;

	MPI_Finalize();

	return;
}

int problem_4(int* argc, char** argv)
{
	int* A;
	int* B;
	
	int size = 0;
	int sum = 0;

	int thread, thread_size;

	MPI_Init(argc, &argv);

	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Size array: ";
		cin >> size;
	}

	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	A = new int[size];
	B = new int[size];

	if (thread == ZERO_PROCCESSOR)
	{
		srand(unsigned int(time(0)));
		for (int i(0); i < size; i++)
			A[i] = rand() % 20;

		for (int i(0); i < size; i++)
			cout << A[i] << " ";
		cout << endl;
	}

	MPI_Scatter(A, size / thread_size, MPI_INT, B, size / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	int* local_sum_all = new int[thread_size];
	int local_sum = 0;

	for (int j(0); j < size / thread_size; j++)
		local_sum += B[j];

	MPI_Gather(&local_sum, 1, MPI_INT, local_sum_all, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (thread == ZERO_PROCCESSOR)
	{
		int result_real = 0;
		for (int i(0); i < size; i++)
			result_real += A[i];
		cout << result_real << endl;

		int result_local_sum = 0;
		for (int i(0); i < thread_size; i++)
			result_local_sum += local_sum_all[i];
		cout << result_local_sum << endl;
	}

	MPI_Finalize();

	return 1;
}

int main(int* argc, char** argv)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	problem_3(argc, argv);

	return 1;
}