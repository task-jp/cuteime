isEmpty(CUTEIMEDATA_PRI) {
CUTEIMEDATA_PRI = 1

include(./cuteime.pri)

defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

# For use in custom compilers which just copy files
win32:i_flag = i
defineReplace(stripSrcDir) {
    return($$relative_path($$absolute_path($$1, $$OUT_PWD), $$_PRO_FILE_PWD_))
}

copy_files.path = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_DATA_PATH/$$COPY_TARGET
copy_files.input = COPY_FILES
copy_files.output = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_DATA_PATH/$$COPY_TARGET/${QMAKE_FUNC_FILE_IN_stripSrcDir}
copy_files.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_files.name = COPY $$COPY_FILES
copy_files.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += copy_files
PRE_TARGETDEPS += compiler_copy_files_make_all

install_files.files = $$COPY_FILES
install_files.path = $$PREFIX/$$CUTEIME_TARGET_PATH/$$CUTEIME_DATA_PATH/$$COPY_TARGET
INSTALLS += install_files

} # CUTEIMEDATA_PRI
