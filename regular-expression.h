#pragma once
#include <string>
#include "nfa.h"

namespace CompilerPlus {
	namespace Lexical {
		using std::string;
		using CompilerPlus::Utility::Iterable;
		using CompilerPlus::Utility::Iterator;
		using Identity = int;

		template<typename TEntity, typename TToken = string>
		class TransitionSystem
		{
		public:
			TransitionSystem() {}
			~TransitionSystem() {}
			virtual TToken Epsilon() = 0;
			virtual TToken AcceptingToken() = 0;
		private:

		};

		class StringTransitionSystem : public TransitionSystem<int> {
		public:
			string Epsilon() { return string("!Epsilon Token"); }
			string AcceptingToken() { return string("!End Token"); }
		};

		template<typename TEntity, typename TToken>
		class DirectDFAConstruction;

		template<typename TEntity, typename TToken = string>
		class EpsilonRegularExpression;


		template<typename TEntity, typename TToken = string>
		class SingleTokenRegularExpression;

		template<typename TEntity, typename TToken = string>
		class UnionRegularExpression;

		template<typename TEntity, typename TToken = string>
		class KleenClosureRegularExpression;

		template<typename TEntity, typename TToken = string>
		class ConcatRegularExpression;

		template<typename TEntity, typename TToken = string>
		class ThompsonConstruction {
			using NFAT = NFA<TEntity, TToken>;
			using NFANodeT = NFANode<TEntity, TToken>;
			using SetT = set<NFANodeT*>;
			using MapT = map<TEntity, SetT*>;
		private:
			TToken epsilon;
		public:
			TransitionSystem<TEntity, TToken>* value_system;
			ThompsonConstruction(TransitionSystem<TEntity, TToken>* value_system) : value_system(value_system) {
				epsilon = value_system->Epsilon();
			}
			NFAT* EpsilonRegExpToNFA(EpsilonRegularExpression<TEntity, TToken>* rex);
			NFAT* SingleTokenRegExpToNFA(SingleTokenRegularExpression<TEntity, TToken>* rex);
			NFAT* UnionRegExpToNFA(UnionRegularExpression<TEntity, TToken>* rex);
			NFAT* ConcatRegExpToNFA(ConcatRegularExpression<TEntity, TToken>* rex);
			NFAT* KleeneClosureRegExpToNFA(KleenClosureRegularExpression<TEntity, TToken>* rex);
		};

		template<typename TEntity, typename TToken = string>
		class RegularExpression
		{
		public:
			using RegExp = RegularExpression<TEntity, TToken>;
			friend class ThompsonConstruction<TEntity, TToken>;
			friend class DirectDFAConstruction<TEntity, TToken>;
			RegularExpression(){}
			~RegularExpression(){}
			RegularExpression<TEntity, TToken>* Union(RegularExpression<TEntity, TToken>* other);
			RegularExpression<TEntity, TToken>* Concat(RegularExpression<TEntity, TToken>* other);
			RegularExpression<TEntity, TToken>* KleeneClosure();
			virtual NFA<TEntity, TToken>* ThompsonToNFA(ThompsonConstruction<TEntity, TToken>* thompson) = 0;
			bool Nullable() {
				if (!nullable_calculated) {
					nullable = this->NullableFirstCalculate();
					nullable_calculated = true;
				}
				return nullable;
			}
			bool nullable_calculated = false;
			bool nullable = false;
			virtual bool NullableFirstCalculate() = 0;
			virtual string ToString() = 0;
			virtual void GetFirstAndLastPosition(DirectDFAConstruction<TEntity, TToken>* dc_method) = 0;
			set<Identity> first_postion;
			set<Identity> last_position;
			set<Identity> follow_position;
		protected:
		private:
		};

		template<typename TEntity, typename TToken>
		class EpsilonRegularExpression: public RegularExpression<TEntity, TToken>
		{
		public:
			EpsilonRegularExpression() {}
			~EpsilonRegularExpression() {}
			string ToString() {
				return string("!Epsilon RegExp");
			}
			NFA<TEntity, TToken>* ThompsonToNFA(ThompsonConstruction<TEntity, TToken>* thompson) {
				return thompson->EpsilonRegExpToNFA(this);
			}
		protected:
			bool NullableFirstCalculate() { return true; }
			void GetFirstAndLastPosition(DirectDFAConstruction<TEntity, TToken>* dc_method) {}
		private:

		};

		template<typename TEntity, typename TToken>
		class SingleTokenRegularExpression: public RegularExpression<TEntity, TToken>
		{
		public:
			TToken* token;
			SingleTokenRegularExpression(TToken* character): token(character){}
			~SingleTokenRegularExpression() { }
			string ToString() {
				return string(*this->token);
			}
		protected:
			NFA<TEntity, TToken>* ThompsonToNFA(ThompsonConstruction<TEntity, TToken>* thompson) {
				return thompson->SingleTokenRegExpToNFA(this);
			}
			bool NullableFirstCalculate() { return false; }
			void GetFirstAndLastPosition(DirectDFAConstruction<TEntity, TToken>* dc_method) {
				Node<TToken>* node = new Node<TToken>();
				node->entity = this->token;
				if (dc_method->value_system->AcceptingToken() != *this->token) {
					dc_method->token_map[node->Id] = node;
				}
				this->first_postion.insert(node->Id);
				this->last_position.insert(node->Id);
			}
		private:

		};

		template<typename TEntity, typename TToken>
		class UnionRegularExpression: public RegularExpression<TEntity, TToken>
		{
		public:
			RegularExpression<TEntity, TToken>* left, * right;
			UnionRegularExpression(RegularExpression<TEntity,TToken>* left, RegularExpression<TEntity, TToken>* right) : left(left), right(right){}
			~UnionRegularExpression() { if (this->left != nullptr) delete this->left; if (this->right != nullptr) delete this->right; }
			string ToString() {
				return string("((").append(this->left->ToString()).append(string(")+(")).append(this->right->ToString()).append(string("))"));
			}
		protected:
			NFA<TEntity, TToken>* ThompsonToNFA(ThompsonConstruction<TEntity, TToken>* thompson) {
				return thompson->UnionRegExpToNFA(this);
			}
			bool NullableFirstCalculate() { return this->left->NullableFirstCalculate() || this->right->NullableFirstCalculate(); }
			void GetFirstAndLastPosition(DirectDFAConstruction<TEntity, TToken>* dc_method) {
				this->left->GetFirstAndLastPosition(dc_method);
				this->right->GetFirstAndLastPosition(dc_method);
				SetMerge<Identity>(&this->first_postion, &left->first_postion);
				SetMerge<Identity>(&this->first_postion, &right->first_postion);
				SetMerge<Identity>(&this->last_position, &left->last_position);
				SetMerge<Identity>(&this->last_position, &right->last_position);
			}
		private:

		};

		template<typename TEntity, typename TToken>
		class KleenClosureRegularExpression: public RegularExpression<TEntity, TToken>
		{
		public:
			RegularExpression<TEntity, TToken>* regular_expression;
			KleenClosureRegularExpression(RegularExpression<TEntity, TToken>* regular_expression) : regular_expression(regular_expression){ }
			~KleenClosureRegularExpression() { if (this->regular_expression != nullptr) delete this->regular_expression; }
			string ToString() {
				return string("(").append(this->regular_expression->ToString()).append(string(")*"));
			}
		protected:
			NFA<TEntity, TToken>* ThompsonToNFA(ThompsonConstruction<TEntity, TToken>* thompson) {
				return thompson->KleeneClosureRegExpToNFA(this);
			}
			bool NullableFirstCalculate() { return true; }
			void GetFirstAndLastPosition(DirectDFAConstruction<TEntity, TToken>* dc_method) {
				this->regular_expression->GetFirstAndLastPosition(dc_method);
				SetMerge<Identity>(&this->first_postion, &this->regular_expression->first_postion);
				SetMerge<Identity>(&this->last_position, &this->regular_expression->last_position);
				for (auto last_id : this->last_position) {
					for (auto first_id : this->first_postion) {
						dc_method->follow_position[last_id].insert(first_id);
					}
				}
			}
		private:

		};

		template<typename TEntity, typename TToken>
		class ConcatRegularExpression: public RegularExpression<TEntity, TToken>
		{
		public:
			RegularExpression<TEntity, TToken>* left, * right;
			ConcatRegularExpression(RegularExpression<TEntity, TToken>* left, RegularExpression<TEntity, TToken>* right) : left(left), right(right) {}
			~ConcatRegularExpression() { if (this->left != nullptr) delete this->left; if (this->right != nullptr) delete this->right; }
			string ToString() {
				return this->left->ToString().append(this->right->ToString());
			}
		protected:
			NFA<TEntity, TToken>* ThompsonToNFA(ThompsonConstruction<TEntity, TToken>* thompson) {
				return thompson->ConcatRegExpToNFA(this);
			}
			bool NullableFirstCalculate() { return this->left->Nullable() && this->right->Nullable(); }
			void GetFirstAndLastPosition(DirectDFAConstruction<TEntity, TToken>* dc_method) {
				this->left->GetFirstAndLastPosition(dc_method);
				this->right->GetFirstAndLastPosition(dc_method);
				SetMerge<Identity>(&this->first_postion, &this->left->first_postion);
				SetMerge<Identity>(&this->last_position, &this->right->last_position);
				if (this->left->Nullable()) {
					SetMerge<Identity>(&this->first_postion, &this->right->first_postion);
				}
				if (this->right->Nullable()) {
					SetMerge<Identity>(&this->last_position, &this->left->last_position);
				}
				for (auto last_id : this->left->last_position) {
					for (auto first_id : this->right->first_postion) {
						dc_method->follow_position[last_id].insert(first_id);
					}
				}
			}
		private:

		};
		template<typename TEntity, typename TToken>
		inline RegularExpression<TEntity, TToken>* RegularExpression<TEntity, TToken>::Union(RegularExpression<TEntity, TToken>* other)
		{
			return new UnionRegularExpression<TEntity, TToken>(this, other);
		}
		template<typename TEntity, typename TToken>
		inline RegularExpression<TEntity, TToken>* RegularExpression<TEntity, TToken>::Concat(RegularExpression<TEntity, TToken>* other)
		{
			return new ConcatRegularExpression<TEntity, TToken>(this, other);
		}
		template<typename TEntity, typename TToken>
		inline RegularExpression<TEntity, TToken>* RegularExpression<TEntity, TToken>::KleeneClosure()
		{
			return new KleenClosureRegularExpression<TEntity, TToken>(this);
		}
}
}
#include "direct-construction.h"