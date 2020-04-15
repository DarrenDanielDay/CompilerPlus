#pragma once
#include "subset-construction.h"

namespace CompilerPlus {
	namespace Lexical {
		template<typename TEntity, typename TToken>
		NFA<TEntity, TToken>* ThompsonConstruction<TEntity, TToken>::EpsilonRegExpToNFA(EpsilonRegularExpression<TEntity, TToken>* rex) {
			using NFAT = NFA<TEntity, TToken>;
			using NFANodeT = NFANode<TEntity, TToken>;
			using SetT = set<NFANodeT*>;
			using MapT = map<TEntity, SetT*>;
			NFAT* nfa = new NFAT();
			NFANodeT* start = new NFANodeT(), * end = new NFANodeT();
			end->SetState(true);

			start->AddTransition(epsilon, end);

			nfa->start_node = start;
			nfa->end_node = end;
			return nfa;
		}

		template<typename TEntity, typename TToken>
		NFA<TEntity, TToken>* ThompsonConstruction<TEntity, TToken>::SingleTokenRegExpToNFA(SingleTokenRegularExpression<TEntity, TToken>* rex) {
			using NFAT = NFA<TEntity, TToken>;
			using NFANodeT = NFANode<TEntity, TToken>;
			using SetT = set<NFANodeT*>;
			using MapT = map<TEntity, SetT*>;
			NFAT* nfa = new NFAT();
			NFANodeT* start = new NFANodeT(), * end = new NFANodeT();
			end->SetState(true);

			start->AddTransition(*rex->token, end);

			nfa->start_node = start;
			nfa->end_node = end;
			return nfa;
		}
		template<typename TEntity, typename TToken>
		NFA<TEntity, TToken>* ThompsonConstruction<TEntity, TToken>::UnionRegExpToNFA(UnionRegularExpression<TEntity, TToken>* rex) {
			using NFAT = NFA<TEntity, TToken>;
			using NFANodeT = NFANode<TEntity, TToken>;
			using SetT = set<NFANodeT*>;
			using MapT = map<TEntity, SetT*>;
			NFAT* left = rex->left->ThompsonToNFA(this);
			NFAT* right = rex->right->ThompsonToNFA(this);
			left->end_node->SetState(false);
			right->end_node->SetState(false);

			NFAT* nfa = new NFAT();
			NFANodeT* start = new NFANodeT(), * end = new NFANodeT();
			end->SetState(true);
			nfa->sub_nfa.insert(left);
			nfa->sub_nfa.insert(right);

			start->AddTransition(epsilon, left->start_node);
			start->AddTransition(epsilon, right->start_node);
			left->end_node->AddTransition(epsilon,end);
			right->end_node->AddTransition(epsilon,end);

			nfa->start_node = start;
			nfa->end_node = end;
			return nfa;
		}
		template<typename TEntity, typename TToken>
		NFA<TEntity, TToken>* ThompsonConstruction<TEntity, TToken>::ConcatRegExpToNFA(ConcatRegularExpression<TEntity, TToken>* rex) {
			using NFAT = NFA<TEntity, TToken>;
			using NFANodeT = NFANode<TEntity, TToken>;
			using SetT = set<NFANodeT*>;
			using MapT = map<TEntity, SetT*>;
			NFAT* left = rex->left->ThompsonToNFA(this);
			NFAT* right = rex->right->ThompsonToNFA(this);
			left->end_node->SetState(false);
			right->end_node->SetState(false);

			NFAT* nfa = new NFAT();
			NFANodeT* start = new NFANodeT(), * end = new NFANodeT();
			end->SetState(true);
			nfa->sub_nfa.insert(left);
			nfa->sub_nfa.insert(right);


			start->AddTransition(epsilon, left->start_node);
			left->end_node->AddTransition(epsilon, right->start_node);
			right->end_node->AddTransition(epsilon, end);

			nfa->start_node = start;
			nfa->end_node = end;
			return nfa;
		}
		template<typename TEntity, typename TToken>
		NFA<TEntity, TToken>* ThompsonConstruction<TEntity, TToken>::KleeneClosureRegExpToNFA(KleenClosureRegularExpression<TEntity, TToken>* rex) {
			using NFAT = NFA<TEntity, TToken>;
			using NFANodeT = NFANode<TEntity, TToken>;
			using SetT = set<NFANodeT*>;
			using MapT = map<TEntity, SetT*>;
			NFAT* sub_nfa = rex->regular_expression->ThompsonToNFA(this);
			sub_nfa->end_node->SetState(false);

			NFAT* nfa = new NFAT();
			NFANodeT* start = new NFANodeT(), * end = new NFANodeT();
			end->SetState(true);
			nfa->sub_nfa.insert(sub_nfa);

			start->AddTransition(epsilon, sub_nfa->start_node);
			sub_nfa->end_node->AddTransition(epsilon, end);
			start->AddTransition(epsilon, end);
			sub_nfa->end_node->AddTransition(epsilon, sub_nfa->start_node);

			nfa->start_node = start;
			nfa->end_node = end;
			return nfa;
		}
	}
}