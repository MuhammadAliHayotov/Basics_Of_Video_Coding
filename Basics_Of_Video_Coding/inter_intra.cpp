#include "inter_intra.h"

int mv_sum_ = 0;
static int counter0 = 0, counter1 = 0;


int SUM(unsigned char** matrix)
{
	int sum = 0;

	for (int i = 0; i < M_SIZE; i++)
	{
		for (int j = 0; j < M_SIZE; j++)
		{
			sum += *(*(matrix + i) + j);
		}
	}

	return sum;
}

int SUM(int** matrix)
{
	int sum = 0;

	for (int i = 0; i < M_SIZE; i++)
	{
		for (int j = 0; j < M_SIZE; j++)
		{
			sum += *(*(matrix + i) + j);
		}
	}

	return sum;
}

unsigned char** inter_intra_enc_dec(int search_size, unsigned char** previous_image, unsigned char** current_image,
                                    int idx)
{
	unsigned char** firstFrame = initializer(ROWS, COLS);
	unsigned char** secondFrame = initializer(ROWS, COLS);
	unsigned char** intraPred = initializer(ROWS, COLS);
	unsigned char** interPred = initializer(ROWS, COLS);
	unsigned char** interOrIntra = initializer(ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** intraMatrix = initializer(M_SIZE, M_SIZE);
	unsigned char** interMatrix = initializer(M_SIZE, M_SIZE);
	unsigned char** currentMatrix = initializer(M_SIZE, M_SIZE);
	unsigned char** resMatrix = initializer(M_SIZE, M_SIZE);
	unsigned char** result = initializer(ROWS, COLS);
	int** interResidual = initializerI(M_SIZE, M_SIZE);
	int** intraResidual = initializerI(M_SIZE, M_SIZE);
	vector<int> countSort(9), countSort1(9);
	int sumIntra = 0, sumInter = 0;
	int counter0 = 0, counter1 = 0;
	int counterTmp = 0, mv_sum = 0;
	secondFrame = current_image;
	//the other one save 
	const char *interOrIntraName, *intraFmName, *intraModesName, *intraQuanName, *interFmName,
	           *interMvName, *interQuanName;

	interOrIntraName = change_names("iOi", idx);
	intraFmName = change_names("intra_encoded", idx);
	intraModesName = change_names("intra_modes", idx);
	intraQuanName = change_names("intraQuan", idx);
	interFmName = change_names("inter_encoded", idx);
	interMvName = change_names("interMV", idx);
	interQuanName = change_names("interQuan", idx);

	if (idx == 2)
	{
		firstFrame = apply_intra(previous_image, change_names("intra_encoded", idx - 1),
		                         change_names("intra_modes", idx - 1), change_names("intraQuan", idx - 1));
		save_file(firstFrame, change_names("enc_dec_res", 1), ROWS, COLS);
	}
	else
	{
		firstFrame = previous_image;
	}

	intraPred = apply_intra(current_image, intraFmName, intraModesName, intraQuanName);
	interPred = apply_inter(search_size, firstFrame, secondFrame, interFmName, interQuanName, interMvName);

	//for demonstration purposes only----start
	unsigned char** mNumbers = read_file(intraModesName, ROWS / 4, COLS / 4);
	unsigned char** mNumbers1 = read_file(change_names("intra_modes", 1), ROWS / 4, COLS / 4);
	int** mv = readFileInt(interMvName, 2, (ROWS * COLS) / (M_SIZE * M_SIZE));
	//for demonstration purposes only----end

	for (int r = 0; r < ROWS; r += M_SIZE)
	{
		for (int c = 0; c < COLS; c += M_SIZE)
		{
			//------------------------------
			if (idx == 2)
			{
				for (int i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (int j = c / M_SIZE; j < c / M_SIZE + 2; j++)
					{
						countSort1[static_cast<int>(*(*(mNumbers1 + i) + j))]++;
					}
				}
			}
			//------------------------------
			//current matrices
			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(currentMatrix + i) + j) = *(*(current_image + r + i) + c + j);
					*(*(intraMatrix + i) + j) = *(*(intraPred + r + i) + c + j);
					*(*(interMatrix + i) + j) = *(*(interPred + r + i) + c + j);
				}
			}

			//residuals of both modes with the current matrix
			intraResidual = difference_blocks_in(currentMatrix, intraMatrix);
			interResidual = difference_blocks_in(currentMatrix, interMatrix);

			sumInter = SAD(currentMatrix, interMatrix);
			sumIntra = SADint(intraResidual, currentMatrix);

			if (sumInter < sumIntra)
			{
				//inter Mode
				*(*(interOrIntra + r / M_SIZE) + c / M_SIZE) = 0;
				resMatrix = interMatrix;
				//-------------------------------
				int idx_c = *(*(mv) + counterTmp);
				int idx_r = *(*(mv + 1) + counterTmp);
				mv_sum += sqrt((c - idx_c) * (c - idx_c) + (r - idx_r) * (r - idx_r));
				//-------------------------------
				counter0++;
			}
			else
			{
				//intra mode
				*(*(interOrIntra + r / M_SIZE) + c / M_SIZE) = 1;
				resMatrix = intraMatrix;
				//------------------------------
				for (int i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (int j = c / M_SIZE; j < c / M_SIZE + 2; j++)
					{
						countSort[static_cast<int>(*(*(mNumbers + i) + j))]++;
					}
				}
				//------------------------------
				counter1++;
			}

			for (int i = 0; i < M_SIZE; i++)
			{
				for (int j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(resMatrix + i) + j);
					*(*(current_image + r + i) + c + j) = *(*(resMatrix + i) + j);
				}
			}
			counterTmp++;
		}
	}
	//demonstration
	string str1 = "enc_dec_res";
	string str2 = "testPics/";
	//string str_decoder = "decoder_res";
	if (idx == 2)
	{
		//	checkForMismatch(changeNames(str_enc_dec, idx), changeNames(str_decoder, idx));
		get_psnr(change_names(str1, 1), change_names(str2, 1), ROWS, COLS);
		cout << "Sum of Motion vectors: " << 0
			<< endl << "Number of Inter Blocks: " << 0
			<< endl << "Number of Intra Blocks: " << counterTmp
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
	//checkForMismatch(changeNames(str_enc_dec, idx), changeNames(str_decoder, idx));
	get_psnr(change_names(str1, idx), change_names(str2, idx), ROWS, COLS);
	cout << "Sum of Motion vectors: " << mv_sum
		<< endl << "Number of Inter Blocks: " << counter0
		<< endl << "Number of Intra Blocks: " << counter1
		<< endl << "num of mode0: " << countSort[0]
		<< endl << "num of mode1: " << countSort[1]
		<< endl << "num of mode2: " << countSort[2]
		<< endl << "num of mode3: " << countSort[3]
		<< endl << "num of mode4: " << countSort[4]
		<< endl << "num of mode5: " << countSort[5]
		<< endl << "num of mode6: " << countSort[6]
		<< endl << "num of mode7: " << countSort[7]
		<< endl << "num of mode8: " << countSort[8] << endl << endl;


	save_file(interOrIntra, interOrIntraName, ROWS / M_SIZE, COLS / M_SIZE);

	return result;
}
