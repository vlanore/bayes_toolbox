//p unif(0,1)
//b bern(p)

//fixer b à 1, et normalement 2/3

#include <iostream>
#include "basic_moves.hpp"
#include "mcmc_utils.hpp"
#include "operations/backup.hpp"
#include "operations/logprob.hpp"
#include "operations/raw_value.hpp"
#include "distributions/bernoulli.hpp"
#include "distributions/uniform.hpp"
using namespace std;


int main(){
	
	auto gen = make_generator();
	
	auto a = make_node<uniform>(0,1);
	auto b = make_node<bernoulli>(a);
	
	get<value>(b) = 1;
	draw(a,gen);
	cout<<"a="<<get<value>(a)<<"\n";
}
