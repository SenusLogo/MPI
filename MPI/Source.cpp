#include <iostream>
#include <Windows.h>
#include <time.h>
#include "mpi.h"

using namespace std;

#define ZERO_PROCCESSOR 0

int main(int* argc, char** argv)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	//int* A;
	//int* B;
	//int* local_sum;
	//int local_summ;

	//int size = 0;
	//int summ = 0;

	int thread, thread_size;
	//double* number;
	char hello[12] = "hello world";

	MPI_Init(argc, &argv);

	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	//number = new double[2]; // new memory for each processor

	//if (thread == ZERO_PROCCESSOR)
	//{
	//	cout << "Size array: ";
	//	cin >> size;
	//}

	//MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//A = new int[size];
	//B = new int[size];

	//if (thread == ZERO_PROCCESSOR)
	//{
	//	srand(unsigned int(time(0)));
	//	for (int i(0); i < size; i++)
	//		A[i] = rand() % 20;

	//	for (int i(0); i < size; i++)
	//	{
	//		cout << A[i] << " ";
	//	}
	//	cout << endl;
	//}

	//MPI_Scatter(A, size / thread_size, MPI_INT, B, size / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	//local_sum = new int[thread_size];
	//for (int i(0); i < thread_size; i++)
	//	local_sum[i] = 0;

	//local_summ = 0;
	//for (int j(0); j < size / thread_size; j++)
	//	local_summ += B[j];

	//MPI_Gather(&local_summ, 1, MPI_INT, local_sum, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//if (thread == ZERO_PROCCESSOR)
	//{
	//	int result1 = 0;
	//	for (int i(0); i < size; i++)
	//		result1 += A[i];
	//	cout << result1 << endl;

	//	int result = 0;
	//	for (int i(0); i < thread_size; i++)
	//		result += local_sum[i];
	//	cout << result << endl;
	//}

	if (thread == ZERO_PROCCESSOR)
	{
		//cout << "Enter number: ";
		//for (int i(0); i < 2; i++)
		//	cin >> number[i]; // enter 2 number

		//cout << "a = " << number[0] << "\nb = " << number[1] << endl;

		cout << hello << " " << thread << endl;
		MPI_Send(hello, 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

		//cout << "Send successful." << endl;

	}
	else if (thread > ZERO_PROCCESSOR && thread < thread_size - 1)
	{
		MPI_Recv(&hello, 1, MPI_CHAR, thread - 1, 0, MPI_COMM_WORLD, &status);

		cout << hello << " " << thread << endl;

		MPI_Send(&hello, 1, MPI_CHAR, thread + 1, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(&hello, 1, MPI_CHAR, thread - 1, 0, MPI_COMM_WORLD, &status);

		cout << hello << " " << thread << endl;
	}

	/*cout << "a " << operation[thread] << " b = " << (thread == 0 ? number[0] + number[1] : thread == 1 ? number[0] - number[1] : thread == 2 ? number[0] / number[1] : number[0] * number[1]);*/

	/*switch (thread)
	{
	case 0:
		cout << "a + b = " << number[0] + number[1] << endl;
		break;
	case 1:
		cout << "a - b = " << number[0] - number[1] << endl;
		break;
	case 2:
		cout << "a * b = " << number[0] * number[1] << endl;
		break;
	case 3:
		cout << "a / b = " << number[0] / number[1] << endl;
		break;

	}*/

	MPI_Finalize();

	return 1;
}