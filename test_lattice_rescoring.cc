#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_lattice_rescoring

#include <vector>
#include <utility> 
#include <boost/test/unit_test.hpp>

#include <fst/fstlib.h>

std::vector<std::pair<std::string, float>> all_strings(fst::StdVectorFst &fst);


BOOST_AUTO_TEST_CASE( single_path_all_strings ) {

  fst::SymbolTable syms;
  syms.AddSymbol("<eps>", 0);
  syms.AddSymbol("a", syms.AvailableKey());
  syms.AddSymbol("a@1", syms.AvailableKey());
  syms.AddSymbol("b", syms.AvailableKey());
  syms.AddSymbol("b@2", syms.AvailableKey());
  syms.AddSymbol("c", syms.AvailableKey());
  syms.AddSymbol("c@3", syms.AvailableKey());

  fst::StdVectorFst fst;

  auto state1 = fst.AddState();
  auto state2 = fst.AddState();
  auto state3 = fst.AddState();
  auto state4 = fst.AddState();

  fst.SetStart(state1);
  fst.SetFinal(state4, 0);

  fst.SetInputSymbols(&syms);
  fst.SetOutputSymbols(&syms);
  
  fst.AddArc(state1 ,fst::StdArc(syms.Find("a@1"), syms.Find("a"), 1, state2));
  fst.AddArc(state2 ,fst::StdArc(syms.Find("b@2"), syms.Find("b"), 2, state3));
  fst.AddArc(state3 ,fst::StdArc(syms.Find("c@3"), syms.Find("c"), 3, state4));

  auto res = all_strings(fst);

  BOOST_CHECK_EQUAL(1, res.size());
  BOOST_CHECK_EQUAL("a@1 b@2 c@3", std::get<0>(res[0]));
  BOOST_CHECK_EQUAL(6, std::get<1>(res[0]));

}

BOOST_AUTO_TEST_CASE( two_paths_all_strings ) {

  fst::SymbolTable syms;
  syms.AddSymbol("<eps>", 0);
  syms.AddSymbol("a", syms.AvailableKey());
  syms.AddSymbol("a@1", syms.AvailableKey());
  syms.AddSymbol("b", syms.AvailableKey());
  syms.AddSymbol("b@2", syms.AvailableKey());
  syms.AddSymbol("c", syms.AvailableKey());
  syms.AddSymbol("c@3", syms.AvailableKey());

  fst::StdVectorFst fst;

  auto state1 = fst.AddState();
  auto state2 = fst.AddState();
  auto state3 = fst.AddState();
  auto state4 = fst.AddState();
  auto state5 = fst.AddState();
  auto state6 = fst.AddState();

  fst.SetStart(state1);
  fst.SetFinal(state4, 0);

  fst.SetInputSymbols(&syms);
  fst.SetOutputSymbols(&syms);
  
  fst.AddArc(state1 ,fst::StdArc(syms.Find("a@1"), syms.Find("a"), 1, state2));
  fst.AddArc(state2 ,fst::StdArc(syms.Find("b@2"), syms.Find("b"), 2, state3));
  fst.AddArc(state3 ,fst::StdArc(syms.Find("c@3"), syms.Find("c"), 3, state4));

  fst.AddArc(state1 ,fst::StdArc(syms.Find("b@2"), syms.Find("b"), 2, state5));
  fst.AddArc(state5 ,fst::StdArc(syms.Find("a@1"), syms.Find("a"), 3, state6));
  fst.AddArc(state6 ,fst::StdArc(syms.Find("c@3"), syms.Find("c"), 3, state4));

  std::unordered_set<std::string> res;
  for (auto el : all_strings(fst)) {
    res.insert(std::get<0>(el));
  }
  
  BOOST_CHECK_EQUAL(2, res.size());

  std::unordered_set<std::string> expected_res = {"a@1 b@2 c@3",
						  "b@2 a@1 c@3"};

  for (auto el : res) {
    BOOST_CHECK(expected_res.find(el) != expected_res.end());
  }

}

BOOST_AUTO_TEST_CASE( many_paths_all_strings ) {

  fst::SymbolTable syms;
  syms.AddSymbol("<eps>", 0);
  syms.AddSymbol("a", syms.AvailableKey());
  syms.AddSymbol("b", syms.AvailableKey());
  syms.AddSymbol("c", syms.AvailableKey());
  syms.AddSymbol("d", syms.AvailableKey());
  syms.AddSymbol("e", syms.AvailableKey());
  syms.AddSymbol("f", syms.AvailableKey());
  syms.AddSymbol("g", syms.AvailableKey());
  syms.AddSymbol("h", syms.AvailableKey());
  
  fst::StdVectorFst fst;

  auto state1 = fst.AddState();
  auto state2 = fst.AddState();
  auto state3 = fst.AddState();
  auto state4 = fst.AddState();
  auto state5 = fst.AddState();
  auto state6 = fst.AddState();

  fst.SetStart(state1);
  fst.SetFinal(state6, 0);

  fst.SetInputSymbols(&syms);
  fst.SetOutputSymbols(&syms);
  
  fst.AddArc(state1 ,fst::StdArc(syms.Find("a"), syms.Find("a"), 1, state2));
  fst.AddArc(state1 ,fst::StdArc(syms.Find("b"), syms.Find("b"), 2, state3));
  fst.AddArc(state2 ,fst::StdArc(syms.Find("c"), syms.Find("c"), 3, state4));
  fst.AddArc(state2 ,fst::StdArc(syms.Find("d"), syms.Find("b"), 2, state5));
  fst.AddArc(state3 ,fst::StdArc(syms.Find("e"), syms.Find("e"), 3, state4));
  fst.AddArc(state3 ,fst::StdArc(syms.Find("f"), syms.Find("f"), 3, state5));
  fst.AddArc(state4 ,fst::StdArc(syms.Find("g"), syms.Find("g"), 3, state6));
  fst.AddArc(state5 ,fst::StdArc(syms.Find("h"), syms.Find("h"), 3, state6));
  
  std::unordered_set<std::string> res;
  for (auto el : all_strings(fst)) {
    res.insert(std::get<0>(el));
  }
  
  BOOST_CHECK_EQUAL(4, res.size());

  std::unordered_set<std::string> expected_res = {"a c g",
						  "b e g",
						  "b f h",
  						  "a d h"};

  for (auto el : res) {
    BOOST_CHECK(expected_res.find(el) != expected_res.end());
  }

}

