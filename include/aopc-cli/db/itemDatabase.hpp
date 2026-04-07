#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>


struct RecipeIngredient {
    std::string materialItemId;
    int quantity;
};

class ItemDatabase {
    public:
        ItemDatabase(const std::string& dbPath);
        ~ItemDatabase();

        std::string getItemIdByDisplayName(const std::string& displayName);

        std::vector<RecipeIngredient> getRecipeIngredients(const std::string& craftedItemId);

    private:
        sqlite3* db;
};