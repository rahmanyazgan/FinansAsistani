cmake_minimum_required(VERSION 3.21)
project(FinansAsistani VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Charts PrintSupport)

# Saf hesaplama kütüphanesi (GUI bağımsız, test edilebilir)
add_library(fincore STATIC src/taxcalc.cpp src/taxcalc.h src/compound.cpp src/compound.h)
target_include_directories(fincore PUBLIC src)

set(SOURCES
    src/main.cpp
    src/util.h src/util.cpp
    src/theme.h src/theme.cpp
    src/settings.h src/settings.cpp
    src/rateprovider.h src/rateprovider.cpp
    src/mainwindow.h src/mainwindow.cpp
    src/tabs/currencytab.h src/tabs/currencytab.cpp
    src/tabs/percenttab.h src/tabs/percenttab.cpp
    src/tabs/taxtab.h src/tabs/taxtab.cpp
    src/tabs/compoundinteresttab.h src/tabs/compoundinteresttab.cpp
    src/dialogs/taxsettingsdialog.h src/dialogs/taxsettingsdialog.cpp
    src/dialogs/resultdetailsdialog.h src/dialogs/resultdetailsdialog.cpp
    resources.qrc
)
if(WIN32)
    list(APPEND SOURCES app.rc)
endif()

add_executable(FinansAsistani ${SOURCES})
target_include_directories(FinansAsistani PRIVATE src)
target_link_libraries(FinansAsistani PRIVATE fincore
    Qt6::Widgets Qt6::Network Qt6::Charts Qt6::PrintSupport)
set_target_properties(FinansAsistani PROPERTIES WIN32_EXECUTABLE ON)

# Testler
enable_testing()
add_executable(taxcalc_test tests/taxcalc_test.cpp)
target_link_libraries(taxcalc_test PRIVATE fincore)
add_test(NAME taxcalc_test COMMAND taxcalc_test)
