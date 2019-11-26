#include "afx.h"
#include "SmMarket.h"
#include "SmProduct.h"

SmMarket::SmMarket()
{
}


SmMarket::~SmMarket()
{
	for (auto it = _CategoryList.begin(); it != _CategoryList.end(); ++it) {
		delete* it;
	}
}

SmProduct* SmMarket::AddProduct(std::string code)
{
	SmProduct* cat = new SmProduct();
	cat->Code(code);
	_CategoryList.emplace_back(cat);
	return cat;
}

SmProduct* SmMarket::FindProduct(std::string code)
{
	for (auto it = _CategoryList.begin(); it != _CategoryList.end(); ++it) {
		SmProduct* cat = *it;
		if (cat->Code().compare(code) == 0) {
			return cat;
		}
	}

	return nullptr;
}

SmProduct* SmMarket::FindAddProduct(std::string code)
{
	SmProduct* product = FindProduct(code);
	if (!product) {
		product = AddProduct(code);
	}

	return product;
}
