# ~~~
# Copyright (c) 2026 LunarG, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ~~~

# Generates the Vulkan Profiles Tools whitepaper PDF from its Markdown source.

set(WHITEPAPER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Profiles-Tools-Whitepaper.md")
# The PDF is written next to its Markdown source so the committed copy stays up to date.
set(WHITEPAPER_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Profiles-Tools-Whitepaper.pdf")
set(DOC_PDF_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/gen_doc_pdf.py")
set(DOC_PDF_CSS "${CMAKE_CURRENT_SOURCE_DIR}/doc_pdf.css")

file(GLOB WHITEPAPER_IMAGES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/images/*")

# The modules listed in doc/requirements.txt are installed into the project venv by the
# python_venv target (see the top-level CMakeLists.txt).
if (SKIP_PYTHON_AUTO_INSTALL)
    message(STATUS "SKIP_PYTHON_AUTO_INSTALL is ON: install the modules listed in ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt to build the whitepaper PDF")
endif()

add_custom_command(
    OUTPUT "${WHITEPAPER_OUTPUT}"
    COMMAND "${PYTHON_EXECUTABLE}" "${DOC_PDF_SCRIPT}"
        --input "${WHITEPAPER_SOURCE}"
        --output "${WHITEPAPER_OUTPUT}"
        --css "${DOC_PDF_CSS}"
        --footer "The Vulkan Profiles Tools"
    DEPENDS "${WHITEPAPER_SOURCE}" "${DOC_PDF_SCRIPT}" "${DOC_PDF_CSS}" ${WHITEPAPER_IMAGES}
    COMMENT "Generating Vulkan-Profiles-Tools-Whitepaper.pdf..."
    VERBATIM
)

add_custom_target(VpWhitepaperPdf ALL DEPENDS "${WHITEPAPER_OUTPUT}")
set_target_properties(VpWhitepaperPdf PROPERTIES FOLDER "Documentation")
if (TARGET python_venv)
    add_dependencies(VpWhitepaperPdf python_venv)
endif()
