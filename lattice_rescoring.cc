#include <string>
#include <assert.h>

#include <fst/fst.h>

#include <boost/algorithm/string.hpp>
#include <fst/fstlib.h>

std::vector<std::pair<std::string, float>> all_strings(fst::StdVectorFst &fst) {

  std::vector<std::tuple<fst::StdArc::StateId, std::string, float>> queue;
  std::vector<std::pair<std::string, float>> res;
  auto st = fst.InputSymbols();

  queue.push_back(std::make_tuple(fst.Start(), "", 0));
  
  while (!queue.empty()) {
    auto p = queue.back();
    auto current_state = std::get<0>(p);
    auto current_str = std::get<1>(p);
    auto current_weight = std::get<2>(p);
    queue.pop_back();
    std::cerr << "a" << std::endl;
    fst.Write("pouet.fst");
    std::cerr << "kk" << std::endl;
    for (fst::ArcIterator<fst::StdFst> aiter(fst, current_state); !aiter.Done(); aiter.Next()) {
      std::cerr << "d" << std::endl;
      const fst::StdArc &arc = aiter.Value();
      std::cerr << "b" << std::endl;
      if (fst.Final(arc.nextstate) != fst::TropicalWeight::Zero()) {
	auto final_str = current_str + " " + st->Find(arc.ilabel);
	boost::algorithm::trim(final_str);
	res.push_back(std::make_pair(final_str, current_weight + arc.weight.Value()));
  	continue;
      }
      std::cerr << "c" << std::endl;
      queue.push_back(std::make_tuple(arc.nextstate, current_str + " " + st->Find(arc.ilabel), current_weight + arc.weight.Value()));
    }
      std::cerr << "l" << std::endl;
  }
  std::cerr << "k" << std::endl;
  return res;
}

