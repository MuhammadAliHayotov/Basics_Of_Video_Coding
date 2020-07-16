#include "inter_intra.h"

//#define a 1
//#if a
static int counter1 = 0;
//#endif
vector<unsigned char**> tempMatrices;
int mv_sum = 0;

int start_row(int r, int search_size)
{
	int startIdx = 0;

	if ((r == 0) || (r < search_size)) startIdx = 0;
	else if ((r != 0) && (r > search_size)) startIdx = r - search_size;

	return startIdx;
}

int start_col(int c, int search_size)
{
	int startIdx = 0;

	if ((c == 0) || (c < search_size)) startIdx = 0;
	else if ((c != 0) && (c > search_size))startIdx = c - search_size;

	return startIdx;
}

int end_row(int r, int search_size, int rows)
{
	int endIdx = 0;

	if ((r == ROWS - 1) || ((r + search_size) > (ROWS - 1))) endIdx = ROWS - 1;
	else if ((r < (ROWS - 1)) && ((r + search_size) < (ROWS - 1))) endIdx = r + search_size;

	return endIdx;
}

int end_col(int c, int search_size, int cols)
{
	int endIdx = 0;

	if ((c == COLS - 1) || ((c + search_size) > (COLS - 1))) endIdx = COLS - 1;
	else if ((c < (COLS - 1)) && ((c + search_size) < (COLS - 1))) endIdx = c + search_size;

	return endIdx;
}

int SAD(unsigned char** current_matrix, unsigned char** comparing_matrix)
{
	int sad = 0;

	for (int i = 0; i < M_SIZE; i++)
	{
		for (int j = 0; j < M_SIZE; j++)
		{
			sad += abs((*(*current_matrix + i) + j) - *(*(comparing_matrix + i) + j));
		}
	}

	return sad;
}

int SADint(int** current_matrix, unsigned char** comparing_matrix)
{
	int sad = 0;

	for (int i = 0; i < M_SIZE; i++)
	{
		for (int j = 0; j < M_SIZE; j++)
		{
			sad += abs((*(*current_matrix + i) + j) - *(*(comparing_matrix + i) + j));
		}
	}

	return sad;
}

int** difference_blocks_in(unsigned char** current_matrix, unsigned char** min_matrix)
{
	//printf("1-1\n");
	int** difference_matrix = initializerI(M_SIZE, M_SIZE);
	//differenceMatrix = new int*[SIZE];
	//printf("1-2\n");
	for (int i = 0; i < M_SIZE; i++)
	{
		//differenceMatrix[i] = new int[SIZE];
		for (int j = 0; j < M_SIZE; j++)
		{
			*(*(difference_matrix + i) + j) = *(*(current_matrix + i) + j) - *(*(min_matrix + i) + j);
		}
	}

	return difference_matrix;
}

int** quantize_in(int** difference_matrix, float ql, int rs, int cs)
{
	int** quantized_matrix = initializerI(rs, cs);

	for (int i = 0; i < rs; i++)
	{
		for (int j = 0; j < cs; j++)
		{
			*(*(quantized_matrix + i) + j) = *(*(difference_matrix + i) + j) / ql;
		}
	}

	return quantized_matrix;
}

int** dequantize_in(int** quantized_matrix, float ql, int rs, int cs)
{
	int** dequantized_matrix = initializerI(rs, cs);

	for (int i = 0; i < rs; i++)
	{
		for (int j = 0; j < cs; j++)
		{
			*(*(dequantized_matrix + i) + j) = *(*(quantized_matrix + i) + j) * ql;
		}
	}

	return dequantized_matrix;
}

unsigned char** reconstructed_block_in(int** dequantized_matrix, unsigned char** min_matrix, int rs, int cs)
{
	unsigned char** reconstructed_matrix = initializer(rs, cs);

	int sum = 0;

	for (int i = 0; i < rs; i++)
	{
		for (int j = 0; j < cs; j++)
		{
			sum = *(*(dequantized_matrix + i) + j) + *(*(min_matrix + i) + j);
			if (sum > 255) sum = 255;
			else if (sum < 0) sum = 0;
			*(*(reconstructed_matrix + i) + j) = sum;
		}
	}

	return reconstructed_matrix;
}

unsigned char** apply_inter(const int search_size, unsigned char** previous_image, unsigned char** current_image,
                            const char* name, const char* quan_name, const char* mv_name)
{
	int** motionVectors = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int min = numeric_limits<int>::max();
	int idx_r = 0, idx_c = 0;
	unsigned char** result = initializer(ROWS, COLS);

	int counter0 = 0;
	//initialization
	unsigned char** matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** currentMatrix = initializer(M_SIZE, M_SIZE);
	unsigned char** minMatrix = initializer(M_SIZE, M_SIZE);
	int** differenceMatrix = initializerI(M_SIZE, M_SIZE);
	int** quantizedMatrix = initializerI(M_SIZE, M_SIZE);
	int** dequantizedMatrix = initializerI(M_SIZE, M_SIZE);
	unsigned char** reconstructedMatrix = initializer(M_SIZE, M_SIZE);
	int** dataTobeSavedQ = initializerI(ROWS, COLS);
	mv_sum = 0;


	//current matrix for calculating cost function
	for (int r = 0; r < ROWS; r += M_SIZE)
	{
		for (int c = 0; c < COLS; c += M_SIZE)
		{
			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(currentMatrix + i) + j) = *(*(current_image + r + i) + c + j);
				}
			}

			for (int a = start_row(r, search_size); a < end_row(r, search_size, ROWS); a++)
			{
				for (int b = start_col(c, search_size); b < end_col(c, search_size, COLS); b++)
				{
					if ((a > ROWS - M_SIZE - 1) || (b > COLS - M_SIZE - 1))
						break;
					for (int i = 0; i < M_SIZE; i++)
					{
						for (int j = 0; j < M_SIZE; j++)
						{
							*(*(matrix + i) + j) = *(*(previous_image + a + i) + b + j);
							if ((a + i > ROWS - 1) || (b + j) > COLS - 1)
							{
								break;
							}
						}
					}
					//tempMatrices.push_back(matrix);
					if (min > SAD(currentMatrix, matrix))
					{
						min = SAD(currentMatrix, matrix);
						idx_r = a;
						idx_c = b;
					}
				}
			}
			//saving motion vector coordinates
			*(*(motionVectors) + counter0) = idx_c;
			*(*(motionVectors + 1) + counter0) = idx_r;


			min = numeric_limits<int>::max();

			mv_sum += sqrt((c - idx_c) * (c - idx_c) + (r - idx_r) * (r - idx_r));

			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(minMatrix + i) + j) = *(*(previous_image + idx_r + i) + idx_c + j);
				}
			}

			differenceMatrix = difference_blocks_in(currentMatrix, minMatrix);


			quantizedMatrix = non_uniform_quantization2(dct_transform(differenceMatrix, M_SIZE, M_SIZE), M_SIZE);

			//quantizedMatrix = quantizeIn(differenceMatrix, 1.75, MBSize, MBSize);
			//add make send file to decoder
			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(dataTobeSavedQ + r + i) + c + j) = *(*(quantizedMatrix + i) + j);
				}
			}

			dequantizedMatrix = idct_transform(non_uniform_dequantization2(quantizedMatrix, M_SIZE), M_SIZE, M_SIZE);
			//dequantizedMatrix = dequantizeIn(quantizedMatrix, 1.75, MBSize, MBSize);	
			reconstructedMatrix = reconstructed_block_in(dequantizedMatrix, minMatrix, M_SIZE, M_SIZE);

			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(reconstructedMatrix + i) + j);
				}
			}
			counter0++;
		}
	}

	saveFileInt(motionVectors, mv_name, 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(dataTobeSavedQ, quan_name, ROWS, COLS);
	save_file(result, name, ROWS, COLS);

	//returns predictedMatrix
	return result;
}

unsigned char** apply_inter_adaptive(int search_size, vector<unsigned char**> previous_images,
                                     unsigned char** current_image,
                                     const char* name, const char* quan_name, const char* mv_name, int idx)
{
	int** motion_vectors = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int min = numeric_limits<int>::max();
	int idx_r = 0, idx_c = 0;
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** previous_image = initializer(ROWS, COLS);
	unsigned char** prev_frame_number = initializer(ROWS / M_SIZE, COLS / M_SIZE);

	//int MBSize = SIZE;
	int counter0 = 0;
	//initialization
	unsigned char** matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** current_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** min_matrix = initializer(M_SIZE, M_SIZE);
	int** difference_matrix = initializerI(M_SIZE, M_SIZE);
	int** quantized_matrix = initializerI(M_SIZE, M_SIZE);
	int** dequantized_matrix = initializerI(M_SIZE, M_SIZE);
	unsigned char** reconstructed_matrix = initializer(M_SIZE, M_SIZE);
	int** data_tobe_saved_q = initializerI(ROWS, COLS);
	mv_sum = 0;
	int min_idx = 0;
	vector<int> count_sort(9);


	//current matrix for calculating cost function
	for (int r = 0; r < ROWS; r += M_SIZE)
	{
		for (int c = 0; c < COLS; c += M_SIZE)
		{
			
			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(current_matrix + i) + j) = *(*(current_image + r + i) + c + j);
				}
			}
			for (int l = 0; l < previous_images.size(); l++)
			{
				previous_image = previous_images[l];
				for (int a = start_row(r, search_size); a < end_row(r, search_size, ROWS); a++)
				{
					for (int b = start_col(c, search_size); b < end_col(c, search_size, COLS); b++)
					{
						if ((a > ROWS - M_SIZE - 1) || (b > COLS - M_SIZE - 1))
							break;
						for (int i = 0; i < M_SIZE; i++)
						{
							for (int j = 0; j < M_SIZE; j++)
							{
								*(*(matrix + i) + j) = *(*(previous_image + a + i) + b + j);
								if ((a + i > ROWS - 1) || (b + j) > COLS - 1)
								{
									break;
								}
							}
						}
						//tempMatrices.push_back(matrix);
						if (min > SAD(current_matrix, matrix))
						{
							min = SAD(current_matrix, matrix);
							idx_r = a;
							idx_c = b;
							min_idx = l;
						}
					}
				}

				//reinitialize
				previous_image = initializer(ROWS, COLS);
			}
			//previous Frame numbers for stats
			*(*(prev_frame_number + r / M_SIZE) + c / M_SIZE) = min_idx + 1;
			//saving motion vector coordinates
			*(*(motion_vectors) + counter0) = idx_c;
			*(*(motion_vectors + 1) + counter0) = idx_r;


			min = numeric_limits<int>::max();

			//mv_sum += sqrt((c - idx_c) * (c - idx_c) + (r - idx_r) * (r - idx_r));

			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(min_matrix + i) + j) = *(*(previous_image + idx_r + i) + idx_c + j);
				}
			}

			difference_matrix = difference_blocks_in(current_matrix, min_matrix);


			quantized_matrix = non_uniform_quantization2(dct_transform(difference_matrix, M_SIZE, M_SIZE), M_SIZE);

			//quantizedMatrix = quantizeIn(differenceMatrix, 1.75, MBSize, MBSize);
			//add make send file to decoder
			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(data_tobe_saved_q + r + i) + c + j) = *(*(quantized_matrix + i) + j);
				}
			}

			dequantized_matrix = idct_transform(non_uniform_dequantization2(quantized_matrix, M_SIZE), M_SIZE, M_SIZE);
			//dequantizedMatrix = dequantizeIn(quantizedMatrix, 1.75, MBSize, MBSize);	
			reconstructed_matrix = reconstructed_block_in(dequantized_matrix, min_matrix, M_SIZE, M_SIZE);

			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(reconstructed_matrix + i) + j);
				}
			}
			counter0++;
		}
	}

	save_file(prev_frame_number, change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	saveFileInt(motion_vectors, mv_name, 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(data_tobe_saved_q, quan_name, ROWS, COLS);
	save_file(result, name, ROWS, COLS);

	//returns predictedMatrix
	return result;
}

unsigned char** apply_inter_bi_directional(int search_size, vector<unsigned char**> previous_images,
                                           unsigned char** current_image,
                                           const char* name, const char* quan_name, const char* mv_name,
                                           const char* mv_name_1, int idx)
{
	int** motion_vectors = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** motion_vectors_1 = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int min = numeric_limits<int>::max();
	int idx_r = 0, idx_c = 0, idx_r1 = 0, idx_c1 = 0;
	int sum_bi_MV = 0, sum_MV = 0;
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** min_mat_res = initializer(M_SIZE, M_SIZE);
	unsigned char** prev_frame_number = initializer(ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** prev_frame_number_1 = initializer(ROWS / M_SIZE, COLS / M_SIZE);
	int** averaging_coefficients = initializerI(ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** bi_or_mono = initializer(1, (ROWS * COLS) / (M_SIZE * M_SIZE));

	//int MBSize = SIZE;
	int counter0 = 0;
	//initialization
	unsigned char** matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** current_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** min_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** bi_min_matrix = initializer(M_SIZE, M_SIZE);
	int** difference_matrix = initializerI(M_SIZE, M_SIZE);
	int** quantized_matrix = initializerI(M_SIZE, M_SIZE);
	int** dequantized_matrix = initializerI(M_SIZE, M_SIZE);
	unsigned char** reconstructed_matrix = initializer(M_SIZE, M_SIZE);
	int** data_tobe_saved_q = initializerI(ROWS, COLS);
	mv_sum = 0;
	int min_idx1 = 0, min_idx2 = 0, min_coef = 0;
	vector<int> count_sort(9);
	vector<float> coefficients = {0.2f, 0.4f, 0.6f, 0.8f};


	//current matrix for calculating cost function
	for (auto r = 0; r < ROWS; r += M_SIZE)
	{
		for (auto c = 0; c < COLS; c += M_SIZE)
		{
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(current_matrix + i) + j) = *(*(current_image + r + i) + c + j);
				}
			}
			//first the minimum MV is found throughout the previous frames
			for (auto l = 0; l < previous_images.size(); l++)
			{
				for (auto a = start_row(r, search_size); a < end_row(r, search_size, ROWS); a++)
				{
					for (auto b = start_col(c, search_size); b < end_col(c, search_size, COLS); b++)
					{
						if ((a > ROWS - M_SIZE - 1) || (b > COLS - M_SIZE - 1))
							break;
						for (auto i = 0; i < M_SIZE; i++)
						{
							for (auto j = 0; j < M_SIZE; j++)
							{
								*(*(matrix + i) + j) = *(*(previous_images[l] + a + i) + b + j);
								if ((a + i > ROWS - 1) || (b + j) > COLS - 1)
								{
									break;
								}
							}
						}
						//tempMatrices.push_back(matrix);
						if (min > SAD(current_matrix, matrix))
						{
							min = SAD(current_matrix, matrix);
							idx_r = a;
							idx_c = b;
							min_idx1 = l;
						}
					}
				}
			}
			//min_matrix is the minimum MV found from the previous frames
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(min_matrix + i) + j) = *(*(previous_images[min_idx1] + idx_r + i) + idx_c + j);
				}
			}
			min = numeric_limits<int>::max();
			for (auto l = 0; l < previous_images.size(); l++)
			{
				for (auto a = start_row(r, search_size); a < end_row(r, search_size, ROWS); a++)
				{
					for (auto b = start_col(c, search_size); b < end_col(c, search_size, COLS); b++)
					{
						if ((a > ROWS - M_SIZE - 1) || (b > COLS - M_SIZE - 1))
							break;
						for (auto coef = 0; coef < coefficients.size(); coef++)
						{
							for (auto i = 0; i < M_SIZE; i++)
							{
								for (auto j = 0; j < M_SIZE; j++)
								{
									const int pxl1 = static_cast<int>(*(*(previous_images[l] + a + i) + b + j) *
										coefficients[coef]);
									const int pxl2 = static_cast<int>(*(*(previous_images[min_idx1] + idx_r + i) + idx_c
										+ j) * (1 - coefficients[coef]));
									*(*(matrix + i) + j) = pxl1 + pxl2;

									if ((a + i > ROWS - 1) || (b + j) > COLS - 1)
									{
										break;
									}
								}
							}
							if (min > SAD(current_matrix, matrix))
							{
								min = SAD(current_matrix, matrix);
								idx_r1 = a;
								idx_c1 = b;
								min_idx2 = l;
								min_coef = coef;
							}
						}
					}
				}
			}
			//averaged min matrix
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					const int pxl1 = static_cast<int>(*(*(previous_images[min_idx2] + idx_r1 + i) + idx_c1 + j) *
						coefficients[min_coef]);
					const int pxl2 = static_cast<int>(*(*(previous_images[min_idx1] + idx_r + i) + idx_c + j) * (1 -
						coefficients[min_coef]));

					*(*(bi_min_matrix + i) + j) = pxl1 + pxl2;
				}
			}
			*(*(averaging_coefficients + r / M_SIZE) + c / M_SIZE) = min_coef;

			sum_bi_MV = SAD(current_matrix, bi_min_matrix);
			sum_MV = SAD(current_matrix, min_matrix);

			//saving the motion vector coordinates
			*(*(motion_vectors) + counter0) = idx_c;
			*(*(motion_vectors + 1) + counter0) = idx_r;

			*(*(motion_vectors_1) + counter0) = idx_c1;
			*(*(motion_vectors_1 + 1) + counter0) = idx_r1;

			*(*(prev_frame_number + r / M_SIZE) + c / M_SIZE) = min_idx1 + 1;
			*(*(prev_frame_number_1 + r / M_SIZE) + c / M_SIZE) = min_idx2 + 1;
			//do not save bi_directional and fixed in one file use separate files for decoder
			if (sum_MV < sum_bi_MV)
			{
				*(*(bi_or_mono) + counter0) = 1; // if the it is fixed MV			
				min_mat_res = min_matrix;
			}
			else
			{
				*(*(bi_or_mono) + counter0) = 0; // 0 if it is averaged MV				
				min_mat_res = bi_min_matrix;
			}

			min = numeric_limits<int>::max();

			difference_matrix = difference_blocks_in(current_matrix, min_mat_res);

			quantized_matrix = non_uniform_quantization2(dct_transform(difference_matrix, M_SIZE, M_SIZE), M_SIZE);

			//add make send file to decoder
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(data_tobe_saved_q + r + i) + c + j) = *(*(quantized_matrix + i) + j);
				}
			}

			dequantized_matrix = idct_transform(non_uniform_dequantization2(quantized_matrix, M_SIZE), M_SIZE, M_SIZE);
			reconstructed_matrix = reconstructed_block_in(dequantized_matrix, min_mat_res, M_SIZE, M_SIZE);

			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(reconstructed_matrix + i) + j);
				}
			}
			counter0++;
		}
	}
	save_file(bi_or_mono, change_names("bi_or_mono", idx), 1, (ROWS * COLS) / (M_SIZE * M_SIZE));
	save_file(prev_frame_number, change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	save_file(prev_frame_number_1, change_names("prevFrame_bi", idx), ROWS / M_SIZE, COLS / M_SIZE);
	saveFileInt(averaging_coefficients, change_names("averaging_coefficients", idx), ROWS / M_SIZE, COLS / M_SIZE);
	saveFileInt(motion_vectors, mv_name, 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(motion_vectors_1, mv_name_1, 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(data_tobe_saved_q, quan_name, ROWS, COLS);
	save_file(result, name, ROWS, COLS);

	//returns predictedMatrix
	return result;
}

unsigned char** apply_inter_predictive(int search_size, vector<unsigned char**> previous_images,
                                       unsigned char** current_image,
                                       const char* name, const char* quan_name, const char* mv_name,
                                       const char* mv_name_1, int idx)
{
	int** motion_vectors = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** motion_vectors_1 = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** PMV = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** PMV_1 = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** MVD = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** MVD_1 = initializerI(2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int min = numeric_limits<int>::max();
	int idx_r = 0, idx_c = 0, idx_r1 = 0, idx_c1 = 0;
	int sp_r = 0, sp_r1 = 0, sp_c = 0, sp_c1 = 0;
	int sp_r_1 = 0, sp_r1_1 = 0, sp_c_1 = 0, sp_c1_1 = 0;
	int sum_bi_MV = 0, sum_MV = 0;
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** min_mat_res = initializer(M_SIZE, M_SIZE);
	unsigned char** prev_frame_number = initializer(ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** prev_frame_number_1 = initializer(ROWS / M_SIZE, COLS / M_SIZE);
	int** averaging_coefficients = initializerI(ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** bi_or_mono = initializer(1, (ROWS * COLS) / (M_SIZE * M_SIZE));

	//int MBSize = SIZE;
	int counter0 = 0;
	//initialization
	unsigned char** matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** current_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** min_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** bi_min_matrix = initializer(M_SIZE, M_SIZE);
	int** difference_matrix = initializerI(M_SIZE, M_SIZE);
	int** quantized_matrix = initializerI(M_SIZE, M_SIZE);
	int** dequantized_matrix = initializerI(M_SIZE, M_SIZE);
	unsigned char** reconstructed_matrix = initializer(M_SIZE, M_SIZE);
	int** data_tobe_saved_q = initializerI(ROWS, COLS);
	mv_sum = 0;
	int min_idx1 = 0, min_idx2 = 0, min_coef = 0;
	
	vector<int> count_sort(9);
	vector<float> coefficients = { 0.2f, 0.4f, 0.5f, 0.6f, 0.8f };


	//current matrix for calculating cost function
	for (auto r = 0; r < ROWS; r += M_SIZE)
	{
		for (auto c = 0; c < COLS; c += M_SIZE)
		{
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(current_matrix + i) + j) = *(*(current_image + r + i) + c + j);
				}
			}
			//first the minimum MV is found throughout the previous frames
			for (auto l = 0; l < previous_images.size(); l++)
			{
				for (auto a = start_row(r, search_size); a < end_row(r, search_size, ROWS); a++)
				{
					for (auto b = start_col(c, search_size); b < end_col(c, search_size, COLS); b++)
					{
						if ((a > ROWS - M_SIZE - 1) || (b > COLS - M_SIZE - 1))
							break;
						for (auto i = 0; i < M_SIZE; i++)
						{
							for (auto j = 0; j < M_SIZE; j++)
							{
								*(*(matrix + i) + j) = *(*(previous_images[l] + a + i) + b + j);
								if ((a + i > ROWS - 1) || (b + j) > COLS - 1)
								{
									break;
								}
							}
						}
						//tempMatrices.push_back(matrix);
						if (min > SAD(current_matrix, matrix))
						{
							min = SAD(current_matrix, matrix);
							idx_r = a;
							idx_c = b;
							min_idx1 = l;
						}
					}
				}
			}
			//min_matrix is the minimum MV found from the previous frames
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(min_matrix + i) + j) = *(*(previous_images[min_idx1] + idx_r + i) + idx_c + j);
				}
			}
			min = numeric_limits<int>::max();
			for (auto l = 0; l < previous_images.size(); l++)
			{
				for (auto a = start_row(r, search_size); a < end_row(r, search_size, ROWS); a++)
				{
					for (auto b = start_col(c, search_size); b < end_col(c, search_size, COLS); b++)
					{
						if ((a > ROWS - M_SIZE - 1) || (b > COLS - M_SIZE - 1))
							break;
						for (auto coef = 0; coef < coefficients.size(); coef++)
						{
							for (auto i = 0; i < M_SIZE; i++)
							{
								for (auto j = 0; j < M_SIZE; j++)
								{
									const int pxl1 = static_cast<int>(*(*(previous_images[l] + a + i) + b + j) *
										coefficients[coef]);
									const int pxl2 = static_cast<int>(*(*(previous_images[min_idx1] + idx_r + i) + idx_c
										+ j) * (1 - coefficients[coef]));
									*(*(matrix + i) + j) = pxl1 + pxl2;

									if ((a + i > ROWS - 1) || (b + j) > COLS - 1)
									{
										break;
									}
								}
							}
							if (min > SAD(current_matrix, matrix))
							{
								min = SAD(current_matrix, matrix);
								idx_r1 = a;
								idx_c1 = b;
								min_idx2 = l;
								min_coef = coef;
							}
						}
					}
				}
			}
			//averaged min matrix
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					const int pxl1 = static_cast<int>(*(*(previous_images[min_idx2] + idx_r1 + i) + idx_c1 + j) *
						coefficients[min_coef]);
					const int pxl2 = static_cast<int>(*(*(previous_images[min_idx1] + idx_r + i) + idx_c + j) * (1 -
						coefficients[min_coef]));

					*(*(bi_min_matrix + i) + j) = pxl1 + pxl2;
				}
			}
			*(*(averaging_coefficients + r / M_SIZE) + c / M_SIZE) = min_coef;

			sum_bi_MV = SAD(current_matrix, bi_min_matrix);
			sum_MV = SAD(current_matrix, min_matrix);

			//saving the motion vector coordinates
			// Fixed MV params --------------------------------------------------------------------------
			*(*(motion_vectors) + counter0) = idx_c;
			*(*(motion_vectors + 1) + counter0) = idx_r;
			//Averaged MV params
			*(*(motion_vectors_1)+counter0) = idx_c1;
			*(*(motion_vectors_1 + 1) + counter0) = idx_r1;

			//MVD for fixed MV
			//1. finding PMVs
			if(r != 0)
			{
				//fixed MV
				sp_r = *(*(motion_vectors + 1) + counter0 - 1);
				sp_r1 = *(*(motion_vectors + 1) + (((r - 1) * c) / (M_SIZE * M_SIZE)));
				//Averaged MV
				sp_r_1 = *(*(motion_vectors_1 + 1) + counter0 - 1);
				sp_r1_1 = *(*(motion_vectors_1 + 1) + (((r - 1) * c) / (M_SIZE * M_SIZE)));
			}
			if(c != 0)
			{
				//fixed MV
				sp_c = *(*(motion_vectors)+counter0 - 1);
				sp_c1 = *(*(motion_vectors)+(((c - 1) * r) / (M_SIZE * M_SIZE)));
				//Averaged MV
				sp_c_1 = *(*(motion_vectors_1)+counter0 - 1);
				sp_c1_1 = *(*(motion_vectors_1)+(((c - 1) * r) / (M_SIZE * M_SIZE)));
			}			

			//PMV = (sp_c + sp_c1)/2
			//MVD = PMV - MV
			//Fixed PMV
			*(*(PMV)+counter0) = (sp_c + sp_c1) / 2;
			*(*(PMV + 1)+counter0) = (sp_r + sp_r1) / 2;
			//Fixed MVD
			*(*(MVD)+counter0) = ((sp_c + sp_c1) / 2) - idx_c;
			*(*(MVD + 1)+counter0) = ((sp_r + sp_r1) / 2) - idx_r;		

			//Averaged PMV
			*(*(PMV_1)+counter0) = (sp_c_1 + sp_c1_1) / 2;
			*(*(PMV_1 + 1) + counter0) = (sp_r_1 + sp_r1_1) / 2;
			//Fixed MVD
			*(*(MVD_1)+counter0) = ((sp_c_1 + sp_c1_1) / 2) - idx_c1;
			*(*(MVD_1 + 1) + counter0) = ((sp_r_1 + sp_r1_1) / 2) - idx_r1;
			//---------------------------------------------------------------------------------------------------------
			
						
			*(*(prev_frame_number + r / M_SIZE) + c / M_SIZE) = min_idx1 + 1;
			*(*(prev_frame_number_1 + r / M_SIZE) + c / M_SIZE) = min_idx2 + 1;
			
			//do not save bi_directional and fixed in one file use separate files for decoder
			if (sum_MV <= sum_bi_MV)
			{
				*(*(bi_or_mono)+counter0) = 1;
				min_mat_res = min_matrix;				
			}
			else
			{	
				*(*(bi_or_mono)+counter0) = 0;
				min_mat_res = bi_min_matrix;
			}

			min = numeric_limits<int>::max();

			difference_matrix = difference_blocks_in(current_matrix, min_mat_res);

			quantized_matrix = non_uniform_quantization2(dct_transform(difference_matrix, M_SIZE, M_SIZE), M_SIZE);

			//add make send file to decoder
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(data_tobe_saved_q + r + i) + c + j) = *(*(quantized_matrix + i) + j);
				}
			}

			dequantized_matrix = idct_transform(non_uniform_dequantization2(quantized_matrix, M_SIZE), M_SIZE, M_SIZE);
			reconstructed_matrix = reconstructed_block_in(dequantized_matrix, min_mat_res, M_SIZE, M_SIZE);

			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(reconstructed_matrix + i) + j);
				}
			}
			counter0++;
		}
	}
	//need to save PMV and MVD
	//the PMV - MV = MVD
	//so  MV = PMV - MVD --> this step for decoding purposes
	//PMV and MVD need to be constructed for both Averaged and Fixed MVs since the requirements
	//state that along with PMV and MVD averaging coefficients needed to be sent via bitstream
	//according to HW_5, otherwise the more vise version would be simply make one PMV and MVD
	//since each frame uses either Fixed or Averaged MBs.
	
	save_file(bi_or_mono, change_names("bi_or_mono", idx), 1, (ROWS * COLS) / (M_SIZE * M_SIZE));
	save_file(prev_frame_number, change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	save_file(prev_frame_number_1, change_names("prevFrame_bi", idx), ROWS / M_SIZE, COLS / M_SIZE);
	saveFileInt(averaging_coefficients, change_names("averaging_coefficients", idx), ROWS / M_SIZE, COLS / M_SIZE);
	saveFileInt(MVD, change_names("MVD", idx), 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(PMV, change_names("PMV", idx), 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(MVD_1, change_names("MVD_bi", idx), 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(PMV_1, change_names("PMV_bi", idx), 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(motion_vectors, change_names("inter_mv", idx), 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(motion_vectors_1, change_names("inter_mv_bi", idx), 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	saveFileInt(data_tobe_saved_q, quan_name, ROWS, COLS);
	save_file(result, name, ROWS, COLS);

	//returns predictedMatrix
	return result;		
			
}

int check_for_row(int r, int c, int a, int searchSize)
{
	int start_idx = 0;

	if ((r == 0) || (r < searchSize)) start_idx = 0;
	else if ((r != 0) && (r > searchSize)) start_idx = r - searchSize;

	return start_idx;
}

int check_for_col(int r, int c, int b, int searchSize)
{
	int start_idx = 0;

	if ((c == 0) || (c < searchSize)) start_idx = 0;
	else if ((c != 0) && (c > searchSize))start_idx = c - searchSize;

	return start_idx;
}
