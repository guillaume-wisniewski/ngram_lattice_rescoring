#include <iostream>
#include <string>

#include <fst/shortest-path.h>
#include <fst/extensions/far/far.h>

#include <ngram/lexicographic-map.h>
#include <ngram/ngram-output.h>

#include "json/json.h"


#include <boost/program_options.hpp>
std::vector<std::pair<std::string, float>> all_strings(fst::StdVectorFst &fst);

namespace po = boost::program_options;

bool process_command_line(int argc,
			  char** argv,
			  std::string& lm,
			  std::string& far_fn,
			  std::string& output_fn,
			  int& nbest_size) {

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("lm", po::value<std::string>(&lm)->required(), "FST containing the language model")
    ("far", po::value<std::string>(&far_fn)->required(), "FAR containing the lattices to rescore")
    ("output", po::value<std::string>(&output_fn)->required(), "file in which hypotheses will be dumped")
      ("nbest_size", po::value<int>(&nbest_size)->required(), "size of the nbest to generate")
      ;
    
    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    
    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return false;
    }
    
    po::notify(vm);
  } catch(std::exception& e) {
      std::cerr << "Error: " << e.what() << "\n";
      return false;
  } catch(...) {
    std::cerr << "Unknown error!" << "\n";
    return false;
  }

  return true;

}


int main(int argc, char** argv) {

  std::string lm;
  std::string far_fn;
  std::string output_fn;
  int nbest_size;
  
  if (!process_command_line(argc, argv, lm, far_fn, output_fn, nbest_size)) return EXIT_FAILURE;

  std::cerr << "Loading LM" << std::endl;
  std::unique_ptr<fst::StdVectorFst> lmfst(fst::StdVectorFst::Read(lm));
  if (!lmfst) {
    std::cerr << "Error when loading LM" << std::endl;
    return EXIT_FAILURE;
  }

  ngram::NGramOutput ngram(lmfst.get());
  if (ngram.Error()) {
    std::cerr << argv[0] << ": Failed to initialize ngram model." << std::endl;
    return EXIT_FAILURE;
  }
  ngram.MakePhiMatcherLM(ngram::kSpecialLabel);
  std::cerr << "done" << std::endl;
  
  std::unique_ptr<fst::FarReader<fst::StdArc>> far_reader(fst::FarReader<fst::StdArc>::Open(far_fn));
  if (!far_reader) {
    std::cerr << "Can't open " << far_fn << " for reading" << std::endl;
    return EXIT_FAILURE;
  }
  
  int count = 0;
  std::vector<std::vector<std::string>> res;
  Json::Value root;
  while (!far_reader->Done()) {
    std::cout << "reading: " << far_reader->GetKey() << std::endl;
    count += 1;
    std::unique_ptr<fst::StdVectorFst> lattice(new fst::StdVectorFst(*far_reader->GetFst()));

    if (lattice->InputSymbols() == NULL) {
      std::cerr << "no input symbols. The FST must be compiled with the --keep_isymbols option" << std::endl;
      return EXIT_FAILURE;
    }

    std::unique_ptr<fst::StdVectorFst> weighted_fst;
    weighted_fst.reset(new fst::StdVectorFst());
    std::cerr << "2" << std::endl;
    ngram.FailLMCompose(*lattice, weighted_fst.get(), ngram::kSpecialLabel);
    
    weighted_fst->SetInputSymbols(lattice->InputSymbols());
    weighted_fst->SetOutputSymbols(lattice->OutputSymbols());
    weighted_fst->Write("pp.fst");
    std::cerr << "3" << std::endl;
    fst::StdVectorFst best_hyp_fst;
    ShortestPath(*weighted_fst.get(), &best_hyp_fst, nbest_size);
    std::cerr << "4" << std::endl;
    Json::Value vect;
    for (auto el : all_strings(best_hyp_fst)) {
      Json::Value v;
      std::cerr << "5" << std::endl;
      v.append(el.second);
      v.append(el.first);

      vect.append(v);
      std::cerr << "6" << std::endl;
    }
    root[far_reader->GetKey()] = vect;
    far_reader->Next();
  }
  std::cout << "serializing..." << std::endl;

  std::ofstream ofile(output_fn);
  ofile << root;
  
  std::cout << "read: " << count << " fst" << std::endl;

}
