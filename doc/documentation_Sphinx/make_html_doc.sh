echo "refreshing ../../CREDITS.html and ../../CHANGES.html"
rst2html.py ../../CREDITS.txt ../CREDITS.html
rst2html.py ../../CHANGES.txt ../CHANGES.html
rst2html.py ../../RELEASE_CHANGES.txt ../RELEASE_CHANGES.html
echo "building HTML documentation with Sphinx"
echo
echo "1st pass for 'cleaning'"
echo
make html
echo
echo "*******"
echo
echo "2nd pass for real 'building'"
echo
make html

