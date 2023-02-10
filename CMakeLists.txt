cmake_minimum_required(VERSION 3.12)
project(dbe VERSION 0.0.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

add_compile_options(-Wno-deprecated)

find_package(oks REQUIRED)
find_package(dal REQUIRED)
find_package(config REQUIRED)
find_package(qtutils REQUIRED)
find_package(logging REQUIRED)

find_package( Qt5 REQUIRED COMPONENTS Widgets Gui Core PrintSupport)

add_definitions(
   -DQT_NO_DEBUG
   )

#====================================================================================#
# COMMON LIBS                                                                        #
#====================================================================================#
set(dbe_common_qt_libs
        Qt5::PrintSupport
        Qt5::Widgets
        Qt5::Gui 
        Qt5::Core
	qtutils::qtutils
)

set(dbe_common_boost_libs
        Boost::program_options 
        )
        
set(dbe_common_test_libs
        Boost::system
        Boost::unit_test_framework
        )

set(dbe_common_tdaq_libs
        qtutils::qtutils
        config::config
        dal::dal 
        ers::ers
        )
        
set(dbe_common_libs
        ${dbe_common_tdaq_libs}
        ${dbe_common_qt_libs}
        )        

#====================================================================================#
#                                    LIB_DBE_CONFIG_API                              #
#====================================================================================#
set(dbe_config_api_src
                    src/internal/Command.cpp
                    src/internal/Conversion.cpp
                    src/internal/config_api.cpp
                    src/internal/config_api_set.cpp
                    src/internal/config_api_get.cpp
                    src/internal/config_api_info.cpp
                    src/internal/config_api_graph.cpp
                    src/internal/config_api_commands.cpp
   )


add_library(dbe_config_api SHARED ${dbe_config_api_src})
#target_include_directories(dbe_config_api PUBLIC ${CMAKE_CURRENT_LIST_DIR}/dbe)
target_link_libraries(dbe_config_api PUBLIC ${dbe_common_libs})

# JCF: force creation of UI headers
add_dependencies(dbe_config_api dbe_core_ui_header_gen)

#====================================================================================#
#                                    LIBDBE_STRUCTURE                                #
#====================================================================================#

qt5_wrap_cpp(dbe_structure_moc_src
                    include/dbe/CustomDelegate.hpp
                    include/dbe/CustomFileView.hpp
                    include/dbe/CustomTableView.hpp
                    include/dbe/CustomTreeView.hpp
                    include/dbe/FileModel.hpp
                    include/dbe/subtreeproxy.hpp
                    include/dbe/table.hpp
                    include/dbe/tableselection.hpp
                    include/dbe/tree.hpp
                    include/dbe/treeselection.hpp
)

set(dbe_structure_src
                    src/structure/CustomDelegate.cpp
                    src/structure/CustomFileView.cpp
                    src/structure/CustomTableView.cpp
                    src/structure/CustomTreeView.cpp
                    src/structure/FileModel.cpp
                    src/structure/treenode.cpp
                    src/structure/subtreeproxy.cpp
                    src/structure/table.cpp
                    src/structure/TableNode.cpp
                    src/structure/tableselection.cpp
                    src/structure/tree.cpp
                    src/structure/treeselection.cpp
                    src/structure/model_common_interface.cpp
  )

add_library(dbe_structure SHARED ${dbe_structure_src} ${dbe_structure_moc_src})
target_link_libraries(dbe_structure PUBLIC ${dbe_common_libs})
target_include_directories(dbe_structure PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# dbe_structure depends on the generated header files of the dbe_core library
# Since dbe_core depends on dbe_structure we introduce a custom target
# that will trigger only the ui header generation and depend on.

add_dependencies(dbe_structure dbe_core_ui_header_gen)

#====================================================================================#
#                                    LIBDBE_WRAP                                     #
#====================================================================================#
set(dbe_wrap_src
                    src/internal/rwdacc.cpp
                    src/internal/dbcontroller.cpp
                    src/internal/dbaccessor.cpp
  )

add_library(dbe_wrap SHARED ${dbe_wrap_src})
target_link_libraries(dbe_wrap PUBLIC  dbe_config_api ${dbe_common_tdaq_libs})

#====================================================================================#
#                                    LIBDBE_INTERNAL                                 #
#====================================================================================#
qt5_wrap_cpp(dbe_internal_moc_src
                    include/dbe/Validator.hpp
                    include/dbe/messenger_proxy.hpp
                    include/dbe/datahandler.hpp                    
                    include/dbe/confaccessor.hpp
)

set(dbe_internal_src
                    src/internal/datahandler.cpp
                    src/internal/confaccessor.cpp
                    src/internal/messenger.cpp
                    src/internal/messenger_proxy.cpp
                    src/internal/config_ui_info.cpp
  )
  
add_library(dbe_internal SHARED ${dbe_internal_src} ${dbe_internal_moc_src})
target_link_libraries(dbe_internal PUBLIC dbe_config_api dbe_wrap ${dbe_common_libs})

#====================================================================================#
#                                    LIBDBECORE                                      #
#====================================================================================#
set(dbe_core_ui
                    ui/BatchChangeWidget.ui
                    ui/CommitDialog.ui
                    ui/CreateDatabaseWidget.ui
                    ui/DBE.ui
                    ui/EditCombo.ui
                    ui/GraphView.ui
                    ui/IncludeFileWidget.ui
                    ui/NumericAttributeWidgetForm.ui
                    ui/ObjectCreator.ui
                    ui/ObjectEditor.ui
                    ui/OracleWidget.ui
                    ui/RelationshipWidgetForm.ui
                    ui/StringAttributeWidgetForm.ui
                    )
                    
qt5_wrap_ui(dbe_core_ui_header ${dbe_core_ui})

add_custom_target(dbe_core_ui_header_gen 
  DEPENDS ${dbe_core_ui_header}
)

qt5_wrap_cpp(dbe_core_moc_src
                    include/dbe/TableTab.hpp
                    include/dbe/MyApplication.hpp
                    include/dbe/GraphicalClass.hpp
                    include/dbe/GraphView.hpp
                    include/dbe/MainWindow.hpp
                    include/dbe/CustomLineEdit.hpp
                    include/dbe/string_attr_text_edit.hpp
                    include/dbe/SearchComboBox.hpp
                    include/dbe/ValidatorComboBox.hpp
                    include/dbe/BatchChangeWidget.hpp
                    include/dbe/BuildingBlockEditors.hpp
                    include/dbe/CommitDialog.hpp
                    include/dbe/CreateDatabaseWidget.hpp
                    include/dbe/IncludeFileWidget.hpp
                    include/dbe/ObjectCreator.hpp
                    include/dbe/ObjectEditor.hpp
                    include/dbe/OracleWidget.hpp
)

set(dbe_core_src
                    src/internal/MainWindow.cpp
                    src/internal/StyleUtility.cpp
                    src/internal/Validator.cpp
                    src/object/CustomLineEdit.cpp
                    src/object/string_attr_text_edit.cpp
                    src/object/SearchComboBox.cpp
                    src/object/ValidatorComboBox.cpp
                    src/widgets/BatchChangeWidget.cpp
                    src/widgets/BuildingBlockEditors.cpp
                    src/widgets/CommitDialog.cpp
                    src/widgets/CreateDatabaseWidget.cpp
                    src/widgets/IncludeFileWidget.cpp
                    src/widgets/ObjectCreator.cpp
                    src/widgets/ObjectEditor.cpp
                    src/widgets/OracleWidget.cpp
                    src/graphical/GraphicalClass.cpp
                    src/graphical/GraphView.cpp
                    src/MyApplication.cpp
                    src/TableTab.cpp
  )

qt5_add_resources(qrc_resource icons/resource.qrc OPTIONS -name DBEimg)

add_library(dbe_core SHARED ${dbe_core_src} ${dbe_core_moc_src} ${dbe_core_ui_header} ${qrc_resource})
target_link_libraries(dbe_core PUBLIC dbe_config_api dbe_internal dbe_structure ${dbe_common_libs})
target_include_directories(dbe_core PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#====================================================================================#
#                                       SCHEMACORE                                   #
#====================================================================================#
set(schema_core_ui
                    ui/SchemaAttributeEditor.ui
                    ui/SchemaClassEditor.ui
                    ui/SchemaMainWindow.ui
                    ui/SchemaMethodEditor.ui
                    ui/SchemaMethodImplementationEditor.ui
                    ui/SchemaRelationshipEditor.ui
                    )

qt5_wrap_ui(schema_core_ui_header ${schema_core_ui})

set(schema_core_src
                    src/SchemaEditor/SchemaAttributeEditor.cpp
                    src/SchemaEditor/SchemaCustomMethodImplementationModel.cpp
                    src/SchemaEditor/SchemaGraphicArrow.cpp
                    src/SchemaEditor/SchemaMainWindow.cpp
                    src/SchemaEditor/SchemaClassEditor.cpp
                    src/SchemaEditor/SchemaCustomMethodModel.cpp
                    src/SchemaEditor/SchemaGraphicObject.cpp
                    src/SchemaEditor/SchemaMethodEditor.cpp
                    src/SchemaEditor/SchemaCommand.cpp
                    src/SchemaEditor/SchemaCustomModelInterface.cpp
                    src/SchemaEditor/SchemaGraphicsScene.cpp
                    src/SchemaEditor/SchemaMethodImplementationEditor.cpp
                    src/SchemaEditor/SchemaCustomAttributeModel.cpp
                    src/SchemaEditor/SchemaCustomRelationshipModel.cpp
                    src/SchemaEditor/SchemaKernelWrapper.cpp
                    src/SchemaEditor/SchemaRelationshipEditor.cpp
                    src/SchemaEditor/SchemaCustomFileModel.cpp
                    src/SchemaEditor/SchemaCustomTableModel.cpp
                    src/SchemaEditor/SchemaCustomSuperClassModel.cpp
                    src/SchemaEditor/SchemaCustomSubClassModel.cpp
                    src/SchemaEditor/SchemaTab.cpp
  )

qt5_wrap_cpp(schema_core_moc_src
                    include/dbe/SchemaAttributeEditor.hpp
                    include/dbe/SchemaClassEditor.hpp
                    include/dbe/SchemaCustomFileModel.hpp
                    include/dbe/SchemaCustomModelInterface.hpp
                    include/dbe/SchemaCustomTableModel.hpp
                    include/dbe/SchemaGraphicObject.hpp
                    include/dbe/SchemaGraphicsScene.hpp
                    include/dbe/SchemaKernelWrapper.hpp
                    include/dbe/SchemaMainWindow.hpp
                    include/dbe/SchemaMethodEditor.hpp
                    include/dbe/SchemaMethodImplementationEditor.hpp
                    include/dbe/SchemaRelationshipEditor.hpp
                    include/dbe/SchemaTab.hpp
)

qt5_add_resources(qrc_SchemaResource icons/SchemaResource.qrc OPTIONS -name SchemaResource)

add_library(schemacore SHARED ${schema_core_src} ${schema_core_moc_src} ${schema_core_ui_header} ${qrc_SchemaResource})
target_link_libraries(schemacore ${dbe_common_libs} oks::oks)
target_include_directories(schemacore PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


# JCF: force creation of UI headers
add_dependencies(schemacore dbe_core_ui_header_gen)

#====================================================================================#
# SCHEMA EDITOR                                                                      #
#====================================================================================#

add_executable(schemaeditor src/SchemaEditor/SchemaMain.cpp)
target_link_libraries(schemaeditor PRIVATE schemacore ${dbe_common_boost_libs} )

#====================================================================================#
# DATABASE EDITOR (DBE)                                                              #
#====================================================================================#

add_executable(dbe_main src/Main.cpp)
#target_include_directories(dbe_main PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dbe)
target_link_libraries(dbe_main PRIVATE dbe_core ${dbe_common_boost_libs})

#====================================================================================#
# DBE GRAPHTOOL (DBE_GTOOL)                                                          #
#====================================================================================#
set(dbe_gtool_src
      src/graphtool.cpp
      src/graphtool/gtool.cpp
      src/graphtool/segregate.cpp
      src/graphtool/stats.cpp
      )

add_executable(dbe_gtool ${dbe_gtool_src})
target_include_directories(dbe_gtool PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dbe)
target_link_libraries(dbe_gtool PRIVATE dbe_core ${dbe_common_boost_libs} )

# #====================================================================================#
# # TEST_CONFIG_API                                                                    #
# #====================================================================================#
# tdaq_add_executable(dbe_config_api_info_test_exec
#   test/config_api_info_test.cpp
#   INCLUDE_DIRECTORIES dbe
#   LINK_LIBRARIES dbe_core ${dbe_common_test_libs}
#   )

# add_test( 
#   NAME dbe_config_api_info_test
#   COMMAND ${TDAQ_RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/dbe_config_api_info_test_exec --log_level=all
#   )
  
# tdaq_add_executable(dbe_config_api_set_test_exec 
#   test/config_api_set_test.cpp
#   INCLUDE_DIRECTORIES dbe
#   LINK_LIBRARIES dbe_core ${dbe_common_test_libs}
#   NOINSTALL
#   )

# add_test( 
#   NAME dbe_config_api_set_test
#   COMMAND ${TDAQ_RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/dbe_config_api_set_test_exec --log_level=all
#   )

# #====================================================================================#
# # TEST_CONFACCESSOR                                                                  #
# #====================================================================================#
# tdaq_add_executable(dbe_confaccessor_test_exec
#   test/confaccessor_test.cpp
#   INCLUDE_DIRECTORIES dbe
#   LINK_LIBRARIES dbe_core ${dbe_common_test_libs}
#   NOINSTALL
#   )

# add_test( 
#   NAME dbe_confaccessor_test
#   COMMAND ${TDAQ_RUNNER} ${CMAKE_CURRENT_BINARY_DIR}/dbe_confaccessor_test_exec --log_level=all
#   )
  
# if(DEFINED ENV{CI_JOB_TOKEN})
#    set_tests_properties(dbe_confaccessor_test dbe_config_api_set_test dbe_config_api_info_test
#       PROPERTIES
#          ENVIRONMENT "TDAQ_DB_REPOSITORY=https://gitlab-ci-token:$ENV{CI_JOB_TOKEN}@gitlab.cern.ch/atlas-tdaq-oks/p1/tdaq-09-04-00.git"
#    )
# elseif(DBE_ROOT)
#    set_tests_properties(dbe_confaccessor_test dbe_config_api_set_test dbe_config_api_info_test
#       PROPERTIES 
#          ENVIRONMENT "TDAQ_DB_REPOSITORY=${DBE_ROOT}/atlas-tdaq-oks/p1/tdaq-09-04-00.git"
#    )
# else()
#    set_tests_properties(dbe_confaccessor_test dbe_config_api_set_test dbe_config_api_info_test
#       PROPERTIES DISABLED TRUE
#    )
# endif()

daq_install()
