echo "refreshing ../../CREDITS.html and ../../CHANGES.html"
#rst2html.py ../../CREDITS.txt ../CREDITS.html
#rst2html.py ../../CHANGES.txt ../CHANGES.html
#rst2html.py ../../RELEASE_CHANGES.txt ../RELEASE_CHANGES.html
echo "building HTML documentation with Sphinx"
echo "1st pass for 'cleaning'"
echo
#make html
echo
echo "*******"
echo
echo "2nd pass for real 'building'"
echo
#make html
echo "copying new HTML docs to atlasdaq.cern.ch:/var/www/html/dbe/ folder"
scp -r _build/html/* atlasdaq.cern.ch:/var/www/html/dbe/
echo "Done!"

