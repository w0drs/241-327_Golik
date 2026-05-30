from logging.config import fileConfig
from sqlalchemy import engine_from_config
from sqlalchemy import pool
from alembic import context
import sys
import os
from dotenv import load_dotenv

project_path = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, project_path)

print(f"Добавлен путь: {project_path}")

env_path = os.path.join(project_path, '.env')
load_dotenv(env_path)


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

load_dotenv(os.path.join(os.path.dirname(os.path.dirname(__file__)), '.env'))

POSTGRES_USER = "postgres"
POSTGRES_PASSWORD = "qwerty0504"
POSTGRES_HOST = "postgres"
POSTGRES_PORT = "5432"
POSTGRES_DB = "anime_db"

# Формируем строку подключения БЕЗ специальных символов
DATABASE_URL = f"postgresql+psycopg2://{POSTGRES_USER}:{POSTGRES_PASSWORD}@{POSTGRES_HOST}:{POSTGRES_PORT}/{POSTGRES_DB}"


print(f"Подключение к БД: {POSTGRES_USER}@{POSTGRES_HOST}:{POSTGRES_PORT}/{POSTGRES_DB}")


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