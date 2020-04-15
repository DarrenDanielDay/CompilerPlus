#pragma once

#include "regular-expression.h"

namespace CompilerPlus {
	namespace Lexical {
		template<typename TEntity, typename TToken>
		class DirectDFAConstruction {
			using DFAT = DFA<TEntity, TToken>;
			using DFANodeT = DFANode<TEntity, TToken>;
			using RegExpT = RegularExpression<TEntity, TToken>;
		public:
			friend class RegularExpression<TEntity, TToken>;
			DirectDFAConstruction(TransitionSystem<TEntity, TToken>* value_system): value_system(value_system){}
			~DirectDFAConstruction(){}
			DFAT* Construct(RegularExpression<TEntity, TToken>* rex) {
				DFAT* dfa = new DFAT();
				TToken accept_token = value_system->AcceptingToken();
				RegExpT* accepting_rex = new SingleTokenRegularExpression<TEntity, TToken>(&accept_token);
				RegExpT* root = new ConcatRegularExpression<TEntity, TToken>(rex, accepting_rex);

				root->GetFirstAndLastPosition(this);
				Identity accepting_id = *accepting_rex->last_position.begin();
				dfa->start_node = new DFANodeT();
				map<Identity, set<Identity>> dfa_id_to_states;
				map<Identity, DFANodeT*>* dfa_node_map = new map<Identity, DFANodeT*>();
				dfa->dfa_node_map = dfa_node_map;
				SetMerge<Identity>(&dfa_id_to_states[dfa->start_node->Id], &root->first_postion);
				dfa_node_map->operator[](dfa->start_node->Id) = dfa->start_node;
				
				queue<Identity> unmarked_nodes;
				unmarked_nodes.push(dfa->start_node->Id);
				while (!unmarked_nodes.empty()) {
					Identity front = unmarked_nodes.front();
					unmarked_nodes.pop();
					DFANodeT* front_node = dfa_node_map->at(front);

					// calculate all transition and target states
					map<TToken, set<Identity>> transition_to_states;
					for (auto state : dfa_id_to_states[front]) {
						if (state == accepting_id) {
							continue;
						}
						Node<TToken>* node = token_map[state];
						SetMerge(&transition_to_states[*node->entity], &follow_position[state]);
					}

					for (auto item : transition_to_states) {
						TToken token = item.first;
						set<Identity>& states = item.second;
						DFANodeT* target_node = nullptr;
						if (!any_of(dfa_id_to_states.begin(), dfa_id_to_states.end(), [&](pair<Identity, set<Identity>> item) ->bool {
							bool result = SetEquals(&item.second, &states);
							if (result) {
								target_node = dfa_node_map->at(item.first);
							}
							return result;
							})) {
							target_node = new DFANodeT();
							unmarked_nodes.push(target_node->Id);
							dfa_node_map->operator[](target_node->Id) = target_node;
							SetMerge(&dfa_id_to_states[target_node->Id], &states);
						}
						target_node->SetState(any_of(states.begin(), states.end(), [accepting_id](Identity id)-> bool {return id == accepting_id; }));

						front_node->AddTransition(token, target_node);
					}

				}

				return dfa;
			}
			map<Identity, Node<TToken>*> token_map;
			map<Identity, set<Identity>> follow_position;
			TransitionSystem<TEntity, TToken>* value_system;
		private:
		};
	}
}
