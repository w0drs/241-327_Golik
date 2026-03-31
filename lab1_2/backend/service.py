import sys
from pathlib import Path

# Добавляем путь к backend папке
backend_dir = Path(__file__).parent.absolute()
sys.path.insert(0, str(backend_dir))

from fastapi import FastAPI, Depends, HTTPException, Query
from sqlalchemy.orm import Session
from typing import Optional, List

import schemas

from database import get_db, engine
from models import AnimeModel, Base

# Создаем таблицы (если еще нет)
Base.metadata.create_all(bind=engine)

app = FastAPI(
    title="Anime API",
    description="API для аниме с CRUD операциями",
    version="1.0.0"
)


# LIST (GET /anime)
@app.get("/anime", response_model=schemas.AnimeListResponse)
def list_anime(
        page: int = Query(1, ge=1),
        limit: int = Query(20, ge=1, le=100),
        search: Optional[str] = None,
        year: Optional[int] = None,
        genre: Optional[str] = None,
        db: Session = Depends(get_db)
):
    query = db.query(AnimeModel)

    # Фильтры
    if search:
        query = query.filter(AnimeModel.title.ilike(f"%{search}%"))
    if year:
        query = query.filter(AnimeModel.year == year)
    if genre:
        query = query.filter(AnimeModel.genres.ilike(f"%{genre}%"))

    # Пагинация
    total = query.count()
    items = query.offset((page - 1) * limit).limit(limit).all()

    return {
        "total": total,
        "items": items
    }


# CREATE (POST /anime)
@app.post("/anime", response_model=schemas.Anime, status_code=201)
def create_anime(
        anime: schemas.AnimeCreate,
        db: Session = Depends(get_db)
):
    """Создать новое аниме"""
    db_anime = AnimeModel(**anime.model_dump())
    db.add(db_anime)
    db.commit()
    db.refresh(db_anime)
    return db_anime


# READ (GET /anime/{id})
@app.get("/anime/{anime_id}", response_model=schemas.Anime)
def get_anime(
        anime_id: int,
        db: Session = Depends(get_db)
):
    """Получить аниме по ID"""
    anime = db.query(AnimeModel).filter(AnimeModel.id == anime_id).first()
    if not anime:
        raise HTTPException(status_code=404, detail="Anime not found")
    return anime


# UPDATE (PUT /anime/{id})
@app.put("/anime/{anime_id}", response_model=schemas.Anime)
def update_anime(
        anime_id: int,
        anime_update: schemas.AnimeUpdate,
        db: Session = Depends(get_db)
):
    """Обновить аниме"""
    anime = db.query(AnimeModel).filter(AnimeModel.id == anime_id).first()
    if not anime:
        raise HTTPException(status_code=404, detail="Anime not found")

    for key, value in anime_update.model_dump(exclude_unset=True).items():
        setattr(anime, key, value)

    db.commit()
    db.refresh(anime)
    return anime


# DELETE (DELETE /anime/{id})
@app.delete("/anime/{anime_id}", status_code=204)
def delete_anime(
        anime_id: int,
        db: Session = Depends(get_db)
):
    """Удалить аниме"""
    anime = db.query(AnimeModel).filter(AnimeModel.id == anime_id).first()
    if not anime:
        raise HTTPException(status_code=404, detail="Anime not found")

    db.delete(anime)
    db.commit()
    return None

@app.get("/years", response_model=List[int])
def get_years(db: Session = Depends(get_db)):
    """Получить все доступные года"""
    years = db.query(AnimeModel.year).distinct().order_by(AnimeModel.year).all()
    return [y[0] for y in years if y[0]]


@app.get("/stats")
def get_stats(db: Session = Depends(get_db)):
    """Статистика по базе"""
    total = db.query(AnimeModel).count()
    years = db.query(AnimeModel.year).distinct().count()
    return {
        "total_anime": total,
        "total_years": years
    }


@app.get("/")
def root():
    return {
        "message": "Anime API",
        "endpoints": {
            "list": "GET /anime",
            "create": "POST /anime",
            "get": "GET /anime/{id}",
            "update": "PUT /anime/{id}",
            "delete": "DELETE /anime/{id}",
            "years": "GET /years",
            "stats": "GET /stats",
            "docs": "/docs"
        }
    }