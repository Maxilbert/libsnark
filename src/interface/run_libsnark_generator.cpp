/*
 * run_libsnark.cpp
 *
 *      Author: Yuan LU
 */

#include "../interface/CircuitReader.hpp"
#include "gadgetlib2/examples/simple_example.hpp"
#include "gadgetlib2/adapters.hpp"
#include "gadgetlib2/gadget.hpp"
#include "gadgetlib2/integration.hpp"

int main(int argc, char **argv) {

	start_profiling();
	GadgetLibAdapter::resetVariableIndex();
	gadgetlib2::initPublicParamsFromDefaultPp();
	gadgetlib2::GadgetLibAdapter::resetVariableIndex();
	ProtoboardPtr pb = gadgetlib2::Protoboard::create(gadgetlib2::R1P);

	// Read the circuit, evaluate, and translate constraints
	CircuitReader reader(argv[1], pb);

	r1cs_constraint_system<FieldT> cs = get_constraint_system_from_gadgetlib2(*pb);
	const r1cs_variable_assignment<FieldT> full_assignment = get_variable_assignment_from_gadgetlib2(*pb);
	cs.primary_input_size = reader.getNumInputs() + reader.getNumOutputs();
	cs.auxiliary_input_size = full_assignment.size() - cs.num_inputs();
	assert(cs.is_valid());
	//assert(cs.is_satisfied(primary_input, auxiliary_input));
	libsnark::run_r1cs_ppzksnark_generator<libsnark::default_ec_pp>(cs);
	//assert(bit);
	return 0;

}

