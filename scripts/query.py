import sqlite3


conn = sqlite3.connect('../database/aopc_items.db')
cursor = conn.cursor()

def query_crafting_requirements(item_id):
    cursor.execute('''
        SELECT material_item_id, quantity
        FROM Recipes
        WHERE crafted_item_id = ?
    ''', (item_id,))
    
    results = cursor.fetchall()
    requirements = []
    for material_item_id, quantity in results:
        requirements.append({
            'material_item_id': material_item_id,
            'quantity': quantity,
        })
    
    return requirements

def query_item_info(item_id):
    cursor.execute('''
        SELECT display_name, silver_cost, crafting_focus
        FROM Items
        WHERE internal_id = ?
    ''', (item_id,))
    
    result = cursor.fetchone()
    if result:
        display_name, silver_cost, crafting_focus = result
        return {
            'display_name': display_name,
            'silver_cost': silver_cost,
            'crafting_focus': crafting_focus
        }
    return None

def items_without_display_names():
    cursor.execute('''
        SELECT internal_id
        FROM Items
        WHERE display_name IS NULL
    ''')
    
    results = cursor.fetchall()
    return [row[0] for row in results]

if __name__ == "__main__":
    print(query_crafting_requirements('T5_BAG@3'))
    print(query_item_info('T5_BAG@4'))
    print(query_item_info('T7_WOOD_LEVEL3'))
    print(items_without_display_names())