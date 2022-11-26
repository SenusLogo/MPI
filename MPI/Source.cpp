#include <iostream>
#include <Windows.h>
#include <time.h>
#include <conio.h>
#include <string>
#include <vector>
#include <thread>
#include <iomanip>
#include <omp.h>
#include <fstream>

#include "mpi.h"

using namespace std;

#define ZERO_PROCCESSOR 0
#define LAST_PROCESSOR(a) a - 1 // rofl

//a - timer in milliseconds
#define Sleep(a) this_thread::sleep_for(chrono::milliseconds(a))

void problem_1()
{
	int thread, thread_size;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	cout << "Hello world from proccessor " << thread << " of " << thread_size << endl;
}

void problem_2()
{
	int thread, thread_size;

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
}

void problem_3()
{
	int thread, thread_size;
	char hello[13] = "Hello world!";

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
}

void problem_4()
{
	int size = 0, sum = 0;

	int thread, thread_size;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Size array: ";
		cin >> size;
	}

	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int* A = new int[size] {};
	int* B = new int[size] {};

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

	int* local_sum_all = new int[thread_size] {};
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

	delete[] A;
	delete[] B;

	delete[] local_sum_all;
}

void problem_5()
{
	int thread = 0, thread_size = 0, size = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter size matrix NxN: ";
		cin >> size;
	}

	size *= size;
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int* A = new int[size] {};
	int* B = new int[size] {};
	int* a = new int[size] {};
	int* b = new int[size] {};

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

	int* result = new int[size] {};
	int* local_sum = new int[size / thread_size]{};

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

	delete[] A;
	delete[] B;
	delete[] a;
	delete[] b;

	delete[] result;
	delete[] local_sum;
}

void problem_6()
{
	int thread = 0, thread_size = 0, size_vector = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter size matrix NxN: ";
		cin >> size_vector;
	}

	int size_matrix = size_vector * size_vector;

	MPI_Bcast(&size_vector, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&size_matrix, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int* A = new int[size_vector] {};
	int* B = new int[size_matrix] {};
	int* b = new int[size_matrix] {};

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

	int* result = new int[size_vector] {};
	int* local_sum = new int[size_vector / thread_size] {};

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

	delete[] A;
	delete[] B;
	delete[] b;

	delete[] result;
	delete[] local_sum;
}

void problem_7()
{
	int thread = 0, thread_size = 0, size_matrix = 0, size_vector = 0, iterations = 0;

	double eps = 0.;

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

	size_matrix = size_vector * size_vector;

	int* A = new int[size_vector] {};
	int* B = new int[size_matrix] {};
	int* D = new int[size_vector] {};
	
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

	int* a = new int[N] {};
	int* d = new int[N] {};
	int* b = new int[M] {};

	MPI_Scatter(D, N, MPI_INT, d, N, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	MPI_Scatter(A, N, MPI_INT, a, N, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	MPI_Scatter(B, M, MPI_INT, b, M, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	
	double* X = new double[size_vector] {};
	double* X_buff = new double[N] {};
	double* TempX = new double[N] {};

	int* flags = new int[thread_size] {};

	double norm = 0.;
	int exit = 1;

	do
	{
		for (int k(0); k < N; k++)
		{
			TempX[k] = a[k];

			for (int j(0); j < size_vector; j++)
				if (b[j + k * size_vector] != d[k])
					TempX[k] -= b[j + k * size_vector] * X[j];

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
}

void problem_8()
{
	int thread = 0, thread_size = 0, size_vector = 0, iterations = 0;

	double eps = 0., h = 0.;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "h: "; cin >> h;
		cout << "Precision ( eps ) = "; cin >> eps;

		size_vector = int(1. / h);
	}

	MPI_Bcast(&size_vector, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&eps, 1, MPI_DOUBLE, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	int N = size_vector / thread_size;
	int M = int(pow(size_vector, 2)) / thread_size;

	//Формирование матрицы A и b, A * u = b
	//Посредство распаралеливанием по процессорам
	int* b = new int[N] {};
	int* d = new int[N] {};
	int* a = new int[M] {};
	for (int i(thread * N), l(0); i < M; i += (size_vector + 1))
	{
		if (i == 0)
		{
			a[0] = 1;
			b[0] = 1;
			d[l++] = 1;			
		}
		else if (i == M - 1)
		{
			a[M - 1] = 1;
			d[l] = 1;
		}
		else
		{
			a[i - 1] = 1;
			a[i] = -2;
			a[i + 1] = 1;
			d[l++] = -2;
		}
	}

	//Метод якоби из практической работы 7.
	//--------------------------------------------------------------------------------------------------------
	double* X = new double[size_vector] {};
	double* X_buff = new double[N] {};
	double* TempX = new double[N] {};

	int* flags = new int[thread_size] {};

	double norm = 0.;
	int exit = 1;

	do
	{
		for (int k(0); k < N; k++)
		{
			TempX[k] = b[k];

			for (int j(0); j < size_vector; j++)
			{
				if (a[j + k * size_vector] != d[k])
					TempX[k] -= a[j + k * size_vector] * X[j];
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
	//Конец алгоритма
	//----------------------------------------------------------------------------------------------------------

	//Ответ. Проверка(закоментированна).
	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Answer: ";

		for (int i(0); i < size_vector - 1; i++)
			cout << X[i] << ", ";
		cout << X[size_vector - 1];

		wstring filepath(L"C:\\Users\\Senus Ev'Ov\\source\\repos\\Graphics_generator\\Graphics_generator\\data.csv");

		ofstream fout(filepath);
		fout << "x" << "\n";

		for (int i(0); i < size_vector; i++)
			fout << X[i] << "\n";

		fout.close();

		cout << "\n\nCount iterations: " << iterations << endl;

		/*double* check = new double[size_vector]{};
		double* A = new double[size_matrix * thread_size]{};

		cout << "\n\n\n";
		A[0] = 1; A[size_matrix * thread_size - 1] = 1;

		for (int i(size_vector + 1); i < (size_matrix * thread_size) - size_vector; i += size_vector)
		{
			A[i - 1] = 1;
			A[i] = -2;
			A[i + 1] = 1;
		}

		for (int i(0); i < size_vector; i++)
		{
			for (int j(0); j < size_vector; j++)
				check[i] += A[i * size_vector + j] * X[j];
		}

		cout << "\nCheck:\n";
		cout.precision(5);
		for (int i(0); i < size_vector; i++)
			cout << check[i] << " ";
		cout << endl;
		
		delete[] check;
		delete[] A;*/
	}

	//Освобождение выделенной памяти
	delete[] a;
	delete[] b;

	delete[] X;
	delete[] X_buff;
	delete[] TempX;
	delete[] flags;
}

void problem_9()
{
	int thread = 0, thread_size = 0;
	int m = 0, n = 0, l = 0, k = 0;
	int left, right;

	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "Enter size 1 matrix: "; cin >> m >> n;
		cout << "Enter size 2 matrix: "; cin >> l >> k;

		if (n != l)
		{
			cout << "IMPOSSIBLE!" << endl;
			return;
		}
	}

	MPI_Bcast(&m, 1, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	MPI_Bcast(&l, 1, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	MPI_Bcast(&k, 1, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	int size_matrix_1 = n * m, size_matrix_2 = l * k;

	int* A = new int[size_matrix_1] {};
	int* B = new int[size_matrix_2] {};

	int** C = new int*[l];
	for (int i(0); i < l; i++)
		C[i] = new int[k] {};

	if (thread == ZERO_PROCCESSOR)
	{
		srand(unsigned int(time(NULL)));

		for (int i(0); i < size_matrix_1; i++)
			A[i] = 1 + rand() % 9;

		for (int i(0); i < l; i++)
			for (int j(0); j < k; j++)
				C[i][j] = 1 + rand() % 9;

		for (int i(0); i < k; i++)
			for (int j(0); j < l; j++)
				B[i * l + j] = C[j][i];

		cout << "\n\nMatrix 1:\n";

		for (int i(0); i < m; i++)
		{
			for (int j(0); j < n; j++)
				cout << A[i * n + j] << " ";
			cout << endl;
		}

		cout << "\nMatrix 2:\n";

		for (int i(0); i < l; i++)
		{
			for (int j(0); j < k; j++)
				cout << C[i][j] << " ";
			cout << endl;
		}

		cout << "==========================\n\n";
	}

	int* a = new int[size_matrix_1 / thread_size]{};
	int* b = new int[size_matrix_2 / thread_size]{};

	MPI_Scatter(A, size_matrix_1 / thread_size, MPI_INT, a, size_matrix_1 / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);
	MPI_Scatter(B, size_matrix_2 / thread_size, MPI_INT, b, size_matrix_2 / thread_size, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	int* result = new int[m * k] {};
	int* local_result = new int[(m * k) / thread_size]{};

	MPI_Status status;

	for (int i(0), h(0), d(thread * (k / thread_size)); i < m / thread_size; i++, h = (h >= m / thread_size - 1 ? 0 : h + 1))
	{
		for (int g(0), f(0); g < k; g++, d = (d >= k - 1 ? 0 : d + 1), f = (f >= k / thread_size - 1 ? 0 : f + 1))
		{
			for (int j(0); j < l; j++)
				local_result[h * k + d] += a[h * n + j] * b[f * l + j];
			

			if ((g + 1) % (k / thread_size) == 0)
			{
				right = (thread + 1) % thread_size;
				left = thread - 1;
				if (left < 0)
					left = thread_size - 1;

				MPI_Sendrecv_replace(b, size_matrix_2 / thread_size, MPI_INT, left, 0, right, 0, MPI_COMM_WORLD, &status);
			}
		}		
	}

	MPI_Gather(local_result, (m * k) / thread_size, MPI_INT, result, (m * k) / thread_size, MPI_INT, 0, MPI_COMM_WORLD);

	if (thread == ZERO_PROCCESSOR)
	{
		cout << "\n=====================\n\nParallel programming solution:\n";

		for (int i(0); i < m; i++)
		{
			for (int j(0); j < k; j++)
				cout << result[i * k + j] << " ";
			cout << endl;
		}
	}
}

int main(int* argc, char** argv)
{
	MPI_Init(argc, &argv);

	problem_9();

	/*int** A = new int* [10];

	for (int i(0); i < 10; i++)
	{
		A[i] = new int[10];

		for (int j(0); j < 10; j++)
			A[i][j] = 1 + rand() % 9;
	}

	for (int i(0); i < 10; i++)
	{
		for (int j(0); j < 10; j++)
			cout << A[i][j] << " ";
		cout << endl;
	}
			

	MPI_Datatype acol, acoltype;

	MPI_Type_vector(10, 1, 10, MPI_INT, &acol);
	MPI_Type_commit(&acol);
	MPI_Type_create_resized(acol, 0, 1 * sizeof(int), &acoltype);
	MPI_Type_commit(&acoltype);

	int* b = new int[10]{};
	MPI_Scatter(A, 1, acoltype, b, 10, MPI_INT, ZERO_PROCCESSOR, MPI_COMM_WORLD);

	for (int i(0); i < 10; i++)
		cout << b[i] << endl;*/

	MPI_Finalize();

	return 0;
}