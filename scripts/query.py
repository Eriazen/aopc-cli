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

print(query_crafting_requirements('T4_BAG'))
print(query_item_info('T4_BAG'))