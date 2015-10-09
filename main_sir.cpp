 /* main.cpp
 *
 *  Created on: May 20, 2014
 *      Author: Tommaso Dreossi
 */

#include <stdio.h>
#include <iostream>

#include "Common.h"
#include "BaseConverter.h"
#include "LinearSystem.h"
#include "Polyhedron.h"
#include "Parallelotope.h"

#include "DynamicalSystem.h"
#include "DiscreteDynamicalSystem.h"

#include "ParameterSynthesizer.h"

#include "STL/STL.h"
#include "STL/Atom.h"
#include "STL/Conjunction.h"
#include "STL/Disjunction.h"
#include "STL/Until.h"

#include "Models/SIR.h"

using namespace std;

int main(int argc,char** argv){

	// SIR model
	Model *model = new SIR();

	lst vars, params, dyns;
	vars = model->getVars();
	params = model->getParams();
	dyns = model->getDyns();

	cout<<"Model name: "<<model->getName()<<"\n";
	cout<<"Dynamics:\n";
	for(int i=0; i<(signed)dyns.nops(); i++){
		cout<<"\td"<<vars[i]<<" = "<<dyns[i]<<"\n";
	}
	cout<<"Specification:";
	model->getSpec()->print();
	cout<<"\n\n";


	// Create a new dynamic system with model's dynamics
	bool rational_dyns = false;
	DynamicalSystem *cont_SIR = new DynamicalSystem(vars,params,dyns,rational_dyns);

	vector<Polyhedron*> multi_template = model->getReachSets();
	DiscreteDynamicalSystem *disc_SIR = new DiscreteDynamicalSystem(vars,params,cont_SIR->eulerDisc(0.5), multi_template, false);


	// Create the parameter synthesizer
	synthesizer_opt options;
	options.largest_para_set = true;

	ParameterSynthesizer *PS = new ParameterSynthesizer(disc_SIR,model->getSpec(),options);

	//ciao


	// Plot reach set
	ofstream matlab_script;
	matlab_script.open ("matlab_script.m");
	matlab_script<<"figure(1)\n";
	matlab_script.close();
	PS->reach(model->getInitConds(),model->getInitParaSet()->at(0),50);
	// Synthesize and plot result
	cout<<"\n\nStart synthesis...\n";
	clock_t tStart = clock();
	LinearSystemSet *result = PS->synthesizeSTL(model->getInitConds(),model->getInitParaSet());
	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	matlab_script.open ("matlab_script.m", std::ios_base::app);
	matlab_script<<"grid on;\n";
	matlab_script<<"axis([0 1 0 1 0 1]);\n";
	matlab_script<<"xlabel('s','FontSize',15);\n";
	matlab_script<<"ylabel('i','FontSize',15);\n";
	matlab_script<<"zlabel('r','FontSize',15);\n";
	matlab_script.close();

	matlab_script.open ("matlab_script.m", std::ios_base::app);
	matlab_script<<"figure(2)\n";
	matlab_script.close();
	model->getInitParaSet()->at(0)->plotRegionToFile("matlab_script.m",'y');
	result->at(0)->plotRegionToFile("matlab_script.m",'r');
	matlab_script.open ("matlab_script.m", std::ios_base::app);
	matlab_script<<"grid on;\n";
	matlab_script<<"axis([0.345 0.365 0.045 0.065]);\n";
	matlab_script<<"xlabel('\\beta','FontSize',15);\n";
	matlab_script<<"ylabel('\\gamma','FontSize',15);\n";
	matlab_script.close();

	cout<<"Done";

}

