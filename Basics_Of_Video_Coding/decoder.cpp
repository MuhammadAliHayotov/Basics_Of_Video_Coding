#include "inter_intra.h"

constexpr int SIZE = M_SIZE;

constexpr int m = 4;
constexpr int n = 4;

unsigned char* get_up_matrix(unsigned char** result, int r, int c)
{
	unsigned char* upMatrix = static_cast<unsigned char*>(calloc(8, sizeof(unsigned char)));
	if (r == 0) for (int a = 0; a < 8; a++) *(upMatrix + a) = 128;
	else if ((c == (COLS - 4)) && r > 0)
	{
		//initial 4 elements are from the input image
		for (int a = 0; a < 4; a++)
			*(upMatrix + a) = *(*(result + r - 1) + c + a);
		//the rest is out of the boundary thus
		for (int a = 4; a < 8; a++)
			*(upMatrix + a) = 128;
	}
		//if the image is inside the boundary
	else for (int a = 0; a < 8; a++) *(upMatrix + a) = *(*(result + r - 1) + c + a);

	return upMatrix;
}

unsigned char* get_side_matrix(unsigned char** result, int r, int c)
{
	unsigned char* sideMatrix = static_cast<unsigned char*>(calloc(4, sizeof(unsigned char)));

	if (c == 0) for (int a = 0; a < 4; a++) *(sideMatrix + a) = 128;
	else for (int a = 0; a < 4; a++) *(sideMatrix + a) = *(*(result + r + a) + c - 1);

	return sideMatrix;
}

int get_q(unsigned char** image, int r, int c)
{
	int q = 128;
	if (r == 0 || c == 0) q = 128;
	else q = *(*(image + r - 1) + c - 1);

	return q;
}

unsigned char** decode_intra(const char* modeName, const char* quanName)
{
	unsigned char** predictedMatrix;
	unsigned char** differenceMatrix;
	int** quantizedMatrix;
	int** dequantizedMatrix;
	unsigned char** reconstructedMatrix;
	unsigned char** outputImage;
	unsigned char** modeNumbers;
	int** quantizedData;

	int q = 0;

	unsigned char* upMatrix;
	unsigned char* sideMatrix;

	upMatrix = static_cast<unsigned char*>(calloc(8, sizeof(unsigned char)));
	sideMatrix = static_cast<unsigned char*>(calloc(4, sizeof(unsigned char)));


	predictedMatrix = initializer(m, n);
	differenceMatrix = initializer(m, n);
	quantizedMatrix = initializerI(m, n);
	dequantizedMatrix = initializerI(m, n);
	reconstructedMatrix = initializer(m, n);
	outputImage = initializer(ROWS, COLS);
	modeNumbers = initializer(ROWS / 4, COLS / 4);
	quantizedData = initializerI(ROWS, COLS);

	modeNumbers = read_file(modeName, ROWS / 4, COLS / 4);
	quantizedData = readFileInt(quanName, ROWS, COLS);


	for (int i = 0; i < ROWS; i += m)
	{
		for (int j = 0; j < COLS; j += n)
		{
			//assigning the value of q
			if (i == 0 || j == 0) q = 128;
			else q = *(*(outputImage + i - 1) + j - 1);
			//upMatrix
			if (i == 0) for (int r = 0; r < 8; r++) *(upMatrix + r) = 128;
			else if ((j == (COLS - 4)) && i > 0)
			{
				//initial 4 elements are from the input image
				for (int r = 0; r < 4; r++)
					*(upMatrix + r) = *(*(outputImage + i - 1) + j + r);
				//the rest is out of the boundary thus
				for (int r = 4; r < 8; r++)
					*(upMatrix + r) = 128;
			}
				//if the image is inside the boundary
			else for (int r = 0; r < 8; r++) *(upMatrix + r) = *(*(outputImage + i - 1) + j + r);
			//sideMatrix values
			if (j == 0) for (int r = 0; r < 4; r++) *(sideMatrix + r) = 128;
			else for (int r = 0; r < 4; r++) *(sideMatrix + r) = *(*(outputImage + i + r) + j - 1);


			//assigning quantizedMatrix from quantized Data
			for (int r = 0; r < m; r++)
			{
				for (int c = 0; c < n; c++)
				{
					*(*(quantizedMatrix + r) + c) = *(*(quantizedData + i + r) + j + c);
				}
			}

			//dequantizedMatrix = dequantizeIn(quantizedMatrix, 1.75, 4, 4);
			dequantizedMatrix = idct_transform(non_uniform_dequantization2(quantizedMatrix, 4), 4, 4);
			//iDCT

			//corresponding mode to the and changes matrix 
			int modeNum = *(*(modeNumbers + i / m) + j / n);

			predictedMatrix = apply_mode(modeNum, upMatrix, sideMatrix, q);
			reconstructedMatrix = reconstructed_block_in(dequantizedMatrix, predictedMatrix, 4, 4);

			//assigning new values 
			for (int r = 0; r < m; r++)
			{
				for (int c = 0; c < n; c++)
				{
					//swapping inputImage's predBlock
					*(*(outputImage + i + r) + j + c) = *(*(reconstructedMatrix + r) + c);
				}
			}
		}
	}

	save_file(outputImage, "decoded_intra.y", ROWS, COLS);
	return outputImage;
} //should return 8x8 decoded predictedMatrix
unsigned char** decode_inter(unsigned char** previousFrame, const char* quanName, const char* mvName)
{
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** predictedMatrix = initializer(SIZE, SIZE);
	unsigned char** reconstructedMatrix = initializer(SIZE, SIZE);
	int** dequantizedData = initializerI(ROWS, COLS);
	int** dequantizedMatrix = initializerI(SIZE, SIZE);
	int** quantizedMatrix = initializerI(SIZE, SIZE);

	int** quantizedData = readFileInt(quanName, ROWS, COLS);
	int** motionVectors = readFileInt(mvName, 2, ((ROWS * COLS) / (SIZE * SIZE)));

	//dequantizedData = dequantizeIn(quantizedData, 1.75, ROWS, COLS);

	int idx_r = 0;
	int idx_c = 0;
	int counter = 0;

	for (int r = 0; r < ROWS; r += SIZE)
	{
		for (int c = 0; c < COLS; c += SIZE)
		{
			//getting motion vectors
			idx_c = *(*(motionVectors) + counter);
			idx_r = *(*(motionVectors + 1) + counter);
			counter++;

			//getting predicted matrix and dequantizedMatrix
			for (int i = 0; i < SIZE; i++)
			{
				for (int j = 0; j < SIZE; j++)
				{
					*(*(predictedMatrix + i) + j) = *(*(previousFrame + idx_r + i) + idx_c + j);
					*(*(quantizedMatrix + i) + j) = *(*(quantizedData + r + i) + c + j);
				}
			}
			dequantizedMatrix = idct_transform(non_uniform_dequantization2(quantizedMatrix, SIZE), SIZE, SIZE);

			//reconstructed Matrix
			reconstructedMatrix = reconstructed_block_in(dequantizedMatrix, predictedMatrix, SIZE, SIZE);

			for (int i = 0; i < SIZE; i++)
			{
				for (int j = 0; j < SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(reconstructedMatrix + i) + j);
				}
			}
		}
	}
	save_file(result, "decoded_inter.y", ROWS, COLS);
	return result;
}

unsigned char** decode_inter_adaptive(vector<unsigned char**> previousFrames, const char* quanName, const char* mvName,
                                      int idx)
{
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** predictedMatrix = initializer(SIZE, SIZE);
	unsigned char** reconstructedMatrix = initializer(SIZE, SIZE);
	unsigned char** previousFrame = initializer(ROWS, COLS);
	int** dequantizedData = initializerI(ROWS, COLS);
	int** dequantizedMatrix = initializerI(SIZE, SIZE);
	int** quantizedMatrix = initializerI(SIZE, SIZE);

	int** quantizedData = readFileInt(quanName, ROWS, COLS);
	int** motionVectors = readFileInt(mvName, 2, ((ROWS * COLS) / (SIZE * SIZE)));

	//dequantizedData = dequantizeIn(quantizedData, 1.75, ROWS, COLS);
	unsigned char** prevFmNumbers = read_file(change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);

	int idx_r = 0;
	int idx_c = 0;
	int counter = 0;

	for (int r = 0; r < ROWS; r += SIZE)
	{
		for (int c = 0; c < COLS; c += SIZE)
		{
			//getting motion vectors
			idx_c = *(*(motionVectors) + counter);
			idx_r = *(*(motionVectors + 1) + counter);
			counter++;
			//for (int l = 0; l < previousFrames.size(); l++) {
			int tempIdx = static_cast<int>(*(*(prevFmNumbers + r / M_SIZE) + c / M_SIZE));

			previousFrame = previousFrames[tempIdx - 1];

			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(predictedMatrix + i) + j) = *(*(previousFrame + idx_r + i) + idx_c + j);
				}
			}
			previousFrame = initializer(ROWS, COLS);
			//}
			//getting predicted matrix and dequantizedMatrix
			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(quantizedMatrix + i) + j) = *(*(quantizedData + r + i) + c + j);
				}
			}
			dequantizedMatrix = idct_transform(non_uniform_dequantization2(quantizedMatrix, M_SIZE), M_SIZE, M_SIZE);

			//reconstructed Matrix
			reconstructedMatrix = reconstructed_block_in(dequantizedMatrix, predictedMatrix, M_SIZE, M_SIZE);

			int tempS = 0;
			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					tempS = *(*(dequantizedMatrix + i) + j);
					if (tempS > 255) tempS = 255;
					if (tempS < 0) tempS = 0;
					*(*(result + r + i) + c + j) = tempS;
				}
			}
		}
	}
	save_file(result, "decoded_inter.y", ROWS, COLS);
	return result;
}

unsigned char** decode_inter_bi_directional_mv(vector<unsigned char**> previous_frames, const char* quan_name,
                                               const char* mv_name, const char* mv_name_1, int idx)
{
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** predicted_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** reconstructed_matrix = initializer(M_SIZE, M_SIZE);
	int** dequantizedData = initializerI(ROWS, COLS);
	int** dequantized_matrix = initializerI(M_SIZE, M_SIZE);
	int** quantized_matrix = initializerI(M_SIZE, M_SIZE);

	int** quantizedData = readFileInt(quan_name, ROWS, COLS);
	int** motion_vectors = readFileInt(mv_name, 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));
	int** motion_vectors_1 = readFileInt(mv_name_1, 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));
	vector<float> coefficients = {0.2f, 0.4f, 0.6f, 0.8f};

	unsigned char** prev_fm_numbers = read_file(change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** prev_fm_numbers_1 = read_file(change_names("prevFrame_bi", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** bi_or_mono = read_file(change_names("bi_or_mono", idx), 1, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** averaging_coefs = readFileInt(change_names("averaging_coefficients", idx), ROWS / M_SIZE, COLS / M_SIZE);
	int counter = 0;

	for (int r = 0; r < ROWS; r += M_SIZE)
	{
		for (int c = 0; c < COLS; c += M_SIZE)
		{
			const int idx_c = *(*(motion_vectors) + counter);
			const int idx_r = *(*(motion_vectors + 1) + counter);
			const int idx_c1 = *(*(motion_vectors_1) + counter);
			const int idx_r1 = *(*(motion_vectors_1 + 1) + counter);

			const int temp_idx = static_cast<int>(*(*(prev_fm_numbers + r / M_SIZE) + c / M_SIZE));
			const int temp_idx_1 = static_cast<int>(*(*(prev_fm_numbers_1 + r / M_SIZE) + c / M_SIZE));
			const int min_coef = *(*(averaging_coefs + r / M_SIZE) + c / M_SIZE);

			if (static_cast<int>(*(*(bi_or_mono) + counter)) == 0)
			{
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						const int pxl1 = static_cast<int>(*(*(previous_frames[temp_idx_1 - 1] + idx_r1 + i) + idx_c1 + j
							)
							* (coefficients[min_coef]));
						const int pxl2 = static_cast<int>(*(*(previous_frames[temp_idx - 1] + idx_r + i) + idx_c + j) *
							(1.0f
								- coefficients[min_coef]));
						*(*(predicted_matrix + i) + j) = pxl1 + pxl2;
					}
				}
			}
			else
			{
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						*(*(predicted_matrix + i) + j) = *(*(previous_frames[static_cast<int>(temp_idx) - 1] + idx_r + i
						) + idx_c + j);
					}
				}
			}


			//getting predicted matrix and dequantizedMatrix
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(quantized_matrix + i) + j) = *(*(quantizedData + r + i) + c + j);
				}
			}
			dequantized_matrix = idct_transform(non_uniform_dequantization2(quantized_matrix, M_SIZE), M_SIZE, M_SIZE);

			//reconstructed Matrix
			reconstructed_matrix = reconstructed_block_in(dequantized_matrix, predicted_matrix, M_SIZE, M_SIZE);

			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(reconstructed_matrix + i) + j);
				}
			}

			counter++;
		}
	}
	save_file(result, "decoded_inter.y", ROWS, COLS);
	return result;
}

unsigned char** decode_inter_predictive(vector<unsigned char**> previous_frames, const char* quan_name, int idx)
{
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** predicted_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** reconstructed_matrix = initializer(M_SIZE, M_SIZE);
	int** dequantized_matrix;
	int** quantized_matrix = initializerI(M_SIZE, M_SIZE);

	int** quantizedData = readFileInt(quan_name, ROWS, COLS);
	int** MVD = readFileInt(change_names("MVD", idx), 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));
	int** PMV = readFileInt(change_names("PMV", idx), 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));
	int** MVD_1 = readFileInt(change_names("MVD_bi", idx), 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));
	int** PMV_1 = readFileInt(change_names("PMV_bi", idx), 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));

	
	vector<float> coefficients = { 0.2f, 0.4f, 0.5f, 0.6f, 0.8f };

	unsigned char** prev_fm_numbers = read_file(change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** prev_fm_numbers_1 = read_file(change_names("prevFrame_bi", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** bi_or_mono = read_file(change_names("bi_or_mono", idx), 1, (ROWS * COLS) / (M_SIZE * M_SIZE));
	int** averaging_coefs = readFileInt(change_names("averaging_coefficients", idx), ROWS / M_SIZE, COLS / M_SIZE);
	int counter = 0;

	for (int r = 0; r < ROWS; r += M_SIZE)
	{
		for (int c = 0; c < COLS; c += M_SIZE)
		{
			const int idx_c = *(*(PMV)+counter) - *(*(MVD)+counter);
			const int idx_r = *(*(PMV + 1) + counter) - *(*(MVD + 1) + counter);
			const int idx_c1 = *(*(PMV_1)+counter) - *(*(MVD_1)+counter);
			const int idx_r1 = *(*(PMV_1 + 1) + counter) - *(*(MVD_1 + 1) + counter);

			const int temp_idx = static_cast<int>(*(*(prev_fm_numbers + r / M_SIZE) + c / M_SIZE));
			const int temp_idx_1 = static_cast<int>(*(*(prev_fm_numbers_1 + r / M_SIZE) + c / M_SIZE));
			const int min_coef = *(*(averaging_coefs + r / M_SIZE) + c / M_SIZE);

			if (static_cast<int>(*(*(bi_or_mono)+counter)) == 0)
			{
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						const int pxl1 = static_cast<int>(*(*(previous_frames[temp_idx_1 - 1] + idx_r1 + i) + idx_c1 + j
							)
							* (coefficients[min_coef]));
						const int pxl2 = static_cast<int>(*(*(previous_frames[temp_idx - 1] + idx_r + i) + idx_c + j) *
							(1.0f
								- coefficients[min_coef]));
						*(*(predicted_matrix + i) + j) = pxl1 + pxl2;
					}
				}
			}
			else
			{
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						*(*(predicted_matrix + i) + j) = *(*(previous_frames[static_cast<int>(temp_idx) - 1] + idx_r + i
							) + idx_c + j);
					}
				}
			}


			//getting predicted matrix and dequantizedMatrix
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(quantized_matrix + i) + j) = *(*(quantizedData + r + i) + c + j);
				}
			}
			dequantized_matrix = idct_transform(non_uniform_dequantization2(quantized_matrix, M_SIZE), M_SIZE, M_SIZE);

			//reconstructed Matrix
			reconstructed_matrix = reconstructed_block_in(dequantized_matrix, predicted_matrix, M_SIZE, M_SIZE);

			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(reconstructed_matrix + i) + j);
				}
			}

			counter++;
		}
	}
	save_file(result, "decoded_inter.y", ROWS, COLS);
	return result;
}


void decoder_inter_intra(unsigned char** previousPicture, const char* name, int idx)
{
	unsigned char** firstFrame = initializer(ROWS, COLS);
	unsigned char** secondFrame = initializer(ROWS, COLS);
	unsigned char** intraPred = initializer(ROWS, COLS);
	unsigned char** interPred = initializer(ROWS, COLS);
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** currentMatrix = initializer(SIZE, SIZE);
	int mv_sum = 0;
	int counter = 0, counterInter = 0, counterIntra = 0;
	int idx_c = 0;
	int idx_r = 0;
	vector<int> countSort(9), countSort1(9);
	//to identify which mode should be used currently

	const char *intraOrInter, *intraModes, *intraQuan, *interQuan, *interMv;

	intraOrInter = change_names("iOi", idx);
	intraModes = change_names("intra_modes", idx);
	intraQuan = change_names("intraQuan", idx);
	interQuan = change_names("interQuan", idx);
	interMv = change_names("interMV", idx);


	if (idx == 2)
	{
		firstFrame = decode_intra(change_names("intra_modes", idx - 1), change_names("intraQuan", idx - 1));
		save_file(firstFrame, change_names("decoder_res", 1), ROWS, COLS);
	}
	else
	{
		firstFrame = previousPicture;
	}

	unsigned char** interOrIntra = read_file(intraOrInter, ROWS / SIZE, COLS / SIZE);
	intraPred = decode_intra(intraModes, intraQuan);
	interPred = decode_inter(firstFrame, interQuan, interMv);

	//these are just for testing purposes
	int** motionVectors = readFileInt(interMv, 2, ((ROWS * COLS) / (SIZE * SIZE)));
	unsigned char** modeNumbers = read_file(intraModes, ROWS / 4, COLS / 4);
	unsigned char** modeNumbers1 = read_file(change_names("intra_modes", 1), ROWS / 4, COLS / 4);

	for (int r = 0; r < ROWS; r += SIZE)
	{
		for (int c = 0; c < COLS; c += SIZE)
		{
			//to be shown only in the first part
			if (idx == 2)
				for (int i = r / SIZE; i < r / SIZE + 2; i++)
				{
					for (int j = c / SIZE; j < c / SIZE + 2; j++)
					{
						countSort1[static_cast<int>(*(*(modeNumbers1 + i) + j))]++;
					}
				}

			//inter mode
			if (static_cast<int>(*(*(interOrIntra + r / SIZE) + c / SIZE)) == 0)
			{
				for (int i = 0; i < SIZE; i++)
				{
					for (int j = 0; j < SIZE; j++)
					{
						*(*(currentMatrix + i) + j) = *(*(interPred + r + i) + c + j);
					}
				}
				idx_c = *(*(motionVectors) + counter);
				idx_r = *(*(motionVectors + 1) + counter);
				mv_sum += sqrt((c - idx_c) * (c - idx_c) + (r - idx_r) * (r - idx_r));
				counterInter++;
			}
				//intra mode
			else
			{
				for (int i = 0; i < SIZE; i++)
				{
					for (int j = 0; j < SIZE; j++)
					{
						*(*(currentMatrix + i) + j) = *(*(intraPred + r + i) + c + j);
					}
				}
				//mode Numbers
				for (int i = r / SIZE; i < r / SIZE + 2; i++)
				{
					for (int j = c / SIZE; j < c / SIZE + 2; j++)
					{
						countSort[static_cast<int>(*(*(modeNumbers + i) + j))]++;
					}
				}
				counterIntra++;
			}
			counter++;
			//saving the result
			for (int i = 0; i < SIZE; i++)
			{
				for (int j = 0; j < SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(currentMatrix + i) + j);
				}
			}
		}
	}
	save_file(result, name, ROWS, COLS);
	string str_enc_dec = "enc_dec_res";
	string str_decoder = "decoder_res";
	if (idx == 2)
	{
		check_for_mismatch(change_names(str_enc_dec, 1), change_names(str_decoder, 1), ROWS, COLS);
		cout << "Sum of Motion vectors: " << 0
			<< endl << "Number of Inter Blocks: " << 0
			<< endl << "Number of Intra Blocks: " << counter
			<< endl << "num of mode0: " << countSort1[0]
			<< endl << "num of mode1: " << countSort1[1]
			<< endl << "num of mode2: " << countSort1[2]
			<< endl << "num of mode3: " << countSort1[3]
			<< endl << "num of mode4: " << countSort1[4]
			<< endl << "num of mode5: " << countSort1[5]
			<< endl << "num of mode6: " << countSort1[6]
			<< endl << "num of mode7: " << countSort1[7]
			<< endl << "num of mode8: " << countSort1[8] << endl << endl;
	}
	check_for_mismatch(change_names(str_enc_dec, idx), change_names(str_decoder, idx), ROWS, COLS);
	cout << "Sum of Motion vectors: " << mv_sum
		<< endl << "Number of Inter Blocks: " << counterInter
		<< endl << "Number of Intra Blocks: " << counterIntra
		<< endl << "num of mode0: " << countSort[0]
		<< endl << "num of mode1: " << countSort[1]
		<< endl << "num of mode2: " << countSort[2]
		<< endl << "num of mode3: " << countSort[3]
		<< endl << "num of mode4: " << countSort[4]
		<< endl << "num of mode5: " << countSort[5]
		<< endl << "num of mode6: " << countSort[6]
		<< endl << "num of mode7: " << countSort[7]
		<< endl << "num of mode8: " << countSort[8] << endl << endl;
	//save the result
}

void decoder_inter_intra_adaptive(unsigned char** previousPicture, const char* name, int idx)
{
	unsigned char** firstFrame = initializer(ROWS, COLS);
	unsigned char** secondFrame = initializer(ROWS, COLS);
	unsigned char** intraPred = initializer(ROWS, COLS);
	unsigned char** interPred = initializer(ROWS, COLS);
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** currentMatrix = initializer(SIZE, SIZE);
	static vector<unsigned char**> interPredAdaptive;
	int mv_sum = 0;
	int counter = 0, counterInter = 0, counterIntra = 0;
	int idx_c = 0;
	int idx_r = 0;
	vector<int> countSort(9), countSort1(9), countSort2(9);
	//to identify which mode should be used currently

	const char *intraOrInter, *intraModes, *intraQuan, *interQuan, *interMv;

	intraOrInter = change_names("iOi", idx);
	intraModes = change_names("intra_modes", idx);
	intraQuan = change_names("intraQuan", idx);
	interQuan = change_names("interQuan", idx);
	interMv = change_names("interMV", idx);


	if (idx == 2)
	{
		cout << "Current Frame Number is : " << 1 << endl;
		firstFrame = decode_intra(change_names("intra_modes", idx - 1), change_names("intraQuan", idx - 1));
		save_file(firstFrame, change_names("decoder_res", 1), ROWS, COLS);
		interPredAdaptive.push_back(firstFrame);
	}
	else
	{
		cout << "Current Frame Number is : " << idx << endl;
		firstFrame = previousPicture;
		interPredAdaptive.push_back(firstFrame);
	}

	unsigned char** interOrIntra = read_file(intraOrInter, ROWS / SIZE, COLS / SIZE);
	intraPred = decode_intra(intraModes, intraQuan);
	interPred = decode_inter_adaptive(interPredAdaptive, interQuan, interMv, idx);

	//these are just for testing purposes
	int** motionVectors = readFileInt(interMv, 2, ((ROWS * COLS) / (SIZE * SIZE)));
	unsigned char** modeNumbers = read_file(intraModes, ROWS / 4, COLS / 4);
	unsigned char** modeNumbers1 = read_file(change_names("intra_modes", 1), ROWS / 4, COLS / 4);
	unsigned char** prevFmNumbers = read_file(change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);

	for (int r = 0; r < ROWS; r += SIZE)
	{
		for (int c = 0; c < COLS; c += SIZE)
		{
			//to be shown only in the first part
			if (idx == 2)
				for (int i = r / SIZE; i < r / SIZE + 2; i++)
				{
					for (int j = c / SIZE; j < c / SIZE + 2; j++)
					{
						countSort1[static_cast<int>(*(*(modeNumbers1 + i) + j))]++;
					}
				}

			//inter mode
			if (static_cast<int>(*(*(interOrIntra + r / SIZE) + c / SIZE)) == 0)
			{
				countSort2[static_cast<int>(*(*(prevFmNumbers + r / M_SIZE) + c / M_SIZE))]++;
				for (int i = 0; i < SIZE; i++)
				{
					for (int j = 0; j < SIZE; j++)
					{
						*(*(currentMatrix + i) + j) = *(*(interPred + r + i) + c + j);
					}
				}
				idx_c = *(*(motionVectors) + counter);
				idx_r = *(*(motionVectors + 1) + counter);
				mv_sum += sqrt((c - idx_c) * (c - idx_c) + (r - idx_r) * (r - idx_r));
				counterInter++;
			}
				//intra mode
			else
			{
				for (int i = 0; i < SIZE; i++)
				{
					for (int j = 0; j < SIZE; j++)
					{
						*(*(currentMatrix + i) + j) = *(*(intraPred + r + i) + c + j);
					}
				}
				//mode Numbers
				for (int i = r / SIZE; i < r / SIZE + 2; i++)
				{
					for (int j = c / SIZE; j < c / SIZE + 2; j++)
					{
						countSort[static_cast<int>(*(*(modeNumbers + i) + j))]++;
					}
				}
				counterIntra++;
			}
			counter++;
			//saving the result
			for (int i = 0; i < SIZE; i++)
			{
				for (int j = 0; j < SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(currentMatrix + i) + j);
				}
			}
		}
	}
	save_file(result, name, ROWS, COLS);
	string str_enc_dec = "enc_dec_res";
	string str_decoder = "decoder_res";
	if (idx == 2)
	{
		check_for_mismatch(change_names(str_enc_dec, 1), change_names(str_decoder, 1), ROWS, COLS);
		cout << endl << "Number of Inter Blocks: " << 0
			<< endl << "Selected Target Frames: "
			<< endl << "num of frame 1: " << 0
			<< endl << "num of frame 2: " << 0
			<< endl << "num of frame 3: " << 0
			<< endl << "num of frame 4: " << 0
			<< endl << "num of frame 5: " << 0
			<< endl << "num of frame 6: " << 0
			<< endl << "num of frame 7: " << 0
			<< endl << "num of frame 8: " << 0
			<< endl << "num of frame 9: " << 0
			<< endl << "num of frame 10: " << 0 << endl
			<< endl << "Number of Intra Blocks: " << 4096
			<< endl << "num of mode0: " << countSort1[0]
			<< endl << "num of mode1: " << countSort1[1]
			<< endl << "num of mode2: " << countSort1[2]
			<< endl << "num of mode3: " << countSort1[3]
			<< endl << "num of mode4: " << countSort1[4]
			<< endl << "num of mode5: " << countSort1[5]
			<< endl << "num of mode6: " << countSort1[6]
			<< endl << "num of mode7: " << countSort1[7]
			<< endl << "num of mode8: " << countSort1[8] << endl << endl;
	}
	check_for_mismatch(change_names(str_enc_dec, idx), change_names(str_decoder, idx), ROWS, COLS);
	cout << endl << "Number of Inter Blocks: " << counterInter
		<< endl << "Selected Target Frames: "
		<< endl << "num of frame 1: " << countSort2[1]
		<< endl << "num of frame 2: " << countSort2[2]
		<< endl << "num of frame 3: " << countSort2[3]
		<< endl << "num of frame 4: " << countSort2[4]
		<< endl << "num of frame 5: " << countSort2[5]
		<< endl << "num of frame 6: " << countSort2[6]
		<< endl << "num of frame 7: " << countSort2[7]
		<< endl << "num of frame 8: " << countSort2[8]
		<< endl << "num of frame 9: " << countSort2[9]
		<< endl << "num of frame 10: " << 0 << endl
		<< endl << "Number of Intra Blocks: " << counterIntra
		<< endl << "num of mode0: " << countSort[0]
		<< endl << "num of mode1: " << countSort[1]
		<< endl << "num of mode2: " << countSort[2]
		<< endl << "num of mode3: " << countSort[3]
		<< endl << "num of mode4: " << countSort[4]
		<< endl << "num of mode5: " << countSort[5]
		<< endl << "num of mode6: " << countSort[6]
		<< endl << "num of mode7: " << countSort[7]
		<< endl << "num of mode8: " << countSort[8] << endl << endl;
	//save the result
}

void decoder_inter_intra_bi_directional_mv(unsigned char** previousPicture, const char* name, int idx)
{
	unsigned char** first_frame = initializer(ROWS, COLS);
	unsigned char** second_frame = initializer(ROWS, COLS);
	unsigned char** intra_pred = initializer(ROWS, COLS);
	unsigned char** inter_pred = initializer(ROWS, COLS);
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** current_matrix = initializer(SIZE, SIZE);
	static vector<unsigned char**> previous_frames;
	int mv_sum = 0;
	int counter = 0, counter_inter = 0, counter_intra = 0;
	int idx_c = 0;
	int idx_r = 0;
	vector<int> count_sort(9), count_sort1(9), count_sort2(4), count_sort3(4);
	//to identify which mode should be used currently

	const char *intra_or_inter, *intra_modes, *intra_quan, *inter_quan, *inter_mv, *inter_mv_1;

	intra_or_inter = change_names("iOi", idx);
	intra_modes = change_names("intra_modes", idx);
	intra_quan = change_names("intra_quan", idx);
	inter_quan = change_names("inter_quan", idx);
	inter_mv = change_names("inter_mv", idx);
	inter_mv_1 = change_names("inter_mv_bi", idx);


	if (idx == 2)
	{
		cout << "Current Frame Number is : " << 1 << endl;
		first_frame = decode_intra(change_names("intra_modes", 1), change_names("intra_quan", idx - 1));
		save_file(first_frame, change_names("decoder_res", 1), ROWS, COLS);
		previous_frames.push_back(first_frame);
	}
	else
	{
		first_frame = previousPicture;
		previous_frames.push_back(first_frame);
	}
	if (previous_frames.size() > 3) previous_frames.erase(previous_frames.begin());

	unsigned char** inter_or_intra = read_file(intra_or_inter, ROWS / M_SIZE, COLS / M_SIZE);
	intra_pred = decode_intra(intra_modes, intra_quan);
	inter_pred = decode_inter_bi_directional_mv(previous_frames, inter_quan, inter_mv, inter_mv_1, idx);

	//these are just for testing purposes
	int** motion_vectors = readFileInt(inter_mv, 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));
	unsigned char** mode_numbers = read_file(intra_modes, ROWS / 4, COLS / 4);
	unsigned char** mode_numbers1 = read_file(change_names("intra_modes", 1), ROWS / 4, COLS / 4);
	unsigned char** prev_frame_numbers = read_file(change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** prev_frame_numbers_1 = read_file(change_names("prevFrame_bi", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** bi_or_mono = read_file(change_names("bi_or_mono", idx), 1, (ROWS * COLS) / (M_SIZE * M_SIZE));


	for (auto r = 0; r < ROWS; r += M_SIZE)
	{
		for (auto c = 0; c < COLS; c += M_SIZE)
		{
			//to be shown only in the first part
			if (idx == 2)
				for (auto i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (auto j = c / SIZE; j < c / M_SIZE + 2; j++)
					{
						count_sort1[static_cast<int>(*(*(mode_numbers1 + i) + j))]++;
					}
				}

			//inter mode
			if (static_cast<int>(*(*(inter_or_intra + r / M_SIZE) + c / M_SIZE)) == 0)
			{
				//countSort2[static_cast<int>(*(*(prevFmNumbers + r / M_SIZE) + c / M_SIZE))]++;
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						*(*(current_matrix + i) + j) = *(*(inter_pred + r + i) + c + j);
					}
				}
				if (static_cast<int>(*(*(bi_or_mono) + counter)) == 1) //fixed
				{
					count_sort3[static_cast<int>(*(*(prev_frame_numbers + r / M_SIZE) + c / M_SIZE))]++;
				}
				else //averaged MV
				{
					count_sort2[static_cast<int>(*(*(prev_frame_numbers_1 + r / M_SIZE) + c / M_SIZE))]++;
				}
				counter_inter++;
			}
				//intra mode
			else
			{
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						*(*(current_matrix + i) + j) = *(*(intra_pred + r + i) + c + j);
					}
				}
				//mode Numbers
				for (auto i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (auto j = c / M_SIZE; j < c / M_SIZE + 2; j++)
					{
						count_sort[static_cast<int>(*(*(mode_numbers + i) + j))]++;
					}
				}
				counter_intra++;
			}
			counter++;
			//saving the result
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(current_matrix + i) + j);
				}
			}
		}
	}
	save_file(result, name, ROWS, COLS);
	const string str_enc_dec = "enc_dec_res";
	const string str_decoder = "decoder_res";
	if (idx == 2)
	{
		check_for_mismatch(change_names(str_enc_dec, 1), change_names(str_decoder, 1), ROWS, COLS);
		cout << endl << "Number of Inter Blocks: " << 0
			<< endl << "Selected Target Frames: " << "\nFixed MV: "
			<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << 0
			<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << 0
			<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << 0
			<< endl << "BI_Directional MV: "
			<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << 0
			<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << 0
			<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << 0 << endl
			<< endl << "Number of Intra Blocks: " << 4096
			<< endl << "num of mode0: " << count_sort1[0]
			<< endl << "num of mode1: " << count_sort1[1]
			<< endl << "num of mode2: " << count_sort1[2]
			<< endl << "num of mode3: " << count_sort1[3]
			<< endl << "num of mode4: " << count_sort1[4]
			<< endl << "num of mode5: " << count_sort1[5]
			<< endl << "num of mode6: " << count_sort1[6]
			<< endl << "num of mode7: " << count_sort1[7]
			<< endl << "num of mode8: " << count_sort1[8] << endl << endl;
	}
	cout << "Current frame number is: " << idx << endl;
	check_for_mismatch(change_names(str_enc_dec, idx), change_names(str_decoder, idx), ROWS, COLS);
	cout << endl << "Number of Inter Blocks: " << counter_inter
		<< endl << "Selected Target Frames: " << "\nFixed MV: "
		<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << count_sort3[1]
		<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << count_sort3[2]
		<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << count_sort3[3]
		<< endl << "BI_Directional MV: "
		<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << count_sort2[1]
		<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << count_sort2[2]
		<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << count_sort2[3] << endl
		<< endl << "Number of Intra Blocks: " << counter_intra
		<< endl << "num of mode0: " << count_sort[0]
		<< endl << "num of mode1: " << count_sort[1]
		<< endl << "num of mode2: " << count_sort[2]
		<< endl << "num of mode3: " << count_sort[3]
		<< endl << "num of mode4: " << count_sort[4]
		<< endl << "num of mode5: " << count_sort[5]
		<< endl << "num of mode6: " << count_sort[6]
		<< endl << "num of mode7: " << count_sort[7]
		<< endl << "num of mode8: " << count_sort[8] << endl << endl;
	//save the result
}

void decoder_inter_intra_predictive(unsigned char** previousPicture, const char* name, int idx)
{
	unsigned char** first_frame = initializer(ROWS, COLS);
	unsigned char** second_frame = initializer(ROWS, COLS);
	unsigned char** intra_pred = initializer(ROWS, COLS);
	unsigned char** inter_pred = initializer(ROWS, COLS);
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** current_matrix = initializer(SIZE, SIZE);
	static vector<unsigned char**> previous_frames;
	int mv_sum = 0;
	int counter = 0, counter_inter = 0, counter_intra = 0;
	int idx_c = 0;
	int idx_r = 0;
	vector<int> count_sort(9), count_sort1(9), count_sort2(4), count_sort3(4);
	//to identify which mode should be used currently

	const char* intra_or_inter, * intra_modes, * intra_quan, * inter_quan, * inter_mv, * inter_mv_1;

	intra_or_inter = change_names("iOi", idx);
	intra_modes = change_names("intra_modes", idx);
	intra_quan = change_names("intra_quan", idx);
	inter_quan = change_names("inter_quan", idx);
	inter_mv = change_names("inter_mv", idx);
	inter_mv_1 = change_names("inter_mv_bi", idx);


	if (idx == 2)
	{
		cout << "Current Frame Number is : " << 1 << endl;
		first_frame = decode_intra(change_names("intra_modes", 1), change_names("intra_quan", idx - 1));
		save_file(first_frame, change_names("decoder_res", 1), ROWS, COLS);
		previous_frames.push_back(first_frame);
	}
	else
	{
		first_frame = previousPicture;
		previous_frames.push_back(first_frame);
	}
	if (previous_frames.size() > 3) previous_frames.erase(previous_frames.begin());

	unsigned char** inter_or_intra = read_file(intra_or_inter, ROWS / M_SIZE, COLS / M_SIZE);
	intra_pred = decode_intra(intra_modes, intra_quan);
	inter_pred = decode_inter_predictive(previous_frames, inter_quan, idx);

	//these are just for testing purposes
	int** motion_vectors = readFileInt(inter_mv, 2, ((ROWS * COLS) / (M_SIZE * M_SIZE)));
	unsigned char** mode_numbers = read_file(intra_modes, ROWS / 4, COLS / 4);
	unsigned char** mode_numbers1 = read_file(change_names("intra_modes", 1), ROWS / 4, COLS / 4);
	unsigned char** prev_frame_numbers = read_file(change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** prev_frame_numbers_1 = read_file(change_names("prevFrame_bi", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** bi_or_mono = read_file(change_names("bi_or_mono", idx), 1, (ROWS * COLS) / (M_SIZE * M_SIZE));


	for (auto r = 0; r < ROWS; r += M_SIZE)
	{
		for (auto c = 0; c < COLS; c += M_SIZE)
		{
			//to be shown only in the first part
			if (idx == 2)
				for (auto i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (auto j = c / SIZE; j < c / M_SIZE + 2; j++)
					{
						count_sort1[static_cast<int>(*(*(mode_numbers1 + i) + j))]++;
					}
				}

			//inter mode
			if (static_cast<int>(*(*(inter_or_intra + r / M_SIZE) + c / M_SIZE)) == 0)
			{
				//countSort2[static_cast<int>(*(*(prevFmNumbers + r / M_SIZE) + c / M_SIZE))]++;
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						*(*(current_matrix + i) + j) = *(*(inter_pred + r + i) + c + j);
					}
				}
				if (static_cast<int>(*(*(bi_or_mono)+counter)) == 1) //fixed
				{
					count_sort3[static_cast<int>(*(*(prev_frame_numbers + r / M_SIZE) + c / M_SIZE))]++;
				}
				else //averaged MV
				{
					count_sort2[static_cast<int>(*(*(prev_frame_numbers_1 + r / M_SIZE) + c / M_SIZE))]++;
				}
				counter_inter++;
			}
			//intra mode
			else
			{
				for (auto i = 0; i < M_SIZE; i++)
				{
					for (auto j = 0; j < M_SIZE; j++)
					{
						*(*(current_matrix + i) + j) = *(*(intra_pred + r + i) + c + j);
					}
				}
				//mode Numbers
				for (auto i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (auto j = c / M_SIZE; j < c / M_SIZE + 2; j++)
					{
						count_sort[static_cast<int>(*(*(mode_numbers + i) + j))]++;
					}
				}
				counter_intra++;
			}
			counter++;
			//saving the result
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(current_matrix + i) + j);
				}
			}
		}
	}
	save_file(result, name, ROWS, COLS);
	const string str_enc_dec = "enc_dec_res";
	const string str_decoder = "decoder_res";
	if (idx == 2)
	{
		check_for_mismatch(change_names(str_enc_dec, 1), change_names(str_decoder, 1), ROWS, COLS);
		cout << endl << "Number of Inter Blocks: " << 0
			<< endl << "Selected Target Frames: " << "\nFixed MV: "
			<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << 0
			<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << 0
			<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << 0
			<< endl << "BI_Directional MV: "
			<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << 0
			<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << 0
			<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << 0 << endl
			<< endl << "Number of Intra Blocks: " << 4096
			<< endl << "num of mode0: " << count_sort1[0]
			<< endl << "num of mode1: " << count_sort1[1]
			<< endl << "num of mode2: " << count_sort1[2]
			<< endl << "num of mode3: " << count_sort1[3]
			<< endl << "num of mode4: " << count_sort1[4]
			<< endl << "num of mode5: " << count_sort1[5]
			<< endl << "num of mode6: " << count_sort1[6]
			<< endl << "num of mode7: " << count_sort1[7]
			<< endl << "num of mode8: " << count_sort1[8] << endl << endl;
	}
	cout << "Current frame number is: " << idx << endl;
	check_for_mismatch(change_names(str_enc_dec, idx), change_names(str_decoder, idx), ROWS, COLS);
	cout << endl << "Number of Inter Blocks: " << counter_inter
		<< endl << "Selected Target Frames: " << "\nFixed MV: "
		<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << count_sort3[1]
		<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << count_sort3[2]
		<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << count_sort3[3]
		<< endl << "BI_Directional MV: "
		<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << count_sort2[1]
		<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << count_sort2[2]
		<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << count_sort2[3] << endl
		<< endl << "Number of Intra Blocks: " << counter_intra
		<< endl << "num of mode0: " << count_sort[0]
		<< endl << "num of mode1: " << count_sort[1]
		<< endl << "num of mode2: " << count_sort[2]
		<< endl << "num of mode3: " << count_sort[3]
		<< endl << "num of mode4: " << count_sort[4]
		<< endl << "num of mode5: " << count_sort[5]
		<< endl << "num of mode6: " << count_sort[6]
		<< endl << "num of mode7: " << count_sort[7]
		<< endl << "num of mode8: " << count_sort[8] << endl << endl;
	//save the result
}
