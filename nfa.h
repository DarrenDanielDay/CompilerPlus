#pragma once

#include "utility.h"
#include <map>
#include <string>
#include <set>


namespace CompilerPlus {
	namespace Lexical {
		using std::cout;
		using std::endl;
		using std::map;
		using std::string;
		using std::set;
		using std::ostream;
		using CompilerPlus::Utility::Iterable;
		using CompilerPlus::Utility::Iterator;
		using Identity = int;
		class IdentityProvider;
		IdentityProvider* _instance = nullptr;
		class IdentityProvider {
		private:
			Identity current_id = 0;
		public:
			static IdentityProvider* GetInstance() {
				if (_instance == nullptr) {
					_instance = new IdentityProvider();
				}
				return _instance;
			}
			Identity GetIdentity() {
				return current_id++;
			}
		};

		template<typename TEntity>
		class Node {
		private:
		protected:
			Identity id;
		public:
			Node():id(IdentityProvider::GetInstance()->GetIdentity()) {}
			~Node() {}
			const Identity& Id = id;
			TEntity* entity = nullptr;		
		};
	
		template<typename TEntity>
		class StateNode: public Node<TEntity> {
		protected:
			bool is_accepting_node = false;
		public:
			StateNode() {}
			~StateNode() {}
			StateNode(bool is_accepting):Node(),is_accepting_node(is_accepting){}
			void SetState(bool is_accepting) { is_accepting_node = is_accepting; }
			const bool& IsAcceptingNode = is_accepting_node;
		};
		template<typename TEntity, typename TToken = string>
		class SubsetConstructionMethod;
		template<typename TEntity, typename TToken = string>
		class NFANode : public StateNode<TEntity> {
		public:
			NFANode() {}
			~NFANode() {}
			using NodePointer = NFANode<TEntity, TToken>*;
			map<TToken, set<NodePointer>*>* transition_map = new map<TToken, set<NodePointer>*>();
			friend class SubsetConstructionMethod<TEntity, TToken>;
			void AddTransition(TToken token, NFANode<TEntity, TToken>* target) {
				auto position = this->transition_map->find(token);
				if (position == this->transition_map->end()) {
					this->transition_map->operator[](token) = new set<NFANode<TEntity, TToken>*>();
				}
				this->transition_map->operator[](token)->insert(target);
			}

			friend ostream& operator<<(ostream& os, NFANode<TEntity, TToken>* node) {
				os << "NFA Node { " << endl
					<< "  -- ID : " << node->Id << endl;
				for (pair<TToken, set<NodePointer>*> item : *node->transition_map) {
					TToken token = item.first;
					set<NodePointer> next_nodes = *item.second;
					os << "  " << token << " : {";
					for (NodePointer pointer : next_nodes) {
						os << pointer->Id << " ";
					}
					os << "}" << endl;
				}
				os << "}";
				return os;
			}
		protected:
		};
		template<typename TEntity, typename TToken = string>
		class DFA;
		template<typename TEntity, typename TToken>
		class SubsetConstructionMethod;
		template<typename TEntity, typename TToken>
		class DirectDFAConstruction;
		template<typename TEntity, typename TToken>
		class DFASimplifyMethod;
		template<typename TEntity, typename TToken = string>
		class DFANode : public StateNode<TEntity> {
		public:
			friend class DFA<TEntity, TToken>;
			friend class SubsetConstructionMethod<TEntity, TToken>;
			friend class DirectDFAConstruction<TEntity, TToken>;
			friend class DFASimplifyMethod<TEntity, TToken>;
			friend ostream& operator<<(ostream& os, DFANode<TEntity, TToken>* node) {
				os << "DFA Node { " << endl
					<< "  -- ID : " << node->Id << endl;
				for (auto item : *node->transition_map) {
					TToken token = item.first;
					DFANode<TEntity, TToken>* next_node = item.second;
					os << "  " << token << " : " << next_node->Id << endl;
				}
				os << "}";
				return os;
			}
			using NodePointer = DFANode<TEntity, TToken>*;
			DFANode() {}
			~DFANode() { if (this->transition_map != nullptr)delete this->transition_map; }
		protected:
			map<TToken, NodePointer>* transition_map = new map<TToken, NodePointer>();
		private:
			void AddTransition(const TToken& token, NodePointer next) { this->transition_map->operator[](token) = next; }
		};
		
		
		template<typename TEntity, typename TToken>
		class SubsetConstructionMethod;
		template<typename TEntity, typename TToken>
		class DFA {
		public:
			using NodePointer =  DFANode<TEntity, TToken>*;
			friend class SubsetConstructionMethod<TEntity, TToken>;
			friend class DirectDFAConstruction<TEntity, TToken>;
			friend class DFASimplifyMethod<TEntity, TToken>;
			NodePointer start_node = nullptr;
			DFA() {}
			~DFA() {}
			bool Match(Iterable<TToken>* sequence) {
				Iterator<TToken>* iterator = sequence->GetIterator();
				NodePointer position = this->start_node;
				while (iterator->HasNext()) {
					TToken* current = iterator->Next();
					map<TToken, NodePointer>* transitions = position->transition_map;
					if (transitions->find(*current) == transitions->end()) {
						return false;
					}
					position = transitions->at(*current);
				}
				return position->IsAcceptingNode;

			}
			static void Free(DFA<TEntity, TToken>* dfa) {
				if (dfa == nullptr) {
					return;
				}
				for (pair<Identity, DFANode<TEntity, TToken>*> item : *dfa->dfa_node_map) {
					delete item.second;
				}
				if (dfa->dfa_node_map != nullptr) {
					delete dfa->dfa_node_map;
				}
				delete dfa;
			}

			friend ostream& operator<<(ostream& os, DFA<TEntity, TToken>* dfa) {
					os << "DFA {" << endl;
					for (auto item : *dfa->dfa_node_map) {
						DFANode<TEntity, TToken>* node = item.second;
						os << node << endl;
					}
					os << "}" << endl;
					return os;

			}
		private:
			map<Identity, DFANode<TEntity, TToken>*>* dfa_node_map = nullptr;
			
		};
		template<typename TEntity, typename TToken>
		class SubsetConstructionMethod;
		template<typename TEntity, typename TToken>
		class ThompsonConstruction;

		template<typename TEntity, typename TToken = string>
		class NFA
		{
		public:
			using NodePointer = NFANode<TEntity, TToken>*;
			friend class SubsetConstructionMethod<TEntity, TToken>;
			friend class ThompsonConstruction<TEntity, TToken>;
			NFA(){}
			~NFA() { if (node_map != nullptr) delete node_map; }
			NodePointer start_node = nullptr, end_node = nullptr;
			map<Identity, NodePointer>* node_map = nullptr;
			static void Free(NFA<TEntity, TToken>* nfa) {
				for (auto sub_nfa : nfa->sub_nfa) {
					FreeSubNFA(sub_nfa);
				}
				if (nfa->node_map != nullptr) {					
					for (auto item : *nfa->node_map) {
						delete item.second;
					}
				}
				delete nfa;
			}
			friend ostream& operator<<(ostream& os, NFA<TEntity, TToken>* nfa) {
				os << "NFA {" << endl;
				if (nfa->node_map == nullptr) {
					nfa->node_map = GetNFANodeMap<TEntity, TToken>(nfa);
				}
				for (pair<Identity, NFANode<TEntity, TToken>*> item : *nfa->node_map) {
					NFANode<TEntity, TToken>* node = item.second;
					os << node << endl;
				}
				os << "}" << endl;
				return os;

			}
		private:
			set<NFA<TEntity, TToken>*> sub_nfa;
			static void FreeSubNFA(NFA<TEntity, TToken>* nfa) {
				for (auto item : nfa->sub_nfa) {
					FreeSubNFA(item);
				}
				delete nfa;
			}
		};

		template<typename TEntity, typename TToken>
		void _NFADFSAddToMap(map<Identity, NFANode<TEntity, TToken>*>* node_map, NFANode<TEntity, TToken>* current_node) {
			auto it = node_map->find(current_node->Id);
			if (it != node_map->end()) {
				return;
			}
			node_map->operator[](current_node->Id) = current_node;
			auto transition_map = current_node->transition_map;
			for (auto item : *transition_map) {
				for (auto node : *item.second) {
					_NFADFSAddToMap<TEntity, TToken>(node_map, node);
				}
			}
		}

		template<typename TEntity, typename TToken>
		map<Identity, NFANode<TEntity, TToken>*>* GetNFANodeMap(NFA<TEntity, TToken>* nfa) {
			auto node_map = new map<Identity, NFANode<TEntity, TToken>*>();
			_NFADFSAddToMap<TEntity, TToken>(node_map, nfa->start_node);
			return node_map;
		}

		

	}
}