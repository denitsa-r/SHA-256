#include <iostream>
#include <fstream>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

using namespace std;

//CONSTANTS
const char* const HASH_FILE = "C:\\Users\\user\\CLionProjects\\sha-256\\hash.txt";
const int COL_MAX = 256;
const int ROW_MAX = 32;
const int ELEMENT_BITS = 8;
const int MESSAGE_MAX = 10240;
const int BINARY_MSG_ROWS = 16;
const int BINARY_COLS = 4;
const int MOD_512 = 512;
const int LAST_BITS = 64;
//break character for reading from file
const char BREAK_CHAR = ';';
//const int CHUNK_SIZE = 2048;
//initialization of constant arrays
const char H[ELEMENT_BITS][ELEMENT_BITS] = {
        '6','a','0','9','e','6','6','7',
        'b','b','6','7','a','e','8','5',
        '3','c','6','e','f','3','7','2',
        'a','5','4','f','f','5','3','a',
        '5','1','0','e','5','2','7','f',
        '9','b','0','5','6','8','8','c',
        '1','f','8','3','d','9','a','b',
        '5','b','e','0','c','d','1','9'
};
const char K[LAST_BITS][ELEMENT_BITS] = {
        '4','2','8','a','2','f','9','8', '7','1','3','7','4','4','9','1', 'b','5','c','0','f','b','c','f', 'e','9','b','5','d','b','a','5', '3','9','5','6','c','2','5','b', '5','9','f','1','1','1','f','1', '9','2','3','f','8','2','a','4', 'a','b','1','c','5','e','d','5',
        'd','8','0','7','a','a','9','8', '1','2','8','3','5','b','0','1', '2','4','3','1','8','5','b','e', '5','5','0','c','7','d','c','3', '7','2','b','e','5','d','7','4', '8','0','d','e','b','1','f','e', '9','b','d','c','0','6','a','7', 'c','1','9','b','f','1','7','4',
        'e','4','9','b','6','9','c','1', 'e','f','b','e','4','7','8','6', '0','f','c','1','9','d','c','6', '2','4','0','c','a','1','c','c', '2','d','e','9','2','c','6','f', '4','a','7','4','8','4','a','a', '5','c','b','0','a','9','d','c', '7','6','f','9','8','8','d','a',
        '9','8','3','e','5','1','5','2', 'a','8','3','1','c','6','6','d', 'b','0','0','3','2','7','c','8', 'b','f','5','9','7','f','c','7', 'c','6','e','0','0','b','f','3', 'd','5','a','7','9','1','4','7', '0','6','c','a','6','3','5','1', '1','4','2','9','2','9','6','7',
        '2','7','b','7','0','a','8','5', '2','e','1','b','2','1','3','8', '4','d','2','c','6','d','f','c', '5','3','3','8','0','d','1','3', '6','5','0','a','7','3','5','4', '7','6','6','a','0','a','b','b', '8','1','c','2','c','9','2','e', '9','2','7','2','2','c','8','5',
        'a','2','b','f','e','8','a','1', 'a','8','1','a','6','6','4','b', 'c','2','4','b','8','b','7','0', 'c','7','6','c','5','1','a','3', 'd','1','9','2','e','8','1','9', 'd','6','9','9','0','6','2','4', 'f','4','0','e','3','5','8','5', '1','0','6','a','a','0','7','0',
        '1','9','a','4','c','1','1','6', '1','e','3','7','6','c','0','8', '2','7','4','8','7','7','4','c', '3','4','b','0','b','c','b','5', '3','9','1','c','0','c','b','3', '4','e','d','8','a','a','4','a', '5','b','9','c','c','a','4','f', '6','8','2','e','6','f','f','3',
        '7','4','8','f','8','2','e','e', '7','8','a','5','6','3','6','f', '8','4','c','8','7','8','1','4', '8','c','c','7','0','2','0','8', '9','0','b','e','f','f','f','a', 'a','4','5','0','6','c','e','b', 'b','e','f','9','a','3','f','7', 'c','6','7','1','7','8','f','2'
};

//DYNAMIC ARRAYS FUNCTIONS
bool** make_2d_binary ( const int rows, const int cols) {
    bool** binary = new bool*[rows];
    for ( int current_row = 0; current_row < rows; current_row++ ) {
        binary[current_row] = new bool[cols];
    }
    return binary;
}

char** make_2d_chars ( const int rows, const int cols) {
    char** content = new char*[rows];
    for ( int current_row = 0; current_row < rows; current_row++ ) {
        content[current_row] = new char[cols];
    }
    return content;
}

void delete_2d_bool_array ( bool** array, const int &rows ) {
    for ( int current_row = 0; current_row < rows; current_row++ ) {
        delete [] array[current_row];
    }
    delete [] array;
}

void delete_2d_char_array ( char** array, const int &rows ) {
    for ( int current_row = 0; current_row < rows; current_row++ ) {
        delete [] array[current_row];
    }
    delete [] array;
}

//SHA-256 FUNCTIONS
void print_hash ( char* final_result ) {

    for ( int col = 0; col < COL_MAX; col++ ){
        cout << final_result[col];
    }

}

bool reset_on_32 ( int iteration ){
    if ( iteration % ROW_MAX == 0 ){
        return true;
    }

    return false;
}

int get_message_length ( const char* message ){
    int col = 0, length = 0;

    while ( message[col] != 0 ){
        length++;
        col++;
    }

    return length;
}
//action within making chunks
void copy_binary_to_chunk ( bool** binary, bool** chunk, int &col, int &chunk_row, int &binary_row, int &iteration ) {
    while ( chunk_row < BINARY_MSG_ROWS ) {

        for ( col = 0; col < ROW_MAX; col++, iteration++ ){
            chunk[chunk_row][col] = binary[binary_row][col];
            if (reset_on_32( iteration )){
                binary_row++;
                chunk_row++;
            }
        }

    }
}

void and_sigma_row ( const bool* first, const bool* second, bool* chunk ) {
    for ( int col = 0; col < ROW_MAX; col++ ) {
        chunk[col] = (first[col] & second[col]);
    }
}

void xor_sigma_row ( const bool* first, const bool* second, bool* chunk ) {
    for ( int col = 0; col < ROW_MAX; col++ ) {
        chunk[col] = (first[col] ^ second[col]);
    }
}

void right_rotate_row ( const bool* chunk, bool* sigma, const int &rotate_with ) {
    for ( int col = 0; col < ROW_MAX; col++ ){

        if ( col < rotate_with ){
            sigma[col] = chunk[ROW_MAX + col - rotate_with];
        } else {
            sigma[col] = chunk[col - rotate_with];
        }

    }

}

void right_shift_row ( const bool* chunk, bool* sigma, const int &shift_with ) {
    for ( int col = 0; col < ROW_MAX; col++ ){

        if ( col < shift_with ){
            sigma[col] = false;
        } else {
            sigma[col] = chunk[col - shift_with];
        }

    }
}

bool* reassign_value ( bool* first, const bool* second ) {
    for ( int col = 0; col < ROW_MAX; col++ ) {
        first[col] = second[col];
    }

    return first;
}

bool* capital_sigma_one ( bool* e, bool* S1 ) {
    bool** sigma = make_2d_binary(2, ROW_MAX);
    right_rotate_row(e, sigma[0],  6);

    right_rotate_row(e, sigma[1],  11);

    xor_sigma_row(sigma[0], sigma[1], sigma[0]);

    right_rotate_row(e, sigma[1], 25);

    xor_sigma_row(sigma[0], sigma[1], S1);

    delete_2d_bool_array(sigma, 2);

    return S1;
}

bool* capital_sigma_zero ( bool* a, bool* S0 ) {
    bool** sigma = make_2d_binary(2, ROW_MAX);
    right_rotate_row(a, sigma[0],  2);

    right_rotate_row(a, sigma[1],  13);

    xor_sigma_row(sigma[0], sigma[1], sigma[0]);

    right_rotate_row(a, sigma[1], 22);

    xor_sigma_row(sigma[0], sigma[1], S0);

    delete_2d_bool_array(sigma, 2);

    return S0;
}

bool* lower_sigma_zero ( bool* result, bool* parent ) {
    bool** sigma = make_2d_binary(2, ROW_MAX);

    right_rotate_row(parent, sigma[0], 7);
    right_rotate_row(parent, sigma[1],  18);

    xor_sigma_row(sigma[0], sigma[1], sigma[1]);

    right_shift_row(parent, sigma[0], 3);
    xor_sigma_row(sigma[0], sigma[1], result);

    delete_2d_bool_array(sigma, 2);

    return result;
}

bool* lower_sigma_one ( bool* result, bool* parent ) {
    bool** sigma = make_2d_binary(2, ROW_MAX);

    right_rotate_row(parent, sigma[0], 17);
    right_rotate_row(parent, sigma[1],  19);

    xor_sigma_row(sigma[0], sigma[1], sigma[1]);

    right_shift_row(parent, sigma[0], 10);
    xor_sigma_row(sigma[0], sigma[1], result);

    delete_2d_bool_array(sigma, 2);

    return result;
}

void addition_modulo_2 ( const bool* first, const bool* second, bool* chunk, bool &reminder ) {

    for ( int col = ROW_MAX - 1; col >= 0; col-- ) {
        chunk[col] = (first[col] ^ second[col]) ^ reminder;

        if ( (first[col] == 1 && second[col] == 1) || ( first[col] == 1 && reminder == 1 ) || (reminder == 1 && second[col] == 1) ){
            reminder = true;
        } else {
            reminder = false;
        }

    }

}

bool* get_lower_sigmas ( bool** chunk, int &row ) {
     bool** sigma = make_2d_binary(3, ROW_MAX);
     bool reminder = false;
    //sigma zero calculations
    sigma[0] = lower_sigma_zero(sigma[0], chunk[row - 15]);

    sigma[1] = lower_sigma_one(sigma[1], chunk[row - 2]);
    //s0 + w0
    addition_modulo_2(sigma[0], chunk[row - 16], sigma[2], reminder);
    //s0 + w0 + s1
    reminder = false;
    addition_modulo_2(sigma[2], sigma[1], sigma[0], reminder);
    //s0 + w0 + s1 + w9
    reminder = false;
    addition_modulo_2(sigma[0], chunk[row - 7], chunk[row], reminder);

    delete_2d_bool_array(sigma, 3);

    return chunk[row];
}

//CONSTANTS TO BINARY
void from_hex_to_binary ( bool* H_binary, const char character, const int &col ) {
    switch ( character ) {
        case '0':
            H_binary[col] = false, H_binary[col + 1] = false, H_binary[col + 2] = false, H_binary[col + 3] = false;
            break;
        case '1':
            H_binary[col] = false, H_binary[col + 1] = false, H_binary[col + 2] = false, H_binary[col + 3] = true;
            break;
        case '2':
            H_binary[col] = false, H_binary[col + 1] = false, H_binary[col + 2] = true, H_binary[col + 3] = false;
            break;
        case '3':
            H_binary[col] = false, H_binary[col + 1] = false, H_binary[col + 2] = true, H_binary[col + 3] = true;
            break;
        case '4':
            H_binary[col] = false, H_binary[col + 1] = true, H_binary[col + 2] = false, H_binary[col + 3] = false;
            break;
        case '5':
            H_binary[col] = false, H_binary[col + 1] = true, H_binary[col + 2] = false, H_binary[col + 3] = true;
            break;
        case '6':
            H_binary[col] = false, H_binary[col + 1] = true, H_binary[col + 2] = true, H_binary[col + 3] = false;
            break;
        case '7':
            H_binary[col] = false, H_binary[col + 1] = true, H_binary[col + 2] = true, H_binary[col + 3] = true;
            break;
        case '8':
            H_binary[col] = true, H_binary[col + 1] = false, H_binary[col + 2] = false, H_binary[col + 3] = false;
            break;
        case '9':
            H_binary[col] = true, H_binary[col + 1] = false, H_binary[col + 2] = false, H_binary[col + 3] = true;
            break;
        case 'a':
            H_binary[col] = true, H_binary[col + 1] = false, H_binary[col + 2] = true, H_binary[col + 3] = false;
            break;
        case 'b':
            H_binary[col] = true, H_binary[col + 1] = false, H_binary[col + 2] = true, H_binary[col + 3] = true;
            break;
        case 'c':
            H_binary[col] = true, H_binary[col + 1] = true, H_binary[col + 2] = false, H_binary[col + 3] = false;
            break;
        case 'd':
            H_binary[col] = true, H_binary[col + 1] = true, H_binary[col + 2] = false, H_binary[col + 3] = true;
            break;
        case 'e':
            H_binary[col] = true, H_binary[col + 1] = true, H_binary[col + 2] = true, H_binary[col + 3] = false ;
            break;
        case 'f':
            H_binary[col] = true, H_binary[col + 1] = true, H_binary[col + 2] = true, H_binary[col + 3] = true;
            break;
    }
}

bool* assign_hash_values ( bool* a, const int &row ){
    bool* H_binary = new bool[ROW_MAX];
    int hash_col = 0;

    for ( int col = 0; col < ROW_MAX; col += 4, hash_col++ ) {
        from_hex_to_binary(H_binary, H[row][hash_col], col);
    }

    for ( int col = 0; col < ROW_MAX; col++ ){
        a[col] = H_binary[col];
    }

    delete [] H_binary;

    return a;
}

bool* assign_K_values ( bool* a, const int &row ){
    bool* H_binary = new bool[ROW_MAX];
    int hash_col = 0;

    for ( int col = 0; col < ROW_MAX; col += 4, hash_col++ ) {
        from_hex_to_binary(H_binary, K[row][hash_col], col);
    }

    for ( int col = 0; col < ROW_MAX; col++ ){
        a[col] = H_binary[col];
    }

    delete [] H_binary;

    return a;
}

void delete_a_to_g ( const bool * a, const bool* b, const bool* c, const bool* d, const bool* e, const bool* f, const bool* g, const bool* h, const bool* S0, const bool* S1, const bool* ch, const bool* maj, const bool* temp1, const bool* temp2) {
    delete [] a;
    delete [] b;
    delete [] c;
    delete [] d;
    delete [] e;
    delete [] f;
    delete [] g;
    delete [] h;
    delete [] S0;
    delete [] S1;
    delete [] ch;
    delete [] maj;
    delete [] temp1;
    delete [] temp2;
}

bool* not_array ( const bool* array ) {
    bool* result = new bool[ROW_MAX];
    for ( int col = 0; col < ROW_MAX; col++ ) {
        result[col] = !array[col];
    }
    return result;
}

bool* choose ( bool* choose, bool* e, bool* f, bool* g ) {
    bool** choice = make_2d_binary(2, ROW_MAX);
    bool* not_e = new bool[ROW_MAX];

    and_sigma_row(e, f, choice[0]);

    not_e = not_array(e);
    and_sigma_row(not_e, g, choice[1]);

    xor_sigma_row(choice[0], choice[1], choose);

    delete [] choice;
    delete [] not_e;

    return choose;
}

bool* majority ( bool* maj, bool* a, bool* b, bool* c ) {
    bool** result = make_2d_binary(2, ROW_MAX);

    and_sigma_row(a, b, result[0]);
    and_sigma_row(a, c, result[1]);

    xor_sigma_row(result[0], result[1], result[1]);

    and_sigma_row(b, c, result[0]);

    xor_sigma_row(result[0], result[1], maj);

    delete_2d_bool_array(result, 2);

    return maj;
}

//FOURTH STEP SEPARATING CHUNKS OF 512BITS
void fill_chunk ( bool** chunk, int &chunk_row, int &col ) {
    while ( chunk_row < LAST_BITS ) {
        chunk[chunk_row] = get_lower_sigmas(chunk, chunk_row);
        chunk_row++;
    }
}

bool* find_temp1 ( bool* temp, bool* h, bool* S1, bool* ch, bool** chunk, const int &i ) {
    bool** result = make_2d_binary(3, ROW_MAX);
    bool reminder = false;

    result[0] = assign_K_values(result[0], i);
    // h + S1
    addition_modulo_2(h, S1, result[1], reminder);

    reminder = false;
    // + ch
    addition_modulo_2(result[1], ch, result[2], reminder);

    reminder = false;
    // + ki
    addition_modulo_2(result[2], result[0], result[1], reminder);

    reminder = false;
    // h + s1 + ch + ki + wi
    addition_modulo_2(result[1], chunk[i], temp, reminder);

    return temp;
}

bool* find_temp2 ( bool* temp, bool* maj, bool* S0 ) {
    bool reminder = false;

    addition_modulo_2(S0, maj, temp, reminder);

    return temp;
}

void compression ( bool** chunk, bool * a, bool* b, bool* c, bool* d, bool* e, bool* f, bool* g, bool* h, bool* S0, bool* S1, bool* ch, bool* maj, bool* temp1, bool* temp2 ) {
    for ( int i = 0; i < LAST_BITS; i++ ) {
        S1 = capital_sigma_one(e, S1);
        ch = choose(ch, e, f, g);
        temp1 = find_temp1(temp1, h, S1, ch, chunk, i);
        S0 = capital_sigma_zero(a, S0);
        maj = majority( maj, a, b, c);
        temp2 = find_temp2(temp2, maj, S0);
        h = reassign_value(h, g);
        g = reassign_value(g, f);
        f = reassign_value(f, e);
        bool reminder = false;
        addition_modulo_2(d, temp1, e, reminder);
        d = reassign_value(d, c);
        c = reassign_value(c, b);
        b = reassign_value(b, a);
        reminder = false;
        addition_modulo_2(temp1, temp2, a, reminder);
    }
}

void modify_final_values ( bool** hashes, const bool * a, const bool* b, const bool* c, const bool* d, const bool* e, const bool* f, const bool* g, const bool* h ) {
    bool** hash_copy = make_2d_binary(ELEMENT_BITS, ROW_MAX);

    bool reminder = false;
    addition_modulo_2(hashes[0], a, hash_copy[0], reminder);
    reminder = false;
    addition_modulo_2(hashes[1], b, hash_copy[1], reminder);
    reminder = false;
    addition_modulo_2(hashes[2], c, hash_copy[2], reminder);
    reminder = false;
    addition_modulo_2(hashes[3], d, hash_copy[3], reminder);
    reminder = false;
    addition_modulo_2(hashes[4], e, hash_copy[4], reminder);
    reminder = false;
    addition_modulo_2(hashes[5], f, hash_copy[5], reminder);
    reminder = false;
    addition_modulo_2(hashes[6], g, hash_copy[6], reminder);
    reminder = false;
    addition_modulo_2(hashes[7], h, hash_copy[7], reminder);

    for ( int hash_index = 0; hash_index < ELEMENT_BITS; hash_index++ ) {
        hashes[hash_index] = reassign_value(hashes[hash_index], hash_copy[hash_index]);
    }

    delete_2d_bool_array(hash_copy, ELEMENT_BITS);
}

void initial_hash_values ( bool** hashes ) {
    for ( int row = 0; row < ELEMENT_BITS; row++ ) {
        hashes[row] = assign_hash_values(hashes[row], row);
    }
}

void making_chunks ( bool** binary, bool** hashes, const int &needed_length ) {
    //when my binary array has more than 512 elements I need to make a loop that will give me chunks of 512 bits
    //the count of the chunks is actually the bit length divided by 64 because I will have 64 row in each chunk
    int count_chunks = needed_length / MOD_512;
    int col = 0, iteration = 1, binary_row = 0, chunk_row = 0;
    bool** chunk = make_2d_binary(LAST_BITS, ROW_MAX);
    //make a to h
    bool* a = new bool[ROW_MAX];
    bool* b = new bool[ROW_MAX];
    bool* c = new bool[ROW_MAX];
    bool* d = new bool[ROW_MAX];
    bool* e = new bool[ROW_MAX];
    bool* f = new bool[ROW_MAX];
    bool* g = new bool[ROW_MAX];
    bool* h = new bool[ROW_MAX];
    bool* S0 = new bool[ROW_MAX];
    bool* S1 = new bool[ROW_MAX];
    bool* ch = new bool[ROW_MAX];
    bool* maj = new bool[ROW_MAX];
    bool* temp1 = new bool[ROW_MAX];
    bool* temp2 = new bool[ROW_MAX];

    for ( int index_chunk = 1; index_chunk <= count_chunks; index_chunk++ ) {
        //setting initial hash values on the first chunk
        if ( index_chunk == 1 ) {
            initial_hash_values(hashes);
        }

        a = reassign_value(a,  hashes[0]);
        b = reassign_value(b, hashes[1]);
        c = reassign_value(c, hashes[2]);
        d = reassign_value(d, hashes[3]);
        e = reassign_value(e, hashes[4]);
        f = reassign_value(f, hashes[5]);
        g = reassign_value(g, hashes[6]);
        h = reassign_value(h, hashes[7]);
        //resetting
        chunk_row = 0;
        iteration = 1;
        col = 0;
        //copy the first 16 rows from my binary array to the chunk
        copy_binary_to_chunk(binary, chunk, col, chunk_row, binary_row, iteration);

        fill_chunk(chunk, chunk_row, col);
        //using formula to get row from 16 to 63
        compression(chunk, a, b, c, d, e, f, g, h, S0, S1, ch, maj, temp1, temp2);
        //modify final values
        modify_final_values(hashes, a, b, c, d, e, f, g, h);
    }

    delete_a_to_g(a, b, c, d, e, f, g, h, S0, S1, ch, maj, temp1, temp2);
    delete_2d_bool_array(chunk, LAST_BITS);
}

bool binary_convert ( int iteration, int index, int bit_count ){
    for ( int i = 0; i < bit_count; i++ ) {

        if( i == ( bit_count - 1 ) - iteration ){
            return index % 2;
        }

        index /= 2;
    }
}

//THIRD STEP LAST 64 BITS ARE USED TO ENCODE THE BIT LENGTH OF THE MESSAGE
void get_last_64_bits ( bool** binary, const int length_message,  const int needed_length, int &difference, int &iteration){
    //I will work in reverse order here
    difference = LAST_BITS - 1;
    int current_row = ( needed_length / ROW_MAX ) - 1, current_col = ROW_MAX - 1;

    while ( iteration < needed_length ){
        binary[current_row][current_col] = binary_convert( difference, length_message, LAST_BITS );

        if ( current_col == 0 ){
            current_col = ROW_MAX;
            current_row--;
        }

        if ( difference == 0 ){
            difference = ELEMENT_BITS;
        }

        difference--;
        current_col--;
        iteration++;
    }
}
//SECOND STEP MAKING PADDING
void make_padding (  bool** binary, const int &needed_length, int &current_row, int &current_col, int &iteration) {
    //looping only 64 bits away from the end of our 512 dividable needed_length
    while ( iteration < (needed_length - LAST_BITS) ) {
        if (reset_on_32(iteration + 1) ) {
            current_col = 0;
            current_row++;
        }

        binary[current_row][current_col] = false;

        iteration++;
        current_col++;
    }
}

void set_separating_one ( bool** binary, int &row, int &col, int &iteration ) {
    if (reset_on_32(iteration) ) {
        col = 0;
    }

    binary[row][col] = true;
    col++;
    iteration++;
}
//FIRST STEP CONVERTING MESSAGE TO BINARY
//this functions finds the nearest number that is can be divided by 512
int find_needed_length ( int length_message ) {
    int iteration = 2, needed_length = MOD_512;
    while ( needed_length < ( length_message + LAST_BITS ) ){
        needed_length = iteration*MOD_512;
        iteration++;
    }
    return needed_length;
}

void filling_binary ( const char* message, bool** binary, int &row, int &col, int &iteration, int &message_index, int &difference ) {
    for ( col = 0; col < ROW_MAX ; col++, iteration++, difference++ ){
        //casting each letter of the message to int to pass it to out function
        int index = (int) ( (unsigned char) message[message_index] );

        if ( difference == ELEMENT_BITS ){
            difference = 0;
        }

        if ( difference + 1 == ELEMENT_BITS ){
            message_index++;
        }

        binary[row][col] = binary_convert( difference, index, ELEMENT_BITS );

        if ( reset_on_32(iteration) ) {
            row++;
        }

        if ( message[message_index] == '\0' ) {
            col++;
            break;
        }
    }
}

void message_to_binary ( const char* message, bool** binary, bool** hashes, const int &length_message ) {
    int col = 0, row = 0, iteration = 1, difference = 0, message_index = 0;
    //difference -> with it we will get the needed binary value; iteration -> keep track of out bits
    //resetting the row value to start our work; while we have a message to read from we will transform it to binary using ascii
    while ( message_index < length_message ){
        filling_binary(message, binary, row, col, iteration, message_index, difference);
    }
    //separates our binary message from the padded input
    set_separating_one(binary, row, col, iteration);

    int bit_length = length_message * 8;
    int needed_length = find_needed_length(bit_length);
    make_padding(binary, needed_length, row, col, iteration);
    //the last 64 bits are used to store the bit length of the message in binary
    get_last_64_bits(binary, bit_length, needed_length, difference, iteration);
    //making chunks, each with 512 bits
    making_chunks(binary, hashes, needed_length);
}

void loop_4_elements ( bool** hashes, int &col, int &char_index, const int &hash_row ) {
    for ( int iteration = col; iteration < (col + 4); iteration++ ) {
        char_index = char_index*2 + hashes[hash_row][iteration];
    }
}

void transform_to_hex ( char* final_result, bool** hashes, int &char_index, int &result_index, const int &hash_row ) {
    for ( int col = 0; col < ROW_MAX; col += 4, char_index = 0, result_index++ ) {
        loop_4_elements(hashes, col, char_index, hash_row);
        //using ascii to transform to hex
        if ( char_index > 9 ) {
            //to get lowercase letters
            final_result[result_index] = char_index + 87;
        } else {
            //to get numbers
            final_result[result_index] = char_index + 48;
        }
    }
}

void sha_algorithm ( const char* message, bool** binary, char* final_result, const int &length_message ) {
    bool** hashes = make_2d_binary(ELEMENT_BITS, ROW_MAX);

    int char_index = 0, hash_row = 0, result_index = 0;
    //message to binary conversion part
    message_to_binary(message, binary, hashes, length_message);
    //to get the final result
    while ( hash_row < ELEMENT_BITS ) {
        transform_to_hex(final_result, hashes, char_index, result_index, hash_row);
        hash_row++;
    }

    delete_2d_bool_array(hashes, ELEMENT_BITS);
}

/* FILE FUNCTIONS */
//void read_line( char** content, fstream& file ) {
//    while( ! file.eof() ){
//        char** line = new char*[MESSAGE_MAX];
//        for( int rows = 0; rows < MESSAGE_MAX; rows++ ){
//            line[rows] = new char[COL_MAX];
//            content[rows] = new char [COL_MAX];
//        }
//        for( int row = 0; row < MESSAGE_MAX; row++ ){
//            for( int col = 0; col < COL_MAX; col++ ){
//                file.get(line[row][col]);
//                if( line[row][col] == BREAK_CHAR ){
//                    break;
//                }
//                content[row][col] = line[row][col];
//            }
//
//        }
//        delete [] line;
//    }
//}

//bool read_file ( char** content ) {
//    fstream file(HASH_FILE, ios::in);
//    //checking if the file is open
//    if( ! file.is_open() ){
//        return false;
//    }
//
//    read_line(content, file);
//
//    file.close();
//    return true;
//}

int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    //making an array, which will store the current file content
//    char** content = new char*[MESSAGE_MAX];
    //array for user's message and setting default value of all zeros
    char* message = new char[MESSAGE_MAX];
    //making an array for the bits of the user's message
    bool** binary = make_2d_binary(MESSAGE_MAX, COL_MAX);
    char* final_result = new char[COL_MAX];
    //starting off I need to open my file and be able to read it
//    bool success = read_file( content );

//    if( ! success ){
//        cerr << "Couldn't open file!" << endl;
//    }

    //I need to hash a message from the user
    cout << "Please enter your message: " << endl;
    cin.getline(message, MESSAGE_MAX);
    cout << message << endl;

    int message_length = get_message_length(message);

    sha_algorithm(message, binary, final_result, message_length);
    print_hash(final_result);

    //deleting arrays
    delete [] message;
    delete [] final_result;
    delete_2d_bool_array(binary, MESSAGE_MAX);
//    delete_2d_char_array(content, MESSAGE_MAX);

    return 0;
}
