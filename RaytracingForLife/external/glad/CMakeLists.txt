add_library(glad STATIC src/glad.c)

# Tell projects linking to glad where to find the headers
target_include_directories(glad PUBLIC include)