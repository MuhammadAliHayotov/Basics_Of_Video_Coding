#include "inter_intra.h"


int** initializerI(int rows, int cols)
{
	int** matrix = static_cast<int**>(calloc(rows, sizeof(int*)));
	for (int c = 0; c < rows; c++)
		*(matrix + c) = static_cast<int*>(calloc(cols, sizeof(int)));
	//matrix[c] = new int[cols];
	return matrix;
}

float** initializerF(int rows, int cols)
{
	float** matrix;

	matrix = static_cast<float**>(calloc(rows, sizeof(float*)));
	for (int c = 0; c < rows; c++)
		*(matrix + c) = static_cast<float*>(calloc(cols, sizeof(float)));

	return matrix;
}

unsigned char** initializer(int rows, int cols)
{
	unsigned char** matrix;

	matrix = static_cast<unsigned char**>(calloc(rows, sizeof(unsigned char*)));
	for (int c = 0; c < rows; c++)
		*(matrix + c) = static_cast<unsigned char*>(calloc(cols, sizeof(unsigned char)));

	return matrix;
}

unsigned char** read_file(const char* name, int rows, int cols)
{
	unsigned char **image, *temp;
	FILE* fp;

	image = initializer(rows, cols);
	temp = static_cast<unsigned char*>(calloc(rows * cols, sizeof(unsigned char)));


	fopen_s(&fp, name, "rb");
	fread(temp, sizeof(unsigned char), rows * cols, fp);

	//getting image pixel data into 2D array
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			*(*(image + r) + c) = *(temp + r * cols + c);
		}
	}

	fclose(fp);
	free(temp);

	return image;
}

unsigned char* read_file(const char* name, int size)
{
	unsigned char *temp, *res;
	FILE* fp;

	temp = static_cast<unsigned char*>(calloc(size, sizeof(unsigned char)));


	fopen_s(&fp, name, "rb");
	fread(temp, sizeof(unsigned char), size, fp);

	res = temp;
	fclose(fp);
	free(temp);

	return res;
}

char* readFileC(const char* name, int size)
{
	char *temp, *res;
	FILE* fp;

	temp = static_cast<char*>(calloc(size, sizeof(char)));

	fopen_s(&fp, name, "rb");
	fread(temp, sizeof(char), size, fp);

	res = temp;
	fclose(fp);
	free(temp);

	return res;
}

int** readFileInt(const char* name, int rows, int cols)
{
	int **image, *temp;
	FILE* fp;

	image = initializerI(rows, cols);
	temp = static_cast<int*>(calloc(rows * cols, sizeof(int)));


	fopen_s(&fp, name, "rb");
	fread(temp, sizeof(int), rows * cols, fp);

	//getting image pixel data into 2D array
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			*(*(image + r) + c) = *(temp + r * cols + c);
		}
	}

	fclose(fp);
	free(temp);

	return image;
}

float** read_file_float(const char* name, int rows, int cols)
{
	float **image, *temp;
	FILE* fp;

	image = initializerF(rows, cols);
	temp = static_cast<float*>(calloc(rows * cols, sizeof(float)));


	fopen_s(&fp, name, "rb");
	fread(temp, sizeof(int), rows * cols, fp);

	//getting image pixel data into 2D array
	for (auto r = 0; r < rows; r++)
	{
		for (auto c = 0; c < cols; c++)
		{
			*(*(image + r) + c) = *(temp + r * cols + c);
		}
	}

	fclose(fp);
	free(temp);

	return image;
}

void save_file(unsigned char** image, const char* name, int rows, int cols)
{
	FILE* fp;
	unsigned char* result;

	fopen_s(&fp, name, "wb");
	//fopen(name, "wb");
	result = static_cast<unsigned char*>(calloc(rows * cols, sizeof(unsigned char)));

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			*(result + r * cols + c) = *(*(image + r) + c);
		}
	}

	fwrite(result, sizeof(unsigned char), rows * cols, fp);
	fclose(fp);
	free(result);
}

void save_file(unsigned char* image, const char* name, int size)
{
	FILE* fp;
	fopen_s(&fp, name, "wb");
	fwrite(image, sizeof(unsigned char), size, fp);
	fclose(fp);
	free(image);
}

void saveFileInt(int** image, const char* name, int rows, int cols)
{
	FILE* fp;
	int* result;

	fopen_s(&fp, name, "wb");
	//fopen(name, "wb");
	result = static_cast<int*>(calloc(rows * cols, sizeof(int)));

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			*(result + r * cols + c) = *(*(image + r) + c);
		}
	}

	fwrite(result, sizeof(int), rows * cols, fp);
	fclose(fp);
	free(result);
}

void save_file_float(float** image, const char* name, int rows, int cols)
{
	FILE* fp;
	float* result;

	fopen_s(&fp, name, "wb");
	//fopen(name, "wb");
	result = static_cast<float*>(calloc(rows * cols, sizeof(float)));

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			*(result + r * cols + c) = *(*(image + r) + c);
		}
	}

	fwrite(result, sizeof(float), rows * cols, fp);
	fclose(fp);
	free(result);
}

const char* change_names(string str, int idx)
{
	string s = str + to_string(idx) + ".y";
	char* name;
	if (idx < 10)
		name = static_cast<char*>(calloc(str.size() + 4, sizeof(char)));
	else name = static_cast<char*>(calloc(str.size() + 5, sizeof(char)));

	for (int i = 0; i < s.size(); i++)
	{
		*(name + i) = s[i];
	}
	return name;
}

void check_for_mismatch(const char* img1, const char* img2, int rows, int cols)
{
	unsigned char** first_frame = read_file(img1, rows, cols);
	unsigned char** second_frame = read_file(img2, rows, cols);
	int counter = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			const unsigned char pxl_img1 = *(*(first_frame + i) + j);
			const unsigned char pxl_img2 = *(*(second_frame + i) + j);

			if (pxl_img1 != pxl_img2)
				counter++;
		}
	}

	cout << "Mismatch: " << counter << endl;
}

void get_psnr(const char* img1, const char* img2, int rows, int cols)
{
	unsigned char** image1 = read_file(img1, ROWS, COLS);
	unsigned char** image2 = read_file(img2, ROWS, COLS);
	double ssd = 0;
	double mse = 0;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			ssd += (*(*(image1 + i) + j) - *(*(image2 + i) + j))
				* (*(*(image1 + i) + j) - *(*(image2 + i) + j));
		}
	}

	mse = ssd / (rows * cols);
	const double psnr = 10 * log10((255 * 255) / mse);

	cout << "MSE = " << mse << endl
		<< "PSNR = " << psnr << endl;
}

int** non_uniform_quantization1(float** dct_matrix, const int size, const int boundary)
{
	int** quantizedMatrix = initializerI(size, size);
	//--------------------------
	/* let's suppose size = 4 and boundary = 3
	 _ _ _ _ 
	|+|+|+|_| then starting from first row the boundary gets decreased each row
	|+|+|_|_| until it gets 0, these will determine the low frequency pixels and
	|+|_|_|_| gets divided by (int) 1 the rest by 100 
	|_|_|_|_| 
	*/
	//--------------------------
	int div = 1;
	int counter = boundary;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (j < counter) div = 1;
			else div = 100;
			*(*(quantizedMatrix + i) + j) = static_cast<int>(*(*(dct_matrix + i) + j) / div);
		}
		counter--;
	}
	return quantizedMatrix;
}

int** non_uniform_dequantization1(int** quantized_matrix, int size, int boundary)
{
	int** dequantized_matrix = initializerI(size, size);
	//--------------------------
	/* let's suppose size = 4 and boundary = 3
	 _ _ _ _
	|+|+|+|_| then starting from first row the boundary gets decreased each row
	|+|+|_|_| until it gets 0, these will determine the low frequency pixels and
	|+|_|_|_| gets divided by (int) 1 the rest by 100
	|_|_|_|_|
	*/
	//--------------------------
	int mul = 1;
	int counter = boundary;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (j < counter) mul = 1;
			else mul = 100;
			*(*(dequantized_matrix + i) + j) = static_cast<int>(*(*(quantized_matrix + i) + j) * mul);
		}
		counter--;
	}
	return dequantized_matrix;
}

int** non_uniform_quantization2(float** dct_matrix, int size)
{
	int** quantized_matrix = initializerI(size, size);
	int** quantization_table = initializerI(size, size);
	if (size == 4)
	{
		*(*(quantization_table)) = 16;
		*(*(quantization_table) + 1) = 10;
		*(*(quantization_table) + 2) = 24;
		*(*(quantization_table) + 1) = 51;

		*(*(quantization_table + 1)) = 12;
		*(*(quantization_table + 1) + 1) = 14;
		*(*(quantization_table + 1) + 2) = 26;
		*(*(quantization_table + 1) + 3) = 60;

		*(*(quantization_table + 2)) = 14;
		*(*(quantization_table + 2) + 1) = 16;
		*(*(quantization_table + 2) + 2) = 40;
		*(*(quantization_table + 2) + 3) = 69;

		*(*(quantization_table + 3)) = 14;
		*(*(quantization_table + 3) + 1) = 22;
		*(*(quantization_table + 3) + 2) = 51;
		*(*(quantization_table + 3) + 3) = 80;
	}
	else
	{
		*(*(quantization_table)) = 16;
		*(*(quantization_table) + 1) = 11;
		*(*(quantization_table) + 2) = 10;
		*(*(quantization_table) + 3) = 16;
		*(*(quantization_table) + 4) = 24;
		*(*(quantization_table) + 5) = 40;
		*(*(quantization_table) + 6) = 51;
		*(*(quantization_table) + 7) = 61;

		*(*(quantization_table + 1)) = 12;
		*(*(quantization_table + 1) + 1) = 12;
		*(*(quantization_table + 1) + 2) = 14;
		*(*(quantization_table + 1) + 3) = 19;
		*(*(quantization_table + 1) + 4) = 26;
		*(*(quantization_table + 1) + 5) = 58;
		*(*(quantization_table + 1) + 6) = 60;
		*(*(quantization_table + 1) + 7) = 55;

		*(*(quantization_table + 2)) = 14;
		*(*(quantization_table + 2) + 1) = 13;
		*(*(quantization_table + 2) + 2) = 16;
		*(*(quantization_table + 2) + 3) = 24;
		*(*(quantization_table + 2) + 4) = 40;
		*(*(quantization_table + 2) + 5) = 57;
		*(*(quantization_table + 2) + 6) = 69;
		*(*(quantization_table + 2) + 7) = 56;

		*(*(quantization_table + 3)) = 14;
		*(*(quantization_table + 3) + 1) = 17;
		*(*(quantization_table + 3) + 2) = 22;
		*(*(quantization_table + 3) + 3) = 29;
		*(*(quantization_table + 3) + 4) = 51;
		*(*(quantization_table + 3) + 5) = 86;
		*(*(quantization_table + 3) + 6) = 80;
		*(*(quantization_table + 3) + 7) = 62;

		*(*(quantization_table + 4)) = 18;
		*(*(quantization_table + 4) + 1) = 22;
		*(*(quantization_table + 4) + 2) = 37;
		*(*(quantization_table + 4) + 3) = 56;
		*(*(quantization_table + 4) + 4) = 68;
		*(*(quantization_table + 4) + 5) = 109;
		*(*(quantization_table + 4) + 6) = 103;
		*(*(quantization_table + 4) + 7) = 77;

		*(*(quantization_table + 5)) = 24;
		*(*(quantization_table + 5) + 1) = 35;
		*(*(quantization_table + 5) + 2) = 55;
		*(*(quantization_table + 5) + 3) = 64;
		*(*(quantization_table + 5) + 4) = 81;
		*(*(quantization_table + 5) + 5) = 104;
		*(*(quantization_table + 5) + 6) = 113;
		*(*(quantization_table + 5) + 7) = 92;

		*(*(quantization_table + 6)) = 49;
		*(*(quantization_table + 6) + 1) = 64;
		*(*(quantization_table + 6) + 2) = 78;
		*(*(quantization_table + 6) + 3) = 87;
		*(*(quantization_table + 6) + 4) = 103;
		*(*(quantization_table + 6) + 5) = 121;
		*(*(quantization_table + 6) + 6) = 120;
		*(*(quantization_table + 6) + 7) = 101;

		*(*(quantization_table + 4)) = 72;
		*(*(quantization_table + 4) + 1) = 92;
		*(*(quantization_table + 4) + 2) = 95;
		*(*(quantization_table + 4) + 3) = 98;
		*(*(quantization_table + 4) + 4) = 112;
		*(*(quantization_table + 4) + 5) = 100;
		*(*(quantization_table + 4) + 6) = 103;
		*(*(quantization_table + 4) + 7) = 99;
	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			*(*(quantized_matrix + i) + j) = static_cast<int>(*(*(dct_matrix + i) + j) / *(*(quantization_table + i) + j
			));
		}
	}

	return quantized_matrix;
}

int** non_uniform_dequantization2(int** quantized_matrix, int size)
{
	int** dequantized_matrix = initializerI(size, size);
	int** quantization_table = initializerI(size, size);
	if (size == 4)
	{
		*(*(quantization_table)) = 16;
		*(*(quantization_table) + 1) = 10;
		*(*(quantization_table) + 2) = 24;
		*(*(quantization_table) + 1) = 51;

		*(*(quantization_table + 1)) = 12;
		*(*(quantization_table + 1) + 1) = 14;
		*(*(quantization_table + 1) + 2) = 26;
		*(*(quantization_table + 1) + 3) = 60;

		*(*(quantization_table + 2)) = 14;
		*(*(quantization_table + 2) + 1) = 16;
		*(*(quantization_table + 2) + 2) = 40;
		*(*(quantization_table + 2) + 3) = 69;

		*(*(quantization_table + 3)) = 14;
		*(*(quantization_table + 3) + 1) = 22;
		*(*(quantization_table + 3) + 2) = 51;
		*(*(quantization_table + 3) + 3) = 80;
	}
	else
	{
		*(*(quantization_table)) = 16;
		*(*(quantization_table) + 1) = 11;
		*(*(quantization_table) + 2) = 10;
		*(*(quantization_table) + 3) = 16;
		*(*(quantization_table) + 4) = 24;
		*(*(quantization_table) + 5) = 40;
		*(*(quantization_table) + 6) = 51;
		*(*(quantization_table) + 7) = 61;

		*(*(quantization_table + 1)) = 12;
		*(*(quantization_table + 1) + 1) = 12;
		*(*(quantization_table + 1) + 2) = 14;
		*(*(quantization_table + 1) + 3) = 19;
		*(*(quantization_table + 1) + 4) = 26;
		*(*(quantization_table + 1) + 5) = 58;
		*(*(quantization_table + 1) + 6) = 60;
		*(*(quantization_table + 1) + 7) = 55;

		*(*(quantization_table + 2)) = 14;
		*(*(quantization_table + 2) + 1) = 13;
		*(*(quantization_table + 2) + 2) = 16;
		*(*(quantization_table + 2) + 3) = 24;
		*(*(quantization_table + 2) + 4) = 40;
		*(*(quantization_table + 2) + 5) = 57;
		*(*(quantization_table + 2) + 6) = 69;
		*(*(quantization_table + 2) + 7) = 56;

		*(*(quantization_table + 3)) = 14;
		*(*(quantization_table + 3) + 1) = 17;
		*(*(quantization_table + 3) + 2) = 22;
		*(*(quantization_table + 3) + 3) = 29;
		*(*(quantization_table + 3) + 4) = 51;
		*(*(quantization_table + 3) + 5) = 86;
		*(*(quantization_table + 3) + 6) = 80;
		*(*(quantization_table + 3) + 7) = 62;

		*(*(quantization_table + 4)) = 18;
		*(*(quantization_table + 4) + 1) = 22;
		*(*(quantization_table + 4) + 2) = 37;
		*(*(quantization_table + 4) + 3) = 56;
		*(*(quantization_table + 4) + 4) = 68;
		*(*(quantization_table + 4) + 5) = 109;
		*(*(quantization_table + 4) + 6) = 103;
		*(*(quantization_table + 4) + 7) = 77;

		*(*(quantization_table + 5)) = 24;
		*(*(quantization_table + 5) + 1) = 35;
		*(*(quantization_table + 5) + 2) = 55;
		*(*(quantization_table + 5) + 3) = 64;
		*(*(quantization_table + 5) + 4) = 81;
		*(*(quantization_table + 5) + 5) = 104;
		*(*(quantization_table + 5) + 6) = 113;
		*(*(quantization_table + 5) + 7) = 92;

		*(*(quantization_table + 6)) = 49;
		*(*(quantization_table + 6) + 1) = 64;
		*(*(quantization_table + 6) + 2) = 78;
		*(*(quantization_table + 6) + 3) = 87;
		*(*(quantization_table + 6) + 4) = 103;
		*(*(quantization_table + 6) + 5) = 121;
		*(*(quantization_table + 6) + 6) = 120;
		*(*(quantization_table + 6) + 7) = 101;

		*(*(quantization_table + 4)) = 72;
		*(*(quantization_table + 4) + 1) = 92;
		*(*(quantization_table + 4) + 2) = 95;
		*(*(quantization_table + 4) + 3) = 98;
		*(*(quantization_table + 4) + 4) = 112;
		*(*(quantization_table + 4) + 5) = 100;
		*(*(quantization_table + 4) + 6) = 103;
		*(*(quantization_table + 4) + 7) = 99;
	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			*(*(dequantized_matrix + i) + j) = static_cast<int>(*(*(quantized_matrix + i) + j) * *(*(quantization_table
				+ i) + j));
		}
	}

	return dequantized_matrix;
}
