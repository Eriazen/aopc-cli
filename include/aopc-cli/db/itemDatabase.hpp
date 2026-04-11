#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>


struct RecipeIngredient {
    std::string materialItemId;
    std::string materialItemName;
    int quantity { 0 };
    int marketPrice { 0 };
};

struct ItemInfo {
    std::string itemId;
    int silverCost { 0 };
    int craftingFocus { 0 };
};

class ItemDatabase {
    public:
        ItemDatabase(const std::string& dbPath);
        ~ItemDatabase();

        ItemInfo getItemInfoByDisplayName(const std::string& displayName);

        std::vector<RecipeIngredient> getRecipeIngredients(const std::string& craftedItemId);

    private:
        sqlite3* m_db;
};