#include "Vector.hpp"
#include <iostream>
#include <cassert>

;

int main() {

	//select Level (1-4)
	const int Level = 4;

	//default_constructor, size, capacity
	if (Level >= 1)
	{
		Vector<char> v;
		assert(v.size() == 0);
		assert(v.capacity() >= 0);
		assert(v.Invariant());

		std::cout << "Level 1 completed" << std::endl;
	}

	//parameterized_contrctor, copy_constructor, copy_assignment 
	if (Level >= 2)
	{
		
		Vector<char> fox("fox");
		assert(fox.size() == 3);
		assert(fox == fox);

		Vector<char> bar(fox);
		assert(bar.size() == 3);
		assert(fox == bar);
		Vector<char> foo = bar;
		assert(foo == fox);

		std::cout << "Level 2 completed" << std::endl;
	}
	
	//push_back, pop_back, move_assignment
	if (Level >= 3)
	{
		Vector<char> fox("fox"), bar("");
		bar.push_back('f');
		assert(bar == "f");
		bar.push_back('o');
		assert(bar == "fo");
		fox.pop_back();
		assert(fox == "fo");
		assert(fox == bar);
		fox.push_back('x');
		fox.push_back('x');
		assert(fox == "foxx");
		
		Vector<char> fox2(std::move(fox));
		assert(fox2 == "foxx" && fox == "");
		
		std::cout << "Level 3 completed" << std::endl;
	}

	//iterator
	if (Level >= 4)
	{
		Vector<char> fox("ffooxx");
		Vector<char> ftest;
		for (auto it = fox.begin(); it < fox.end(); it += 2)
			ftest.push_back(*it);

		assert(ftest == "fox");

		for (auto it = fox.rbegin(); it < fox.rend(); it++)
			ftest.push_back(*it);

		assert(ftest == "foxxxooff");

		std::cout << "Level 4 completed" << std::endl;
	}

}