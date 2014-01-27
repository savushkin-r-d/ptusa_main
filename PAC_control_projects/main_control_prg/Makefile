#
#  There exist several targets which are by default empty and which can be
#  used for execution of your targets. These targets are usually executed
#  before and after some main targets. They are:
#
#     .build-pre:              called before 'build' target
#     .build-post:             called after 'build' target
#     .clean-pre:              called before 'clean' target
#     .clean-post:             called after 'clean' target
#     .clobber-pre:            called before 'clobber' target
#     .clobber-post:           called after 'clobber' target
#     .all-pre:                called before 'all' target
#     .all-post:               called after 'all' target
#     .help-pre:               called before 'help' target
#     .help-post:              called after 'help' target
#
#  Targets beginning with '.' are not intended to be called on their own.
#
#  Main targets can be executed directly, and they are:
#
#     build                    build a specific configuration
#     clean                    remove built files from a configuration
#     clobber                  remove all built files
#     all                      build all configurations
#     help                     print help mesage
#
#  Targets .build-impl, .clean-impl, .clobber-impl, .all-impl, and
#  .help-impl are implemented in nbproject/makefile-impl.mk.
#
#  Available make variables:
#
#     CND_BASEDIR                base directory for relative paths
#     CND_DISTDIR                default top distribution directory (build artifacts)
#     CND_BUILDDIR               default top build directory (object files, ...)
#     CONF                       name of current configuration
#     CND_PLATFORM_${CONF}       platform name (current configuration)
#     CND_ARTIFACT_DIR_${CONF}   directory of build artifact (current configuration)
#     CND_ARTIFACT_NAME_${CONF}  name of build artifact (current configuration)
#     CND_ARTIFACT_PATH_${CONF}  path to build artifact (current configuration)
#     CND_PACKAGE_DIR_${CONF}    directory of package (current configuration)
#     CND_PACKAGE_NAME_${CONF}   name of package (current configuration)
#     CND_PACKAGE_PATH_${CONF}   path to package (current configuration)
#
# NOCDDL


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib

#my var
out_dir     = ""${HOME}/id_share/out/""
lua_sys_dir = ""${HOME}/PTUSA_projects/system scripts/""
lua_prg_dir = ""${HOME}/PTUSA_projects/projects/Ñûגמנמעךא (ÑÎÌ)/""

# build
build: .build-post

.build-pre:
# Add your pre 'build' code here...

.build-post: .build-impl
# Add your post 'build' code here...

	@if [ ${CONF} = "release_wago_750-860" ]; then \
	    echo ;\
	    echo copying files...;\
\
	    echo "${CND_ARTIFACT_PATH_${CONF}}main >> ${out_dir}main";\
	    cp    ${CND_ARTIFACT_PATH_${CONF}} "${out_dir}main";\
	fi

	@if [ ${CONF} = "release_wago_750-860_khutor" ]; then \
	    echo ;\
	    echo copying files...;\
\
	    echo "${CND_ARTIFACT_PATH_${CONF}}main >> ${out_dir}main_khutor";\
	    cp    ${CND_ARTIFACT_PATH_${CONF}} "${out_dir}main_khutor";\
	fi
	
	@if [ ${CONF} = "debug_PFC200" ]; then \
	    echo ;\
	    echo copying files...;\
\
	    echo "${CND_ARTIFACT_PATH_${CONF}}main >> ${out_dir}main_PFC200";\
	    cp    ${CND_ARTIFACT_PATH_${CONF}} "${out_dir}main_PFC200";\
	fi	

	@if [ ${CONF} = "release_wago_750-860_khutor" ] || [ ${CONF} = "release_wago_750-860" ]; then \
\
		echo "";\
		echo "${lua_sys_dir}sys.wago.lua    >> ${out_dir}sys.wago.lua";\
		cp   "${lua_sys_dir}sys.wago.lua" ${out_dir};\
		echo "${lua_sys_dir}sys.devices.lua >> ${out_dir}sys.devices.lua";\
		cp   "${lua_sys_dir}sys.devices.lua" ${out_dir};\
		echo "${lua_sys_dir}sys.objects.lua >> ${out_dir}sys.objects.lua";\
		cp   "${lua_sys_dir}sys.objects.lua" ${out_dir};\
		echo "";\
		echo "${lua_prg_dir}main.wago.lua    >> ${out_dir}";\
		cp   "${lua_prg_dir}main.wago.lua" ${out_dir};\
		echo "${lua_prg_dir}main.devices.lua >> ${out_dir}";\
		cp   "${lua_prg_dir}main.devices.lua" ${out_dir};\
		echo "${lua_prg_dir}main.objects.lua >> ${out_dir}";\
		cp   "${lua_prg_dir}main.objects.lua" ${out_dir};\
		echo "${lua_prg_dir}main.plua        >> ${out_dir}";\
		cp   "${lua_prg_dir}main.plua" ${out_dir};\
\
		echo done;\
	fi

# clean
clean: .clean-post

.clean-pre:
# Add your pre 'clean' code here...

.clean-post: .clean-impl
# Add your post 'clean' code here...


# clobber
clobber: .clobber-post

.clobber-pre:
# Add your pre 'clobber' code here...

.clobber-post: .clobber-impl
# Add your post 'clobber' code here...


# all
all: .all-post

.all-pre:
# Add your pre 'all' code here...

.all-post: .all-impl
# Add your post 'all' code here...


# help
help: .help-post

.help-pre:
# Add your pre 'help' code here...

.help-post: .help-impl
# Add your post 'help' code here...



# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk
