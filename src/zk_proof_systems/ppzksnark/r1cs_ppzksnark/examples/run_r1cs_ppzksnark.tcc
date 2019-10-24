/** @file
 *****************************************************************************

 Implementation of functionality that runs the R1CS ppzkSNARK for
 a given R1CS example.

 See run_r1cs_ppzksnark.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef RUN_R1CS_PPZKSNARK_TCC_
#define RUN_R1CS_PPZKSNARK_TCC_

#include "zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <type_traits>

#include "common/profiling.hpp"

namespace libsnark {


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
///////Serialize to a file///////
template<typename T>
void serializeToFile(const T &obj, std::string filename)
{
    std::ofstream file;
    file.open(filename);
    file << obj;
    file.close();
}

///////Reconstitute from a file///////
template<typename T>
T reconstituteFromFile(std::string filename)
{
    std::ifstream file;
    file.open(filename);
    T tmp;
    file >> tmp;
    file.close();
    return tmp;
}

///////Reconstitute from a file with check correctness///////
template<typename T>
T reconstituteFromFile(const T &obj, std::string filename)
{
    std::ifstream file;
    file.open(filename);
    T tmp;
    file >> tmp;
    assert(obj == tmp);
    file.close();
    return tmp;
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

/*
template<typename ppT>
typename std::enable_if<ppT::has_affine_pairing, void>::type
test_affine_verifier(const r1cs_ppzksnark_verification_key<ppT> &vk,
                     const r1cs_ppzksnark_primary_input<ppT> &primary_input,
                     const r1cs_ppzksnark_proof<ppT> &proof,
                     const bool expected_answer)
{
    print_header("R1CS ppzkSNARK Affine Verifier");
    const bool answer = r1cs_ppzksnark_affine_verifier_weak_IC<ppT>(vk, primary_input, proof);
    assert(answer == expected_answer);
}

template<typename ppT>
typename std::enable_if<!ppT::has_affine_pairing, void>::type
test_affine_verifier(const r1cs_ppzksnark_verification_key<ppT> &vk,
                     const r1cs_ppzksnark_primary_input<ppT> &primary_input,
                     const r1cs_ppzksnark_proof<ppT> &proof,
                     const bool expected_answer)
{
    print_header("R1CS ppzkSNARK Affine Verifier");
    printf("Affine verifier is not supported; not testing anything.\n");
}
*/

/**
 * The code below provides an example of all stages of running a R1CS ppzkSNARK.
 *
 * Of course, in a real-life scenario, we would have three distinct entities,
 * mangled into one in the demonstration below. The three entities are as follows.
 * (1) The "generator", which runs the ppzkSNARK generator on input a given
 *     constraint system CS to create a proving and a verification key for CS.
 * (2) The "prover", which runs the ppzkSNARK prover on input the proving key,
 *     a primary input for CS, and an auxiliary input for CS.
 * (3) The "verifier", which runs the ppzkSNARK verifier on input the verification key,
 *     a primary input for CS, and a proof.
 */

template<typename ppT>
bool run_r1cs_ppzksnark(const r1cs_example<Fr<ppT> > &example,
                        const bool test_serialization)
{
    enter_block("Call to run_r1cs_ppzksnark");

    //Generator needs constraint system
    //Generator produces 1. proving key, 2. Verification key, 3. Processed verification key (for online);
 
    print_header("R1CS ppzkSNARK Generator");
    r1cs_ppzksnark_keypair<ppT> keypair = r1cs_ppzksnark_generator<ppT>(example.constraint_system);
    printf("\n"); print_indent(); print_mem("after generator");

    //I do not need processed verfication key for offline verification
    //print_header("Preprocess verification key");
    //r1cs_ppzksnark_processed_verification_key<ppT> pvk = r1cs_ppzksnark_verifier_process_vk<ppT>(keypair.vk);

    if (test_serialization)
    {
        enter_block("Test serialization of keys");

        //keypair.pk = reserialize<r1cs_ppzksnark_proving_key<ppT> >(keypair.pk);
	serializeToFile<r1cs_ppzksnark_proving_key<ppT> >(keypair.pk, "Proving_Key");
	keypair.pk = reconstituteFromFile<r1cs_ppzksnark_proving_key<ppT> >(keypair.pk, "Proving_Key");

        //keypair.vk = reserialize<r1cs_ppzksnark_verification_key<ppT> >(keypair.vk);
	serializeToFile<r1cs_ppzksnark_verification_key<ppT> > (keypair.vk, "Verification_Key");
	keypair.vk = reconstituteFromFile<r1cs_ppzksnark_verification_key<ppT> >(keypair.vk, "Verification_Key");

std::cout<<keypair.vk.alphaA_g2<<std::endl;
keypair.vk.alphaA_g2.print_coordinates();
//std::cout<<keypair.vk.alphaB_g1<<std::endl;
//std::cout<<keypair.vk.alphaC_g2<<std::endl;
//std::cout<<keypair.vk.gamma_g2<<std::endl;
//std::cout<<keypair.vk.gamma_beta_g1<<std::endl;
//std::cout<<keypair.vk.gamma_beta_g2<<std::endl;
//std::cout<<keypair.vk.rC_Z_g2<<std::endl;
//std::cout<<keypair.vk.encoded_IC_query<<std::endl;

        //I do not need processed verfication key for offline verification
        ////pvk = reserialize<r1cs_ppzksnark_processed_verification_key<ppT> >(pvk);
	//serializeToFile<r1cs_ppzksnark_processed_verification_key<ppT> >(pvk, "Processed_Verification_Key");
	//pvk = reconstituteFromFile<r1cs_ppzksnark_processed_verification_key<ppT> >(pvk, "Processed_Verification_Key");
        //leave_block("Test serialization of keys");
    }

    //Prover needs 1. proving key, 2. primary input, 3. auxiliary input;
    //Prover produces proof
    print_header("R1CS ppzkSNARK Prover");
    r1cs_ppzksnark_proof<ppT> proof = r1cs_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input);
    printf("\n"); print_indent(); print_mem("after prover");

    if (test_serialization)
    {
        enter_block("Test serialization of proof");
        //proof = reserialize<r1cs_ppzksnark_proof<ppT> >(proof);
	serializeToFile<r1cs_ppzksnark_proof<ppT> > (proof, "Proof");
	proof = reconstituteFromFile<r1cs_ppzksnark_proof<ppT> >(proof, "Proof");
        leave_block("Test serialization of proof");
    }

    //Verifier needs proof and primary input
    //Verifier produce answer
    print_header("R1CS ppzkSNARK Verifier");
    const bool ans = r1cs_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
    printf("\n"); print_indent(); print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

/* I do not need online verfifier or affine verifier
    print_header("R1CS ppzkSNARK Online Verifier");
    const bool ans2 = r1cs_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
    assert(ans == ans2);

    test_affine_verifier<ppT>(keypair.vk, example.primary_input, proof, ans);
*/
    leave_block("Call to run_r1cs_ppzksnark");

    return ans;
}




/*
template<typename T>
T reserialize(const T &obj)
{
    std::stringstream ss;
    ss << obj;
    T tmp;
    ss >> tmp;
    assert(obj == tmp);
    return tmp;
}
*/


} // libsnark

#endif // RUN_R1CS_PPZKSNARK_TCC_
