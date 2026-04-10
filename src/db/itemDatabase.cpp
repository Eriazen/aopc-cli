#include "aopc-cli/db/itemDatabase.hpp"
#include <iostream>


// Constructor: Opens the SQLite database at the specified path
// If the database cannot be opened, it logs an error message and sets the db pointer to nullptr
ItemDatabase::ItemDatabase(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
        m_db = nullptr;
    }
}

// Destructor: Closes the SQLite database connection if it was successfully opened
ItemDatabase::~ItemDatabase() {
    if (m_db) sqlite3_close(m_db);
}

// Retrieves the internal item ID from the database based on the provided display name (case-insensitive)
// If the item is not found or if the database connection is not available, it returns an empty string
std::string ItemDatabase::getItemIdByDisplayName(const std::string& displayName) {
    if (!m_db) return ""; // Return empty string if database connection is not available

    sqlite3_stmt* stmt;
    std::string internalId;

    // Prepare the SQL query to retrieve the internal ID based on the display name (case-insensitive)
    const char* query = "SELECT internal_id FROM Items WHERE display_name = ? COLLATE NOCASE LIMIT 1";
    if (sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, displayName.c_str(), -1, SQLITE_TRANSIENT); // Bind the display name parameter

        // Execute the query and retrieve the internal ID if a matching item is found
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            internalId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
    }
    sqlite3_finalize(stmt); // Finalize the prepared statement to free resources

    return internalId;
}

// Retrieves the list of recipe ingredients for a given crafted item ID from the database
// If the database connection is not available, it returns an empty vector
std::vector<RecipeIngredient> ItemDatabase::getRecipeIngredients(const std::string& craftedItemId) {
    std::vector<RecipeIngredient> ingredients;

    if (!m_db) return ingredients; // Return empty vector if database connection is not available
    
    sqlite3_stmt* stmt;

    // Prepare the SQL query to retrieve the recipe ingredients for the given crafted item ID
    const char* query = "SELECT material_item_id, quantity FROM Recipes WHERE crafted_item_id = ?";
    if (sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) == SQLITE_OK) {
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