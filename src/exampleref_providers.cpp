/*
 * example_providers.cpp
 *
 *  Created on: Mar 22, 2019
 *      Author: dkoes
 */

#include "libmolgrid/exampleref_providers.h"
#include <boost/algorithm/string.hpp>

namespace libmolgrid {

using namespace std;
using namespace boost::algorithm;

template<>
void ReceptorStratifiedExampleRefProvider<BalancedExampleRefProvider, 2>::setup() {
  //balanced requires acive and decoys, so filter out receptors that don't have both for convenience
  vector<BalancedExampleRefProvider> tmp;
  currenti = 0; currentk = 0;

  for(unsigned i = 0, n = examples.size(); i < n; i++)
  {
    if(examples[i].num_actives() > 0 && examples[i].num_decoys() > 0) {
      //eliminate empty buckets
      tmp.push_back(examples[i]);
      tmp.back().setup();
    }
    else if(examples[i].num_actives() > 0)
    {
      ExampleRef tmp;
      examples[i].next_active(tmp);
      log(INFO) << "Dropping receptor " << tmp.files[0] << " with no decoys.\n";
    }
    else if(examples[i].num_decoys() > 0)
    {
      ExampleRef tmp;
      examples[i].next_decoy(tmp);
      log(INFO) << "Dropping receptor " << tmp.files[0] << " with no actives.\n";
    }
  }

  swap(examples,tmp);
  if(randomize) shuffle(examples.begin(), examples.end(), random_engine);

}

int ExampleRefProvider::populate(std::istream& lines, int numlabels, bool hasgroup) {
  if(!lines) throw invalid_argument("Could not read lines");

  string line;
  while (getline(lines, line))
  {
    trim(line);
    if(line.length() > 0) { //ignore blank lines
      ExampleRef ref(line, numlabels, hasgroup);
      addref(ref);
    }
  }

  return size();
}


} /* namespace libmolgrid */
