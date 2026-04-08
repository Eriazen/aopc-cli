import sqlite3
import json
import pathlib
from typing import Optional


db_path = pathlib.Path('../database')
db_path.mkdir(exist_ok=True)


def initialize_database(conn: sqlite3.Connection) -> None:
    """Create tables if they don't exist."""
    cursor = conn.cursor()
    
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS Items (
            internal_id TEXT PRIMARY KEY,
            display_name TEXT,
            silver_cost INTEGER,
            crafting_focus INTEGER
        )
    ''')

    cursor.execute('''
        CREATE TABLE IF NOT EXISTS Recipes (
            crafted_item_id TEXT,
            material_item_id TEXT,
            quantity INTEGER NOT NULL,
            PRIMARY KEY (crafted_item_id, material_item_id),
            FOREIGN KEY (crafted_item_id) REFERENCES Items(internal_id),
            FOREIGN KEY (material_item_id) REFERENCES Items(internal_id)
        )
    ''')
    
    conn.commit()


def parse_items_json(json_file: pathlib.Path) -> dict:
    """Parse the items JSON file and return a dictionary of items with crafting requirements."""
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    items_with_recipes = {}
    items_section = data.get('items', {})
    
    # Iterate through all item types (simpleitem, hideoutitem, etc.)
    for item_type, item_list in items_section.items():
        if item_type.startswith('@'):  # Skip attributes
            continue
        
        # Handle both single items and lists
        if not isinstance(item_list, list):
            item_list = [item_list]
        
        for item in item_list:
            if item is None or not isinstance(item, dict):
                continue
            
            unique_name = item.get('@uniquename')
            if not unique_name:
                continue
            
            # Check if item has crafting requirements
            if 'craftingrequirements' in item:
                crafting_reqs = item['craftingrequirements']
                
                items_with_recipes[unique_name] = {
                    'craftingrequirements': crafting_reqs,
                    'item_type': item_type
                }
            
            # Parse enchantments
            if 'enchantments' in item:
                enchantments = item['enchantments']
                if isinstance(enchantments, dict) and 'enchantment' in enchantments:
                    enchantment_list = enchantments['enchantment']
                    if not isinstance(enchantment_list, list):
                        enchantment_list = [enchantment_list]
                    
                    for enchantment in enchantment_list:
                        if enchantment is None or not isinstance(enchantment, dict):
                            continue
                        
                        enchantment_level = enchantment.get('@enchantmentlevel')
                        if 'craftingrequirements' in enchantment:
                            # Create enchanted item key
                            enchanted_unique_name = f"{unique_name}@{enchantment_level}"
                            crafting_reqs = enchantment['craftingrequirements']
                            
                            items_with_recipes[enchanted_unique_name] = {
                                'craftingrequirements': crafting_reqs,
                                'item_type': item_type,
                                'base_item': unique_name,
                                'enchantment_level': enchantment_level
                            }
    
    return items_with_recipes


def insert_items_and_recipes(conn: sqlite3.Connection, items_data: dict) -> None:
    """Insert items and their recipes into the database."""
    cursor = conn.cursor()
    
    for unique_name, item_info in items_data.items():
        crafting_reqs = item_info['craftingrequirements']
        if "LEVEL" in unique_name:
            level = unique_name.split("LEVEL")[1]
            unique_name += f"@{level}"
        
        # Handle case where craftingrequirements is a list - use the first entry
        if isinstance(crafting_reqs, list):
            if len(crafting_reqs) == 0:
                continue
            crafting_reqs = crafting_reqs[0]
        
        if not isinstance(crafting_reqs, dict):
            continue
        
        # Extract crafting requirements
        silver_cost = int(crafting_reqs.get('@silver', 0))
        crafting_focus = int(crafting_reqs.get('@craftingfocus', 0))
        
        # Insert or replace the crafted item with its requirements
        try:
            cursor.execute('''
                INSERT OR REPLACE INTO Items (internal_id, display_name, silver_cost, crafting_focus)
                VALUES (?, ?, ?, ?)
            ''', (unique_name, None, silver_cost, crafting_focus))
        except sqlite3.IntegrityError:
            pass
        
        # Insert recipe resources
        craft_resources = crafting_reqs.get('craftresource', [])
        if not isinstance(craft_resources, list):
            craft_resources = [craft_resources]
        
        for resource in craft_resources:
            if resource is None or not isinstance(resource, dict):
                continue
            
            material_unique_name = resource.get('@uniquename')
            if "LEVEL" in material_unique_name:
                level = material_unique_name.split("LEVEL")[1]
                material_unique_name += f"@{level}"
            quantity = int(resource.get('@count', 0))
            
            if material_unique_name and quantity > 0:
                # Insert material item if not exists
                try:
                    cursor.execute('''
                        INSERT OR IGNORE INTO Items (internal_id, display_name)
                        VALUES (?, ?)
                    ''', (material_unique_name, None))
                except sqlite3.IntegrityError:
                    pass
                
                # Insert recipe relationship
                try:
                    cursor.execute('''
                        INSERT OR REPLACE INTO Recipes 
                        (crafted_item_id, material_item_id, quantity)
                        VALUES (?, ?, ?)
                    ''', (unique_name, material_unique_name, quantity))
                except sqlite3.IntegrityError:
                    pass
    
    conn.commit()


if __name__ == "__main__":
    # Connect to the SQLite database
    conn = sqlite3.connect(db_path / 'aopc_items.db')
    
    # Initialize database tables
    initialize_database(conn)
    
    # Parse items from JSON file
    json_file = pathlib.Path(__file__).parent / 'items.json'
    if json_file.exists():
        print(f"Parsing items from {json_file}...")
        items_data = parse_items_json(json_file)
        print(f"Found {len(items_data)} items with crafting requirements")
        
        # Insert into database
        insert_items_and_recipes(conn, items_data)
        print(f"Successfully inserted {len(items_data)} items into database")
    else:
        print(f"Warning: items.json not found at {json_file}")
        print("Creating empty database...")
        initialize_database(conn)
    
    conn.close()
    print("Database initialization completed.")