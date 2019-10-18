#include <iostream>
#include "basic_moves.hpp"
#include "distributions/exponential.hpp"
#include "distributions/gamma.hpp"
#include "distributions/poisson.hpp"
#include "mcmc_utils.hpp"
#include "operations/backup.hpp"
#include "operations/logprob.hpp"
#include "operations/raw_value.hpp"
#include "operations/set_value.hpp"
#include "structure/View.hpp"
#include "structure/array_utils.hpp"
#include "suffstat_utils.hpp"
#include "tagged_tuple/src/fancy_syntax.hpp"

using namespace std;

TOKEN(my_node)
TOKEN(my_node2)

auto model_exp(){
	auto my_node = make_node<exponential>(1.0);
	auto my_node2 = make_node<exponential>(get<value>(my_node)); //difference entre get value et raw value ?
	// get value renvoie ce qu'il y a dans le noeud (possiblement un tableau ou une matrice), raw value fait la même chose si c'est un nombre, et sinon il lui faut les indices pour renvoyer un nombre
	return make_model(
        	my_node_ = move(my_node),
		my_node2_ = move(my_node2)
		);
}

// couverture de markov : l'ensemble des noeuds dont on a besoin pour calculer le changement de log prob lorsqu'on change un noeud. Donc tout les noeuds dont il dépend (puisqu'on a besoin de leur valeur pour calculer sa valeur) et tout les noeuds qui dépendent de lui (puisque s'il change leurs log prob changent). Dans l'implémentation, les couvertures correspondent seulement aux noeuds qui dépendent et pour lesquels il faut recalculer puisque les noeuds contiennent eux même leur dépendance, donc pas besoin de les donner (pour les noeuds dont ils dépendent)

template <class Node, class MB, class Gen, class... IndexArgs> // à quoi sert cette ligne ?
void scaling_move(Node& node, MB blanket, Gen& gen, IndexArgs... args) { // en entrée : le noeud sur lequel on tente un move, la couverture de Markov de ce noeud, le générateur aléatoire, l'indice qu'on modifie si le noeud contient un tableau.
    auto index = make_index(args...);
    auto bkp = backup(node, index);
    double logprob_before = logprob(blanket); // calcule la logprob qu'on va changer
    double log_hastings = scale(raw_value(node, index), gen);// modifie la valeur du noeud et retourne le rapport de hastings, si on veut move de v1 à v2, ce sera proba de move de v1 vers v2 divisé par v2 vers v1 (pour les distribution non symétrique).
    bool accept = decide(logprob(blanket) - logprob_before + log_hastings, gen);
    if (!accept) { restore(node, bkp, index); } // restore si on ne veut pas changer
}

int main() {

	//constexpr size_t nb_it{100'000}, len_lambda{5}, len_K{3}; syntaxe pour dire size_t nb_it = 100 000; len_lambda = ... 
	//size_t : type entier non signé
	size_t nb_it = 100'000;
    	auto gen = make_generator();
	auto m = model_exp(); // instancie le modele 
    	auto v = make_view<my_node>(m); // permet de ne garder comme variable libre du modele que my_node
    	draw(v,gen); 
	set_value(my_node2_(m), 5);
    	std::cout << raw_value(my_node_(m))<<"\n"; // raw_value : donne la valeur du noeud, my_node_(m) : le my_node du modele m
	std::cout << raw_value(my_node2_(m))<<"\n";
	
	double my_node_sum = 0;
    	for (size_t it = 0; it < nb_it; it++) {
		scaling_move(my_node_(m), make_view<my_node, my_node2>(m), gen);
		my_node_sum+=raw_value(my_node_(m));
	}
	std::cout << "my_node = " << my_node_sum / float(nb_it);
	
}
