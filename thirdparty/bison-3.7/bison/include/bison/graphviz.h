/* Output Graphviz specification of a state machine generated by Bison.

   Copyright (C) 2006, 2010-2015, 2018-2020 Free Software Foundation,
   Inc.

   This file is part of Bison, the GNU Compiler Compiler.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert and Satya Kiran Popuri.  */

#ifndef GRAPHVIZ_H_
# define GRAPHVIZ_H_

#include "bison/state.h"

/** Begin a Dot graph.
 *
 * \param fout   output stream.
 */
void start_graph (FILE *fout);

/** Output a Dot node.
 *
 * \param id     identifier of the node
 * \param label  human readable label of the node (no Dot escaping needed).
 * \param fout   output stream.
 */
void output_node (int id, char const *label, FILE *fout);

/** Output a Dot edge.
 * \param source       id of the source node
 * \param destination  id of the target node
 * \param label        human readable label of the edge
 *                     (no Dot escaping needed).  Can be 0.
 * \param style        Dot style of the edge (e.g., "dotted" or "solid").
 * \param fout         output stream.
 */
void output_edge (int source, int destination, char const *label,
                  char const *style, FILE *fout);

/** Output a reduction.
 * \param s            current state
 * \param reds         the set of reductions
 * \param fout         output stream.
 */
void output_red (state const *s, reductions const *reds, FILE *fout);

/** End a Dot graph.
 *
 * \param fout  output stream.
 */
void finish_graph (FILE *fout);

#endif /* ! GRAPHVIZ_H_ */