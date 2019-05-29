#pragma once
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <iterator>
#include <cstddef>
#include <optional>
#include <memory>
#include <functional>
#include "Pseudonyms.h"

template<class data_t, class data_iter_t, class = is_arithm<data_t>>
class Node;


template<class iter_t>
class Subrange;

template<class data_t, class = is_floating<data_t>>
class DecisionTree
{
public:

	template<class InputIt>
	static  data_t calcRSS(InputIt first, InputIt last) {
		const data_t avarageValue = std::accumulate(first, last, static_cast < data_t>(0)) / (last - first);

		data_t RSS = 0;
		for (; first != last; first++) {
			RSS += pow(*first - avarageValue, 2);
		}

		return RSS;
	}

	template<class InputIt>
	static std::vector<data_t> calcRSSForAllGroups(InputIt first,
		InputIt last) {

		std::vector<data_t> valuesRSS;
		valuesRSS.reserve(last- first);
		
		for (auto bound = first + 1; bound != last; bound++) {
			valuesRSS.push_back(calcRSS(first, bound)
				+ calcRSS(bound, last));
		}

		return valuesRSS;	
	}

	template<class InputIt>
	static auto partitionPointMinRSS(InputIt first, InputIt last){

		const auto& valuesRSS =	calcRSSForAllGroups(first,last);
		const auto minRssIter = std::min_element(valuesRSS.cbegin(), valuesRSS.cend());
		const auto offset = std::distance(valuesRSS.cbegin(), minRssIter);
		return first + offset;
	}

	template<class data_t, class data_iter_t>
	static void pushYes(Node<data_t, data_iter_t>& currNode,
		const Node<data_t, data_iter_t>& nodeToPush) {
		currNode.yes = std::make_shared< Node<data_t, data_iter_t>>(nodeToPush);
	}

	template<class data_t, class data_iter_t>
	static void pushNo(Node<data_t, data_iter_t>& currNode,
		const Node<data_t, data_iter_t>& nodeToPush) {
		currNode.no = std::make_shared< Node<data_t, data_iter_t>>(nodeToPush);
	}

	template<class InputIt>
	static void build(InputIt first, InputIt last) {
		const auto point = partitionPointMinRSS(first, last);
		if (last - first < 5)
			return;

		// build
	}

private:
//	Tree<data_t> tree;
};

template<class iter_t>
class Subrange {
public:
	iter_t first;
	iter_t last;
public:
	Subrange(const iter_t& first, const iter_t& last) :first(first), last(last) {};
};

template<class data_t, class data_iter_t, class = is_arithm<data_t>>
class Node {
	public:
	std::function<bool(data_t)> question;
	Subrange<data_iter_t> answers;
	std::shared_ptr<Node<data_t,data_iter_t>> yes;
	std::shared_ptr<Node<data_t,data_iter_t>> no;

public:
	Node(const Subrange<data_iter_t>& answers) : answers(answers) {};
	Node<data_t,data_iter_t>& operator=(const Node<data_t, data_iter_t>& other) {

		if (this == &other) {
			return *this;
		}
		question = other.question;
		answers = other.answers;
		return *this;
	}

};

//template<class data_t, class = is_arithm<data_t>>
//class Tree {
//	Node<data_t> root;
//};
