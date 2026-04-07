#include "aopc-cli/db/itemDatabase.hpp"
#include <iostream>


ItemDatabase::ItemDatabase(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

ItemDatabase::~ItemDatabase() {
    if (db) sqlite3_close(db);
}

std::string ItemDatabase::getItemIdByDisplayName(const std::string& displayName) {
    if (!db) return "";

    sqlite3_stmt* stmt;
    std::string internalId;

    // Prepare the SQL query to retrieve the internal ID based on the display name (case-insensitive)
    const char* query = "SELECT internal_id FROM Items WHERE display_name = ? COLLATE NOCASE LIMIT 1";
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, displayName.c_str(), -1, SQLITE_TRANSIENT); // Bind the display name parameter

        // Execute the query and check if a row is returned
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            internalId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
    }
    sqlite3_finalize(stmt);

    return internalId;
}

std::vector<RecipeIngredient> ItemDatabase::getRecipeIngredients(const std::string& craftedItemId) {
    std::vector<RecipeIngredient> ingredients;

    if (!db) return ingredients;
    
    sqlite3_stmt* stmt;

    // Prepare the SQL query to retrieve the recipe ingredients for the given crafted item ID
    const char* query = "SELECT material_item_id, quantity FROM Recipes WHERE crafted_item_id = ?";
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, craftedItemId.c_str(), -1, SQLITE_TRANSIENT); // Bind the crafted item ID parameter

        // Execute the query and populate the ingredients vector with the results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RecipeIngredient ingredient;
            ingredient.materialItemId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            ingredient.quantity = sqlite3_column_int(stmt, 1);
            ingredients.push_back(ingredient);
        }
    }
    sqlite3_finalize(stmt);
    
    return ingredients;
}