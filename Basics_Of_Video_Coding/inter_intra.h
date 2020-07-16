#pragma once

#include <iostream>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

//pi value
constexpr auto pi = 3.14159265359;

//counters for modes
static int index0, index1, index2, index3,
           index4, index5, index6, index7, index8;

#define ROWS		512
#define COLS		512
#define M_SIZE		 8
#define SEARCH_SIZE	 4
#define INTRA_ROWS	 4
#define INTRA_COLS	 4


struct Data_Save
{
	vector<int> idx_c;
	vector<int> idx_r;
	vector<char*> intra_modes;
	vector<unsigned char> inter_or_intra;
};

//dct and idct
float** dct_transform(int** matrix, int rows, int cols);
int** idct_transform(int** matrix, int rows, int cols);
float** normalized_dct(float** matrix, int rows, int cols);

//inter or intra
unsigned char** inter_intra_enc_dec(int search_size, unsigned char** previous_image, unsigned char** current_image,
                                    int idx);


//adaptive target image selection
unsigned char** adaptive_target(int search_size, unsigned char** previous_image, unsigned char** current_image, int idx,
                                const char* encoder_name);

//bi directional motion estimation
unsigned char** bi_directional_me(unsigned char** previous_image, unsigned char** current_image, int idx,
                                  const char* encoder_name);
unsigned char** predictive_ME(unsigned char** previous_image, unsigned char** current_image, int idx,
                              const char* encoder_name);


//inter
int start_row(int r, int search_size);
int start_col(int c, int search_size);
int end_row(int r, int search_size, int rows);
int end_col(int c, int search_size, int cols);
unsigned char** apply_inter(int search_size, unsigned char** previous_image, unsigned char** current_image,
                            const char* name, const char* quan_name, const char* mv_name);
unsigned char** apply_inter_adaptive(int search_size, vector<unsigned char**> previous_images,
                                     unsigned char** current_image,
                                     const char* name, const char* quan_name, const char* mv_name, int idx);
unsigned char** apply_inter_bi_directional(int search_size, vector<unsigned char**> previous_images,
                                           unsigned char** current_image,
                                           const char* name, const char* quan_name, const char* mv_name,
                                           const char* mv_name_1, int idx);
unsigned char** apply_inter_predictive(int search_size, vector<unsigned char**> previous_images,
                                       unsigned char** current_image,
                                       const char* name, const char* quan_name, const char* mv_name,
                                       const char* mv_name_1, int idx);


int SAD(unsigned char** current_matrix, unsigned char** comparing_matrix);
int SUM(int** matrix);
int** difference_blocks_in(unsigned char** current_matrix, unsigned char** min_matrix);
int** quantize_in(int** difference_matrix, float ql, int rs, int cs);
int** dequantize_in(int** quantized_matrix, float ql, int rs, int cs);
unsigned char** reconstructed_block_in(int** dequantized_matrix, unsigned char** min_matrix, int rs, int cs);

//intra

unsigned char** mode0(unsigned char* up_matrix, unsigned char* side_matrix);
unsigned char** mode1(unsigned char* up_matrix, unsigned char* side_matrix);
unsigned char** mode2(unsigned char* up_matrix, unsigned char* side_matrix);
unsigned char** mode3(unsigned char* up_matrix, unsigned char* side_matrix);
unsigned char** mode4(unsigned char* up_matrix, unsigned char* side_matrix, int q);
unsigned char** mode5(unsigned char* up_matrix, unsigned char* side_matrix);
unsigned char** mode6(unsigned char* up_matrix, unsigned char* side_matrix, int q);
unsigned char** mode7(unsigned char* up_matrix, unsigned char* side_matrix, int q);
unsigned char** mode8(unsigned char* up_matrix, unsigned char* side_matrix, int q);
unsigned char** predictionBlock(unsigned char** matrix, unsigned char* up_matrix, unsigned char* side_matrix, int q);
int ssd(unsigned char** matrix, unsigned char** predicted_matrix);
int** difference_block(unsigned char** matrix, unsigned char** predicted_matrix);
unsigned char** reconstructed_block(unsigned char** dequantized_matrix, unsigned char** predicted_matrix);
void save_into_file(char*, const char*, int);
void save_data(unsigned char**, const char*);
unsigned char** apply_mode(int, unsigned char*, unsigned char*, int);
int SADint(int**, unsigned char**);
unsigned char** apply_intra(unsigned char** input_image, const char* name, const char* modes_name,
                            const char* quan_name);


//side functions
int** initializerI(int rows, int cols);
float** initializerF(int rows, int cols);
unsigned char** initializer(int rows, int cols);
unsigned char** read_file(const char* name, int rows, int cols);
void save_file(unsigned char** image, const char* name, int rows, int cols);
unsigned char* read_file(const char* name, int size);
void save_file(unsigned char* image, const char* name, int size);
const char* change_names(string name, int idx);
void check_for_mismatch(const char* img1, const char* img2, int rows, int cols);
void get_psnr(const char* img1, const char* img2, int rows, int cols);
int** non_uniform_quantization1(float** dct_matrix, int size, int boundary);
int** non_uniform_dequantization1(int** quantized_matrix, int size, int boundary);
int** non_uniform_quantization2(float** dct_matrix, int size);
int** non_uniform_dequantization2(int** quantized_matrix, int size);
void save_file_float(float** image, const char* name, int rows, int cols);
float** read_file_float(const char* name, int rows, int cols);

int** readFileInt(const char* name, int rows, int cols);
void saveFileInt(int** image, const char* name, int rows, int cols);
unsigned char* get_up_matrix(unsigned char** result, int r, int c);
unsigned char* get_side_matrix(unsigned char** result, int r, int c);
char* readFileC(const char* name, int size);
void saveIntoFileI(char* savingMat, const char* name, int size);
int get_q(unsigned char** image, int r, int c);

//decoder functions
unsigned char** decode_inter(unsigned char** previousFrame, const char* quanName, const char* mvName);
unsigned char** decode_inter_adaptive(vector<unsigned char**> previousFrames, const char* quanName, const char* mvName,
                                      int idx);
unsigned char** decode_inter_bi_directional_mv(vector<unsigned char**> previous_frames, const char* quan_name,
                                               const char* mv_name, const char* mv_name_1, int idx);
unsigned char** decode_inter_predictive(vector<unsigned char**> previous_frames, const char* quan_name, int idx);

unsigned char** decode_intra(const char* modeName, const char* quanName);
void decoder_inter_intra(unsigned char** previousPicture, const char* name, int idx);
void decoder_inter_intra_adaptive(unsigned char** previousPicture, const char* name, int idx);
void decoder_inter_intra_bi_directional_mv(unsigned char** previousPicture, const char* name, int idx);
void decoder_inter_intra_predictive(unsigned char** previousPicture, const char* name, int idx);
