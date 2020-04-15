#pragma once
#include <set>
#include <string>
namespace CompilerPlus {
	namespace Utility {

		using std::set;
		using std::string;
		template<typename T>
		void SetMerge(set<T>* main_set, set<T>* sub_set) {
			for (T item : *sub_set) {
				main_set->insert(item);
			}
		}

		template<typename T>
		bool SetEquals(set<T>* one, set<T>* other) {
			if (one->size() != other->size()) {
				return false;
			}
			auto one_iter = one->begin();
			auto other_iter = other->begin();
			while (one_iter != one->end()) {
				if (*one_iter != *other_iter) {
					return false;
				}
				one_iter++, other_iter++;
			}
			return true;
		}
		template<typename T>
		std::ostream& operator<<(std::ostream& os, std::set<T>& st);
		template<typename T>
		class Iterator
		{
		public:
			Iterator();
			~Iterator();
			virtual T* Next() = 0;
			virtual bool HasNext() = 0;
		private:

		};
		template<typename T>
		class Iterable
		{
		public:
			Iterable();
			~Iterable();
			virtual Iterator<T>* GetIterator() = 0;
		private:

		};
		template<typename T>
		class ArrayIterator;
		template<typename T>
		class Array : public Iterable<T>
		{
		public:
			Array(T* data_source, size_t size);
			size_t Size();
			Iterator<T>* GetIterator();
			~Array();
			friend class ArrayIterator<T>;
		private:
			size_t size = 0;
			T* head = nullptr;
		};

		template<typename T>
		class ArrayIterator : public Iterator<T>
		{
		public:
			ArrayIterator(Array<T>* array_list);
			bool HasNext();
			T* Next();
			~ArrayIterator();
		private:
			Array<T>* array_list = nullptr;
			size_t index = 0;
		};

		class StringSequence : public Iterable<string>, public Iterator<string> {
		public:
			StringSequence(string& str):str(str){
				size = str.size();
				strs = new string*[str.size()];
				for (size_t i = 0; i < str.size(); i++) {
					strs[i] = new string(1, str[i]);
				}
			}
			~StringSequence(){
				for (size_t i = 0; i < str.size(); i++) {
					delete strs[i];
				}
				delete strs;
			}
			virtual Iterator<string>* GetIterator() override;
			virtual string* Next() override;
			virtual bool HasNext() override;
		private:
			const string& str;
			size_t index = 0;
			size_t size = 0;
			string** strs = nullptr;
		};

		template<typename T>
		inline Iterator<T>::Iterator()
		{
		}

		template<typename T>
		inline Iterator<T>::~Iterator()
		{
		}

		template<typename T>
		inline Iterable<T>::Iterable()
		{
		}

		template<typename T>
		inline Iterable<T>::~Iterable()
		{
		}

		template<typename T>
		inline Array<T>::Array(T* data_source, size_t size) : head(data_source), size(size)
		{
		}

		template<typename T>
		inline size_t Array<T>::Size()
		{
			return this->size;
		}

		template<typename T>
		inline Iterator<T>* Array<T>::GetIterator()
		{
			return new ArrayIterator<T>(this);
		}

		template<typename T>
		inline Array<T>::~Array()
		{
		}

		template<typename T>
		inline ArrayIterator<T>::ArrayIterator(Array<T>* array_list) : array_list(array_list), index(0)
		{
		}

		template<typename T>
		inline bool ArrayIterator<T>::HasNext()
		{
			return this->index < this->array_list->size;
		}

		template<typename T>
		inline T* ArrayIterator<T>::Next()
		{
			return this->array_list->head + this->index++;
		}

		template<typename T>
		inline ArrayIterator<T>::~ArrayIterator()
		{
		}

		template<typename T>
		std::ostream& operator<<(std::ostream& os, std::set<T>& st)
		{
			os << "{";
			for (auto item : st) {
				os << item << ", ";
			}
			os << "}";
			return os;
		}

}
}