#include <iostream>
#include <fstream>

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
const unsigned int H[ELEMENT_BITS] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};
const unsigned int K[LAST_BITS] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74 , 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
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
void print_elements ( bool** binary, int needed_length ) {

    for ( int i = 0; i < needed_length/32; i++ ){
        for ( int j = 0; j < 32 ; j++ ){
            cout << binary[i][j];
            if( (j + 1) % 8 == 0 && j != 0 ) cout << " ";
        }
        cout << endl;
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

void right_rotate_row ( bool** chunk, bool** sigma, const int &sigma_row, const int &row, const int &rotate_with ) {
    for ( int col = 0; col < ROW_MAX; col++ ){
        if ( col < rotate_with ){
            sigma[sigma_row][col] = chunk[row][ROW_MAX + col - rotate_with];
        } else {
            sigma[sigma_row][col] = chunk[row][col - rotate_with];
        }
    }
    //printing
    for (int i = 0; i < ROW_MAX; i++) {
        cout << sigma[sigma_row][i];
    }
}

void right_shift_row ( bool** chunk, bool** sigma, const int &sigma_row, const int &row, const int &shift_with ) {
    for ( int col = 0; col < ROW_MAX; col++ ){
        if ( col < shift_with ){
            sigma[sigma_row][col] = false;
        } else {
            sigma[sigma_row][col] = chunk[row][col - shift_with];
        }
    }
}

void get_sigma_zero ( bool** chunk, int &row ) {
    bool** sigma = make_2d_binary(5, ROW_MAX); // making a 2d array which will store the values from the rotations and shifts and the final result of the sigma
    int sigma_row = 0;
    //row is the current chunk row then for the sigma I need to
//    right_rotate_row(chunk, sigma, sigma_row, row - 15, 7);
//    right_rotate_row(chunk, sigma, sigma_row, row - 15, 18);
    right_shift_row(chunk, sigma, sigma_row, row, 3);

    delete_2d_bool_array(sigma, 5);
}

void fill_chunk_with_zeros ( bool** chunk, int &chunk_row, int &col ) {
    while ( chunk_row < LAST_BITS ) {
        for ( col = 0; col < ROW_MAX; col++ ) {
            chunk[chunk_row][col] = false;
        }
        chunk_row++;
    }
}

void making_chunks ( bool** binary, const int &needed_length ) {
    //when my binary array has more than 512 elements I need to make a loop that will give me chunks of 512 elements -> bits
    //then for each chunk I will need to calculate the rest of the rows by a formula
    //when making the chunks I will need the length of my binary array;
    int count_chunks = needed_length / MOD_512; //the count of the chunks is actually the bit length divided by 64 because I will have 64 row in each chunk
    int col = 0, iteration = 1, binary_row = 0, chunk_row = 0;
    bool** chunk = make_2d_binary(LAST_BITS, ROW_MAX);
    for ( int index_chunk = 1; index_chunk <= count_chunks; index_chunk++ ) {
        chunk_row = 0;
        iteration = 1;
        col = 0;
        //first step: copy the first 16 rows from my binary array to the chunk
        copy_binary_to_chunk(binary, chunk, col, chunk_row, binary_row, iteration);
        //second step: fill the following 64 - 16 = 48 rows with zeros
        fill_chunk_with_zeros(chunk, chunk_row, col);
        //third step: get the 16th etc rows using a formula
        chunk_row = 0;

        get_sigma_zero(chunk, chunk_row);
        cout << endl << endl;

        print_elements(chunk, 2048 );
    }
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

void make_padding (  bool** binary, const int &needed_length, int &current_row, int &current_col, int &iteration) {
    //get the closest needed length to the length of the message which is dividable by 512]
    //when making padding I have to put 1 where my binary message ends, where is my current row and my next column
    //only adding 1 to the column because when the message ends the separating 1 is always first in its set of 8 bits
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
//this functions finds the nearest number that is can be divided by 512
int find_needed_length ( int length_message ) {
    int iteration = 2, needed_length = MOD_512;
    while ( needed_length < ( length_message + LAST_BITS ) ){
        needed_length = iteration*MOD_512;
        iteration++;
    }
    return needed_length;
}

void message_to_binary ( const char* message, bool** binary, const int &length_message ) {
    int col = 0, row = 0, iteration = 1, difference = 0, message_index = 0;
    //we make a difference variable with which we will return the needed binary value
    //we make an iterations variable to keep track of out bits starting off of iteration = 1
    //resetting the row value to start our work
    //while we have a message to read from we will transform it to binary using ascii
    while ( message_index < length_message ){
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

    set_separating_one(binary, row, col, iteration);

//  making padding;
    int bit_length = length_message * 8;
    int needed_length = find_needed_length(bit_length);
    make_padding(binary, needed_length, row, col, iteration);
    get_last_64_bits(binary, bit_length, needed_length, difference, iteration);
    making_chunks(binary, needed_length);
//    print_elements(binary, needed_length);
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
    //making an array, which will store the current file content
//    char** content = new char*[MESSAGE_MAX];
    //array for user's message and setting default value of all zeros
    char* message = new char[MESSAGE_MAX];
    //making an array for the bits of the user's message
    bool** binary = make_2d_binary(MESSAGE_MAX, COL_MAX);
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

    message_to_binary(message, binary, message_length);
//    right_rotate_row(binary, 0, 7);
    //deleting arrays
    delete [] message;
    delete_2d_bool_array(binary, MESSAGE_MAX);
//    delete_2d_char_array(content, MESSAGE_MAX);

    return 0;
}
