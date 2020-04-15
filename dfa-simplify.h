#pragma once
#include "nfa.h"
#include <queue>
#include <vector>
namespace CompilerPlus {
	namespace Lexical {
		using std::queue;
		using std::vector;
		using std::pair;
		using Utility::SetMerge;
		using Utility::SetEquals;
		template<typename TEntity, typename TToken>
		class DFASimplifyMethod {
			using NodeIdentities = set<Identity>;
			using DFANodeT = DFANode<TEntity, TToken>;
			using DFAT = DFA<TEntity, TToken>;
		public:
			void Simplify(DFAT* dfa) {
				IdentityProvider id_generator;
				map<Identity, NodeIdentities> group_map;
				map<Identity, Identity> group_id_of_node_id;

				// initial partition
				NodeIdentities accepting_group;
				Identity accepting_group_id = id_generator.GetIdentity();
				NodeIdentities pending_group;
				Identity pending_group_id = id_generator.GetIdentity();
				map<Identity, DFANodeT*>* dfa_node_map = dfa->dfa_node_map;
				for (auto item : *dfa_node_map) {
					Identity id = item.first;
					DFANodeT* node = item.second;
#ifdef DEBUG
					if (id != node->Id) {
						throw "NO!!!";
					}
#endif // DEBUG
					if (node->IsAcceptingNode) {
						accepting_group.insert(id);
						group_id_of_node_id[id] = accepting_group_id;
					}
					else {
						pending_group.insert(id);
						group_id_of_node_id[id] = pending_group_id;
					}
				}
				SetMerge(&group_map[accepting_group_id], &accepting_group);
				SetMerge(&group_map[pending_group_id], &pending_group);
				// while-loop partition
				size_t group_counts;
				do {		
					group_counts = group_map.size();
					auto copyed_group_map = map<Identity, NodeIdentities>(group_map);
					for (auto group_info: copyed_group_map) {
						NodeIdentities& group = group_info.second;
						map<Identity, NodeIdentities> partitioned_group_map;
						Partition(id_generator, dfa_node_map, group_map, group_id_of_node_id, group, partitioned_group_map);
						for (Identity node_id : group) {
							auto position = group_id_of_node_id.find(node_id);
							if(position == group_id_of_node_id.end()){
								throw "NO!!!";
							}
							group_id_of_node_id.erase(position);
						}
						group_map.erase(group_info.first);
						for (auto item : partitioned_group_map) {
							Identity new_group_id = item.first;
							SetMerge(&group_map[item.first], &item.second);
							for (auto node_id : item.second) {
								group_id_of_node_id[node_id] = new_group_id;
							}
						}
					}

				} while (group_counts < group_map.size());

				// Merge DFA

				// 1. calculate new dfa group mapping
				map<Identity, Identity> node_id_of_group_id;
				for (auto item : group_map) {
					node_id_of_group_id[item.first] = *item.second.begin();
				}
				// 2. make new connection
				for (auto item : node_id_of_group_id) {
					Identity group_id = item.first;
					Identity node_id = item.second;
					DFANodeT* node = dfa_node_map->at(node_id);
					for (pair<TToken, DFANodeT*> sub_item : *node->transition_map) {
						TToken token = sub_item.first;
						DFANodeT* dist_node = sub_item.second;
						Identity dist_group_id = group_id_of_node_id[dist_node->Id];
						node->transition_map->operator[](token) = dfa_node_map->at(node_id_of_group_id[dist_group_id]);
					}

				}
				// 3. prepare for node ids to reserve
				set<Identity> node_id_to_reserve;
				for (auto item : group_map) {
					node_id_to_reserve.insert(*item.second.begin());
				}

				// 4. delete nodes
				map<Identity, DFANodeT*>::iterator it = dfa_node_map->begin();
				while (it != dfa_node_map->end()) {
					Identity id = it->first;
					if (node_id_to_reserve.find(id) == node_id_to_reserve.end()) {
						delete it->second;
						dfa_node_map->erase(it++);
					}
					else {
						it++;
					}
				}
			}

			void Partition(
				IdentityProvider& id_generator,
				map<Identity, DFANodeT*>* dfa_node_map,
				map<Identity, NodeIdentities>& group_map,
				map<Identity, Identity>& group_id_of_node_id,
				NodeIdentities& group,
				map<Identity, NodeIdentities>& partitioned_group_map
				) {
				for (Identity node_id : group) {
					Identity found_group;
					if (std::any_of(partitioned_group_map.begin(), partitioned_group_map.end(), [this,&found_group,&dfa_node_map,&node_id,&group_id_of_node_id](pair<Identity, NodeIdentities> item)->bool {
						bool result = HasSameTransitions(dfa_node_map->at(node_id), dfa_node_map->at(*item.second.begin()), group_id_of_node_id);
						if (result) {
							found_group = item.first;
						}
						return result;
						})) {
						partitioned_group_map[found_group].insert(node_id);
					}
					else {
						partitioned_group_map[id_generator.GetIdentity()].insert(node_id);
					}
				}
			}
			bool HasSameTransitions(DFANodeT* one, DFANodeT* two, map<Identity, Identity>& group_id_of_node_id) {
				return one->transition_map->size() == two->transition_map->size() &&
					std::all_of(
						one->transition_map->begin(),
						one->transition_map->end(),
						[&](pair<TToken, DFANodeT*> item)->bool {
							auto iter = two->transition_map->find(item.first);
							if (iter == two->transition_map->end()) return false;
							Identity target_id = item.second->Id;
							return group_id_of_node_id[target_id] == group_id_of_node_id[iter->second->Id];
						}
				);
			}
		};
	}
}