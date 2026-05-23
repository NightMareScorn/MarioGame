# CopyUIIfExists.cmake
# Called at build time to conditionally copy ui/out to the build directory.
# Variables passed in: SRC_DIR, DST_DIR

if(EXISTS "${SRC_DIR}")
    message(STATUS "[UI] Copying ${SRC_DIR} -> ${DST_DIR}")
    file(COPY "${SRC_DIR}/" DESTINATION "${DST_DIR}")
else()
    message(STATUS "[UI] ui/out not found - skipping. Run 'npm run build' in ui/ first.")
endif()
