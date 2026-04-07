import sqlite3


def remove_null_display_names(conn: sqlite3.Connection) -> int:
    """Remove items with null display names from the database."""
    cursor = conn.cursor()
    
    # Delete items where display_name is NULL
    cursor.execute('''
        DELETE FROM Items
        WHERE display_name IS NULL
    ''')
    
    deleted_count = cursor.rowcount
    conn.commit()
    return deleted_count

if __name__ == "__main__":
    # Connect to the SQLite database
    conn = sqlite3.connect('../database/aopc_items.db')
    
    deleted_count = remove_null_display_names(conn)
    print(f"Deleted {deleted_count} items with null display names.")
    
    conn.close()