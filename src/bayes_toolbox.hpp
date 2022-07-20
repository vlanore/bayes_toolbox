#pragma once

// Structure
#include "structure/array_utils.hpp"
#include "structure/model.hpp"
#include "structure/new_view.hpp"

// Operations
#include "operations/across_values.hpp"
#include "operations/logprob.hpp"
#include "operations/draw.hpp"
#include "operations/backup.hpp"

#include "operations/gather.hpp"
#include "operations/raw_value.hpp"
#include "operations/set_value.hpp"

// Distributions
#include "distributions/bernoulli.hpp"
#include "distributions/beta.hpp"
#include "distributions/categorical.hpp"
#include "distributions/dirichlet.hpp"
#include "distributions/exponential.hpp"
#include "distributions/gamma.hpp"
#include "distributions/normal.hpp"
#include "distributions/poisson.hpp"
#include "distributions/uniform.hpp"

// Deterministic functions
#include "detfunctions/custom_dnode.hpp"
#include "detfunctions/product.hpp"

#include "moves/proposals.hpp"
#include "moves/mh.hpp"
#include "moves/gibbs.hpp"

// Utils
#include "mcmc_utils.hpp"
#include "tagged_tuple/src/fancy_syntax.hpp"

