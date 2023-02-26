/*
 * Copyright [2023] [malab Pinglu Zhang]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Author: Pinglu Zhang
// Contact: zpl010720@gmail.com
// Created: 2023-02-24


#include "../include/utils.h"

KSEQ_INIT(int, read)

/**
 * @brief A timer class that measures elapsed time. 
 * This class uses C++11 chrono library to measure elapsed time in seconds with double precision. 
 * The timer starts at construction and can be reset to zero by calling reset(). 
 * The elapsed time can be obtained by calling elapsed_time() method. 
 * The timer is based on std::chrono::steady_clock, which is a monotonic clock that is not subject to system clock adjustments.
*/
Timer::Timer() {
    start_time_ = std::chrono::steady_clock::now();
}
void Timer::reset() {
    start_time_ = std::chrono::steady_clock::now();
}

double Timer::elapsed_time() const {
    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start_time_;
    return elapsed.count();
}

/**
 * @brief: read fasta and fastq format data
 * @param data_path   the path to the target data
 * @param data store sequence content
 * @param name store sequence name
 * @return multiple sequence stored in vector 
*/
void read_data(const char* data_path, std::vector<std::string>& data, std::vector<std::string>& name){
    // check weather the input path could be accessed 
    if(access_file(data_path)){
        std::cout<< data_path << " could be accessed" << std::endl;
    }else{
        std::cerr << data_path << " could not be accessed, please check the input data path" << std::endl;
        exit(1);
    }

    FILE* f_pointer = fopen(data_path, "r");
    kseq_t* file_t = kseq_init(fileno(f_pointer));
    uint32_t bytes_of_sequence = 0;
    while (kseq_read(file_t) >= 0) // Read one sequence in each iteration of the loop
    {
        std::string tmp_data = clean_sequence(file_t -> seq.s);
        std::string tmp_name = file_t -> name.s;
        data.push_back(tmp_data);
        name.push_back(tmp_name);
        bytes_of_sequence += tmp_data.capacity();
    }
    kseq_destroy(file_t);
    #if M64
    std::cout << "The input data occupies approximately "<< std::fixed << std::setprecision(2) << bytes_of_sequence / pow(2, 30) <<" GB of memory" << std::endl;
    #else
    std::cout << "The input data occupies approximately "<< std::fixed << std::setprecision(2) << bytes_of_sequence / pow(2, 20) <<" MB of memory" << std::endl;
    #endif
    std::cout << "the number of input sequences is " << data.size() << std::endl;
    return;
}

/**
 * @brief: Check whether the file exists in the specified path.
 * @param data_path   The file path to check.
 * @return Returns true if the file exists, otherwise false.
*/
bool access_file(const char* data_path){
    std::ifstream file(data_path);
    if (file.is_open() && file.good()) {
        return true;
    } else {
        return false;
    }
    return true;
}

/**
 * @brief Cleans the input sequence by removing any non-ATCG characters. 
 * This function removes any characters from the input sequence that are not A, T, C, or G (case-insensitive). 
 * The cleaned sequence is then returned as a new string.
 * @param sequence The input DNA sequence to be cleaned.
 * @return The cleaned DNA sequence as a new string.
*/
std::string clean_sequence(std::string sequence){
    // Convert lowercase nucleotides to uppercase
    std::transform(sequence.begin(), sequence.end(), sequence.begin(), ::toupper);

    // Remove any characters that are not A, C, G, or T
    sequence.erase(std::remove_if(sequence.begin(), sequence.end(),
                    [](char c) { return c != 'A' && c != 'C' && c != 'G' && c != 'T'; }),
                    sequence.end());

    return sequence;
}

