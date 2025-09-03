project(app LANGUAGES C CXX)

set(APP_SOURCES
	"app_def.c"
)

add_library(app STATIC ${APP_SOURCES})

target_sources(app PRIVATE ${APP_SOURCES})
