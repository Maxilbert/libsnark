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

#ifndef RUN_R1CS_PPZKSNARK_GENERATOR_TCC_
#define RUN_R1CS_PPZKSNARK_GENERATOR_TCC_

#include "zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <type_traits>

#include "common/profiling.hpp"

namespace libsnark {

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
int run_r1cs_ppzksnark_generator(const r1cs_constraint_system<Fr<ppT> > &cs)
{
    enter_block("Call to run_r1cs_ppzksnark_generator");

    //Generator needs constraint system
    //Generator produces 1. proving key, 2. Verification key, 3. Processed verification key (for online);
    print_header("R1CS ppzkSNARK Generator");

    r1cs_ppzksnark_keypair<ppT> keypair = r1cs_ppzksnark_generator<ppT>(cs);
    printf("\n"); print_indent(); print_mem("after generator");




    enter_block("Serialization of keys");
    serializeToFile<r1cs_ppzksnark_proving_key<ppT> > (keypair.pk, "Proving_Key");
    serializeToFile<r1cs_ppzksnark_verification_key<ppT> > (keypair.vk, "Verification_Key");
    leave_block("Serialization of keys");

    leave_block("Call to run_r1cs_ppzksnark_generator");

    return 1;
}





} // libsnark

#endif // RUN_R1CS_PPZKSNARK_GENERATOR_TCC_
