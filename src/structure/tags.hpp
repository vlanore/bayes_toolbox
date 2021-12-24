/*Copyright or © or Copr. CNRS (2019). Contributors:
- Vincent Lanore. vincent.lanore@gmail.com

This software is a computer program whose purpose is to provide a set of C++ data structures and
functions to perform Bayesian inference with MCMC algorithms.

This software is governed by the CeCILL-C license under French law and abiding by the rules of
distribution of free software. You can use, modify and/ or redistribute the software under the terms
of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy, modify and redistribute
granted by the license, users are provided only with a limited warranty and the software's author,
the holder of the economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated with loading, using,
modifying and/or developing or reproducing the software by the user in light of its specific status
of free software, that may mean that it is complicated to manipulate, and that also therefore means
that it is reserved for developers and experienced professionals having in-depth computer knowledge.
Users are therefore encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or data to be ensured and,
more generally, to use and operate it in the same conditions as regards security.

The fact that you are presently reading this means that you have had knowledge of the CeCILL-C
license and that you accept its terms.*/

#pragma once

// tags for struct indexation
struct rate {};
struct shape {};
struct scale {};
struct weight_a {};
struct weight_b {};
struct prob {};
struct concentration {};
struct intensity {};
struct center {};
struct invconc {};
struct invshape {};
struct mean {};
struct gam_mean {};
struct gam_invshape {};
struct weights {};
struct distrib {};
struct value {};
struct params {};
struct backup_value {};
struct suffstat {};
struct suffstat_type {};
struct target {};
struct lower_bound {};
struct upper_bound {};
struct variance {};
struct exchrates {};
struct freqs {};


// dnode deterministic functions
struct int_arg {};
struct real_a {};
struct real_b {};
struct real_c {};
struct real_arg {};
struct vector_argv {};
struct nucmatrix {};

// tags for class typing
struct model_tag {};

struct node_tag {};
struct lone_node_tag : node_tag {};
struct node_array_tag : node_tag {};
struct node_matrix_tag : node_tag {};
struct node_cubix_tag : node_tag {};

struct dnode_tag {};
struct lone_dnode_tag : dnode_tag {};
struct dnode_array_tag : dnode_tag {};
struct dnode_matrix_tag : dnode_tag {};
struct dnode_cubix_tag : dnode_tag {};

struct view_tag {};
struct unknown_tag {};
struct suffstat_tag {};
struct ref_tag {};
