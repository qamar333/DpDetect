#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/Value.h"
#include "llvm/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"






//#include<fstream>
using namespace llvm;
using namespace std;
namespace {
	struct DpDetect : public ModulePass {
	public:
		map<string,unsigned int> defMap;
		map<string,unsigned int> useMap;

		static char ID;
		int getLID(Instruction* BI);
		string getVariableName(Instruction* I);
		virtual bool runOnModule(Module &M);
		DpDetect() : ModulePass(ID) {}	
	}; // end of struct DpDetect
}  // end of anonymous namespace

string DpDetect::getVariableName(Instruction* I) {
	

	int index = isa<StoreInst>(I) ? 1 : 0;
	//int index = 1;
	Value* operand = I->getOperand(index);
	if (operand->hasName()) {
		return string(operand->getName());
	}
}


int DpDetect::getLID(Instruction* BI){

	int32_t lid = 0;
	int32_t lno = BI->getDebugLoc().getLine();

	if (lno == 0) {
		return 0;
	}
	return lno;
}//end of getLID


bool DpDetect::runOnModule(Module &M) {
	
	for (auto& F : M) {
		//errs() << "\tFunction: " << F.getName() << "\n";

		for (auto& BB : F) {
			//errs() << "\t\tBasic Block: " << BB.getName() << "\n";

			for (auto& I : BB) {
				//errs() << "\t\t\tInstruction: " << I.getOpcodeName() <<"\n";

				if(isa<StoreInst>(I)){
					//int index = isa<StoreInst>(I) ? 1 : 0;
					
					string varname= getVariableName(&I);
					unsigned line= getLID(&I);
					for(auto& find : defMap){
						if(find.first == varname){
							errs()<<"WAW exist for variable  "<<varname<<" : "<<line<<" : "<<find.second<<"\n";
						}
					}

					for(auto& find : useMap){
						if(find.first == varname){
							errs()<<"WAR exist for variable  "<<varname<<" : "<<line<<" : "<<find.second<<"\n";
						}
					}

					defMap[varname]=line;
					for(auto& it : defMap){

					//for(map<string,unsigned int>::iterator it=defMap.begin();it!=defMap.end();it++){
					
						errs()<<"define "<<it.first<<" "<<it.second<<"\n";				
				}//end of loop printing
								
					}//end of Store operand detection
					

					if(isa<LoadInst>(I)){
					//int index = isa<StoreInst>(I) ? 1 : 0;
					
					string varname= getVariableName(&I);
					unsigned line= getLID(&I);
					for(auto& find : useMap){
						if(find.first == varname){
							errs()<<"RAR exist for variable  "<<varname<<" : "<<line<<" : "<<find.second<<"\n";
						}
					}
					useMap[varname]=line;
					for(map<string,unsigned int>::iterator it=useMap.begin();it!=useMap.end();it++){
						errs()<<"use " <<it->first<<" "<<it->second<<" : "<<defMap[it->first]<<"\n";
						
							
					}
				}//end of Load operand detection
					

				/*		for (Value::use_iterator i = I.use_begin(), e = I.use_end(); i != e; ++i){

							//outputFile = new ofstream();
							//outputFile->open("Data.xml", std::ios::app);
 			 

							//if (Instruction *Inst = dyn_cast<Instruction>(*i)) {
							Instruction *vi = dyn_cast<Instruction>(*i);
							Value *v = *i;
							//errs()<<*vi<<"\n";

							string varname= getVariableName(vi);
							//errs()<<varname1<<"\n";
							unsigned line= getLID(vi);
							//errs()<<"Line no:: "<<line<<"\n";
							useMap[varname]=line;
							for(map<string,unsigned int>::iterator it=useMap.begin();it!=useMap.end();it++){
						errs()<<"Used here::"<<it->first<<"-->"<<it->second<<"\n";							
							}
                                                        

						}*///Iteration over Use chain
					}//Iteration over Instruction
				}//Iteration over BB
			}//Iteration over function


		return false;
}//end of runOnModule
    

char DpDetect::ID = 0;
static RegisterPass<DpDetect> X("dpdetect", "Dependence detection", false, false);



