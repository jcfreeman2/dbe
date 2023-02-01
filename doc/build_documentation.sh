#!/bin/bash
#
# This script builds the documentation
# (Doxygen and Sphinx) for the package
# DBE, part of the ATLAS TDAQ C&C.
#
# rbianchi@cern.ch
# 11.10.2012


# bold font
bold=`tput bold`
normal=`tput sgr0`

# font colors - See: http://webhome.csc.uvic.ca/~sae/seng265/fall04/tips/s265s047-tips/bash-using-colors.html
BLUE='\e[1;34m'
PURPLE='\e[0;35m'
endColor='\e[0m'

if ( [ "$1" == "-h" ] || [ "$1" == "" ] ); then
  echo; echo "${bold}DBE - Documentation building script${normal}"; echo
  echo "Options: "
  echo "    all    - build all (Doxygen + Sphinx)"
  echo "    doxy   - build the Doxygen only"
  echo "    sphinx - build the Sphinx doc only"
  echo "    -h     - print this message"
  echo; echo
  exit
fi

if ( [ "$1" == "doxy" ] || [ "$1" == "all" ] ); then
  echo; echo
  echo -e "${bold}${BLUE}=============================="
  echo -e "Building Doxygen documentation"
  echo -e "==============================${normal}${endColor}"

  cd doxygen_doc
  echo; echo "${bold}--------- building the doxygen doc${normal}"; echo; echo
  doxygen Doxyfile
  echo; echo "${bold}--------- copying to atlasdaq${normal}"; echo; echo

  scp -r html atlasdaq.cern.ch:/var/www/html/dbe/doxygen_doc/
  cd ..
fi



if ( [ "$1" == "sphinx" ] || [ "$1" == "all" ] ) ; then
  echo; echo
  echo -e "${bold}${BLUE}============================="
  echo -e "Building Sphinx documentation"
  echo -e "=============================${normal}${endColor}"

  echo; echo -e "${bold}${PURPLE}    ---> Please Notice: "
  echo -e "TDAQ environment should not be sourced, and Sphinx must be installed (See at http://sphinx.pocoo.org/).${endColor}${normal}"
  echo; echo

  cd documentation_Sphinx
  source make_html_doc_atlasdaq.sh
  cd ..
fi


echo; echo
echo -e "${bold}${BLUE}=============================="
echo -e "Documentation built. Enjoy it!"
echo -e "=============================="
echo; echo -e "you find it the at:"; echo
echo -e "Doxygen: https://atlasdaq.cern.ch/dbe/doxygen_doc/html/"
echo -e "Sphinx:  https://atlasdaq.cern.ch/dbe/${normal}${endColor}"
echo; echo

