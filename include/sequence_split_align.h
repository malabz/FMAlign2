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
// Created: 2023-02-25

#ifndef SEQUENCE_SPLIT_ALIGN_H
#define SEQUENCE_SPLIT_ALIGN_H

#include "common.h"
#include "../ext/SW/ssw_cpp.h"
#include "../ext/SW/ssw.h"
#include "utils.h"
#include <algorithm>
#include <sstream>
#include <cstdio>
#ifdef __linux__
#include <sys/stat.h>
#else
#include <direct.h>
#endif


const std::string TMP_FOLDER = "./tmp/";

struct ExpandChainParams {
	std::vector<std::string>* data;
	std::vector<std::vector<std::pair<int_t, int_t>>>* chain;
	uint_t chain_index;
	std::vector<std::vector<std::string>>::iterator result_store;
};

struct ParallelAlignParams {
	std::vector<std::string>* data;
	std::vector<std::vector<std::pair<int_t, int_t>>>::iterator parallel_range;
	uint_t task_index;
	std::vector<std::vector<std::string>>::iterator result_store;
};

void split_and_parallel_align(std::vector<std::string> data, std::vector<std::string> name, std::vector<std::vector<std::pair<int_t, int_t>>> split_points_on_sequence);
/**
* @brief Selects columns from a sequence of split points to enable multi thread.
* @param split_points_on_sequence A vector of vectors of pairs, where each pair represents the start and mem length
* @return A vector of indices of the selected columns.
*/
std::vector<int_t> select_columns(std::vector<std::vector<std::pair<int_t, int_t>>> split_points_on_sequence);

/**
* @brief Get a vector of integers that are not in the selected_cols vector and have a maximum value of n.
* @param n The maximum value of the integers in the resulting vector.
* @param selected_cols A vector of integers that are already selected.
* @return A vector of integers that are not in selected_cols and have a maximum value of n.
*/
std::vector<int_t> get_remaining_cols(int_t n, const std::vector<int_t> selected_cols);

/**
@brief Expands the chain at the given index for all sequences in the input data.
This function takes a void pointer to input arguments and casts it to the correct struct type.
It then retrieves the required variables, which include the data and chain input parameters, and the chain index.
The query sequence is obtained from the chain for each sequence in the data input, and a reference sequence is obtained
using the neighboring chains. If there is no neighboring chain, the reference sequence is obtained from the start of
the sequence to the end of the previous neighboring chain or to the end of the sequence if there is no previous neighboring chain.
The aligner is then used to align the query sequence and the reference sequence, and the result is stored in an alignment object.
If the chain at the given index for the current sequence is empty, the result is stored in the chain. Otherwise, the query sequence
is already aligned, and the aligned fragment is stored in the aligned_fragment vector.
Finally, the function stores the aligned fragments in the result_store vector.
@param arg A void pointer to input arguments.
@return NULL
*/
void* expand_chain(void* arg);

/**
* @brief Store the Smith-Waterman alignment results in a vector of aligned sequences.
* This function takes the alignment results generated by the StripedSmithWaterman algorithm and
* stores the aligned sequences in a vector of strings. The function also returns the alignment
* start and length as a pair of integers. If the alignment failed, the function returns (-1,-1).
* @param alignment The alignment results generated by StripedSmithWaterman algorithm.
* @param ref The reference sequence.
* @param query The query sequence.
* @param res_store A vector of aligned sequences.
* @param seq_index The index of the query sequence in the vector of aligned sequences.
* @return A pair of integers representing the alignment start and length.
*/
std::pair<int_t, int_t> store_sw_alignment(StripedSmithWaterman::Alignment alignment, std::string& ref, std::string& query,
	std::vector<std::string>& res_store, uint_t seq_index);

/**
 * @brief Get the range of each sequence in parallel alignment
 * @param data The vector of sequences to be aligned
 * @param chain The vector of chains representing the alignment
 * @return The vector of ranges for each sequence in the alignment
 */
std::vector<std::vector<std::pair<int_t, int_t>>> get_parallel_align_range(std::vector<std::string> data, std::vector<std::vector<std::pair<int_t, int_t>>> chain);

void* parallel_align(void* arg);

std::string align_fasta(std::string file_name);

/**
* @brief Deletes temporary files generated during sequence alignment tasks.
* @param task_count The number of tasks for which temporary files were created.
*/
void delete_tmp_folder(uint_t task_count);


void concat_alignment(std::vector<std::vector<std::string>>&chain_string, std::vector<std::vector<std::string>>&parallel_string, std::vector<std::string> &name);
#endif