# If you have a program that's just a .c file and it has a main method, define
# an executable. If it has both a .c and .h file, make it a library. Also, link
# libraries appropriately here. Targets should be added in dependency order - if
# executable bar needs library foo, then you should do
#   add_library(foo foo.c foo.h)
#   add_executable(bar bar.c)
#   target_link_libraries(bar PRIVATE foo)
# or similar. The PRIVATE/INTERFACE/PUBLIC keyword will depend on whether the
# library is used only in function bodies (PRIVATE), only in function
# signatures/types (INTERFACE), or both (PUBLIC).

find_package(JPEG REQUIRED)

add_library(upload_image upload_image/upload_image.c upload_image/upload_image.h)
target_include_directories(upload_image
    PUBLIC
    upload_image
)
target_link_libraries(upload_image 
    PRIVATE 
    JPEG::JPEG
)

add_executable(blur blur.c)

target_link_libraries(blur 
    PRIVATE 
    upload_image
)