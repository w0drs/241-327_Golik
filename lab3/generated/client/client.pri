QT += network

HEADERS += \
# Models
    $${PWD}/OAIAnime.h \
    $${PWD}/OAIAnimeCreate.h \
    $${PWD}/OAIAnimeListResponse.h \
    $${PWD}/OAIAnimeUpdate.h \
    $${PWD}/OAIHTTPValidationError.h \
    $${PWD}/OAIValidationError.h \
    $${PWD}/OAIValidationError_loc_inner.h \
# APIs
    $${PWD}/OAIDefaultApi.h \
# Others
    $${PWD}/OAIHelpers.h \
    $${PWD}/OAIHttpRequest.h \
    $${PWD}/OAIObject.h \
    $${PWD}/OAIEnum.h \
    $${PWD}/OAIHttpFileElement.h \
    $${PWD}/OAIServerConfiguration.h \
    $${PWD}/OAIServerVariable.h \
    $${PWD}/OAIOauth.h

SOURCES += \
# Models
    $${PWD}/OAIAnime.cpp \
    $${PWD}/OAIAnimeCreate.cpp \
    $${PWD}/OAIAnimeListResponse.cpp \
    $${PWD}/OAIAnimeUpdate.cpp \
    $${PWD}/OAIHTTPValidationError.cpp \
    $${PWD}/OAIValidationError.cpp \
    $${PWD}/OAIValidationError_loc_inner.cpp \
# APIs
    $${PWD}/OAIDefaultApi.cpp \
# Others
    $${PWD}/OAIHelpers.cpp \
    $${PWD}/OAIHttpRequest.cpp \
    $${PWD}/OAIHttpFileElement.cpp \
    $${PWD}/OAIOauth.cpp
