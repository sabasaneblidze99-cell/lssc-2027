find_path(SAMPSDK_DIR plugincommon.h HINTS ${SAMP_SDK_ROOT} ENV SAMP_SDK_ROOT)
set(SAMPSDK_INCLUDE_DIR ${SAMPSDK_DIR} ${SAMPSDK_DIR}/amx)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SAMPSDK REQUIRED_VARS SAMPSDK_DIR)
