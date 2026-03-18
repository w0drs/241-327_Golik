import pandas as pd
from database import SessionLocal
from models import AnimeModel
from sqlalchemy import text  # <--- ДОБАВЬ ЭТОТ ИМПОРТ
import traceback

print("=" * 50)
print("Начинаем загрузку данных...")
print("=" * 50)

try:
    # Читаем CSV
    print("1. Читаем файл anime_db.csv...")
    df = pd.read_csv('/app/anime_db.csv', encoding='utf-8')
    print(f"   Прочитано строк: {len(df)}")
    print(f"   Колонки в CSV: {list(df.columns)}")

    # Переименовываем колонки
    print("2. Преобразуем колонки...")
    df = df.rename(columns={
        'title_english': 'title',
        'image_webp_large_url': 'image_webp'
    })
    print(f"   Колонки после преобразования: {list(df.columns)}")

    # Обрабатываем пропуски
    print("3. Обрабатываем пропущенные значения...")
    df = df.fillna('')

    # Подключаемся к БД
    print("4. Подключаемся к базе данных...")
    db = SessionLocal()

    # Проверяем соединение (используем text())
    result = db.execute(text("SELECT 1")).scalar()  # <--- ИСПРАВЛЕНО
    print(f"   Соединение OK: {result}")

    # Список полей модели
    model_fields = ['title', 'synopsis', 'genres', 'studios', 'episodes',
                    'image_webp', 'trailer_url', 'year', 'themes',
                    'demographics', 'rating', 'duration']

    # Конвертируем в словари
    print("5. Конвертируем данные...")
    records = df[model_fields].to_dict('records')
    print(f"   Подготовлено записей: {len(records)}")

    # Показываем пример первой записи
    if records:
        print(f"   Пример первой записи: {records[0]}")

    # Очищаем таблицу
    print("6. Очищаем таблицу...")
    db.query(AnimeModel).delete()
    db.commit()

    # Вставляем данные
    print("7. Вставляем новые данные...")
    db.bulk_insert_mappings(AnimeModel, records)
    db.commit()

    # Проверяем результат
    count = db.query(AnimeModel).count()
    print(f"✅ УСПЕХ! В базе теперь {count} записей")

except Exception as e:
    print(f"❌ ОШИБКА: {e}")
    print("\nДетали ошибки:")
    traceback.print_exc()
    db.rollback()

finally:
    if 'db' in locals():
        db.close()
        print("8. Соединение с БД закрыто")

print("=" * 50)