#pragma once
#include "regular-expression.h"
#include <map>
#include <set>
#include <queue>

namespace CompilerPlus {
	namespace Lexical {
		using std::map;
		using std::set;
		using std::queue;
		using CompilerPlus::Utility::SetEquals;
		using CompilerPlus::Utility::SetMerge;
		

		

		template<typename TEntity, typename TToken>
		class SubsetConstructionMethod
		{
		public:
			TransitionSystem<TEntity, TToken>* value_system;
			TToken epsilon;
			map<Identity, NFANode<TEntity, TToken>*>* node_map;
			SubsetConstructionMethod(TransitionSystem<TEntity, TToken>* value_system):value_system(value_system){
				this->epsilon = value_system->Epsilon();
			}
			~SubsetConstructionMethod(){}
			set<Identity>* EpsilonClosure(NFANode<TEntity, TToken>* node);
			DFA<TEntity, TToken>* Construct(NFA<TEntity, TToken>* nfa);
		private:
			map<Identity, NFANode<TEntity, TToken>>* nfa_node_map = new map<Identity, NFANode<TEntity, TToken>>();
			void EpsilonDepthFirstSearch(set<Identity>* visited, NFANode<TEntity, TToken>* current_node);

		};


		template<typename TEntity, typename TToken>
		inline set<Identity>* SubsetConstructionMethod<TEntity, TToken>::EpsilonClosure(NFANode<TEntity, TToken>* node)
		{
			set<Identity>* result_set = new set<Identity>();
			this->EpsilonDepthFirstSearch(result_set, node);
			return result_set;
		}

		template<typename TEntity, typename TToken>
		inline DFA<TEntity, TToken>* SubsetConstructionMethod<TEntity, TToken>::Construct(NFA<TEntity, TToken>* nfa)
		{
			using DFANodeT = DFANode<TEntity, TToken>;
			using NFANodeT = NFANode<TEntity, TToken>;
			DFA<TEntity, TToken>* dfa = new DFA<TEntity, TToken>();

			DFANodeT* start_node = new DFANodeT();
			dfa->start_node = start_node;
			map<Identity, NFANodeT*>* nfa_node_map = GetNFANodeMap<TEntity, TToken>(nfa);
			nfa->node_map = nfa_node_map;
			map<Identity, DFANodeT*>* dfa_node_map = new map<Identity, DFANodeT*>();
			dfa_node_map->operator[](start_node->Id)= start_node;
			dfa->dfa_node_map = dfa_node_map;
			map<Identity, set<Identity>> dfa_id_to_nfa_states;
			auto init_states = EpsilonClosure(nfa->start_node);
			SetMerge<Identity>(&dfa_id_to_nfa_states[dfa->start_node->Id], init_states);
			delete init_states;
			queue<Identity> unmarked_nodes;
			unmarked_nodes.push(start_node->Id);

			while (!unmarked_nodes.empty()) {
				auto front = unmarked_nodes.front();
				unmarked_nodes.pop();
				auto front_node = dfa_node_map->at(front);
				auto nfa_id_set = dfa_id_to_nfa_states.at(front);

				// get the possible transitions
				set<TToken> possible_transition_tokens;
				for (auto id : nfa_id_set) {
					NFANodeT* node = nfa_node_map->at(id);
					for (auto iter = node->transition_map->begin(); iter != node->transition_map->end(); iter++) {
						if (iter->first != this->epsilon) {
							possible_transition_tokens.insert(iter->first);
						}
					}
				}

				for (TToken non_epsilon_token : possible_transition_tokens) {
					// compute moved set and the whole epsilon closure.
					set<Identity> moved_set;
					for (auto n : nfa_id_set) {
						NFANodeT* node = nfa_node_map->at(n);
						auto iter = node->transition_map->find(non_epsilon_token);
						if (iter == node->transition_map->end()) {
							continue;
						}
						for (auto sub_iter = iter->second->begin(); sub_iter != iter->second->end(); sub_iter++) {
							NFANodeT* next = *sub_iter;
							moved_set.insert(next->Id);
							auto closure = this->EpsilonClosure(next);
							SetMerge<Identity>(&moved_set, closure);
							delete closure;
						}
					}
					bool is_accepting = false;
					for (Identity possible_accepting_node_id : moved_set) {
						NFANodeT* possible_node = nfa_node_map->at(possible_accepting_node_id);
						if (possible_node->IsAcceptingNode) {
							is_accepting = true;
							break;
						}
					}

					bool already_created = false;
					DFANodeT* target_node = nullptr;
					for (auto iter : dfa_id_to_nfa_states) {
						if (SetEquals<Identity>(&iter.second, &moved_set)) {
							already_created = true;
							target_node = dfa_node_map->at(iter.first);
							break;
						}
					}
					if (!already_created) {
						target_node = new DFANodeT();
						dfa_node_map->operator[](target_node->Id)= target_node;
						dfa_id_to_nfa_states[target_node->Id]= moved_set;
						if (is_accepting) {
							target_node->SetState(true);
						}
						unmarked_nodes.push(target_node->Id);
					}
					front_node->AddTransition(non_epsilon_token, target_node);
				}
			}
			return dfa;
		}

		template<typename TEntity, typename TToken>
		inline void SubsetConstructionMethod<TEntity, TToken>::EpsilonDepthFirstSearch(set<Identity>* visited, NFANode<TEntity, TToken>* current_node)
		{
			Identity current_id = current_node->Id;
			if (visited->find(current_node->Id) != visited->end()) {
				return;
			}
			visited->insert(current_id);
			map<TToken, set<NFANode<TEntity, TToken>*>*>* transitions = current_node->transition_map;
			map<TToken, set<NFANode<TEntity, TToken>*>*>::const_iterator it = transitions->find(epsilon);
			if (it == transitions->end()) {
				return;
			}
			auto epsilon_targets = *it->second;
			for (NFANode<TEntity, TToken>* item : epsilon_targets) {
				EpsilonDepthFirstSearch(visited, item);
			}
		}

	}
}