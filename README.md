# Лабораторные работы: Архитектура и дизайн ПО

## Лабораторная 1  
Задачи:
1. Установить необходимый для бекенд-разработки язык и библиотеки.
2. Разработать модель данных.
3. Наполнить базу сгенерированными данными.
4. Разработать API для доступа к данным (CRUD+List).

### Установка пакетов
Для лабораторной работы мне понадобились следущие пакеты:  
```text
fastapi==0.104.1
uvicorn[standard]==0.24.0
sqlalchemy==2.0.23
psycopg2-binary==2.9.9
python-dotenv==1.0.0
pydantic==2.5.0
```

### Модель данных  
Моя тема лабораторных - каталог аниме. Моя модель данных будет включать информацию об аниме сериалах.  
Колонки:  
```text
title           # название аниме 
synopsis        # описание
genres          # жанры
studios         # студия
episodes        # число эпизодов
image_webp      # ссылка на картинку с постером
trailer_url     # ссылка на youtube вмдео с трейлером
year            # год выхода
themes          # темы
demographics    # категории (целевая аудитория)
rating          # рейтинг
duration        # длительность серии
```

### Генерация данных
Данные я взял с источника kaggle, поэтому генерироавть мне их не надо.  

### CRUD + List
Реализация была на FastApi. 
Разработанные ручки находятся в .py файле по пути:
```text
lab1_2/backend/service.py
```

---

## Лабораторная 2
Задачи:
1. Развернуть серверное ПО, разработанное ранее, в контейнерной инфраструктуре Docker. 
2. Дополнить архитектуру обратным HTTP-прокси сервером.
3. Сгенерировать самоподписанный сертификат

Нужно развернуть многоконтейнерное приложение. Для этого нам нужен docker-compose.  
docker-compose.yml:
```yml
services:
  postgres:
    image: postgres:15-alpine
    container_name: anime_postgres
    restart: unless-stopped
    env_file:
      - .env
    volumes:
      - postgres_data:/var/lib/postgresql/data/
    networks:
      - anime_network
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U postgres"]
      interval: 5s
      timeout: 5s
      retries: 5

  backend:
    build:
      context: ./backend
      dockerfile: Dockerfile
    container_name: anime_backend
    restart: unless-stopped
    env_file:
      - .env
    depends_on:
      postgres:
        condition: service_healthy
    networks:
      - anime_network

  nginx:
    image: nginx:1.25-alpine
    container_name: anime_nginx
    restart: unless-stopped
    ports:
      - "80:80"
      - "443:443"
    volumes:
      - ./nginx/templates:/etc/nginx/templates
      - ./nginx/certs:/etc/nginx/certs
      - static_volume:/staticfiles
    depends_on:
      - backend
    networks:
      - anime_network
    environment:
      - NGINX_HOST=localhost
      - NGINX_PORT=80

volumes:
  postgres_data:
  static_volume:

networks:
  anime_network:
    driver: bridge
```

Docker файл backend сервиса:
```docker
FROM python:3.11-slim

ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

CMD sh -c "python export.py && uvicorn service:app --host 0.0.0.0 --port 8000 --reload"
```

Docker файл nginx:
```docker
FROM nginx:1.25-alpine
RUN rm /etc/nginx/conf.d/default.conf
COPY ./templates/default.conf.template /etc/nginx/templates/default.conf.template

RUN mkdir -p /staticfiles
```

---

## Лабораторная 3
Задачи:
1. Создать класс, который представляет ваш объект из модели данных
2. Используя классы QJsonDocument, QJsonObject, QJsonArray, QJsonValue провести парсинг буфера, получаемого по reply->readAll(), получить значения полей полученного от сервера объекта, и записать их в С++-объект, представляющий модель данных, т.е. выполнить десериализацию из http/json.
3. Реализованы паттерны “адаптер” и “синглтон”.

Клиент успешно должен обращаеться к серверу из ЛР1 или ЛР2, получать данные и отображать как минимум в терминале (qDebug()).  






