import sqlite3
import pathlib
import json


json_path = pathlib.Path('display_names.json')

def load_display_names(json_file: pathlib.Path) -> dict:
    """Load display names from JSON file."""
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    display_names = {}
    
    # Parse items array
    if isinstance(data, list):
        for item in data:
            if item is None or not isinstance(item, dict):
                continue
            
            unique_name = item.get('UniqueName')
            localized_names = item.get('LocalizedNames', {})
            
            # Get EN-US display name
            if localized_names and isinstance(localized_names, dict):
                display_name = localized_names.get('EN-US')
            
            if unique_name and display_name:
                display_names[unique_name] = display_name
    
    return display_names


def insert_display_names(conn: sqlite3.Connection, display_names: dict) -> None:
    """Insert display names into the database."""
    cursor = conn.cursor()
    levels = [1, 2, 3, 4]
    
    updated_count = 0
    for unique_name, display_name in display_names.items():
        try:
            cursor.execute('''
                UPDATE Items
                SET display_name = ?
                WHERE internal_id = ?
            ''', (display_name, unique_name))
            if cursor.rowcount > 0:
                updated_count += 1
        except sqlite3.IntegrityError:
            pass

        for level in levels:
            try:
                cursor.execute('''
                    UPDATE Items
                    SET display_name = ?
                    WHERE internal_id = ?
                ''', (display_name, f"{unique_name}_LEVEL{level}"))
                if cursor.rowcount > 0:
                    updated_count += 1
            except sqlite3.IntegrityError:
                pass
    
    # Handle enchanted items with @ notation
    cursor.execute('SELECT DISTINCT internal_id FROM Items WHERE internal_id LIKE "%@%"')
    enchanted_items = cursor.fetchall()
    
    for row in enchanted_items:
        enchanted_id = row[0]
        # Parse the base item name and enchantment level
        if '@' in enchanted_id:
            base_name, enchantment_level = enchanted_id.rsplit('@', 1)
            
            # Look up the base item's display name
            if base_name in display_names:
                base_display_name = display_names[base_name]
                # Append enchantment level with dot notation
                enchanted_display_name = f"{base_display_name}.{enchantment_level}"
                
                try:
                    cursor.execute('''
                        UPDATE Items
                        SET display_name = ?
                        WHERE internal_id = ?
                    ''', (enchanted_display_name, enchanted_id))
                    if cursor.rowcount > 0:
                        updated_count += 1
                except sqlite3.IntegrityError:
                    pass
    
    conn.commit()
    return updated_count


if __name__ == "__main__":
    # Connect to the SQLite database
    db_path = pathlib.Path('../database')
    conn = sqlite3.connect(db_path / 'aopc_items.db')
    
    # Load display names from JSON
    if json_path.exists():
        print(f"Loading display names from {json_path}...")
        display_names = load_display_names(json_path)
        print(f"Found {len(display_names)} display names")
        
        # Insert into database
        updated_count = insert_display_names(conn, display_names)
        print(f"Updated {updated_count} items with display names")
    else:
        print(f"Warning: {json_path} not found")
    
    conn.close()
    print("Display name insertion completed.")