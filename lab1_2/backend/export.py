import csv
from database import SessionLocal, engine
from models import AnimeModel, Base
from sqlalchemy import text
import traceback

print("-" * 50)
print("Начинаем загрузку данных...")
print("-" * 50)

try:
    print("1. Создаем таблицы (если не существуют)...")
    Base.metadata.create_all(bind=engine)
    print("   Таблицы созданы")

    print("2. Подключаемся к базе данных...")
    db = SessionLocal()

    # Проверяем соединение
    result = db.execute(text("SELECT 1")).scalar()
    print(f"   Соединение OK: {result}")

    # Проверяем, есть ли данные
    count = db.query(AnimeModel).count()
    if count > 0:
        print(f"В базе уже есть {count} записей. Импорт пропущен.")
        exit(0)

    print("3. Читаем файл anime_db.csv...")
    records = []
    with open('anime_db.csv', 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            # Пропускаем пустые строки
            if not row.get('title_english'):
                continue

            record = {
                'title': row['title_english'],
                'synopsis': row.get('synopsis', ''),
                'genres': row.get('genres', ''),
                'studios': row.get('studios', ''),
                'episodes': int(row['episodes']) if row['episodes'] and row['episodes'].isdigit() else 0,
                'image_webp': row.get('image_webp_large_url', ''),
                'trailer_url': row.get('trailer_url', ''),
                'year': int(row['year']) if row['year'] and row['year'].isdigit() else 0,
                'themes': row.get('themes', ''),
                'demographics': row.get('demographics', ''),
                'rating': row.get('rating', ''),
                'duration': row.get('duration', '')
            }
            records.append(record)

    print(f"   Прочитано записей: {len(records)}")

    if records:
        print("4. Вставляем данные...")
        # Вставляем пачками по 1000 записей
        batch_size = 1000
        for i in range(0, len(records), batch_size):
            batch = records[i:i + batch_size]
            db.bulk_insert_mappings(AnimeModel, batch)
            db.commit()
            print(f"   Вставлено {min(i + batch_size, len(records))} из {len(records)}")

        final_count = db.query(AnimeModel).count()
        print(f"✅ УСПЕХ! В базе теперь {final_count} записей")
    else:
        print("⚠️ Нет данных для вставки")

except Exception as e:
    print(f"❌ ОШИБКА: {e}")
    traceback.print_exc()
    if 'db' in locals():
        db.rollback()
finally:
    if 'db' in locals():
        db.close()

print("=" * 50)