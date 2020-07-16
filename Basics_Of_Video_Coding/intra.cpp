#include "inter_intra.h"


vector<char> mode_nums;
vector<unsigned char> mode_nums1;
static int int0 = 0, int1 = 0, int2 = 0, int3 = 0,
           int4 = 0, int5 = 0, int6 = 0, int7 = 0, int8 = 0, counter = 0, idx = 0,
           idx0 = 0, idx1 = 0, idx2 = 0, idx3 = 0, idx4 = 0, idx5 = 0, idx6 = 0,
           idx7 = 0, idx8 = 0;


const int m = INTRA_ROWS, n = INTRA_COLS, rows = ROWS, cols = COLS;


unsigned char** mode0(unsigned char* up_matrix, unsigned char* side_matrix)
{
	unsigned char** newMat;
	newMat = initializer(m, n);

	for (int r = 0; r < m; r++)
	{
		for (int c = 0; c < n; c++)
		{
			*(*(newMat + r) + c) = *(up_matrix + c);
		}
	}
	return newMat;
}

unsigned char** mode1(unsigned char* up_matrix, unsigned char* side_matrix)
{
	unsigned char** newMat;
	newMat = initializer(m, n);

	for (int r = 0; r < m; r++)
	{
		for (int c = 0; c < n; c++)
		{
			*(*(newMat + r) + c) = *(side_matrix + r);
		}
	}
	return newMat;
}

unsigned char** mode2(unsigned char* up_matrix, unsigned char* side_matrix)
{
	unsigned char** new_mat = initializer(m, n);

	int dc = 0, sum1 = 0, sum2 = 0;

	for (auto i = 0; i < m; i++)
	{
		sum1 += *(side_matrix + i);
		sum2 += *(up_matrix + i);
	}
	dc = (sum1 + sum2 + 4) / 8;
	for (auto r = 0; r < m; r++)
	{
		for (auto c = 0; c < n; c++)
		{
			*(*(new_mat + r) + c) = dc;
		}
	}
	return new_mat;
}

unsigned char** mode3(unsigned char* up_matrix, unsigned char* side_matrix)
{
	unsigned char** matrix;
	matrix = initializer(m, n);

	unsigned char A, B, C, D, E, F, G, H;
	unsigned char I, J, K, L;

	//upMatrix
	A = *(up_matrix + 0);
	B = *(up_matrix + 1);
	C = *(up_matrix + 2);
	D = *(up_matrix + 3);
	E = *(up_matrix + 4);
	F = *(up_matrix + 5);
	G = *(up_matrix + 6);
	H = *(up_matrix + 7);

	//sideMatrix
	I = *(side_matrix + 0);
	J = *(side_matrix + 1);
	K = *(side_matrix + 2);
	L = *(side_matrix + 3);

	//a
	*(*(matrix)) = (A + 2 * B + C + 2) / 4;
	//b and e
	*(*(matrix) + 1) =
		*(*(matrix + 1)) = (B + 2 * C + D + 2) / 4;
	//c, f and i
	*(*(matrix) + 2) =
		*(*(matrix + 1) + 1) =
		*(*(matrix + 2)) = (C + 2 * D + E + 2) / 4;
	//d, g, j, and m
	*(*(matrix) + 3) =
		*(*(matrix + 1) + 2) =
		*(*(matrix + 2) + 1) =
		*(*(matrix + 3)) = (D + 2 * E + F + 2) / 4;
	//h, k and n
	*(*(matrix + 1) + 3) =
		*(*(matrix + 2) + 2) =
		*(*(matrix + 3) + 1) = (E + 2 * F + G + 2) / 4;
	//l and o
	*(*(matrix + 2) + 3) =
		*(*(matrix + 3) + 2) = (F + 2 * G + H + 2) / 4;

	//p
	*(*(matrix + 3) + 3) = (G + 3 * H + 2) / 4;
	//*(*(matrix + 3) + 3) = (*(upMatrix + 6) + 2 * *(upMatrix + 7) + *(sideMatrix) + 2) / 4;
	return matrix;
}

unsigned char** mode4(unsigned char* up_matrix, unsigned char* side_matrix, int q)
{
	unsigned char** matrix;
	matrix = initializer(m, n);

	unsigned char A, B, C, D, E, F, G, H;
	unsigned char I, J, K, L;

	//upMatrix
	A = *(up_matrix + 0);
	B = *(up_matrix + 1);
	C = *(up_matrix + 2);
	D = *(up_matrix + 3);
	E = *(up_matrix + 4);
	F = *(up_matrix + 5);
	G = *(up_matrix + 6);
	H = *(up_matrix + 7);

	//sideMatrix
	I = *(side_matrix + 0);
	J = *(side_matrix + 1);
	K = *(side_matrix + 2);
	L = *(side_matrix + 3);

	//m
	*(*(matrix + 3)) = (J + 2 * K + L + 2) / 4;
	//i and n
	*(*(matrix + 2)) =
		*(*(matrix + 3) + 1) = (I + 2 * J + K + 2) / 4;
	//e, j and o
	*(*(matrix + 1)) =
		*(*(matrix + 2) + 1) =
		*(*(matrix + 3) + 2) = (q + 2 * I + J + 2) / 4;
	//a, f, k and p
	*(*(matrix)) =
		*(*(matrix + 1) + 1) =
		*(*(matrix + 2) + 2) =
		*(*(matrix + 3) + 3) = (A + 2 * q + I + 2) / 4;
	//b, g and l
	*(*(matrix) + 1) =
		*(*(matrix + 1) + 2) =
		*(*(matrix + 2) + 3) = (q + 2 * A + B + 2) / 4;
	//c and h
	*(*(matrix) + 2) =
		*(*(matrix + 1) + 3) = (A + 2 * B + C + 2) / 4;
	//d
	*(*(matrix) + 3) = (B + 2 * C + D + 2) / 4;

	return matrix;
}

unsigned char** mode5(unsigned char* up_matrix, unsigned char* side_matrix)
{
	unsigned char** matrix;
	matrix = initializer(m, n);

	unsigned char A, B, C, D, E, F, G, H;
	unsigned char I, J, K, L;

	//upMatrix
	A = *(up_matrix + 0);
	B = *(up_matrix + 1);
	C = *(up_matrix + 2);
	D = *(up_matrix + 3);
	E = *(up_matrix + 4);
	F = *(up_matrix + 5);
	G = *(up_matrix + 6);
	H = *(up_matrix + 7);

	//sideMatrix
	I = *(side_matrix + 0);
	J = *(side_matrix + 1);
	K = *(side_matrix + 2);
	L = *(side_matrix + 3);

	//a
	*(*(matrix)) = (A + B + 1) / 2;
	//b and i
	*(*(matrix) + 1) =
		*(*(matrix + 2)) = (B + C + 1) / 2;
	//c and j
	*(*(matrix) + 2) =
		*(*(matrix + 2) + 1) = (C + D + 1) / 2;
	//d and k
	*(*(matrix) + 3) =
		*(*(matrix + 2) + 2) = (D + E + 1) / 2;
	//l
	*(*(matrix + 2) + 3) = (E + F + 1) / 2;
	//e
	*(*(matrix + 1)) = (A + 2 * B + C + 2) / 4;
	//f and m
	*(*(matrix + 1) + 1) =
		*(*(matrix + 3)) = (B + 2 * C + D + 2) / 4;
	//g and n
	*(*(matrix + 1) + 2) =
		*(*(matrix + 3) + 1) = (C + 2 * D + E + 2) / 4;
	//h and o
	*(*(matrix + 1) + 3) =
		*(*(matrix + 3) + 2) = (D + 2 * E + F + 2) / 4;
	//p 
	*(*(matrix + 3) + 3) = (E + 2 * F + G + 2) / 4;

	return matrix;
}

unsigned char** mode6(unsigned char* up_matrix, unsigned char* side_matrix, int q)
{
	unsigned char** matrix;
	matrix = initializer(m, n);

	unsigned char A, B, C, D, E, F, G, H;
	unsigned char I, J, K, L;

	//upMatrix
	A = *(up_matrix + 0);
	B = *(up_matrix + 1);
	C = *(up_matrix + 2);
	D = *(up_matrix + 3);
	E = *(up_matrix + 4);
	F = *(up_matrix + 5);
	G = *(up_matrix + 6);
	H = *(up_matrix + 7);

	//sideMatrix
	I = *(side_matrix + 0);
	J = *(side_matrix + 1);
	K = *(side_matrix + 2);
	L = *(side_matrix + 3);

	//a and g
	*(*(matrix)) =
		*(*(matrix + 1) + 2) = (q + I + 1) / 2;
	//b and h
	*(*(matrix) + 1) =
		*(*(matrix + 1) + 3) = (I + 2 * q + A + 2) / 4;
	//c
	*(*(matrix) + 2) = (q + 2 * A + B + 2) / 4;
	//d 
	*(*(matrix) + 3) = (A + 2 * B + C + 2) / 4;
	//e and k
	*(*(matrix + 1)) =
		*(*(matrix + 2) + 2) = (I + J + 1) / 2;
	//f and l
	*(*(matrix + 1) + 1) =
		*(*(matrix + 2) + 3) = (q + 2 * I + J + 2) / 4;
	//i and o
	*(*(matrix + 2)) =
		*(*(matrix + 3) + 2) = (J + K + 1) / 2;
	//j and p
	*(*(matrix + 2) + 1) =
		*(*(matrix + 3) + 3) = (I + 2 * J + K + 2) / 4;
	//m
	*(*(matrix + 3)) = (K + L + 1) / 2;
	//n
	*(*(matrix + 3) + 1) = (J + 2 * K + L + 2) / 4;

	return matrix;
}

unsigned char** mode7(unsigned char* up_matrix, unsigned char* side_matrix, int q)
{
	unsigned char** matrix;
	matrix = initializer(m, n);

	unsigned char A, B, C, D, E, F, G, H;
	unsigned char I, J, K, L;

	//upMatrix
	A = *(up_matrix + 0);
	B = *(up_matrix + 1);
	C = *(up_matrix + 2);
	D = *(up_matrix + 3);
	E = *(up_matrix + 4);
	F = *(up_matrix + 5);
	G = *(up_matrix + 6);
	H = *(up_matrix + 7);

	//sideMatrix
	I = *(side_matrix + 0);
	J = *(side_matrix + 1);
	K = *(side_matrix + 2);
	L = *(side_matrix + 3);

	//a and j
	*(*(matrix)) =
		*(*(matrix + 2) + 1) = (q + A + 1) / 2;
	//b and k
	*(*(matrix) + 1) =
		*(*(matrix + 2) + 2) = (A + B + 1) / 2;
	//c and l
	*(*(matrix) + 2) =
		*(*(matrix + 2) + 3) = (B + C + 1) / 2;
	//d
	*(*(matrix) + 3) = (C + D + 1) / 2;
	//e and n
	*(*(matrix + 1)) =
		*(*(matrix + 3) + 1) = (I + 2 * q + A + 2) / 4;
	//f and o
	*(*(matrix + 1) + 1) =
		*(*(matrix + 3) + 2) = (q + 2 * A + B + 2) / 4;
	//g and p
	*(*(matrix + 1) + 2) =
		*(*(matrix + 3) + 3) = (A + 2 * B + C + 2) / 4;
	//h
	*(*(matrix + 1) + 3) = (B + 2 * C + D + 2) / 4;
	//i 
	*(*(matrix + 2)) = (q + 2 * I + J + 2) / 4;
	//m
	*(*(matrix + 3)) = (I + 2 * J + K + 2) / 4;

	return matrix;
}

unsigned char** mode8(unsigned char* up_matrix, unsigned char* side_matrix, int q)
{
	unsigned char** matrix;
	matrix = initializer(m, n);

	unsigned char A, B, C, D, E, F, G, H;
	unsigned char I, J, K, L;

	//upMatrix
	A = *(up_matrix + 0);
	B = *(up_matrix + 1);
	C = *(up_matrix + 2);
	D = *(up_matrix + 3);
	E = *(up_matrix + 4);
	F = *(up_matrix + 5);
	G = *(up_matrix + 6);
	H = *(up_matrix + 7);

	//sideMatrix
	I = *(side_matrix + 0);
	J = *(side_matrix + 1);
	K = *(side_matrix + 2);
	L = *(side_matrix + 3);

	//a
	*(*(matrix)) = (I + J + 1) / 2;
	//b
	*(*(matrix) + 1) = (I + 2 * J + K + 2) / 4;
	//c and e
	*(*(matrix) + 2) =
		*(*(matrix + 1)) = (J + K + 1) / 2;
	//d and f
	*(*(matrix) + 3) =
		*(*(matrix + 1) + 1) = (J + 2 * K + L + 2) / 4;
	//g and i
	*(*(matrix + 1) + 2) =
		*(*(matrix + 2)) = (K + L + 1) / 2;
	//h and j
	*(*(matrix + 1) + 3) =
		*(*(matrix + 2) + 1) = (K + 3 * L + 2) / 4;
	//l, n, k, m, o and p
	*(*(matrix + 2) + 3) =
		*(*(matrix + 3) + 1) =
		*(*(matrix + 2) + 2) =
		*(*(matrix + 3)) =
		*(*(matrix + 3) + 2) =
		*(*(matrix + 3) + 3) = L;

	return matrix;
}

unsigned char** predictionBlock(unsigned char** matrix, unsigned char* up_matrix, unsigned char* side_matrix, int q)
{
	unsigned char **matrix0, **matrix1, **matrix2, **matrix3, **matrix4,
	              **matrix5, **matrix6, **matrix7, **matrix8, **minMat;
	int* ssd0;
	int idx = 0;
	ssd0 = static_cast<int*>(calloc(9, sizeof(int)));
	minMat = initializer(m, n);


	matrix0 = mode0(up_matrix, side_matrix);
	*(ssd0) = ssd(matrix, matrix0);

	matrix1 = mode1(up_matrix, side_matrix);
	*(ssd0 + 1) = ssd(matrix, matrix1);

	matrix2 = mode2(up_matrix, side_matrix);
	*(ssd0 + 2) = ssd(matrix, matrix2);

	matrix3 = mode3(up_matrix, side_matrix);
	*(ssd0 + 3) = ssd(matrix, matrix3);

	matrix4 = mode4(up_matrix, side_matrix, q);
	*(ssd0 + 4) = ssd(matrix, matrix4);

	matrix5 = mode5(up_matrix, side_matrix);
	*(ssd0 + 5) = ssd(matrix, matrix5);

	matrix6 = mode6(up_matrix, side_matrix, q);
	*(ssd0 + 6) = ssd(matrix, matrix6);

	matrix7 = mode7(up_matrix, side_matrix, q);
	*(ssd0 + 7) = ssd(matrix, matrix7);

	matrix8 = mode8(up_matrix, side_matrix, q);
	*(ssd0 + 8) = ssd(matrix, matrix8);

	float min = *(ssd0);
	for (int i = 0; i < 9; i++)
	{
		if (*(ssd0 + i) < min)
		{
			min = *(ssd0 + i);
			idx = i;
		}
	}

	if (idx == 0)
	{
		minMat = matrix0;
		int0++;
	}
	else if (idx == 1)
	{
		minMat = matrix1;
		int1++;
	}
	else if (idx == 2)
	{
		minMat = matrix2;
		int2++;
	}
	else if (idx == 3)
	{
		minMat = matrix3;
		int3++;
	}
	else if (idx == 4)
	{
		minMat = matrix4;
		int4++;
	}
	else if (idx == 5)
	{
		minMat = matrix5;
		int5++;
	}
	else if (idx == 6)
	{
		minMat = matrix6;
		int6++;
	}
	else if (idx == 7)
	{
		minMat = matrix7;
		int7++;
	}
	else if (idx == 8)
	{
		minMat = matrix8;
		int8++;
	}
	else cout << "Error occurred in PredictionBlock index selection" << endl;
	//saving current modes into vector
	mode_nums.push_back(static_cast<char>(idx));

	return minMat;
}

unsigned char** predictionBlock1(unsigned char** matrix, unsigned char* upMatrix, unsigned char* sideMatrix, int q)
{
	unsigned char **matrix0, **matrix1, **matrix2, **matrix3, **matrix4,
	              **matrix5, **matrix6, **matrix7, **matrix8, **minMat;
	int* ssd0;
	int idx = 0;
	ssd0 = static_cast<int*>(calloc(9, sizeof(int)));
	minMat = initializer(m, n);


	matrix0 = mode0(upMatrix, sideMatrix);
	*(ssd0) = ssd(matrix, matrix0);

	matrix1 = mode1(upMatrix, sideMatrix);
	*(ssd0 + 1) = ssd(matrix, matrix1);

	matrix2 = mode2(upMatrix, sideMatrix);
	*(ssd0 + 2) = ssd(matrix, matrix2);

	matrix3 = mode3(upMatrix, sideMatrix);
	*(ssd0 + 3) = ssd(matrix, matrix3);

	matrix4 = mode4(upMatrix, sideMatrix, q);
	*(ssd0 + 4) = ssd(matrix, matrix4);

	matrix5 = mode5(upMatrix, sideMatrix);
	*(ssd0 + 5) = ssd(matrix, matrix5);

	matrix6 = mode6(upMatrix, sideMatrix, q);
	*(ssd0 + 6) = ssd(matrix, matrix6);

	matrix7 = mode7(upMatrix, sideMatrix, q);
	*(ssd0 + 7) = ssd(matrix, matrix7);

	matrix8 = mode8(upMatrix, sideMatrix, q);
	*(ssd0 + 8) = ssd(matrix, matrix8);

	float min = *(ssd0);
	for (int i = 0; i < 9; i++)
	{
		if (*(ssd0 + i) < min)
		{
			min = *(ssd0 + i);
			idx = i;
		}
	}

	if (idx == 0) { minMat = matrix0; }
	else if (idx == 1) { minMat = matrix1; }
	else if (idx == 2) { minMat = matrix2; }
	else if (idx == 3) { minMat = matrix3; }
	else if (idx == 4) { minMat = matrix4; }
	else if (idx == 5) { minMat = matrix5; }
	else if (idx == 6) { minMat = matrix6; }
	else if (idx == 7) { minMat = matrix7; }
	else if (idx == 8) { minMat = matrix8; }
	else cout << "Error occured in PredictionBlock index selection" << endl;
	//saving current modes into vector
	mode_nums1.push_back(idx);

	return minMat;
}

int ssd(unsigned char** matrix, unsigned char** predicted_matrix)
{
	int ssd = 0;
	for (int r = 0; r < m; r++)
	{
		for (int c = 0; c < n; c++)
		{
			ssd += ((*(*matrix + r) + c) - *(*(predicted_matrix + r) + c)) *
				((*(*matrix + r) + c) - *(*(predicted_matrix + r) + c));
		}
	}
	return ssd;
}

//difference block cannot hold negative values mind that!
int** difference_block(unsigned char** matrix, unsigned char** predicted_matrix)
{
	int** differenceMatrix;
	differenceMatrix = initializerI(m, n);

	for (int r = 0; r < m; r++)
	{
		for (int c = 0; c < n; c++)
		{
			*(*(differenceMatrix + r) + c) = *(*(matrix + r) + c) - *(*(predicted_matrix + r) + c);
		}
	}
	return differenceMatrix;
}

unsigned char** reconstructed_block(unsigned char** dequantized_matrix, unsigned char** predicted_matrix)
{
	unsigned char** reconstructedMatrix;
	reconstructedMatrix = initializer(m, n);

	for (int r = 0; r < m; r++)
	{
		for (int c = 0; c < n; c++)
		{
			*(*(reconstructedMatrix + r) + c) = static_cast<unsigned char>(*(*(dequantized_matrix + r) + c) + *(*(
				predicted_matrix + r) + c));
		}
	}
	return reconstructedMatrix;
}

void save_into_file(char* savingMat, const char* name, const int size)
{
	FILE* fp;
	fopen_s(&fp, name, "wb");
	fwrite(savingMat, sizeof(char), size * size, fp);
	fclose(fp);
	free(savingMat);
}

void saveIntoFileI(char* savingMat, const char* name, int size)
{
	FILE* fp;
	fopen_s(&fp, name, "a");
	fwrite(savingMat, sizeof(char), size, fp);
	fclose(fp);
	free(savingMat);
}

void save_data(unsigned char** matrix, const char* name)
{
	FILE* fp;

	unsigned char* result;

	fopen_s(&fp, "res1.jpg", "wb");
	result = static_cast<unsigned char*>(calloc(512 * 512, sizeof(unsigned char)));

	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			*(result + i * 512 + j) = static_cast<int>(*(*(matrix + i) + j));
		}
	}

	fwrite(result, sizeof(unsigned char), 512 * 512, fp);
	fclose(fp);
	//free(result);
}

unsigned char** apply_mode(int idx, unsigned char* upMatrix, unsigned char* sideMatrix, int q)
{
	unsigned char** matrix = initializer(4, 4);
	//applying modes according to the index received
	if (idx == 0) matrix = mode0(upMatrix, sideMatrix);
	else if (idx == 1) matrix = mode1(upMatrix, sideMatrix);
	else if (idx == 2) matrix = mode2(upMatrix, sideMatrix);
	else if (idx == 3) matrix = mode3(upMatrix, sideMatrix);
	else if (idx == 4) matrix = mode4(upMatrix, sideMatrix, q);
	else if (idx == 5) matrix = mode5(upMatrix, sideMatrix);
	else if (idx == 6) matrix = mode6(upMatrix, sideMatrix, q);
	else if (idx == 7) matrix = mode7(upMatrix, sideMatrix, q);
	else if (idx == 8) matrix = mode8(upMatrix, sideMatrix, q);

	return matrix;
}

unsigned char** apply_intra(unsigned char** input_image, const char* name, const char* modes_name,
                            const char* quan_name)
{
	unsigned char** matrix;
	unsigned char** predictedMatrix;
	int** differenceMatrix;
	int** quantizedMatrix;
	int** dequantizedMatrix;
	unsigned char** reconstructedMatrix;
	unsigned char** outputImage;
	float** dctMatrix;
	unsigned char** idctMatrix;
	char* intraModes;
	int** quanMat;
	//unsigned char **upMatData;
	unsigned char** modeNumbers = initializer(rows / n, cols / m);
	int counter = 0;
	mode_nums.clear();

	int q = 0;

	unsigned char* upMatrix;
	unsigned char* sideMatrix;
	upMatrix = static_cast<unsigned char*>(calloc(8, sizeof(unsigned char)));
	sideMatrix = static_cast<unsigned char*>(calloc(4, sizeof(unsigned char)));
	intraModes = static_cast<char*>(calloc(rows * cols / 16, sizeof(char)));


	matrix = initializer(m, n);
	predictedMatrix = initializer(m, n);
	differenceMatrix = initializerI(m, n);
	quantizedMatrix = initializerI(m, n);
	dequantizedMatrix = initializerI(m, n);
	reconstructedMatrix = initializer(m, n);
	dctMatrix = initializerF(m, n);
	idctMatrix = initializer(m, n);
	outputImage = initializer(rows, cols);
	quanMat = initializerI(rows, cols);

	for (int i = 0; i < rows; i += 4)
	{
		for (int j = 0; j < cols; j += 4)
		{
			//assigning the value of q
			if (i == 0 || j == 0) q = 128;
			else q = *(*(input_image + i - 1) + j - 1);

			//upMatrix
			if (i == 0) for (int r = 0; r < 8; r++) *(upMatrix + r) = 128;
			else if ((j == (cols - 4)) && i > 0)
			{
				//initial 4 elems are from the input image
				for (int r = 0; r < 4; r++)
					*(upMatrix + r) = *(*(input_image + i - 1) + j + r);
				//the rest is out of the boundary thus
				for (int r = 4; r < 8; r++)
					*(upMatrix + r) = 128;
			}
				//if the image is inside the boundary
			else for (int r = 0; r < 8; r++) *(upMatrix + r) = *(*(input_image + i - 1) + j + r);

			//for (int r = 0; r < 8; r++) *(*(upMatData + i / 4) + j + r) = *(upMatrix + r);
			//sideMatrix values
			if (j == 0) for (int r = 0; r < 4; r++) *(sideMatrix + r) = 128;
			else for (int r = 0; r < 4; r++) *(sideMatrix + r) = *(*(input_image + i + r) + j - 1);

			//getting the first matrix from the image
			for (int r = 0; r < m; r++)
			{
				for (int c = 0; c < n; c++)
				{
					*(*(matrix + r) + c) = *(*(input_image + i + r) + j + c);
				}
			}

			predictedMatrix = predictionBlock(matrix, upMatrix, sideMatrix, q);


			*(*(modeNumbers + i / n) + j / m) = mode_nums[counter];
			counter++;
			differenceMatrix = difference_block(matrix, predictedMatrix);

			//quantizedMatrix = quantizeIn(differenceMatrix, 1.75, 4, 4);	

			quantizedMatrix = non_uniform_quantization2(dct_transform(differenceMatrix, 4, 4), 4);
			//save quantized matrix
			for (int r = 0; r < m; r++)
			{
				for (int c = 0; c < n; c++)
				{
					*(*(quanMat + i + r) + j + c) = *(*(quantizedMatrix + r) + c);
				}
			}

			//dequantize
			dequantizedMatrix = idct_transform(non_uniform_dequantization2(quantizedMatrix, 4), 4, 4);

			//dequantizedMatrix = dequantizeIn(quantizedMatrix, 1.75, 4, 4);

			//reconstruct it for for times and then 
			reconstructedMatrix = reconstructed_block_in(dequantizedMatrix, predictedMatrix, 4, 4);

			//assigning new values 
			for (int r = 0; r < m; r++)
			{
				for (int c = 0; c < n; c++)
				{
					//swapping inputImage's predBlock
					*(*(input_image + i + r) + j + c) = *(*(reconstructedMatrix + r) + c);
					//outputImage
					*(*(outputImage + i + r) + j + c) = *(*(reconstructedMatrix + r) + c);
				}
			}
		}
	}

	vector<int> countSort(9);
	for (int i = 0; i < mode_nums.size(); i++)
	{
		countSort[mode_nums[i]]++;
	}


	//saving the output file
	save_file(modeNumbers, modes_name, rows / m, cols / n);
	saveFileInt(quanMat, quan_name, rows, cols);
	save_file(outputImage, name, rows, cols);

	return outputImage;
}
