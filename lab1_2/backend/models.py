from sqlalchemy.orm import Mapped, mapped_column
from sqlalchemy import String, Text, Integer
from database import Base

class AnimeModel(Base):
    __tablename__ = "anime"

    id: Mapped[int] = mapped_column(primary_key=True)
    title: Mapped[str] = mapped_column(String(500))
    synopsis: Mapped[str] = mapped_column(Text)
    genres: Mapped[str] = mapped_column(String(500))
    studios: Mapped[str] = mapped_column(String(500))
    episodes: Mapped[int] = mapped_column(Integer)
    image_webp: Mapped[str] = mapped_column(String(500))
    trailer_url: Mapped[str] = mapped_column(String(500))
    year: Mapped[int] = mapped_column(Integer)
    themes: Mapped[str] = mapped_column(String(500))
    demographics: Mapped[str] = mapped_column(String(500))
    rating: Mapped[str] = mapped_column(String(50))
    duration: Mapped[str] = mapped_column(String(50))
    test1: Mapped[str] = mapped_column(String(50), server_default='', nullable=False)

    def __repr__(self):
        return f"<Anime(title={self.title})>"