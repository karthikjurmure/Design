#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

enum class ProductCategory
{
    ELECTRONICS,
    GROCERY,
    CLOTHING
};

class Product
{
private:
    string sku;
    string name;
    double price;
    int quantity;
    int threshold;
    ProductCategory category;

public:
    Product(string sku,
            string name,
            double price,
            int quantity,
            int threshold,
            ProductCategory category)
        : sku(sku),
          name(name),
          price(price),
          quantity(quantity),
          threshold(threshold),
          category(category)
    {
    }

    string getSku() const
    {
        return sku;
    }

    string getName() const
    {
        return name;
    }

    int getQuantity() const
    {
        return quantity;
    }

    int getThreshold() const
    {
        return threshold;
    }

    void addStock(int qty)
    {
        quantity += qty;
    }

    bool removeStock(int qty)
    {
        if (qty > quantity)
            return false;

        quantity -= qty;
        return true;
    }
};

class Warehouse
{
private:
    int id;
    string name;
    string location;

    unordered_map<string, Product*> products;

public:
    Warehouse(int id,
              string name,
              string location)
        : id(id),
          name(name),
          location(location)
    {
    }

    void addProduct(Product* product)
    {
        products[product->getSku()] = product;
    }

    void removeProduct(string sku, int qty)
    {
        if (products.count(sku))
        {
            bool success =
                products[sku]->removeStock(qty);

            if (success)
                cout << "Removed "
                     << qty
                     << " units\n";
            else
                cout << "Insufficient stock\n";
        }
    }

    void displayInventory()
    {
        cout << "\nInventory of "
             << name << "\n";

        for (auto& p : products)
        {
            cout << p.second->getName()
                 << " -> "
                 << p.second->getQuantity()
                 << endl;
        }
    }

    vector<Product*> getProducts()
    {
        vector<Product*> result;

        for (auto& p : products)
            result.push_back(p.second);

        return result;
    }
};

class InventoryManager
{
private:
    static InventoryManager* instance;

    vector<Warehouse*> warehouses;

    InventoryManager() {}

public:
    static InventoryManager* getInstance()
    {
        if (instance == nullptr)
        {
            instance =
                new InventoryManager();
        }

        return instance;
    }

    void addWarehouse(Warehouse* warehouse)
    {
        warehouses.push_back(warehouse);
    }

    void checkLowStock()
    {
        cout << "\nLow Stock Products:\n";

        for (auto warehouse : warehouses)
        {
            for (auto product :
                 warehouse->getProducts())
            {
                if (product->getQuantity()
                    < product->getThreshold())
                {
                    cout << product->getName()
                         << " needs replenishment\n";
                }
            }
        }
    }
};

InventoryManager*
InventoryManager::instance = nullptr;

int main()
{
    InventoryManager* manager =
        InventoryManager::getInstance();

    Warehouse* hyd =
        new Warehouse(
            1,
            "Hyderabad Warehouse",
            "Hyderabad");

    Product* laptop =
        new Product(
            "LAP123",
            "Laptop",
            50000,
            20,
            10,
            ProductCategory::ELECTRONICS);

    Product* mobile =
        new Product(
            "MOB111",
            "Mobile",
            30000,
            5,
            10,
            ProductCategory::ELECTRONICS);

    hyd->addProduct(laptop);
    hyd->addProduct(mobile);

    manager->addWarehouse(hyd);

    hyd->displayInventory();

    manager->checkLowStock();

    return 0;
}
