#include <iostream>
#include "thompson-construction.h"
#include "function-tools.h"
#include "dfa-simplify.h"
#include "easy-use.h"
using std::cout;
using std::endl;
using namespace CompilerPlus::Lexical;
using namespace CompilerPlus::Utility;
void demo();
int main() {
	demo();
	return 0;
}



void demo() {
	TransitionSystem<int, string>* sys = new StringTransitionSystem();
	ThompsonConstruction<int, string> thompson(sys);
	DirectDFAConstruction<int, string> direct_construction(sys);
	string a = string("a");
	string b = string("b");
	/*
	 * Example regular expression: a(a|b)*a(a|b)a
	 
	 
	 */
	//auto rex = Char(a)->Concat(Char(a)->Union(Char(b))->KleeneClosure())->Concat(Char(a))->Concat(Char(a)->Union(Char(b)))->Concat(Char(a));
	auto rex = Sequence(vector<RegExp*>({ Char(a),Kleene(AnyOf(vector<RegExp*>({Char(a),Char(b)}))), Char(a),Union(Char(a),Char(b)),Char(a) }));
	cout << rex->ToString() << endl;
	//auto rex = Char(a)->KleeneClosure();
	//ConcatRegularExpression<int, string>* rex = (ConcatRegularExpression<int, string>*)(new SingleTokenRegularExpression<int, string>(new string("a")))->Concat(new SingleTokenRegularExpression<int, string>(new string("b")));

	//auto nfa = th.ConcatRegExpToNFA(rex);
	//*
	auto nfa = rex->ThompsonToNFA(&thompson);
	auto method = SubsetConstructionMethod<int, string>(sys);
	auto dfa = method.Construct(nfa);
	cout << nfa << endl;
	//*/

	/*
	auto method = DirectDFAConstruction<int, string>(sys);
	auto dfa = method.Construct(rex);
	//*/
	DFASimplifyMethod<int, string> simplify_method;
	simplify_method.Simplify(dfa);
	cout << dfa << endl;
	StringSequence string_seq{string("aaaaaaaaaa")};

	cout << dfa->Match(&string_seq);

	//NFA<int, string>::Free(nfa);
	DFA<int, string>::Free(dfa);
	delete rex;
}