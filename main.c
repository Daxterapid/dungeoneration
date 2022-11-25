#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define DATASET_SIZE 100000

uint8_t rows[128] = {
	0,   0,   0,   0,   0,   0,   0,   0,
	255, 129, 128, 128, 128, 128, 129, 255,
	255, 129, 129, 129, 129, 129, 129, 195,
	255, 129, 128, 128, 128, 128, 129, 195,
	255, 129, 1,   1,   1,   1,   129, 255,
	255, 129, 0,   0,   0,   0,   129, 255,
	255, 129, 1,   1,   1,   1,   129, 195,
	255, 129, 0,   0,   0,   0,   129, 195,
	195, 129, 129, 129, 129, 129, 129, 255,
	195, 129, 128, 128, 128, 128, 129, 255,
	195, 129, 129, 129, 129, 129, 129, 195,
	195, 129, 128, 128, 128, 128, 129, 195,
	195, 129, 1,   1,   1,   1,   129, 255,
	195, 129, 0,   0,   0,   0,   129, 255,
	195, 129, 1,   1,   1,   1,   129, 195,
	195, 129, 0,   0,   0,   0,   129, 195,
};

void
print_board(uint8_t *board)
{
	char string[8193];

	memset(string, ' ', 8192);
	string[8192] = '\0';

	char chars[2] = { ' ', '#' };

	for (uint32_t i = 0; i < 64; i++) {
		for (uint32_t j = 0; j < 8; j++) {
			uint8_t id = board[i / 8 * 8 + j];

			for (uint32_t k = 0; k < 8; k++) {
				uint8_t bit = rows[id * 8 + i % 8] >> (7 - k) & 1;
				string[i * 128 + j * 16 + k * 2] = chars[bit];
			}
		}

		string[128 * i + 127] = '\n';
	}

	printf("%s\n", string);
}

bool
has_dupe(uint8_t *array, uint8_t length, uint8_t value)
{
	for (uint8_t i = 0; i < length; i++) {
		if (array[i] == value) return true;
	}

	return false;
}

void
grow_board(uint8_t *board, uint8_t *room_positions, uint8_t n_positions,
		uint8_t **out_room_positions, uint8_t *n_out_positions)
{
	const uint16_t e_mask = 0b1010101010101010;
	const uint16_t s_mask = 0b1100110011001100;
	const uint16_t w_mask = 0b1111000011110000;
	const uint16_t n_mask = 0b1111111100000000;

	for (uint32_t i = 0; i < n_positions; i++) {
		uint8_t x = room_positions[i] % 8;
		uint8_t y = room_positions[i] / 8;

		bool r_empty = x != 7 && board[y * 8 + x + 1] == 0;
		bool d_empty = y != 7 && board[y * 8 + 8 + x] == 0;
		bool l_empty = x != 0 && board[y * 8 + x - 1] == 0;
		bool u_empty = y != 0 && board[y * 8 - 8 + x] == 0;

		bool r_open = x != 7 && (board[y * 8 + x + 1] >> 2 & 1) == 1;
		bool d_open = y != 7 && (board[y * 8 + 8 + x] >> 3 & 1) == 1;
		bool l_open = x != 0 && (board[y * 8 + x - 1] >> 0 & 1) == 1;
		bool u_open = y != 0 && (board[y * 8 - 8 + x] >> 1 & 1) == 1;

		uint16_t mask = 0;
		if (!r_empty && r_open) mask |= ~e_mask;
		if (!r_empty && !r_open) mask |= e_mask;
		if (!d_empty && d_open) mask |= ~s_mask;
		if (!d_empty && !d_open) mask |= s_mask;
		if (!l_empty && l_open) mask |= ~w_mask;
		if (!l_empty && !l_open) mask |= w_mask;
		if (!u_empty && u_open) mask |= ~n_mask;
		if (!u_empty && !u_open) mask |= n_mask;

		uint8_t id_choices[15];
		uint8_t n_choices = 0;
		for (uint8_t i = 1; i < 16; i++) {
			if ((mask >> i & 1) == 0) {
#ifdef USE_WORSE_GENERATION
				id_choices[n_choices++] = i;
#else
				if (r_empty || d_empty || l_empty || u_empty) {
					bool e_open = i >> 0 & 1;
					bool s_open = i >> 1 & 1;
					bool w_open = i >> 2 & 1;
					bool n_open = i >> 3 & 1;

					if ((r_empty && e_open) || (d_empty && s_open) ||
							(l_empty && w_open) || (u_empty && n_open)) {
						id_choices[n_choices++] = i;
					}
				} else {
					id_choices[n_choices++] = i;
				}
#endif
			}
		}

		uint8_t id = id_choices[rand() % n_choices];
		board[room_positions[i]] = id;

		bool e_open = id >> 0 & 1;
		bool s_open = id >> 1 & 1;
		bool w_open = id >> 2 & 1;
		bool n_open = id >> 3 & 1;

		if (e_open && r_empty) {
			uint8_t position = y * 8 + x + 1;
			if (!has_dupe(*out_room_positions, *n_out_positions, position)) {
				*out_room_positions = realloc(*out_room_positions, *n_out_positions + 1);
				(*out_room_positions)[(*n_out_positions)++] = position;
			}
		}

		if (s_open && d_empty) {
			uint8_t position = y * 8 + 8 + x;
			if (!has_dupe(*out_room_positions, *n_out_positions, position)) {
				*out_room_positions = realloc(*out_room_positions, *n_out_positions + 1);
				(*out_room_positions)[(*n_out_positions)++] = position;
			}
		}

		if (w_open && l_empty) {
			uint8_t position = y * 8 + x - 1;
			if (!has_dupe(*out_room_positions, *n_out_positions, position)) {
				*out_room_positions = realloc(*out_room_positions, *n_out_positions + 1);
				(*out_room_positions)[(*n_out_positions)++] = position;
			}
		}

		if (n_open && u_empty) {
			uint8_t position = y * 8 - 8 + x;
			if (!has_dupe(*out_room_positions, *n_out_positions, position)) {
				*out_room_positions = realloc(*out_room_positions, *n_out_positions + 1);
				(*out_room_positions)[(*n_out_positions)++] = position;
			}
		}
	}
}

uint32_t
generate_board(uint8_t *board)
{
	uint8_t *room_positions = malloc(1);
	room_positions[0] = 36;
	uint8_t n_positions = 1;

	uint32_t total = 1;

	for (uint32_t i = 0; i < 64; i++) {
		uint8_t *next_room_positions = NULL;
		uint8_t n_next_positions = 0;

		grow_board(board, room_positions, n_positions,
			&next_room_positions, &n_next_positions);

		total += n_next_positions;

		free(room_positions);

		if (n_next_positions == 0) {
			return total;
		}

		room_positions = next_room_positions;
		n_positions = n_next_positions;
	}
}

int
main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "stat") == 0) {
		srand(time(NULL));

		uint32_t bars[65] = { 0 };

		for (uint32_t i = 0; i < DATASET_SIZE; i++) {
			uint8_t board[64] = { 0 };
			bars[generate_board(board)]++;
		}

		uint32_t max = 0;
		for (uint32_t i = 0; i < 65; i++) {
			if (bars[i] > max) {
				max = bars[i];
			}
		}

		printf("With a dataset size of %d:\n\n", DATASET_SIZE);

		for (uint32_t i = 0; i < 65; i++) {
			uint32_t chars = (uint32_t)((float)bars[i] / (float)max * 90.0f);
			char *string = malloc(chars + 1);
			memset(string, '#', chars);
			string[chars] = '\0';

			printf("%02d  %s     (%d)\n", i, string, bars[i]);
			free(string);
		}
	} else {
		srand(time(NULL));

		uint8_t board[64] = { 0 };
		generate_board(board);

		print_board(board);
	}

	return EXIT_SUCCESS;
}
