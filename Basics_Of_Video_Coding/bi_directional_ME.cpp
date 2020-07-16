#include "inter_intra.h"


unsigned char** bi_directional_me(unsigned char** previous_image, unsigned char** current_image, int idx,
                                  const char* encoder_name)
{
	unsigned char** result = initializer(ROWS, COLS);
	unsigned char** first_frame = initializer(ROWS, COLS);
	unsigned char** second_frame = initializer(ROWS, COLS);
	unsigned char** intra_predicted = initializer(ROWS, COLS);
	unsigned char** inter_predicted = initializer(ROWS, COLS);
	unsigned char** inter_or_intra = initializer(ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** intra_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** inter_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** current_matrix = initializer(M_SIZE, M_SIZE);
	unsigned char** res_matrix = initializer(M_SIZE, M_SIZE);
	int** intra_residual = initializerI(M_SIZE, M_SIZE);

	static vector<unsigned char**> previous_frames;
	vector<unsigned char**> temp_matrices;
	vector<int> count_sort(10), count_sort1(10),
	            count_sort2(4), count_sort3(4);
	int sum_intra = 0, sum_inter = 0;
	int counter0 = 0, counter1 = 0;
	int counterTmp = 0;
	int min = numeric_limits<int>::max();
	second_frame = current_image;

	const char* inter_or_intra_name = change_names("iOi", idx);
	const char* intra_frame_name = change_names("intra_encoded", idx);
	const char* intra_modes_name = change_names("intra_modes", idx);
	const char* intra_quan_name = change_names("intra_quan", idx);
	const char* inter_frame_name = change_names("inter_encoded", idx);
	const char* inter_mv_name = change_names("inter_mv", idx);
	const char* inter_mv_name_1 = change_names("inter_mv_bi", idx);
	const char* inter_quan_name = change_names("inter_quan", idx);

	if (idx == 2)
	{
		cout << "Current frame number is: " << 1 << endl;
		first_frame = apply_intra(previous_image, change_names("intra_encoded", 1),
		                          change_names("intra_modes", 1), change_names("intra_quan", 1));
		save_file(first_frame, change_names("enc_dec_res", 1), ROWS, COLS);
		previous_frames.push_back(first_frame);
	}
	else
	{
		first_frame = previous_image;
		previous_frames.push_back(first_frame);
	}


	//choose the last 3 frames and work on them
	if (previous_frames.size() > 3) previous_frames.erase(previous_frames.begin());

	intra_predicted = apply_intra(current_image, intra_frame_name, intra_modes_name, intra_quan_name);
	inter_predicted = apply_inter_bi_directional(SEARCH_SIZE, previous_frames, second_frame, inter_frame_name,
	                                             inter_quan_name, inter_mv_name, inter_mv_name_1, idx);

	unsigned char** m_numbers = read_file(intra_modes_name, ROWS / 4, COLS / 4);
	unsigned char** m_numbers1 = read_file(change_names("intra_modes", 1), ROWS / 4, COLS / 4);
	unsigned char** prev_frame_numbers = read_file(change_names("prevFrame", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** prev_frame_numbers_1 = read_file(change_names("prevFrame_bi", idx), ROWS / M_SIZE, COLS / M_SIZE);
	unsigned char** bi_or_mono = read_file(change_names("bi_or_mono", idx), 1, (ROWS * COLS) / (M_SIZE * M_SIZE));

	for (auto r = 0; r < ROWS; r += M_SIZE)
	{
		for (auto c = 0; c < COLS; c += M_SIZE)
		{
			//----------------------------------
			if (idx == 2)
			{
				for (auto i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (auto j = c / M_SIZE; j < c / M_SIZE + 2; j++)
					{
						count_sort1[static_cast<int>(*(*(m_numbers1 + i) + j))]++;
					}
				}
			}
			//----------------------------------
			//current matrices
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(current_matrix + i) + j) = *(*(current_image + r + i) + c + j);
					*(*(intra_matrix + i) + j) = *(*(intra_predicted + r + i) + c + j);
					*(*(inter_matrix + i) + j) = *(*(inter_predicted + r + i) + c + j);
				}
			}
			//residuals of both modes with the current matrix
			intra_residual = difference_blocks_in(current_matrix, intra_matrix);

			sum_inter = SAD(current_matrix, inter_matrix);
			sum_intra = SADint(intra_residual, current_matrix);

			if (sum_inter < sum_intra)
			{
				*(*(inter_or_intra + r / M_SIZE) + c / M_SIZE) = 0;
				res_matrix = inter_matrix;

				if (static_cast<int>(*(*(bi_or_mono) + counterTmp)) == 1) //fixed
				{
					count_sort3[static_cast<int>(*(*(prev_frame_numbers + r / M_SIZE) + c / M_SIZE))]++;
				}
				else //averaged MV
				{
					count_sort2[static_cast<int>(*(*(prev_frame_numbers_1 + r / M_SIZE) + c / M_SIZE))]++;
				}
				counter0++;
			}
			else
			{
				*(*(inter_or_intra + r / M_SIZE) + c / M_SIZE) = 1;
				res_matrix = intra_matrix;
				//------------------------
				for (auto i = r / M_SIZE; i < r / M_SIZE + 2; i++)
				{
					for (auto j = c / M_SIZE; j < c / M_SIZE + 2; j++)
					{
						count_sort[static_cast<int>(*(*(m_numbers + i) + j))]++;
					}
				}
				//------------------------
				counter1++;
			}
			for (auto i = 0; i < M_SIZE; i++)
			{
				for (auto j = 0; j < M_SIZE; j++)
				{
					*(*(result + r + i) + c + j) = *(*(res_matrix + i) + j);
					*(*(current_image + r + i) + c + j) = *(*(res_matrix + i) + j);
				}
			}
			counterTmp++;
		}
	}
	string str1 = "enc_dec_res";
	string str2 = "testPics/";
	save_file(result, change_names(str1, idx), ROWS, COLS);
	save_file(inter_or_intra, inter_or_intra_name, ROWS / M_SIZE, COLS / M_SIZE);
	if (idx == 2)
	{
		get_psnr(change_names(str1, 1), change_names(str2, 1), ROWS, COLS);
		cout << endl << "Number of Inter Blocks: " << 0
			<< endl << "Selected Target Frames: " << "\nFixed MV: "
			<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << 0
			<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << 0
			<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << 0
			<< endl << "BI_Directional MV: "
			<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << 0
			<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << 0
			<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << 0 << endl
			<< endl << "Number of Intra Blocks: " << counterTmp
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
	get_psnr(change_names(str1, idx), change_names(str2, idx), ROWS, COLS);
	cout << endl << "Number of Inter Blocks: " << counter0
		<< endl << "Selected Target Frames: " << "\nFixed MV: "
		<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << count_sort3[1]
		<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << count_sort3[2]
		<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << count_sort3[3]
		<< endl << "BI_Directional MV: "
		<< endl << "num of frame " << ((idx - 3) > 0 ? (idx - 3) : 1) << ": " << count_sort2[1]
		<< endl << "num of frame " << ((idx - 2) > 1 ? (idx - 2) : 2) << ": " << count_sort2[2]
		<< endl << "num of frame " << ((idx - 1) > 2 ? (idx - 1) : 3) << ": " << count_sort2[3] << endl
		<< endl << "Number of Intra Blocks: " << counter1
		<< endl << "num of mode0: " << count_sort[0]
		<< endl << "num of mode1: " << count_sort[1]
		<< endl << "num of mode2: " << count_sort[2]
		<< endl << "num of mode3: " << count_sort[3]
		<< endl << "num of mode4: " << count_sort[4]
		<< endl << "num of mode5: " << count_sort[5]
		<< endl << "num of mode6: " << count_sort[6]
		<< endl << "num of mode7: " << count_sort[7]
		<< endl << "num of mode8: " << count_sort[8] << endl << endl;
	return result;
}
