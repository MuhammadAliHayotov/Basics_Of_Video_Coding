#include "inter_intra.h"


float** dct_transform(int** matrix, int rows, int cols)
{
	int i, j, k, l;
	float ci, cj, dct1, sum;
	float** dct = initializerF(rows, cols);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (i == 0) ci = 1 / sqrt(rows);
			else ci = sqrt(2) / sqrt(rows);

			if (j == 0) cj = 1 / sqrt(cols);
			else cj = sqrt(2) / sqrt(cols);

			sum = 0;
			for (k = 0; k < rows; k++)
			{
				for (l = 0; l < cols; l++)
				{
					dct1 = (*(*(matrix + k) + l) *
						cos((2 * k + 1) * i * pi / (2 * rows)) *
						cos((2 * l + 1) * j * pi / (2 * cols)));
					sum += dct1;
				}
			}
			*(*(dct + i) + j) = ci * cj * sum;
		}
	}

	return dct;
}

int** idct_transform(int** matrix, int rows, int cols)
{
	int i, j, k, l;
	float ci, cj, idct1, sum;
	int** idct = initializerI(rows, cols);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			sum = 0;
			for (k = 0; k < rows; k++)
			{
				for (l = 0; l < cols; l++)
				{
					if (k == 0) ci = 1 / sqrt(rows);
					else ci = sqrt(2) / sqrt(rows);

					if (l == 0) cj = 1 / sqrt(cols);
					else cj = sqrt(2) / sqrt(cols);

					idct1 = (*(*(matrix + k) + l) * ci * cj *
						cos((2 * i + 1) * k * pi / (2 * rows)) *
						cos((2 * j + 1) * l * pi / (2 * cols)));
					sum += idct1;
				}
			}
			*(*(idct + i) + j) = sum;
		}
	}

	return idct;
}

//for visualization of DCT matrix, not used after iDCT
float** normalized_dct(float** matrix, int rows, int cols)
{
	int** pixelVal = initializerI(rows, cols);
	float** temp = initializerF(rows, cols);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (*(*(matrix + i) + j) < 0)
				*(*(pixelVal + i) + j) = 0;
			else if (*(*(matrix + i) + j) > 255)
				*(*(pixelVal + i) + j) = 255;
			else
				*(*(pixelVal + i) + j) = static_cast<int>((255 * log(1 + abs(*(*(matrix + i) + j))) / log(
					1 + *(*(matrix)))) + 0.5);
			*(*(temp + i) + j) = static_cast<float>(*(*(pixelVal + i) + j));
		}
	}

	return temp;
}
