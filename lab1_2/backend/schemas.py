from pydantic import BaseModel
from typing import Optional, List


# Базовая схема
class AnimeBase(BaseModel):
    title: str
    synopsis: Optional[str] = None
    genres: Optional[str] = None
    studios: Optional[str] = None
    episodes: Optional[int] = None
    image_webp: Optional[str] = None
    trailer_url: Optional[str] = None
    year: Optional[int] = None
    themes: Optional[str] = None
    demographics: Optional[str] = None
    rating: Optional[str] = None
    duration: Optional[str] = None
    test1: Optional[str] = None


# Создание аниме
class AnimeCreate(AnimeBase):
    pass


# Обновление аниме
class AnimeUpdate(AnimeBase):
    pass


# Ответ с аниме (включает id)
class Anime(AnimeBase):
    id: int

    class Config:
        from_attributes = True


# Ответ со списком
class AnimeListResponse(BaseModel):
    total: int
    items: List[Anime]