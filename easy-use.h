#pragma once
#include "regular-expression.h"
#include <vector>
namespace CompilerPlus {
	namespace Lexical {
		using RegExp = RegularExpression<int, string>;
		using std::vector;
		RegExp* Char(string s) {
			return new SingleTokenRegularExpression<int, string>(new string(s));
		}

		RegExp* Concat(RegExp* a, RegExp* b) {
			return new ConcatRegularExpression<int, string>(a, b);
		}

		RegExp* Kleene(RegExp* rex) {
			return new KleenClosureRegularExpression<int, string>(rex);
		}

		RegExp* Union(RegExp* a, RegExp* b) {
			return new UnionRegularExpression<int, string>(a, b);
		}
		RegExp* _Sequence(vector<RegExp*>& rexs, size_t offset) {
			if (rexs.size() - offset == 2) {
				return Concat(rexs[offset], rexs[offset + 1]);
			}
			return Concat(rexs[offset], _Sequence(rexs, offset + 1));
		}

		RegExp* Sequence(vector<RegExp*> rexs) {
			return _Sequence(rexs, 0);
		}

		RegExp* _AnyOf(vector<RegExp*>& rexs, size_t offset) {
			if (rexs.size() - offset < 2) {
				return nullptr;
			}
			if (rexs.size() - offset == 2) {
				return Union(rexs[offset], rexs[offset + 1]);
			}
			return Union(rexs[offset], _AnyOf(rexs, offset + 1));
		}

		RegExp* AnyOf(vector<RegExp*> rexs) {
			if (rexs.size() == 1) {
				return rexs[0];
			}
			else if (rexs.size() == 0) {
				return new EpsilonRegularExpression<int, string>();
			}
			return _AnyOf(rexs, 0);
		}
		

	}
}