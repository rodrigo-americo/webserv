/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   container_test.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 15:29:38 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/22 13:44:29 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <vector>
# include <map>
# include <set>
# include <iostream>
# include <memory>

#include "container.hpp"

void	test_vector();
void	test_vector_ordered();
void	test_vector_unique();
void	test_map();
void	test_set();

int main()
{
	int *test_ptr = new int(10);
	std::shared_ptr<int> test_shared = std::shared_ptr<int>(test_ptr);
	std::weak_ptr<int> test_weak = test_shared;

	std::cout << "testing ptrs: raw_ptr: " << test_ptr << ", shared_ptr: " << test_shared << ", weak_ptr: " << test_weak.lock() << "\n";

	test_vector();
	test_vector_ordered();
	test_vector_unique();
	test_map();
	test_set();
	return 0;
}

void	test_vector()
{
	std::cout << "Testing vector:\n\n";
	using vect_test = structure::vector<size_t>;
	vect_test vect;

	for (size_t i = 0; i < 1000; i++)
		vect.getContainer().push_back(i);


	std::cout << "Testing [test_vector] forEach:\n";
	vect.forEach([](const vect_test::value_type &value, bool&){
		std::cout << value << ", ";
	});
	std::cout << "\n";

	std::cout << "Testing [test_vector] filter:\n";
	auto filtered = vect.filter([](const vect_test::value_type &value){
		return value % 2 == 0 && value > 50 && value < 870;
	});
	for (auto item : filtered)
		std::cout << *item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_vector] map:\n";
	auto mapped = vect.map<std::string>([](const vect_test::value_type &value) -> std::string {
		return "str_" + std::to_string(value);
	});
	for (auto item : mapped)
		std::cout << item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_vector] reduce_ref:\n";
	size_t	sum = 0;
	vect.reduce_ref([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, sum);
	std::cout << "sum: " << sum << "\n";

	std::cout << "Testing [test_vector] reduce:\n";
	size_t	sum2 = vect.reduce([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, (size_t)0);
	std::cout << "sum2: " << sum2 << "\n";
}

void	test_vector_ordered()
{
	std::cout << "Testing vector_ordered:\n\n";
	using vect_test = structure::vector_ordered<size_t>;
	vect_test vect;

	for (size_t i = 1000; i > 0; i--)
		vect.insert(i);

	std::cout << "Testing [test_vector_ordered] find(500):\n";
	auto item = vect.find(500);
	std::cout << "finded item: " << (item ? std::to_string(*item) : "nullptr");
	std::cout << "\n";

	std::cout << "Testing [test_vector_ordered] forEach:\n";
	vect.forEach([](const vect_test::value_type &value, bool&){
		std::cout << value << ", ";
	});
	std::cout << "\n";

	std::cout << "Testing [test_vector_ordered] filter:\n";
	auto filtered = vect.filter([](const vect_test::value_type &value){
		return value % 2 == 0 && value > 50 && value < 870;
	});
	for (auto item : filtered)
		std::cout << *item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_vector_ordered] map:\n";
	auto mapped = vect.map<std::string>([](const vect_test::value_type &value) -> std::string {
		return "str_" + std::to_string(value);
	});
	for (auto item : mapped)
		std::cout << item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_vector_ordered] reduce_ref:\n";
	size_t	sum = 0;
	vect.reduce_ref([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, sum);
	std::cout << "sum: " << sum << "\n";

	std::cout << "Testing [test_vector_ordered] reduce:\n";
	size_t	sum2 = vect.reduce([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, (size_t)0);
	std::cout << "sum2: " << sum2 << "\n";
}

void	test_vector_unique()
{
	std::cout << "Testing vector_unique:\n\n";
	using vect_test = structure::vector_unique<size_t>;
	vect_test vect;

	for (size_t i = 1000; i > 0; i--)
		vect.insert(i);
	for (size_t i = 1000; i > 0; i--)
		vect.insert(i);

	std::cout << "Testing [test_vector_unique] find(500):\n";
	auto item = vect.find(500);
	std::cout << "finded item: " << (item ? std::to_string(*item) : "nullptr");
	std::cout << "\n";

	std::cout << "Testing [test_vector_unique] forEach:\n";
	vect.forEach([](const vect_test::value_type &value, bool&){
		std::cout << value << ", ";
	});
	std::cout << "\n";

	std::cout << "Testing [test_vector_unique] filter:\n";
	auto filtered = vect.filter([](const vect_test::value_type &value){
		return value % 2 == 0 && value > 50 && value < 870;
	});
	for (auto item : filtered)
		std::cout << *item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_vector_unique] map:\n";
	auto mapped = vect.map<std::string>([](const vect_test::value_type &value) -> std::string {
		return "str_" + std::to_string(value);
	});
	for (auto item : mapped)
		std::cout << item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_vector_unique] reduce_ref:\n";
	size_t	sum = 0;
	vect.reduce_ref([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, sum);
	std::cout << "sum: " << sum << "\n";

	std::cout << "Testing [test_vector_unique] reduce:\n";
	size_t	sum2 = vect.reduce([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, (size_t)0);
	std::cout << "sum2: " << sum2 << "\n";
}

void	test_map()
{
	std::cout << "\nTesting map:\n\n";
	using vect_test = structure::map<std::string, size_t>;
	vect_test vect;

	for (size_t i = 0; i < 1000; i++)
		vect.getContainer()["item_" + std::to_string(i)] = i * 3;


	std::cout << "Testing [test_map] forEach:\n";
	vect.forEach([](const vect_test::value_type &value, bool&){
		std::cout << value.first + ": " << value.second << ", ";
	});
	std::cout << "\n";

	std::cout << "Testing [test_map] filter:\n";
	auto filtered = vect.filter([](const vect_test::value_type &value){
		return value.second % 2 == 0 && value.second > 50 && value.second < 870;
	});
	for (auto item : filtered)
		std::cout << item->first + ": " << item->second << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_map] map:\n";
	auto mapped = vect.map<std::string>([](const vect_test::value_type &value) -> std::string {
		return value.first +  ": str_" + std::to_string(value.second);
	});
	for (auto item : mapped)
		std::cout << item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_map] reduce_ref:\n";
	size_t	sum = 0;
	vect.reduce_ref([](const vect_test::value_type &value, size_t &acc) {
		acc += value.second;
	}, sum);
	std::cout << "sum: " << sum << "\n";

	std::cout << "Testing [test_map] reduce:\n";
	size_t	sum2 = vect.reduce([](const vect_test::value_type &value, size_t &acc) {
		acc += value.second;
	}, (size_t)0);
	std::cout << "sum2: " << sum2 << "\n";
}

void	test_set()
{
	std::cout << "Testing set:\n\n";
	using vect_test = structure::set<size_t>;
	vect_test vect;

	for (size_t i = 0; i < 1000; i++)
		vect.getContainer().insert(i);


	std::cout << "Testing [test_set] forEach:\n";
	vect.forEach([](const vect_test::value_type &value, bool&){
		std::cout << value << ", ";
	});
	std::cout << "\n";

	std::cout << "Testing [test_set] filter:\n";
	auto filtered = vect.filter([](const vect_test::value_type &value){
		return value % 2 == 0 && value > 50 && value < 870;
	});
	for (auto item : filtered)
		std::cout << *item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_set] map:\n";
	auto mapped = vect.map<std::string>([](const vect_test::value_type &value) -> std::string {
		return "str_" + std::to_string(value);
	});
	for (auto item : mapped)
		std::cout << item << ", ";
	std::cout << "\n";

	std::cout << "Testing [test_set] reduce_ref:\n";
	size_t	sum = 0;
	vect.reduce_ref([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, sum);
	std::cout << "sum: " << sum << "\n";

	std::cout << "Testing [test_set] reduce:\n";
	size_t	sum2 = vect.reduce([](const vect_test::value_type &value, size_t &acc) {
		acc += value;
	}, (size_t)0);
	std::cout << "sum2: " << sum2 << "\n";
}
