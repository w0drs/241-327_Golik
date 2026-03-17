from logging.config import fileConfig
from sqlalchemy import engine_from_config
from sqlalchemy import pool
from alembic import context
import sys
import os
from dotenv import load_dotenv

# Добавляем путь к папке lab1_2 в sys.path
# __file__ - это путь к env.py, он в lab1_2/alembic/env.py
# поднимаемся на два уровня вверх: alembic/ -> lab1_2/
project_path = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, project_path)

print(f"Добавлен путь: {project_path}")  # Для отладки

# Загружаем .env из папки lab1_2
env_path = os.path.join(project_path, '.env')
load_dotenv(env_path)

# Импортируем Base из backend.database
try:
    from backend.database import Base
    from backend.models import AnimeModel

    print("Импорт успешен!")
except ImportError as e:
    print(f"Ошибка импорта: {e}")
    print(f"Содержимое sys.path: {sys.path}")
    raise

# this is the Alembic Config object
config = context.config

# Interpret the config file for Python logging
if config.config_file_name is not None:
    fileConfig(config.config_file_name)

# Получаем параметры из .env
POSTGRES_USER = os.getenv("POSTGRES_USER", "postgres")
POSTGRES_PASSWORD = os.getenv("POSTGRES_PASSWORD", "12345")
POSTGRES_HOST = os.getenv("POSTGRES_HOST", "localhost")
POSTGRES_PORT = os.getenv("POSTGRES_PORT", "5432")
POSTGRES_DB = os.getenv("POSTGRES_DB", "anime_db")

print(f"Подключение к БД: {POSTGRES_USER}@{POSTGRES_HOST}:{POSTGRES_PORT}/{POSTGRES_DB}")

# Формируем правильный URL
DATABASE_URL = f"postgresql+psycopg2://{POSTGRES_USER}:{POSTGRES_PASSWORD}@{POSTGRES_HOST}:{POSTGRES_PORT}/{POSTGRES_DB}"
config.set_main_option("sqlalchemy.url", DATABASE_URL)

target_metadata = Base.metadata


def run_migrations_offline() -> None:
    url = config.get_main_option("sqlalchemy.url")
    context.configure(
        url=url,
        target_metadata=target_metadata,
        literal_binds=True,
        dialect_opts={"paramstyle": "named"},
    )

    with context.begin_transaction():
        context.run_migrations()


def run_migrations_online() -> None:
    configuration = config.get_section(config.config_ini_section, {})
    configuration["sqlalchemy.url"] = DATABASE_URL

    connectable = engine_from_config(
        configuration,
        prefix="sqlalchemy.",
        poolclass=pool.NullPool,
    )

    with connectable.connect() as connection:
        context.configure(
            connection=connection, target_metadata=target_metadata
        )

        with context.begin_transaction():
            context.run_migrations()


if context.is_offline_mode():
    run_migrations_offline()
else:
    run_migrations_online()