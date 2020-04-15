#pragma once

namespace CompilerPlus {
	namespace Utility {
		namespace FunctionTools {
			template<typename TResult, typename... TParamters>
			struct Lambda
			{
				virtual TResult operator()(TParamters...) = 0;
			};

			template<typename TResult, typename TParamter>
			struct SingleParamterLambda
			{
				virtual TResult operator()(TParamter) = 0;
			};


		}
	}
}