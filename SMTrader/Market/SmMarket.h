#pragma once
#include <string>
#include <vector>
class SmProduct;
class SmMarket
{
public:
	SmMarket();
	~SmMarket();
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	/// <summary>
	/// ǰ���� �߰��Ѵ�.
	/// </summary>
	/// <param name="code"></param>
	/// <returns></returns>
	SmProduct* AddProduct(std::string code);
	SmProduct* FindProduct(std::string code);
	SmProduct* FindAddProduct(std::string code);
	std::vector<SmProduct*>& GetProductList() {
		return _ProductList;
	}
	std::string Code() const { return _Code; }
	void Code(std::string val) { _Code = val; }
private:
	std::string _Name;
	std::string _Code;
	std::vector<SmProduct*> _ProductList;
};

