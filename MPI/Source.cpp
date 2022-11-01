#include <iostream>
#include <Windows.h>
#include <time.h>
#include <conio.h>
#include <string>
#include <vector>
#include <thread>
#include <iomanip>
#include <omp.h>

#include "mpi.h"

using namespace std;

#define ZERO_PROCCESSOR 0
#define LAST_PROCESSOR(a) a - 1 // rofl

//a - timer in milliseconds
#define Sleep(a) this_thread::sleep_for(chrono::milliseconds(a))

void problem_1(int* argc, char** argv)
{
	int thread, thread_size;

	MPI_Init(argc, &argv);

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

	double A[2] = { 0, 0 };

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

void problem_4(int* argc, char** argv)
{
	int* A;
	int* B;
	
	int size = 0;
	int sum = 0;

	int thread, thread_size;

	MPI_Init(argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Size array: ";
		cin >> size;
	}

	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	A = new int[size]; std::memset(A, 0, size * sizeof(int));
	B = new int[size]; std::memset(B, 0, size * sizeof(int));

	if (thread == ZERO_PROCCESSOR)
	{
		
		srand(unsigned int(time(NULL)));
		for (int i(0); i < size; i++)
			A[i] = rand() % 20;


		for (int i(0); i < size; i++)
			cout << A[i] << " ";
		cout << endl;
	}

	MPI_Scatter(A, size / thread_size, MPI_INT, B, size / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	int* local_sum_all = new int[thread_size]; std::memset(local_sum_all, 0, thread_size * sizeof(int));
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
}

void problem_5(int* argc, char** argv)
{
	int* A;
	int* B;

	int* a;
	int* b;

	int thread = 0, thread_size = 0, size = 0;

	MPI_Init(argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter size matrix NxN: ";
		cin >> size;
	}

	size *= size;
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	A = new int[size]; std::memset(A, 0, size * sizeof(int));
	B = new int[size]; std::memset(B, 0, size * sizeof(int));
	a = new int[size]; std::memset(a, 0, size * sizeof(int));
	b = new int[size]; std::memset(b, 0, size * sizeof(int));

	if (thread == ZERO_PROCCESSOR)
	{
		srand(unsigned int(time(NULL)));

		for (int i(0); i < size; i++)
			A[i] = 1 + rand() % 20;

		for (int i(0); i < size; i++)
			B[i] = 1 + rand() % 20;

		cout << "Matrix A:\n";

		for (int i(1); i <= size; i++)
			cout << setw(2) << A[i - 1] << (i % int(sqrt(size)) == 0 ? "\n" : " ");

		cout << "\nMatrix B:\n";

		for (int i(1); i <= size; i++)
			cout << B[i - 1] << (i % int(sqrt(size)) == 0 ? "\n" : " ");

		cout << "==========================\n\n";
	}

	MPI_Scatter(A, size / thread_size, MPI_INT, a, size / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	MPI_Scatter(B, size / thread_size, MPI_INT, b, size / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	int* result = new int[size]; std::memset(result, 0, size * sizeof(int));
	int* local_sum = new int[size / thread_size]; std::memset(local_sum, 0, (size / thread_size) * sizeof(int));

	for (int j(0); j < size / thread_size; j++)
		local_sum[j] += (a[j] + b[j]);

	MPI_Gather(local_sum, size / thread_size, MPI_INT, result, size / thread_size, MPI_INT, 0, MPI_COMM_WORLD);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Real solution:\n";

		for (int i(1); i <= size; i++)
			cout << A[i - 1] + B[i - 1] << (i % int(sqrt(size)) == 0 ? "\n" : " ");
		
		cout << "\n=====================\n\nParallel programming solution:\n";

		for (int i(1); i <= size; i++)
			cout << result[i - 1] << (i % int(sqrt(size)) == 0 ? "\n" : " ");
	}

	MPI_Finalize();
}

void problem_6(int* argc, char** argv)
{
	int* A;
	int* B;

	int* b;

	int thread = 0, thread_size = 0, size_matrix = 0, size_vector = 0;

	MPI_Init(argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter size matrix NxN: ";
		cin >> size_vector;
	}

	size_matrix = int(pow(size_vector, 2));

	MPI_Bcast(&size_vector, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&size_matrix, 1, MPI_INT, 0, MPI_COMM_WORLD);

	A = new int[size_vector]; std::memset(A, 0, size_vector * sizeof(int));
	B = new int[size_matrix]; std::memset(B, 0, size_matrix * sizeof(int));
	b = new int[size_matrix]; std::memset(b, 0, size_matrix * sizeof(int));

	if (thread == ZERO_PROCCESSOR)
	{
		srand(unsigned int(time(NULL)));

		for (int i(0); i < size_vector; i++)
			A[i] = 1 + rand() % 20;

		for (int i(0); i < size_matrix; i++)
			B[i] = 1 + rand() % 20;

		cout << "\nMatrix:\n";

		for (int i(1); i <= size_matrix; i++)
			cout << B[i - 1] << (i % size_vector == 0 ? "\n" : " ");

		cout << "\n\nVector:\n";

		for (int i(0); i < size_vector; i++)
			cout << A[i] << endl;

		cout << "==========================\n\n";
	}

	MPI_Bcast(A, size_vector, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(B, size_matrix / thread_size, MPI_INT, b, size_matrix / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	int* result = new int[size_vector]; std::memset(result, 0, size_vector * sizeof(int));
	int* local_sum = new int[size_vector / thread_size]; std::memset(local_sum, 0, (size_vector / thread_size) * sizeof(int));

	for (int j(0); j < size_matrix / thread_size; j++)
	{
		cout << A[j % size_vector] << " * " << b[j] << " = " << A[j % size_vector] * b[j] << endl;
		local_sum[j / thread_size] += A[j % size_vector] * b[j];
	}

	MPI_Gather(local_sum, size_vector / thread_size, MPI_INT, result, size_vector / thread_size, MPI_INT, 0, MPI_COMM_WORLD);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "\n=====================\n\nParallel programming solution:\n";

		for (int i(0); i < size_vector; i++)
			cout << result[i] << " ";
	}

	MPI_Finalize();
}

void problem_7(int* argc, char** argv)
{
	int* A;
	int* B;
	int* D;

	int* a;
	int* b;
	int* d;

	int thread = 0, thread_size = 0, size_matrix = 0, size_vector = 0, iterations = 0;

	double eps = 0.;

	MPI_Init(argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter size matrix NxN: ";
		cin >> size_vector;

		cout << "Precision ( eps ) = ";
		cin >> eps;
	}

	MPI_Bcast(&size_vector, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&eps, 1, MPI_DOUBLE, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	size_matrix = int(pow(size_vector, 2));

	A = new int[size_vector]; std::memset(A, 0, size_vector * sizeof(int));
	B = new int[size_matrix]; std::memset(B, 0, size_matrix * sizeof(int));
	D = new int[size_vector]; std::memset(D, 0, size_vector * sizeof(int));
	
	if (thread == ZERO_PROCCESSOR)
	{
		srand(unsigned int(time(NULL)));

		for (int i(0); i < size_vector; i++)
			A[i] = rand() % 20;

		
		for (int i(0), l(0); i < size_matrix; i++)
		{
			if (i % (size_vector + 1) == 0)
			{
				B[i] = 100 + rand() % 20;
				D[l++] = B[i];
			}
			else
				B[i] = rand() % 20;			
		}
		
		cout << "\nMatrix:\n";

		for (int i(1); i <= size_matrix; i++)
			cout << B[i - 1] << (i % size_vector == 0 ? "\n" : " ");

		cout << "\n\nVector:\n";

		for (int i(0); i < size_vector; i++)
			cout << A[i] << endl;

		cout << "==========================\n\n";
	}

	int N = size_vector / thread_size;
	int M = size_matrix / thread_size;

	a = new int[N]; std::memset(a, 0, N * sizeof(int));
	d = new int[N]; std::memset(d, 0, N * sizeof(int));

	b = new int[M]; std::memset(b, 0, M * sizeof(int));

	MPI_Scatter(D, N, MPI_INT, d, N, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	MPI_Scatter(A, N, MPI_INT, a, N, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	MPI_Scatter(B, M, MPI_INT, b, M, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	
	double* X = new double[size_vector]; std::memset(X, 0, size_vector * sizeof(double));
	double* X_buff = new double[N]; std::memset(X_buff, 0, N * sizeof(double));
	double* TempX = new double[N]; std::memset(TempX, 0, N * sizeof(double));

	int* flags = new int[thread_size]; std::memset(flags, 0, thread_size * sizeof(int));

	double norm = 0.;
	int exit = 1;

	do
	{
		for (int k(0); k < N; k++)
		{
			TempX[k] = a[k];

			for (int j(0); j < size_vector; j++)
			{
				if (b[j + k * size_vector] != d[k])
					TempX[k] -= b[j + k * size_vector] * X[j];
			}

			TempX[k] /= d[k];
		}

		MPI_Scatter(X, N, MPI_DOUBLE, X_buff, N, MPI_DOUBLE, ZERO_PROCCESSOR, MPI_COMM_WORLD);

		double norm = fabs(X_buff[0] - TempX[0]);
		for (int i(0); i < N; i++)
		{
			if (fabs(X_buff[i] - TempX[i]) > norm)
				norm = fabs(X_buff[i] - TempX[i]);

			X_buff[i] = TempX[i];
		}

		int flag = norm < eps;

		for (int i(0); i < thread_size; i++)
		{
			MPI_Gather(X_buff, N, MPI_DOUBLE, X, N, MPI_DOUBLE, i, MPI_COMM_WORLD);
			MPI_Gather(&flag, 1, MPI_INT, flags, 1, MPI_INT, i, MPI_COMM_WORLD);
		}

		exit = 1;

		for (int i(0); i < thread_size; i++)
			exit *= flags[i];

		iterations++;
	} while (!exit);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Answer: ";

		for (int i(0); i < size_vector; i++)
			cout << X[i] << " ";

		cout << "\n\nCount iterations: " << iterations << endl;
	}

	delete[] A;
	delete[] B;
	delete[] D;

	delete[] a;
	delete[] b;
	delete[] d;

	delete[] X;
	delete[] X_buff;
	delete[] TempX;
	delete[] flags;

	MPI_Finalize();
}

int main(int* argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	/*vector<string> menu = { "Практика 1", "Практика 2", "Практика 3", "Практика 4" };

	int iter = 0;
	int choise = 0;

	_getch();
	do
	{
		system("cls");
		for (int i(0); i < menu.size(); i++)
			cout << menu.at(i).c_str() << (i == choise ? "  <--- " : "") << endl;

		iter = _getch();

		switch (iter)
		{
		case 72:
			choise = choise == 0 ? 3 : choise - 1;
			break;
		case 80:
			choise = choise == 3 ? 0 : choise + 1;
			break;
		}

	} while (choise != 13);

	switch (choise)
	{
	case 0:
		problem_1(argc, argv);
		break;
	case 1:
		problem_2(argc, argv);
		break;
	case 2:
		problem_3(argc, argv);
		break;
	case 3:
		problem_4(argc, argv);
		break;
	}*/

	problem_7(argc, argv);

	return 1;
}