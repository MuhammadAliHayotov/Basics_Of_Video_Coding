#include "inter_intra.h"
#define ENCODER 1


auto main() -> int
{
	string str = "enc_dec_res";

#if ENCODER
	const char* first_frame_name;
	const char* enc_dec_name = change_names(str, 1);

	for (auto i = 2; i <= 10; i++)
	{
		if (i == 2)
		{
			str = "testPics/";
			first_frame_name = change_names(str, i - 1);
		}
		else
			first_frame_name = enc_dec_name;
		const char* second_frame_name = change_names("testPics/", i);

		unsigned char** first_frame = read_file(first_frame_name, ROWS, COLS);
		unsigned char** second_frame = read_file(second_frame_name, ROWS, COLS);
		enc_dec_name = change_names("enc_dec_res", i);
		predictive_ME(first_frame, second_frame, i, enc_dec_name);
	}
#else
	str = "decoder_res";
	unsigned char **previous_frame;
	
	//decoderName = changeNames(str, 1);
	for (auto i = 2; i <= 10; i++) {
		//then all intra will have the previous frame decoded inside the decoder
		if (i == 2) {
			previous_frame = initializer(ROWS, COLS);
		}
		else {
			previous_frame = read_file(change_names(str, i-1), ROWS, COLS);
		}
		const char* decoder_name = change_names(str, i);
		decoder_inter_intra_predictive(previous_frame, decoder_name, i);
	}
	
#endif

	return 0;
}
